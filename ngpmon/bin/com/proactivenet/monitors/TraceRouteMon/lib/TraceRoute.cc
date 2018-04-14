#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#ifdef LINUX
#include <string.h>
#endif
#include "Computation.h"

#define SUCCESS              0
#define FAILURE              -1
#define ICMPHEADERSIZE       8
#define OPTIONALDATASIZE     0   // OPTIONAL DATA IS SET TO ZERO AS TIME EXCEED
                                 // RESPONSES DO NOT COME WITH OPTIONAL DATA
#define MAXSENDPACKSIZE      100
#define MAXRECVPACKSIZE      1500
#define MONITORID            56789

/******************************************************************************
 * This declaration is required to invoke the _main method in the JNI Initialze
 * This is in a extern C block to disable name mangling by C++ compliler
 *
******************************************************************************/

extern "C"
{
	void _main();
};

jmethodID setsendtime = NULL;
static int recvsock = -1;
static int sendsock = -1;
jmethodID setIP = NULL;
static int alldeleted = 0;
struct hostent *host;
pthread_mutex_t mutex_obj = PTHREAD_MUTEX_INITIALIZER;

// Method to compute Check Sum

unsigned short in_cksum(unsigned short *addr, int len )
{
   int            nleft = len;
    u_short        *w = addr;
    u_short        answer;
    int            sum = 0;
    unsigned short odd_byte = 0;

    /*
     *  Our algorithm is simple, using a 32 bit accumulator (sum),
     *  we add sequential 16 bit words to it, and at the end, fold
     *  back all the carry bits from the top 16 bits into the lower
     *  16 bits.
     */
    while (nleft > 1)  {
        sum += *w++;
        nleft -= 2;
    }//end of while
    /* mop up an odd byte, if necessary */
    if (nleft == 1) {
        *(unsigned char *)(&odd_byte) = *(unsigned char *) w;
        sum += odd_byte;
    }//end of if

    /*
     * add back carry outs from top 16 bits to low 16 bits
     */

    sum = (sum >> 16) + (sum & 0xffff);        /* add hi 16 to low 16 */
    sum += (sum >> 16);                        /* add carry */
    answer = ~sum;                             /* truncate to 16 bits */
    return(answer);
}//end of in_cksum

//Method to compute the triptime.

struct timeval getDeltaTime ( struct timeval *to, struct timeval *from )
{

    struct timeval result;
    timerclear(&result);

    if ( !to || !from )
        return result ;

    result.tv_sec  = to->tv_sec - from->tv_sec;
    result.tv_usec = to->tv_usec - from->tv_usec;
    if (result.tv_usec < 0) {
        result.tv_usec += 1000000L;
        result.tv_sec -= 1;
    }//end of if
    while (result.tv_usec >= 1000000L) {
        result.tv_usec -= 1000000L;
        result.tv_sec += 1;
    }//end of while
    if (result.tv_sec < 0) {
        timerclear(&result);
    }//end of if
    return result;
}//end of getDeltaTime

int writetolog(const char *s)
{
#ifdef _DEBUG
        FILE *fp = fopen("/usr/pw/JNIlog.log","a+");
        fputs(s,fp);
        fputs("\n",fp);
        fclose(fp);
#endif
        return 0;
}//writetolog

JNIEXPORT void JNICALL Java_com_proactivenet_monitors_TraceRouteMon_Computation_initializeNative(JNIEnv *env, jclass myclass)
{
#ifdef HPUX
	_main();
#else
	return;
#endif
}//end of initializeNative


JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_TraceRouteMon_Computation_verifyTarget(JNIEnv *env, jclass myclass, jstring target, jint hashkey)
{
   jint retval;
   struct in_addr ip_addr;
   char* dest_name;
   const char * jni_IPAddress;
   jstring java_IPAddr;

   if (setIP == NULL){
   //jmethodID setIP = env->GetStaticMethodID(myclass,"setIPAddress","(ILjava/lang/String;)Z");
   setIP = env->GetStaticMethodID(myclass,"setIPAddress","(ILjava/lang/String;)Z");
   }//end of if

   dest_name = (char*) env->GetStringUTFChars(target,0);

 /* Making a call to gethostbyname to check whether the target is valid or not
  * We do this in pthread mutex lock block to be safe from concurrency problems
  * because gethostbyname uses the sane static structure to fill the results
  * and any other call to it may overwrite the structure
  */
   pthread_mutex_lock(&mutex_obj);
   host = gethostbyname((const char*)dest_name);
   env->ReleaseStringUTFChars(target,dest_name);
   if (host == NULL)
   {
   // The IP Address/ Target name given is not valid, return FAILURE
	pthread_mutex_unlock(&mutex_obj);
	retval = FAILURE;
	return retval;
   }//end of if
   else{
   // The IP Address/ Target name given is valid, return SUCCESS
        bcopy(host->h_addr_list[0], &(ip_addr.s_addr),host->h_length);
	pthread_mutex_unlock(&mutex_obj);
        jni_IPAddress  = inet_ntoa(ip_addr);
	java_IPAddr = env->NewStringUTF(jni_IPAddress);

   /*Setting the  IP Address to the Java Address Holder Hashtable */

#ifdef SOLBUILD
        if(env->CallStaticBooleanMethod(myclass,setIP,hashkey,java_IPAddr) == B_FALSE)
#else
        if(env->CallStaticBooleanMethod(myclass,setIP,hashkey,java_IPAddr) == false)
#endif
        {
	   //printf("TraceRoute.cc : Failed to set the IP Address of the Target / IP Address = [ %s ] to the Java Hashtable \n",dest_name);
	   retval = FAILURE;
	   return retval;
        }//end of if
	env->DeleteLocalRef(java_IPAddr);		
	retval = SUCCESS;
	return retval;
   }//end of else
}//end of verifyTarget

JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_TraceRouteMon_Computation_openRawSocket(JNIEnv *env, jclass myclass)
{
//printf("TraceRoute.cc : inside open raw socket \n");
   struct protoent *protocol;
   int protocol_no, maxrecvsize;

 /*  Using the system call getprotobyname to get the protocol numbar for ICMP
  *  This call fails if it cannot locate "icmp" in the protocol files. In case
  *  it returns NULL we set the number to IPPROTO_ICMP defined in netinet/in.h
  */

   if((protocol = getprotobyname("icmp")) == NULL)
   {
          //printf("getprotobyname failed \n");
          protocol_no = IPPROTO_ICMP;
   }//end of if
   else
   {
          protocol_no = protocol->p_proto;
   }//end of else

 /* Opening a raw socket with ICMP protocol */

   //Padmini's code change for 6812 Begin
   //recvsock = socket(AF_INET,SOCK_RAW, protocol_no);
   //sendsock = socket(AF_INET,SOCK_RAW, protocol_no);
   
   if(recvsock == -1)
   {
   	recvsock = socket(AF_INET,SOCK_RAW, protocol_no);
   }//End of if
   if(sendsock == -1)
   {
   	sendsock = socket(AF_INET,SOCK_RAW, protocol_no);
   }//End of if
   //Padmini's code change for 6812 End
   if((recvsock == -1) || (sendsock == -1))
   {
      switch(errno){
	 case EACCES:
	 {
	    //printf("TraceRoute.cc : Failed to open Raw Socket - Permission Denied\n");
	    return FAILURE;
	 }//end of case EACCES
	 case EMFILE:
	 {
	    //printf("TraceRoute.cc : Failed to open Raw Socket - Pre-processor table is full \n");
	    return FAILURE;
	 }//end of case EMFILE
	 case ENOMEM:
	 {
	    //printf("TraceRoute.cc : Failed to open Raw Socket - Insufficient Memory\n");
	    return FAILURE;
	 }//end of case ENOMEM
	 case ENOSR:
	 {
	    //printf("TraceRoute.cc : Failed to open Raw Socket - Insufficient STREAMS resources to open socket\n");
	    return FAILURE;
	 }//end of case ENOSR
	 case EPROTONOSUPPORT:
	 {
	    //printf("TraceRoute.cc : Failed to open Raw Socket - ICMP protocal not supported on this domain\n");
	    return FAILURE;
	 }//end of case EPROTONOSUPPORT
	 default:
	 {
	    //printf("TraceRoute.cc : Failed to open Raw Socket \n");
	    return FAILURE;
	 }//end of default
      }//end of switch
  }//end of if

 /*  Using the fcntl call to make the sendsock non blocking */

   if (fcntl(sendsock, F_SETFL, O_NONBLOCK) == -1){
        //printf("TraceRoute.cc : Failed to set socket option to Non Blocking\n");
	return FAILURE;
   }//end of if

 /* Using the call setsockopt to set the receive buffer size to 61,440 bytes */

   maxrecvsize = 60 * 1024;
#ifdef SOLBUILD

   setsockopt(recvsock,SOL_SOCKET,SO_RCVBUF,(const char*)&maxrecvsize,sizeof(maxrecvsize));
#else
   setsockopt(recvsock,SOL_SOCKET,SO_RCVBUF,&maxrecvsize,sizeof(maxrecvsize));
#endif
   alldeleted = 0;
   return SUCCESS;

}//end of openRawSocket

JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_TraceRouteMon_Computation_sendRequest(JNIEnv *env, jclass myclass, jlong seq_id, jstring IpAddress, jint ttlcount, jint pingcount)
{
#ifdef AIX
	//Padmini's code change begin for AIX TraceRoute Monitor

	//Define a new structure which has both icmp and ip header in it.
	struct icmp_ip_hdr
	{
		struct ip ii_ip;
		struct icmp ii_ic;
	};//End of struct 

	struct icmp_ip_hdr *ii;
	struct sockaddr_in dest_addr, local_addr;
	char sendpack[MAXSENDPACKSIZE];
	int sendbytes, sendcount;
	int sendpack_len = 48;//Length of IP and ICMP header
	struct timeval tsendtime;
	int on = 1;
	char* dest_ipaddr;
	char **hold_pointer;
	struct hostent * hst;
	char local_ipaddr[250];
	char hostname[250] ;

	gethostname(hostname, sizeof(hostname));
	if((hst = gethostbyname(hostname)) == NULL)
	{
		return FAILURE;
	}

	hold_pointer = hst->h_addr_list;
	inet_ntop(hst->h_addrtype, *hold_pointer, local_ipaddr, sizeof(local_ipaddr));

	/*  Get the method ID of method setSentTime from Java */
	if(setsendtime == NULL)
		setsendtime =  env->GetStaticMethodID(myclass,"setSentTime","(JJJI)Z");


	/* Setting the Source and Destination IP Address */

	dest_ipaddr = (char*) env->GetStringUTFChars(IpAddress,0);

	bzero(&local_addr, sizeof(struct sockaddr_in));
	bzero(&dest_addr, sizeof(struct sockaddr_in));

	local_addr.sin_family = AF_INET;
	dest_addr.sin_family = AF_INET;
	
	local_addr.sin_addr.s_addr = inet_addr(local_ipaddr);
	dest_addr.sin_addr.s_addr = inet_addr(dest_ipaddr);

	/* Using setsockopt call to set the IP_HDRINCL */

	setsockopt(sendsock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));

	for(sendcount = 0; sendcount < pingcount; sendcount++)
	{
		jlong sendtime_sec, sendtime_usec;
		jlong sendseqid;
		u_short lenghtoficmp = sizeof(struct icmp);

		/* Initialise the Send Buffer to 0 */
		memset((char *) sendpack, 0, sizeof(struct icmp_ip_hdr));

		sendseqid = seq_id + sendcount;

		/* BEGIN SETTING THE ICMP and IP HEADER */
	
		ii = (struct icmp_ip_hdr *)sendpack;

		ii->ii_ic.icmp_type = ICMP_ECHO;
		ii->ii_ic.icmp_code = 0;
		ii->ii_ic.icmp_id = MONITORID;
		ii->ii_ic.icmp_seq = (sendseqid);
		ii->ii_ic.icmp_cksum = in_cksum((unsigned short *)&(ii->ii_ic), (sizeof(struct icmp)));

		ii->ii_ip.ip_v = IPVERSION;
		ii->ii_ip.ip_hl = sizeof(struct ip) >> 2;
		ii->ii_ip.ip_tos = 0;
		ii->ii_ip.ip_len = sizeof(struct icmp_ip_hdr) ;
		ii->ii_ip.ip_id = 0;
		ii->ii_ip.ip_off = 0;
		ii->ii_ip.ip_ttl = ttlcount;
		ii->ii_ip.ip_p = IPPROTO_ICMP;
		ii->ii_ip.ip_src.s_addr = local_addr.sin_addr.s_addr;
		ii->ii_ip.ip_dst.s_addr = dest_addr.sin_addr.s_addr;

		//get the time 

		gettimeofday(&tsendtime, NULL);

		//Send the build packet

		sendbytes= sendto(sendsock, sendpack, sizeof(struct icmp_ip_hdr), 0, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr));

		if (sendbytes < sendpack_len)
		{
			writetolog("error in sending");
		}//end of if

		sendtime_sec  = tsendtime.tv_sec;
		sendtime_usec = tsendtime.tv_usec;
		env->CallStaticBooleanMethod(myclass,setsendtime,sendseqid,sendtime_sec,sendtime_usec,pingcount);
		
	}//End of for

	env->ReleaseStringUTFChars(IpAddress,dest_ipaddr);
	return SUCCESS;

#else
   struct sockaddr_in dest_addr;
   char sendpack[MAXSENDPACKSIZE];
   struct icmp *send_icmp_hdr;
   int sendbytes, sendpack_len, sendcount;
   struct timeval tsendtime;
   char* dest_ipaddr;
   //jmethodID setsendtime;

  /*  Get the method ID of method setSentTime from Java */
  if(setsendtime == NULL)
  setsendtime =  env->GetStaticMethodID(myclass,"setSentTime","(JJJI)Z");

  /* Setting the Destination IP Address */

   dest_ipaddr = (char*) env->GetStringUTFChars(IpAddress,0);
   bzero(&dest_addr, sizeof(struct sockaddr_in));
   dest_addr.sin_family = AF_INET;
   dest_addr.sin_addr.s_addr = inet_addr(dest_ipaddr);
   
 /* Set the send pack length to Icmp header size + Optional data size (WHICH IS
  * SET TO ZERO)
  */

   sendpack_len = ICMPHEADERSIZE + OPTIONALDATASIZE;

 /* Using setsockopt call to set the respective ttlcount (Time to live) */
#ifdef SOLBUILD 
   setsockopt(sendsock, IPPROTO_IP, IP_TTL, (const char*)&ttlcount, sizeof(int));
#else
   setsockopt(sendsock, IPPROTO_IP, IP_TTL, &ttlcount, sizeof(int));
#endif

   for(sendcount = 0; sendcount < pingcount; sendcount++){

   jlong sendtime_sec, sendtime_usec;
   jlong sendseqid;

 /* Initialise the Send Buffer to 0 */
   memset((char *) sendpack, 0, sendpack_len);
   sendseqid = seq_id + sendcount;

   /* BEGIN SETTING THE ICMP HEADER */
   send_icmp_hdr = (struct icmp *)sendpack;
   send_icmp_hdr->icmp_type= ICMP_ECHO;
   send_icmp_hdr->icmp_code=0;              // ALWAYS SET TO ZERO
   send_icmp_hdr->icmp_seq= (sendseqid);
   send_icmp_hdr->icmp_id=MONITORID;
   send_icmp_hdr->icmp_cksum = 0;

   gettimeofday(&tsendtime, NULL);

  /* Compute ICMP checksum here using the function in_cksum */
   send_icmp_hdr->icmp_cksum = in_cksum((unsigned short *) send_icmp_hdr, sendpack_len);

   /* END OF SETTING THE ICMP HEADER*/
	//Padmini's code change begin 7-3-2001

      //sendtime_sec  = tsendtime.tv_sec;
      //sendtime_usec = tsendtime.tv_usec;
	//struct timeval p1, p2;
	//gettimeofday(&p1, NULL);
//printf("Value of time in before sec %ld \n", p1.tv_sec);
//printf("Value of time in before usec %ld \n", p1.tv_usec);

      //env->CallStaticBooleanMethod(myclass,setsendtime,sendseqid,sendtime_sec,sendtime_usec,pingcount);

	//Padmini's code change end 
	//gettimeofday(&p2, NULL);
//printf("Value of time in after sec %ld \n", p2.tv_sec);
//printf("Value of time in after usec %ld \n", p2.tv_usec);
//Padmini's code begin 30-4-2001

#ifdef HPUX
   sendbytes= sendto(sendsock, sendpack, sendpack_len, 0, &dest_addr, sizeof(struct sockaddr));
#else
   sendbytes= sendto(sendsock, sendpack, sendpack_len, 0, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr));
#endif

//Padmini's code end 30-4-2001

   if (sendbytes < sendpack_len){
      switch(errno){
	 case EBADF:
	 {
	    //printf("TraceRoute.cc : Failed to send Request to [ %s ] for Ping No = [ %d ]  - Invalid Descriptor \n",dest_ipaddr,sendcount);
	 }//end of case EBADF
	 case EINVAL:
	 {
	    //printf("TraceRoute.cc : Failed to send Request to [ %s ] for Ping No = [ %d ]  - Size of address is not correct \n",dest_ipaddr,sendcount);
	 }//end of case EINVAL
	 case ENOTSOCK:
	 {
	    //printf("TraceRoute.cc : Failed to send Request to [ %s ] for Ping No = [ %d ]  - NOT A SOCKET \n",dest_ipaddr,sendcount);
	 }//end of case ENOTSOCK
	 default:
	 {
	    //printf("TraceRoute.cc : Failed to send Request to [ %s ] for Ping No = [ %d ]\n",dest_ipaddr,sendcount);
	 }//end of default
      }//end of switch
   }//end of if
   //else{
      sendtime_sec  = tsendtime.tv_sec;
      sendtime_usec = tsendtime.tv_usec;
      env->CallStaticBooleanMethod(myclass,setsendtime,sendseqid,sendtime_sec,sendtime_usec,pingcount);
   //}//end of else

   }//end of for

   env->ReleaseStringUTFChars(IpAddress,dest_ipaddr);
   return SUCCESS;
#endif

}//end of sendRequest

JNIEXPORT void JNICALL Java_com_proactivenet_monitors_TraceRouteMon_Computation_startListening(JNIEnv *env, jobject obj)
{
   

   struct timeval trecv;
   jclass cls = env->GetObjectClass(obj);
   jmethodID setdata = env->GetMethodID(cls,"setData","(Ljava/lang/String;Ljava/lang/String;JJJII)Z");

   while(alldeleted == 0)
   {
      struct sockaddr_in recv_addr;
      int recvbytes, recvaddr_len, recviphdr_len, tracestatus, icmp_errcode;
      char recvpack[MAXRECVPACKSIZE];
      //struct timeval trecv;
      struct ip *recv_ip_hdr;
      struct icmp *recv_icmp_hdr, *orig_icmp_hdr;
      jlong recvtime_sec, recvtime_usec;
      jlong recvseqid;
      long unique_id;

      unique_id = 0;
      icmp_errcode = -1;
      tracestatus = -1;
      recvaddr_len = sizeof(recv_addr);

//Padmini's code change begin 30-4-2001
#ifdef HPUX
      recvbytes = recvfrom(recvsock, recvpack, MAXRECVPACKSIZE, 0, &recv_addr, &recvaddr_len);
#elif AIX
	recvbytes = recvfrom(recvsock, recvpack, MAXRECVPACKSIZE, 0,(struct sockaddr *)&recv_addr, (size_t *)&recvaddr_len);
#elif LINUX
	recvbytes = recvfrom(recvsock, recvpack, MAXRECVPACKSIZE, 0,(struct sockaddr *)&recv_addr, (socklen_t *)&recvaddr_len);
#else
      recvbytes = recvfrom(recvsock, recvpack, MAXRECVPACKSIZE, 0,(struct sockaddr *)&recv_addr, &recvaddr_len);
#endif
//Padmini's code change end 30-4-2001

      if (recvbytes > 0)
      {
    /* Recv From succeded :
     *       Take the time required to calculate trip time
     *       Get a pointer to the IP header from  the recvd packet
     *       Calculate the length of the header required to point to the ICMP
     *       Header from the Router / Destination
     */
         gettimeofday(&trecv , NULL);
	 recvtime_sec = trecv.tv_sec;
	 recvtime_usec = trecv.tv_usec;
	 recv_ip_hdr = (struct ip *) recvpack;
	 recviphdr_len = recv_ip_hdr->ip_hl << 2;

      /* Get the ICMP header of Router / Destination */
	 recv_icmp_hdr = (struct icmp *)(recvpack + recviphdr_len);

	 if(recv_icmp_hdr -> icmp_type == ICMP_ECHOREPLY ){

      /* Reached the Destination */

	    recvseqid = recv_icmp_hdr->icmp_seq;
	    unique_id = recv_icmp_hdr->icmp_id;
	    tracestatus = ICMP_ECHOREPLY;
	 }//end of if

	//Padmini's code change begin 16-4-2001
	

	else if(recv_icmp_hdr->icmp_type == ICMP_UNREACH)
	{
		icmp_errcode = recv_icmp_hdr->icmp_code;
		orig_icmp_hdr = (struct icmp *)(recvpack + recviphdr_len + ICMPHEADERSIZE + recviphdr_len);
		recvseqid = orig_icmp_hdr->icmp_seq;
		unique_id = orig_icmp_hdr->icmp_id;
		tracestatus = ICMP_UNREACH;
	}//end of else if

	//Padmini's code change begin 16-4-2001
	 else if(recv_icmp_hdr->icmp_type == ICMP_TIMXCEED){

      /* Reached a router Get the original Icmp Header */

	    orig_icmp_hdr = (struct icmp *)(recvpack + recviphdr_len + ICMPHEADERSIZE + recviphdr_len);
	    recvseqid = orig_icmp_hdr->icmp_seq;
	    unique_id = orig_icmp_hdr->icmp_id;
	    tracestatus = ICMP_TIMXCEED;
	 }//end of else if

         if ( unique_id == MONITORID){

   /* Get the IP Address  & Hostname of the Router / Destination
    * For this we use the system call gethostbyaddr within pthread mutex lock
    * to be safe from concurrency problems
    */
	    jstring java_IPAddress;
	    //jstring java_hostname;
	    const char * jni_IpAddress;
	    //ulong_t ip_addr;

	    jni_IpAddress = inet_ntoa(recv_addr.sin_addr);
	    java_IPAddress = env->NewStringUTF(jni_IpAddress);
	    //ip_addr = inet_addr(inet_ntoa(recv_addr.sin_addr));
		//Padmini's code change begin 9-3-2001
	    /*pthread_mutex_lock(&mutex_obj);
	    //host = gethostbyaddr ( (char *) &ip_addr,sizeof(ip_addr),AF_INET);
	    host = gethostbyaddr ( (char *) &recv_addr.sin_addr,sizeof(recv_addr.sin_addr),AF_INET);
	    if (host == NULL){
		    java_hostname = env->NewStringUTF(jni_IpAddress);
	    }//end of if
	    else{
		    java_hostname = env->NewStringUTF(host->h_name);
	    }//end of else
	    pthread_mutex_unlock(&mutex_obj);
		Padmini's code change end 9-3-2001*/

	/* Setting the Router Object in the Java Hashtable */

	//env->CallBooleanMethod(obj,setdata,java_IPAddress,java_hostname,recvseqid,recvtime_sec,recvtime_usec,tracestatus);
	env->CallBooleanMethod(obj,setdata,java_IPAddress,java_IPAddress,recvseqid,recvtime_sec,recvtime_usec,tracestatus,icmp_errcode);
		//Padmini's code change end 9-3-2001

	env->DeleteLocalRef(java_IPAddress);
	//env->DeleteLocalRef(java_hostname);
	}//end of if
      }//end of if
   }//end of while

 //Closing the raw socket as all the instances of the monitor have been deleted
   close(sendsock);
   close(recvsock);
   //Padmini's code change for 6812 Begin
   sendsock = -1;
   recvsock = -1;
   //Padmini's code change for 6812 End
   return;
}//end of startListening

JNIEXPORT void JNICALL Java_com_proactivenet_monitors_TraceRouteMon_Computation_setalldeleted(JNIEnv *env, jclass myclass)
{
	setsendtime = NULL;
	setIP = NULL;
	alldeleted = 1;
	return;
}
