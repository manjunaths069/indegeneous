#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "PingObjectInfo.h"

#define SUCCESS              0
#define FAILURE              -1
#define ICMPHEADERSIZE       8
#define OPTIONALDATASIZE     48 
#define MAXSENDPACKSIZE      100
#define MAXRECVPACKSIZE      1500
#define MONITORID            (getpid())

#define OPEN_ERROR           65656
#define SEND_ERROR           89898
#define RECV_ERROR           78787
#define NO_VALUE             -3333

//ICMP Types, Names are diff across OS so defining them
#define ICMP_DEST_UNREACH    3
#define ICMP_SOURCE_QUENCH   4
#define ICMP_TIME_EXCEEDED   11
#define ICMP_PARAMETER_PROB  12

/******************************************************************************
 * This declaration is required to invoke the _main method in the JNI Initialze
 * This is in a extern C block to disable name mangling by C++ compliler
 *
******************************************************************************/

extern "C"
{
	void _main();
};

static int alldeleted       =  0;
static int raw_recvsock     = -1;
static int raw_sendsock     = -1;
static int dgram_sendsock   = -1;
static u_short sendUdpPort  =  0;
static jmethodID setUdpsenttime = NULL;
static jmethodID setErrorInfo = NULL;

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

void sendErrorInfoToJava (JNIEnv *env, jclass myclass, jlong seqid, int ERROR_FROM, int error_no, int icmp_error_type, int icmp_error_code, char * ipAddr)
{
    char * errmsg = NULL;
    jstring j_errmsg = NULL;
    jstring j_ipAddr = NULL;
    jint j_icmp_type = NO_VALUE;
    jint j_icmp_code = NO_VALUE;

    if ((ERROR_FROM == OPEN_ERROR) || (ERROR_FROM == SEND_ERROR))
	errmsg = strerror (error_no);
    else if (ERROR_FROM == RECV_ERROR)
    {
	j_icmp_type = icmp_error_type;
	j_icmp_code = icmp_error_code;
	if (ipAddr != NULL)
	    j_ipAddr = env->NewStringUTF(ipAddr);
    }//end of elseif

    if (setErrorInfo == NULL)
	setErrorInfo = env->GetStaticMethodID(myclass,"setErrorDetails","(JIIIILjava/lang/String;Ljava/lang/String;)V");

    if (errmsg != NULL)
	j_errmsg = env->NewStringUTF(errmsg);

    env->CallStaticVoidMethod(myclass, setErrorInfo, seqid, ERROR_FROM, error_no, j_icmp_type, j_icmp_code, j_errmsg, j_ipAddr);

    if (j_errmsg != NULL)
	env->DeleteLocalRef(j_errmsg);

    if (j_ipAddr != NULL)
	env->DeleteLocalRef(j_ipAddr);

    return;
}//end of sendErrorInfoToJava

JNIEXPORT void JNICALL Java_com_proactivenet_monitors_NodePingMon_PingObjectInfo_initializeNative(JNIEnv *env, jclass myclass)
{
#ifdef _HPUX
	_main();
#else
	return;
#endif
}//end of initializeNative

JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_NodePingMon_PingObjectInfo_openSocket(JNIEnv *env, jclass myclass)
{

   struct protoent *protocol;
   int protocol_no, maxrecvsize, retVal, bindVal;
   struct sockaddr_in bindaddr, localaddr;

 /*  Using the system call getprotobyname to get the protocol numbar for ICMP
  *  This call fails if it cannot locate "icmp" in the protocol files. In case
  *  it returns NULL we set the number to IPPROTO_ICMP defined in netinet/in.h
  */

   if((protocol = getprotobyname("icmp")) == NULL)
   {
          //printf("NodePing.cc : getprotobyname failed \n");
          protocol_no = IPPROTO_ICMP;
   }//end of if
   else
   {
          protocol_no = protocol->p_proto;
   }//end of else

 /* Opening a raw socket with ICMP protocol 
  * and Datagram Socket with UDP protocal
  */

   // dgram_sendsock is Commented out as we do not 
   // support UDP for 4.1
   //dgram_sendsock = socket( AF_INET, SOCK_DGRAM, 0);

   if (raw_recvsock == -1)
   raw_recvsock = socket( AF_INET, SOCK_RAW, protocol_no);

   if (raw_sendsock == -1)
   raw_sendsock = socket( AF_INET, SOCK_RAW, protocol_no);

   //if((raw_recvsock == -1) || (raw_sendsock == -1) || (dgram_sendsock == -1))
   if((raw_recvsock == -1) || (raw_sendsock == -1))
   {
      int error_no = NO_VALUE;
      error_no = errno;
      sendErrorInfoToJava (env, myclass, NO_VALUE, OPEN_ERROR, error_no, NO_VALUE, NO_VALUE, NULL);

      switch(error_no){
	 case EACCES:
	 {
	    //printf("NodePing.cc : Failed to open Raw Socket - Permission Denied\n");
	    return FAILURE;
	 }//end of case EACCES
	 case EMFILE:
	 {
	    //printf("NodePing.cc : Failed to open Raw Socket - Pre-processor table is full \n");
	    return FAILURE;
	 }//end of case EMFILE
	 case ENOMEM:
	 {
	    //printf("NodePing.cc : Failed to open Raw Socket - Insufficient Memory\n");
	    return FAILURE;
	 }//end of case ENOMEM
	 case ENOSR:
	 {
	    //printf("NodePing.cc : Failed to open Raw Socket - Insufficient STREAMS resources to open socket\n");
	    return FAILURE;
	 }//end of case ENOSR
	 case EPROTONOSUPPORT:
	 {
	    //printf("NodePing.cc : Failed to open Raw Socket - ICMP protocal not supported on this domain\n");
	    return FAILURE;
	 }//end of case EPROTONOSUPPORT
	 default:
	 {
	    //printf("NodePing.cc : Failed to open Raw Socket \n");
	    return FAILURE;
	 }//end of default
      }//end of switch
  }//end of if

 /*  Using the fcntl call to make the raw_sendsock & dgram_sendsock non blocking */
   if (fcntl(raw_sendsock, F_SETFL, O_NONBLOCK) == -1)
   {
	//printf("NodePing.cc : Failed to set socket option to Non Blocking\n");
	sendErrorInfoToJava (env, myclass, NO_VALUE, OPEN_ERROR, errno, NO_VALUE, NO_VALUE, NULL);
	return FAILURE;
   }//end of if

 /* Using the call setsockopt to set the receive buffer size to 61,440 bytes */

   maxrecvsize = 60 * 1024;
#ifdef SOLBUILD
   setsockopt(raw_recvsock,SOL_SOCKET,SO_RCVBUF,(const char*)&maxrecvsize, sizeof(maxrecvsize));
#else
   setsockopt(raw_recvsock,SOL_SOCKET,SO_RCVBUF,&maxrecvsize, sizeof(maxrecvsize));
#endif

/****************************************************************/
/** Commented out as we donot support UDP Ping for 4.1***********/
/*
   if (fcntl(dgram_sendsock, F_SETFL, O_NONBLOCK) == -1)
   {
	//printf("NodePing.cc : Failed to set socket option to Non Blocking\n");
	return FAILURE;
   }//end of if

  // Bind the Datagram socket to any un used port (assigned by the system)
  // on the Local Machine and get the assigned port no with the system call 
  // getsockname, this port no will be used as the unique id for UDP pings

   bzero(&bindaddr, sizeof(bindaddr));
   bindaddr.sin_family = AF_INET;
   bindaddr.sin_port = 0;
   
#ifdef _HPUX
   int addrlen = 128;
   bindVal = bind(dgram_sendsock, &bindaddr, sizeof(bindaddr));
   retVal = getsockname(dgram_sendsock, &localaddr, &addrlen);
#elif SOLBUILD
   size_t addrlen = 128;
   bindVal = bind(dgram_sendsock, (struct sockaddr *)&bindaddr, sizeof(bindaddr));
   retVal = getsockname(dgram_sendsock, (struct sockaddr *)&localaddr,(int *)&addrlen);
#else
   size_t addrlen = 128;
   bindVal = bind(dgram_sendsock, (struct sockaddr *)&bindaddr, sizeof(bindaddr));
   retVal = getsockname(dgram_sendsock, (struct sockaddr *)&localaddr,&addrlen);
#endif

   if ((retVal == -1) || (bindVal == -1))
   {
	//printf("NodePing.cc : Failed to get Datagram socket info\n");
	return FAILURE;
   }//end of if

   sendUdpPort = localaddr.sin_port;
*/
/****************************************************************/
   alldeleted = 0;
   return raw_sendsock;

}//end of openSocket

JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_NodePingMon_PingObjectInfo_sendIcmpPingReq(JNIEnv *env, jclass myclass, jlong sendseqid, jstring IpAddress)
{
   struct sockaddr_in dest_addr;
   char sendpack[MAXSENDPACKSIZE];
   struct icmp *send_icmp_hdr;
   int sendbytes, sendpack_len;
   struct timeval *tsendtime;
   char* dest_ipaddr;

  /* Setting the Destination IP Address  and the family */

   dest_ipaddr = (char*) env->GetStringUTFChars(IpAddress,0);
   bzero(&dest_addr, sizeof(struct sockaddr_in));
   dest_addr.sin_family = AF_INET;
   dest_addr.sin_addr.s_addr = inet_addr(dest_ipaddr);
   
 /* Set the send pack length to Icmp header size + Optional data size (WHICH IS
  * SET TO ZERO)
  */

   sendpack_len = ICMPHEADERSIZE + OPTIONALDATASIZE;

 /* Initialise the Send Buffer to 0 */
   memset((char *) sendpack, 0, sendpack_len);
   tsendtime = (struct timeval *) &sendpack[8];

   /* BEGIN SETTING THE ICMP HEADER */
   send_icmp_hdr = (struct icmp *)sendpack;
   send_icmp_hdr->icmp_type= ICMP_ECHO;
   send_icmp_hdr->icmp_code=0;              // ALWAYS SET TO ZERO
   send_icmp_hdr->icmp_seq= (sendseqid);
   send_icmp_hdr->icmp_id=MONITORID;
   send_icmp_hdr->icmp_cksum = 0;

   gettimeofday(tsendtime, (struct timezone *) 0);

  /* Compute ICMP checksum here using the function in_cksum */
   send_icmp_hdr->icmp_cksum = in_cksum((unsigned short *) send_icmp_hdr, sendpack_len);

   /* END OF SETTING THE ICMP HEADER*/

#ifdef _HPUX
   sendbytes= sendto(raw_sendsock, sendpack, sendpack_len, 0, &dest_addr, sizeof(struct sockaddr));
#else
   sendbytes= sendto(raw_sendsock, sendpack, sendpack_len, 0, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr));
#endif

   if (sendbytes < sendpack_len){

      int error_no = NO_VALUE;
      error_no = errno;
      sendErrorInfoToJava (env, myclass, sendseqid, SEND_ERROR, error_no, NO_VALUE, NO_VALUE, NULL);

      switch(error_no){
	 case EBADF:
	 {
	    //printf("NodePing.cc : Failed to send Request to [ %s ] - Invalid Descriptor \n",dest_ipaddr);
	    return error_no;
	 }//end of case EBADF
	 case EINVAL:
	 {
	    //printf("NodePing.cc : Failed to send Request to [ %s ] - Size of address is not correct \n",dest_ipaddr);
	    return error_no;
	 }//end of case EINVAL
	 case ENOTSOCK:
	 {
	    //printf("NodePing.cc : Failed to send Request to [ %s ] - NOT A SOCKET \n",dest_ipaddr);
	    return error_no;
	 }//end of case ENOTSOCK
	 default:
	 {
	    //printf("NodePing.cc : Failed to send Request to [ %s ]\n",dest_ipaddr);
	    return error_no;
	 }//end of default
      }//end of switch
   }//end of if

   env->ReleaseStringUTFChars(IpAddress,dest_ipaddr);
   return SUCCESS;

}//end of sendIcmpPingRequest

JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_NodePingMon_PingObjectInfo_sendUdpPingReq(JNIEnv *env, jclass myclass, jlong sendseqid, jstring IpAddress)
{
/****************************************************************/
/** Commented out as we donot support UDP Ping for 4.1***********/
/*
   struct sockaddr_in dest_addr;
   char sendpack[MAXSENDPACKSIZE];
   int sendbytes, sendpack_len;
   struct timeval tsendtime;
   char* dest_ipaddr;
   jlong sendtime_sec, sendtime_usec;

  //  Get the method ID of method setUdpSentTime from Java
  if(setUdpsenttime == NULL)
	setUdpsenttime = env->GetStaticMethodID(myclass,"setUdpSentTime","(JJJ)Z"); 

  // Setting the Destination IP Address  and the family
   dest_ipaddr = (char*) env->GetStringUTFChars(IpAddress,0);
   bzero(&dest_addr, sizeof(struct sockaddr_in));
   dest_addr.sin_family = AF_INET;
   dest_addr.sin_addr.s_addr = inet_addr(dest_ipaddr);
   dest_addr.sin_port = htons(sendseqid);
   
  // Initialise the Send Buffer to 0
   sendpack_len = strlen(sendpack);
   memset((char *) sendpack, 0, sendpack_len);

   gettimeofday(&tsendtime, NULL);


#ifdef _HPUX
   sendbytes= sendto(dgram_sendsock, sendpack, sendpack_len, 0, &dest_addr, sizeof(struct sockaddr));
#else
   sendbytes= sendto(dgram_sendsock, sendpack, sendpack_len, 0, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr));
#endif

   if (sendbytes < sendpack_len){
      switch(errno){
	 case EBADF:
	 {
	    //printf("NodePing.cc : Failed to send Request to [ %s ] - Invalid Descriptor \n",dest_ipaddr);
	    return FAILURE;
	 }//end of case EBADF
	 case EINVAL:
	 {
	    //printf("NodePing.cc : Failed to send Request to [ %s ] - Size of address is not correct \n",dest_ipaddr);
	    return FAILURE;
	 }//end of case EINVAL
	 case ENOTSOCK:
	 {
	    //printf("NodePing.cc : Failed to send Request to [ %s ] - NOT A SOCKET \n",dest_ipaddr);
	    return FAILURE;
	 }//end of case ENOTSOCK
	 default:
	 {
	    //printf("NodePing.cc : Failed to send Request to [ %s ]\n",dest_ipaddr);
	    return FAILURE;
	 }//end of default
      }//end of switch
   }//end of if

   // Setting the Send time for Response Object in the Java Hashtable
   sendtime_sec  = tsendtime.tv_sec;
   sendtime_usec = tsendtime.tv_usec;
   env->CallStaticBooleanMethod(myclass,setUdpsenttime,sendseqid,sendtime_sec,sendtime_usec);

   env->ReleaseStringUTFChars(IpAddress,dest_ipaddr);
*/
/****************************************************************/
   return SUCCESS;

}//end of sendUdpPingRequest

JNIEXPORT void JNICALL Java_com_proactivenet_monitors_NodePingMon_PingObjectInfo_startListening(JNIEnv *env, jobject obj)
{

   jclass cls = env->GetObjectClass(obj);
   jmethodID setResp = env->GetMethodID(cls,"setResponseTime","(JF)Z");
   jmethodID setUdprecvtime = env->GetMethodID(cls,"setUdpRecvTime","(JJJ)Z");

   while(alldeleted == 0)
   {
      struct sockaddr_in recv_addr;
      int recvbytes, recvaddr_len, recviphdr_len, sendiphdr_len;
      char recvpack[MAXRECVPACKSIZE];
      struct timeval trecv, *tsend, trip;
      struct ip *recv_ip_hdr, *send_ip_hdr;
      struct icmp *recv_icmp_hdr, *send_icmp_hdr;
      struct udphdr *recv_udp_hdr;
      u_short recvsrcport, recvdestport;
      jlong recvtime_sec, recvtime_usec, recvseqid;
      jfloat triptime;
      long unique_id;
      char * ipAddress = NULL;

      unique_id = 0;

      recvaddr_len = sizeof(recv_addr);

#ifdef _HPUX
      recvbytes = recvfrom(raw_recvsock, recvpack, MAXRECVPACKSIZE, 0, &recv_addr, &recvaddr_len);
#elif SOLBUILD
      recvbytes = recvfrom(raw_recvsock, recvpack, MAXRECVPACKSIZE, 0,(struct sockaddr *)&recv_addr, &recvaddr_len);
#else
      recvbytes = recvfrom(raw_recvsock, recvpack, MAXRECVPACKSIZE, 0,(struct sockaddr *)&recv_addr,(socklen_t *) &recvaddr_len);
#endif

      if (recvbytes > 0)
      {
    /* Recv From succeded :
     *       Take the time required to calculate trip time
     *       Get a pointer to the IP header from  the recvd packet
     *       Calculate the length of the header required to point to the ICMP
     *       Header from the Destination
     */
         gettimeofday(&trecv , (struct timezone *) 0);
	 recv_ip_hdr = (struct ip *) recvpack;
	 recviphdr_len = recv_ip_hdr->ip_hl << 2;

      /* Get the ICMP header of Destination */
	 recv_icmp_hdr = (struct icmp *)(recvpack + recviphdr_len);
	 unique_id = recv_icmp_hdr->icmp_id;

	 if((recv_icmp_hdr -> icmp_type == ICMP_ECHOREPLY) && (unique_id == MONITORID))
         {
      /* Reached the Destination */

	    recvseqid = recv_icmp_hdr->icmp_seq;
	    tsend = (struct timeval *) ((char *) recv_icmp_hdr + 8);
	    trip = getDeltaTime (&trecv, tsend);
	    triptime = (trip.tv_sec * 1000)+((trip.tv_usec)/1000.0);

	/* Setting the Response Object in the Java Hashtable */

	    env->CallBooleanMethod(obj,setResp,recvseqid,triptime);

	  }//end of if

	/**
	  * We check for the following four ICMP errors only -
	  * ICMP_DEST_UNREACH, ICMP_SOURCE_QUENCH, ICMP_TIME_EXCEEDED
	  * and ICMP_PARAMETER_PROB because from RFC 792 we see that
	  * the ICMP Hdr we had send will be retd in these cases.
	  *
	  * In case of errors, the packet composition is -
	  *  IP Header - ip_hl * 4 bytes (From intermediate destination)
	  *  ICMP Header - 8 bytes (From intermediate destination)
	  *  IP Header - ip_hl * 4 bytes (header prepended when we had send)
	  *  ICMP Header - 8 bytes (Header we had send, has our uniq id etc)
	  *
	  * If we get an error, check if the returned packet has an ICMP
	  * header.
	  *
	  * If its an ICMP header, check if it has our Monitor ID
	  * and only then send the error info to Java
	  **/

	 else if ( (recv_icmp_hdr->icmp_type == ICMP_DEST_UNREACH) || (recv_icmp_hdr->icmp_type == ICMP_SOURCE_QUENCH) || (recv_icmp_hdr->icmp_type == ICMP_TIME_EXCEEDED) || (recv_icmp_hdr->icmp_type == ICMP_PARAMETER_PROB) )
	 {
	    send_ip_hdr = (struct ip *) (recvpack + recviphdr_len + ICMPHEADERSIZE);
	    if (send_ip_hdr->ip_p == IPPROTO_ICMP)
	    {
		sendiphdr_len = send_ip_hdr->ip_hl << 2;
		send_icmp_hdr = (struct icmp *) (recvpack + recviphdr_len + ICMPHEADERSIZE + sendiphdr_len);

		if (send_icmp_hdr->icmp_id == MONITORID)
		{
		    ipAddress = inet_ntoa(recv_addr.sin_addr);
		    recvseqid = send_icmp_hdr->icmp_seq;

		    sendErrorInfoToJava (env, cls, recvseqid, RECV_ERROR, NO_VALUE, recv_icmp_hdr->icmp_type, recv_icmp_hdr->icmp_code, ipAddress);
		    strcpy (ipAddress, "");
		}//end of if
	    }//end of if
	 }//end of elseif
/****************************************************************/
/** Commented out as we donot support UDP Ping for 4.1***********/
/*
	  else if(recv_icmp_hdr->icmp_type == ICMP_UNREACH)
	  {
	     send_ip_hdr = (struct ip *)(recvpack + recviphdr_len + ICMPHEADERSIZE );
	     if( send_ip_hdr ->ip_p ==  IPPROTO_UDP)
	     {
		recv_udp_hdr = (struct udphdr *)(recvpack + recviphdr_len + ICMPHEADERSIZE+ recviphdr_len);

#ifdef LINUX
		recvsrcport = recv_udp_hdr->source;
		recvdestport = ntohs(recv_udp_hdr->dest);
#else
		recvsrcport = ntohs(recv_udp_hdr->uh_sport);
		recvdestport = ntohs(recv_udp_hdr->uh_dport);
#endif

		recvseqid = recvdestport;

		if( recvsrcport == sendUdpPort)
		{
		// Setting the Recvtime for Response Object in the Java Hashtable
		    recvtime_sec  = trecv.tv_sec;
		    recvtime_usec = trecv.tv_usec;
		    env->CallBooleanMethod(obj,setUdprecvtime,recvseqid,recvtime_sec,recvtime_usec);
		}//end of if
	     }//end of if
	  }//end of elseif
*/
/****************************************************************/
      }//end of if
   }//end of while

 //Closing the sockets as all the instances of the monitor have been deleted
   close(raw_recvsock);
   close(raw_sendsock);
   raw_recvsock = -1;
   raw_sendsock = -1;
   // dgram_sendsock is Commented out as we do not
   // support UDP for 4.1
   //close(dgram_sendsock);
   setResp = NULL;
   setUdpsenttime = NULL;
   setUdprecvtime = NULL;
   setErrorInfo = NULL;
   return;

}//end of startListening

JNIEXPORT void JNICALL Java_com_proactivenet_monitors_NodePingMon_PingObjectInfo_setalldeleted(JNIEnv *env, jclass myclass)
{
	alldeleted = 1;
	return;
}
