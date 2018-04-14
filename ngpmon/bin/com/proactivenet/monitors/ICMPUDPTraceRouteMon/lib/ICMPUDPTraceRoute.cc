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
#include <netinet/udp.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
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

static jmethodID setsendtime = NULL;
static jmethodID setudpsendtime = NULL;
static int recvsock = -1;
static int sendsock = -1;
static int udpsendsock = -1;
static  u_short sportstatic = -1;
static int udpportopen = -1;
static jmethodID setIP = NULL;
static int alldeleted = 0;
static struct hostent *host;
static u_short dport = 32768 + 666;
static pthread_mutex_t mutex_obj = PTHREAD_MUTEX_INITIALIZER;

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
        FILE *fp = fopen("/usr/padmini/JNIlog.log","a+");
        fputs(s,fp);
        fputs("\n",fp);
        fclose(fp);
#endif
        return 0;
}//writetolog
int writeinttolog(int s)
{
#ifdef _DEBUG
        FILE *fp = fopen("/usr/padmini/JNIlog.log","a+");
        fprintf(fp, "%d \n", s);
        fclose(fp);
#endif
        return 0;
}//writetolog

JNIEXPORT void JNICALL Java_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_initializeNative(JNIEnv *env, jclass myclass)
{
#ifdef HPUX
	_main();
#else
	//writetolog("in initializeNative");
	return;
#endif
}//end of initializeNative


JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_verifyTarget(JNIEnv *env, jclass myclass, jstring target, jint hashkey)
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

JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_openRawSocket(JNIEnv *env, jclass myclass)
{
printf("ICMPUDPTraceRoute.cc : inside open raw socket \n");
   struct protoent *protocol;
   int protocol_no, maxrecvsize;
   struct sockaddr_in bindaddr, checkaddr;
   u_short sport;
//writetolog("in open raw socket");

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

   recvsock = socket(AF_INET,SOCK_RAW, protocol_no);
   sendsock = socket(AF_INET,SOCK_RAW, protocol_no);
//writetolog("before udp socket");
   //UDP TraceRoute change begin
   udpsendsock = socket(AF_INET, SOCK_DGRAM, 0);
   //UDP TraceRoute change end
//writetolog("after udp socket");

   if((recvsock == -1) || (sendsock == -1) || (udpsendsock == -1))
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

	
	//UDP TraceRoute change begin
	//sport = (getpid() & 0xffff) | 0x8000;
 	bzero(&bindaddr, sizeof(bindaddr));
	bindaddr.sin_family = AF_INET;
	//bindaddr.sin_port = htons(sport);
	bindaddr.sin_port = 0;
#ifdef HPUX
	int len = 128;
	bind(udpsendsock,&bindaddr, sizeof(bindaddr));
	int what =getsockname(udpsendsock,&checkaddr, &len);
#elif AIX
        size_t len = 128;
        bind(udpsendsock,(struct sockaddr *)&bindaddr, sizeof(bindaddr));
        int what =getsockname(udpsendsock, (struct sockaddr *)&checkaddr,&len);
#else
	size_t len = 128;
	bind(udpsendsock,(struct sockaddr *)&bindaddr, sizeof(bindaddr));
	int what =getsockname(udpsendsock, (struct sockaddr *)&checkaddr,(int *)&len);
#endif

	//bind(udpsendsock,(struct sockaddr *)&bindaddr, sizeof(bindaddr));
	//int what =getsockname(udpsendsock, (struct sockaddr *)&checkaddr, &len);
	if(what == 0)
	{

writetolog("value of bound port no");
writeinttolog(checkaddr.sin_port);
	}
	jmethodID setudpport;
	setudpport = env->GetStaticMethodID(myclass, "setUDPPort", "(I)Z");
//writetolog("before writeto log");
	sportstatic = checkaddr.sin_port;
//writetolog("value of sportstatic ");
//writeinttolog(sportstatic);
	jint portvalue = sportstatic;
#ifdef SOLBUILD
        if(env->CallStaticBooleanMethod(myclass,setudpport,portvalue) == B_FALSE)
#else
        if(env->CallStaticBooleanMethod(myclass,setudpport,portvalue) == false)
#endif
	{
		return FAILURE;
	}//end of if

	//UDP TraceRoute change end

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

JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_sendRequest(JNIEnv *env, jclass myclass, jlong seq_id, jstring IpAddress, jint ttlcount, jint pingcount)
{
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

}//end of sendRequest

JNIEXPORT void JNICALL Java_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_startListening(JNIEnv *env, jobject obj)
{
   

   struct timeval trecv;
   jclass cls = env->GetObjectClass(obj);
   jmethodID setdata = env->GetMethodID(cls,"setData","(Ljava/lang/String;Ljava/lang/String;JJJII)Z");
   jmethodID setudpdata = env->GetMethodID(cls,"setUDPData","(Ljava/lang/String;Ljava/lang/String;JJJII)Z");

   while(alldeleted == 0)
   {
      struct sockaddr_in recv_addr;
      int recvbytes, recvaddr_len, recviphdr_len, tracestatus, icmp_errcode;
      //char recvpack[MAXRECVPACKSIZE];
	unsigned int recvpacklong[MAXRECVPACKSIZE/4];
	char *recvpack;
      //struct timeval trecv;
      struct ip *recv_ip_hdr;
      struct icmp *recv_icmp_hdr, *orig_icmp_hdr;
      jlong recvtime_sec, recvtime_usec;
      jlong recvseqid;
      jshort recvjsport, recvjdport;
      u_short recvsport, recvdport;
      long unique_id;

	recvpack = (char *) recvpacklong;
      unique_id = 0;
      icmp_errcode = -1;
      tracestatus = -1;
      recvaddr_len = sizeof(recv_addr);
      int isitUDP = 0;

//Padmini's code change begin 30-4-2001
#ifdef HPUX
      recvbytes = recvfrom(recvsock, recvpack, MAXRECVPACKSIZE, 0, &recv_addr, &recvaddr_len);
#elif AIX
      recvbytes = recvfrom(recvsock, recvpack, MAXRECVPACKSIZE, 0,(struct sockaddr *)&recv_addr, (size_t *)&recvaddr_len);
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
//writetolog("in recvfrom");

      /* Get the ICMP header of Router / Destination */
	 recv_icmp_hdr = (struct icmp *)(recvpack + recviphdr_len);
//writetolog("value of icmp type");
//writeinttolog(recv_icmp_hdr->icmp_type);
//writetolog("value of err code");
//writeinttolog(recv_icmp_hdr->icmp_code);

	if(recv_icmp_hdr -> icmp_type == ICMP_ECHOREPLY ){

      // Reached the Destination 

	    recvseqid = recv_icmp_hdr->icmp_seq;
	    unique_id = recv_icmp_hdr->icmp_id;
//writetolog("in echo reply");
//writetolog("value of uni id");
//writeinttolog(unique_id);
//writetolog("value of seq id");
//writeinttolog(recvseqid);
	    tracestatus = ICMP_ECHOREPLY;
	 }//end of if
	

	//Padmini's code change begin 16-4-2001
	

	else if(recv_icmp_hdr->icmp_type == ICMP_UNREACH)
	{
		icmp_errcode = recv_icmp_hdr->icmp_code;
writetolog("value of icmp errcode");
writeinttolog(icmp_errcode);
		struct ip *hostip;
		struct udphdr *udp;
		hostip = (struct ip *)(recvpack + recviphdr_len + ICMPHEADERSIZE );
		if(hostip->ip_p ==  IPPROTO_UDP){
			udp = (struct udphdr *)(recvpack + recviphdr_len + ICMPHEADERSIZE + recviphdr_len);
#ifdef LINUX
			recvsport = (udp->source);
			recvdport = ntohs(udp->dest);
#else
			recvsport = ntohs(udp->uh_sport);
			recvdport = ntohs(udp->uh_dport);
#endif
			recvseqid = recvdport;
			unique_id = recvsport;
#ifdef LINUX
			if(udp->source == (sportstatic))
#else
			if(udp->uh_sport == htons(sportstatic) )
#endif
			{
				isitUDP = 1;
			}//end of if
		}//end of if

		else
		{
			orig_icmp_hdr = (struct icmp *)(recvpack + recviphdr_len + ICMPHEADERSIZE + recviphdr_len);
			recvseqid = orig_icmp_hdr->icmp_seq;
			unique_id = orig_icmp_hdr->icmp_id;
		}//end of else
		tracestatus = ICMP_UNREACH;
	}//end of else if

	//Padmini's code change begin 16-4-2001
	 else if(recv_icmp_hdr->icmp_type == ICMP_TIMXCEED){
		icmp_errcode = recv_icmp_hdr->icmp_code;
		struct ip *hostip;
		struct udphdr *udp;
		hostip = (struct ip *)(recvpack + recviphdr_len + ICMPHEADERSIZE );
		if(hostip->ip_p ==  IPPROTO_UDP){
			udp = (struct udphdr *)(recvpack + recviphdr_len + ICMPHEADERSIZE + recviphdr_len);
#ifdef LINUX
			recvsport = (udp->source);
			recvdport = ntohs(udp->dest);
#else
			recvsport = ntohs(udp->uh_sport);
			recvdport = ntohs(udp->uh_dport);
#endif
			recvseqid = recvdport;
			unique_id = recvsport;
#ifdef LINUX
			if(udp->source == (sportstatic) )
#else
			if(udp->uh_sport == htons(sportstatic) )
#endif
			{
				isitUDP = 1;
			}//end of if
		}//end of if

      /* Reached a router Get the original Icmp Header */
		else{

	    		orig_icmp_hdr = (struct icmp *)(recvpack + recviphdr_len + ICMPHEADERSIZE + recviphdr_len);
	    		recvseqid = orig_icmp_hdr->icmp_seq;
	    		unique_id = orig_icmp_hdr->icmp_id;
		}//end of else
	    tracestatus = ICMP_TIMXCEED;
	 }//end of else if

         if ( unique_id == MONITORID || unique_id == sportstatic){

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

	if(isitUDP)
	{
		env->CallBooleanMethod(obj,setudpdata,java_IPAddress,java_IPAddress,recvseqid,recvtime_sec,recvtime_usec,tracestatus,icmp_errcode);
	}//end of if
	else
	{
		env->CallBooleanMethod(obj,setdata,java_IPAddress,java_IPAddress,recvseqid,recvtime_sec,recvtime_usec,tracestatus,icmp_errcode);
	}//end of else
		//Padmini's code change end 9-3-2001

	env->DeleteLocalRef(java_IPAddress);
	//env->DeleteLocalRef(java_hostname);
	}//end of if
      }//end of if
   }//end of while

 //Closing the raw socket as all the instances of the monitor have been deleted
   close(sendsock);
   close(recvsock);
   close(udpsendsock);
//writetolog("all have been closed");
   return;

}//end of startListening

JNIEXPORT void JNICALL Java_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_setalldeleted(JNIEnv *env, jclass myclass)
{

	setsendtime = NULL;
	setIP = NULL;
	setudpsendtime = NULL;
	alldeleted = 1;
	udpportopen = -1;
//writetolog("set all deleted invoked");
//writetolog("value of alldeleted");
//writeinttolog(alldeleted);
	return;
}
/*	This method is written to send the udp packets.        
*/
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_sendUDPRequest(JNIEnv *env, jclass myclass, jlong seq_id, jstring IpAddress, jint ttlcount, jint pingcount, jshort udpseq_id)
{
writetolog("value of ttl count");
writeinttolog(ttlcount);
   struct sockaddr_in dest_addr, checkaddr;
   char sendpack[MAXSENDPACKSIZE];
   int sendbytes;
   struct timeval tsendtime;
   char* dest_ipaddr;
   dest_ipaddr = (char*) env->GetStringUTFChars(IpAddress,0);
   //jmethodID setudpsendtime;

  /*  Get the method ID of method setSentTime from Java */
  if(setudpsendtime == NULL)
  setudpsendtime =  env->GetStaticMethodID(myclass,"setUDPSentTime","(JJJIII)Z");

 /* Using setsockopt call to set the respective ttlcount (Time to live) */

  /* Setting tudphe Destination IP Address */
   jshort sendcount;


   for(sendcount = 0; sendcount < pingcount; sendcount++){
 /* Using setsockopt call to set the respective ttlcount (Time to live) */
   jlong sendtime_sec, sendtime_usec;
   jlong sendseqid;
   jint sendudpseqid, sendportno;
#ifdef HPUX
   int len = 128;
#else
   size_t len = 128;
#endif
   sendseqid = seq_id + sendcount;
   sendudpseqid = udpseq_id + sendcount;
   sendportno = dport + udpseq_id + sendcount;
   bzero(&dest_addr, sizeof(struct sockaddr_in));
   dest_addr.sin_family = AF_INET;
   dest_addr.sin_addr.s_addr = inet_addr(dest_ipaddr);
   dest_addr.sin_port = htons(sendportno);

 /* Initialise the Send Buffer to 0 */
   memset((char *) sendpack, 0, strlen(sendpack));


pthread_mutex_lock(&mutex_obj);
#ifdef SOLBUILD 
   setsockopt(udpsendsock, IPPROTO_IP, IP_TTL, (const char*)&ttlcount, sizeof(int));
//writetolog("set sol socket options");
#else
   setsockopt(udpsendsock, IPPROTO_IP, IP_TTL, &ttlcount, sizeof(int));
#endif
   gettimeofday(&tsendtime, NULL);

#ifdef HPUX
   sendbytes= sendto(udpsendsock, sendpack, strlen(sendpack), 0, &dest_addr, sizeof(struct sockaddr));
#else
   sendbytes= sendto(udpsendsock, sendpack, strlen(sendpack), 0, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr));
#endif

pthread_mutex_unlock(&mutex_obj);
//Padmini's code end 30-4-2001

   if (sendbytes < strlen(sendpack)){
writetolog("error in sending");
writeinttolog(errno);
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
   else{
      sendtime_sec  = tsendtime.tv_sec;
      sendtime_usec = tsendtime.tv_usec;
writetolog("value of sendportno");
writeinttolog(sendportno);
writetolog("value of sendudpseqid");
writeinttolog(sendudpseqid);
writetolog("value of ttlcound");
writeinttolog(ttlcount);
writetolog("value of sendseqid" );
writeinttolog(sendseqid);
      env->CallStaticBooleanMethod(myclass,setudpsendtime,sendseqid,sendtime_sec,sendtime_usec,pingcount, sendportno, sendudpseqid);
   }//end of else

   }//end of for

   env->ReleaseStringUTFChars(IpAddress,dest_ipaddr);
   return SUCCESS;

}//end of sendRequest
