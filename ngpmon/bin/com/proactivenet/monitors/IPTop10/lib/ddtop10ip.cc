
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/timeb.h>
#include <thread.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <net/if.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>

#include <pcap.h>
// #include <string.h>


//sojan start
#include <map>
#include <string>
//sojan end

#include "linklist.h"
#include "ddtoptenip.h"

//JNI Header file.
#include "IPTop10.h"


//sojan start
using namespace std;
typedef map<string, string> StringMap;
typedef std::map <string, string>::const_iterator StringMapItr;
#define IPTOSBUFFERS    12
StringMap ip_map; // map of IP and their interface
string strListDev;
//sojan end


//Global Declarations.

//To store error message.
char mErrorMessage[PCAP_ERRBUF_SIZE + 255];

char *dev;  
pcap_if_t *alldevs, *d;
int devCnt=0;
char *lisDev;

char errbuf[PCAP_ERRBUF_SIZE];
pcap_t *handle;

//For exclusive locking.
static mutex_t itrMutex;

char *giv_ip;
int giv_int;
int top_n;

time_t start_time,end_time;

longlong_t pkt_tot=0;
struct pcap_stat ps;

listPtr ip_traf_list;



void FreeAndReset(){
    
    FreeList(ip_traf_list, NULL);
    ip_traf_list = NULL;
    delete dev;
    delete lisDev;
    pkt_tot = 0;

}

void InitializeDD(char *ip,int interval,int n){

    giv_ip = new char[strlen(ip) + 1];
    strcpy(giv_ip,ip);
    giv_int = interval;
    top_n = n;

}

void Destroy(){

    free(giv_ip);
    giv_ip = NULL;
    
}

//sojan start
const char *GetInterface(string strIP)
{
	StringMapItr ipMapItr=ip_map.find(strIP);
	if (ipMapItr == ip_map.end())
		return NULL;
	else{ 
		return ((string)ipMapItr->second).c_str() ;
	}
}
/* From tcptraceroute, convert a numeric IP address to a string */

char *iptos(u_long in)
{
    static char output[IPTOSBUFFERS][3*4+3+1];
    static short which;
    u_char *p;

    p = (u_char *)&in;
    which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
    //#ifdef WIN32 
    sprintf(output[which], "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    //#else
    //sprintf(output[which], "%d.%d.%d.%d", p[3], p[2], p[1], p[0]);
    //#endif
    return output[which];
}

int isFound(string strIP)
{
	StringMapItr ipMapItr=ip_map.find(strIP);
	if (ipMapItr == ip_map.end())
		return 0;
	else 
		return 1;
}
//sojan end

//void DoPktCapForTop10IP(){
int DoPktCapForTop10IP(){
	
    struct tm *ltime;
    char timestr[128];
    
    int j=0;

    const u_char *packet;
    struct pcap_pkthdr *header;

    struct ether_header *ethernet;
    struct ip *ip;

    int size_ethernet = sizeof(struct ether_header);
    int size_ip = sizeof(struct ip);

    char *ip_src,*ip_dst;

    //declarations related to list
    listnodePtr node_tmp = NULL;
    IpTrafRec *traf_rec,*traf_rec_tmp;

    time_t end_t = 0;
    struct timeb tm_stbuf, tm_buf;

    
	/* Retrieve the device list */
    if (pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        sprintf(mErrorMessage,"Error in finding all Interfaces : %s", errbuf);
        printf("\n%s", mErrorMessage);
        pcap_freealldevs(alldevs);
 		return 0;
    }
    
    /* Print the list */
    devCnt = 0;
    for(d=alldevs; d; d=d->next)
    {
	devCnt++;
	//sojan start
	
	pcap_addr_t *a;
	for(a=d->addresses;a;a=a->next) {

		switch(a->addr->sa_family)
		{
		  case AF_INET:
			//printf("\tAddress Family Name: AF_INET\n");
			if (a->addr){
			  //printf("\tAddress: %s\n",iptos(((struct sockaddr_in *)a->addr)->sin_addr.s_addr));

				string strIP (iptos(((struct sockaddr_in *)a->addr)->sin_addr.s_addr));
				if(d->description)
				{
					string strInterface(d->description);
					ip_map[strIP] = strInterface;
				}
				else 
				{
					string strInterface(d->name);
					ip_map[strIP] = strInterface;
				}
				
		
			}
		}
	}
	
	//sojan end
        //printf("\n%d. %s", ++devCnt, d->name);
        if (d->description)
            printf("\n(%s)", d->description);
        else
            printf("\n(No description available)");
    }
    
    if(devCnt==0)
    {
        sprintf(mErrorMessage,"Error:No interfaces which support pcap found![user should have 'root' priviliege.]");
        printf("\n%s", mErrorMessage);
        return 0;
    }
    
    //printf("\nNo. of devices found: %d", devCnt);
	dev = new char[strlen(alldevs->name) + 1];
    strcpy(dev,alldevs->name); 
	
    handle = pcap_open_live(dev, BUFSIZ, 1, 250, errbuf);
    if(!handle){
        sprintf(mErrorMessage,"Unable to open the adapter. %s is not supported by pcap or permission denied: %s", dev, errbuf);
        printf("\n%s", mErrorMessage);
        pcap_freealldevs(alldevs);
        return 0;
    }
    //printf("\nDevice Open Success..");
	
    
    /*Print listening Device Description or Name as on availablility*/
    /*
    if(alldevs->description){
        //printf("\nlistening on %s...", alldevs->description);
        lisDev = new char[strlen(alldevs->name) + strlen(alldevs->description) + 3];
        sprintf(lisDev,"%s(%s)",alldevs->name,alldevs->description);
    }else{
        //printf("\nlistening on %s...", alldevs->name);
        lisDev = new char[strlen(alldevs->name) + 1];
        strcpy(lisDev,alldevs->name); 
    }
    */
    //sojan start
    strListDev = "";
    for(d=alldevs; d; d=d->next)
    {
	/*Print listening Device Description or Name as on availablility*/
	char *lisDevTmp;
	if(d->description){
		//printf("\nlistening on %s...\n", alldevs->description);
		lisDevTmp = new char[strlen(d->name) + strlen(d->description) + 3];
		sprintf(lisDevTmp,"%s(%s)",d->name,d->description);


	}else{
	   // printf("\nlistening on %s...\n", alldevs->name);
		lisDevTmp = new char[strlen(d->name) + 1];
		strcpy(lisDevTmp,d->name); 
	}
	string temp(lisDevTmp);
	strListDev = strListDev + temp;
	strListDev = strListDev + ",";		
    }

    lisDev = new char[strListDev.size() + 1];
    strcpy(lisDev, strListDev.c_str());
    int len = strlen(lisDev);
    lisDev[len - 1] =0;

    printf("\nListDev = %s\n", lisDev);

	//sojan end
    /* At this point, we don't need any more the device list. Free it */
    pcap_freealldevs(alldevs);
	
    
    
   // printf("\nCreating IP Traffic List...");
    ip_traf_list = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)IpTrafRecComp);
    //printf("\nCreated IP Traffic List...");

    ftime(&tm_stbuf);
    start_time = tm_stbuf.time;
    end_t = start_time + giv_int;
    pkt_tot = 0;

    while(1){
    
        //Checking duration to exit the loop--Can be optimized by threading.
        ftime(&tm_buf);
        if((tm_buf.time == end_t && tm_buf.millitm > tm_stbuf.millitm) || tm_buf.time > end_t){
           break; 
        }
        
		header = (struct pcap_pkthdr*)malloc(sizeof(struct pcap_pkthdr));
		packet = pcap_next(handle, header);
		
        //Extract the ethernet header.
        ethernet = (struct ether_header*)(packet);

        /*
        Only IP(IPV4) packets are considered.So check the type of packet and 
        extract the header if it is IP.(Discarding ARP, RARP, IPV6,IPX,etc for the time being...)
        //printf("\ntype:%d", ntohs(ethernet->ether_type));
        */
        switch(ntohs(ethernet->ether_type)){
            case ETHERTYPE_IP    :
                                  //printf("\n ETHERTYPE of Packet: IP");
                                  ip = (struct ip*)(packet + size_ethernet);
                                  break;

            /*
             Below checkings are done just to print the type of packets.
             In order to handle below protocols, code has to be added and 
             'switch' block has to be restructured.
            */
            case ETHERTYPE_PUP   :
                                  //printf("\n ETHERTYPE of Packet: PUP");
                                  free(header); 
                                  continue;
            case ETHERTYPE_ARP   :
                                 // printf("\n ETHERTYPE of Packet: ARP");
                                  free(header); 
                                  continue;
            case ETHERTYPE_REVARP:
                                  //printf("\n ETHERTYPE of Packet: RARP"); 
                                  free(header); 
                                  continue; 
            default              :
                                  //printf("\n ETHERTYPE of Packet: OTHER");
                                  free(header); 
                                  continue;
        } 
        
	    ip_src = (char*)malloc(strlen(inet_ntoa(ip->ip_src)) + 1);
        strcpy(ip_src, inet_ntoa(ip->ip_src));
        ip_dst = (char*)malloc(strlen(inet_ntoa(ip->ip_dst)) + 1);
        strcpy(ip_dst,inet_ntoa(ip->ip_dst));

        //if(!((strcmp(giv_ip,ip_src) == 0) || (strcmp(giv_ip,ip_dst) == 0)))
	//sojan start
	int isSrcIPMatched = isFound(string(ip_src));
	int isDestIPMatched = isFound(string(ip_dst));//(ip_map.find(ip_dst) == ip_map.end()) ;
	if (!(isSrcIPMatched || isDestIPMatched) )
	//sojan end
	{
	    free(ip_src);
            free(ip_dst);
            free(header); 
            continue;
        }


		/* convert the timestamp to readable format */
        ltime=localtime(&header->ts.tv_sec);
        strftime( timestr, sizeof timestr, "%c", ltime);
        
        // char *str_pkt_cnt = new char[128];
                
        // printf("\n\nPacket Count: %s \n *Last Captured Packet* \n Len: %d\t CapLen: %d\t TimeStamp: %s,%.6d", lltostr(++pkt_tot,str_pkt_cnt), header->len,header->caplen,timestr,header->ts.tv_usec);
        //printf("\nSource:%s",ip_src);
       ++pkt_tot; 
       //printf("\n%s -> %s ",ip_src,ip_dst ); 
       // printf("\nDestination:%s",ip_dst);
        
        // delete str_pkt_cnt;

	//sojan start
	//if(strcmp(giv_ip,ip_src) == 0)
	if(isSrcIPMatched)
        {
	    //printf(" Source Matched : \n");
            traf_rec = new IpTrafRec;
            traf_rec->ip_addr = ip_dst;
            traf_rec->sort_on = IP_ADDR;
	    //sojan start
	    char *nw_interface = (char*)GetInterface(ip_src);
	    traf_rec->nw_interface = nw_interface ;
	    //sojan end

            traf_rec_tmp = (IpTrafRec*)FindNode(ip_traf_list,traf_rec);
            if(traf_rec_tmp != NULL){
               traf_rec_tmp->pkts_to++;
               traf_rec_tmp->bytes_to += header->len;
	       //printf(" -> **OLD: [TO Pkt] : %ld ",(long)traf_rec_tmp->pkts_to);
               traf_rec_tmp = NULL;

            }else{
               traf_rec_tmp = new IpTrafRec;
               traf_rec_tmp->ip_addr = (char*)malloc(strlen(ip_dst) + 1);

               strcpy(traf_rec_tmp->ip_addr,ip_dst);
   		//sojan start
		traf_rec_tmp->nw_interface = (char*)malloc(strlen(nw_interface) + 1); 
		strcpy(traf_rec_tmp->nw_interface,nw_interface);
		//sojan end

               traf_rec_tmp->pkts_frm = 0;
               traf_rec_tmp->pkts_to = 1;
               traf_rec_tmp->bytes_frm = 0;
               traf_rec_tmp->bytes_to = header->len;
               //printf("	-> NEW**: [TO Pkt] : ");
               //Currently setting byte rate as the default sort key
               //traf_rec_tmp->sort_on = BYTE_TOT;
               
               //Currently setting packet count as the default sort key
		       traf_rec_tmp->sort_on = PKT_TOT;

               AddNode(ip_traf_list,NewNode(traf_rec_tmp));

               traf_rec_tmp = NULL;

            }

        	delete traf_rec;

        }
	//sojan start
	//else if(strcmp(giv_ip,ip_dst) == 0)
	else if(isDestIPMatched)
	{ 
		//printf(" Destination Matched : \n");
        	traf_rec = new IpTrafRec;

            traf_rec->ip_addr = ip_src;
            traf_rec->sort_on = IP_ADDR;

	    //sojan start
	    char *nw_interface = (char*)GetInterface(ip_dst);
	    traf_rec->nw_interface = nw_interface ;
	    //sojan end

            traf_rec_tmp = (IpTrafRec*)FindNode(ip_traf_list,traf_rec);
            if(traf_rec_tmp != NULL){
               traf_rec_tmp->pkts_frm++;
               traf_rec_tmp->bytes_frm += header->len;
		//printf("\n <- **OLD: [FROM Pkt] :  %ld  "+(long)traf_rec_tmp->pkts_frm);
               traf_rec_tmp = NULL;

            }else{
        	   traf_rec_tmp = new IpTrafRec;
               traf_rec_tmp->ip_addr = (char*)malloc(strlen(ip_src) + 1);

               strcpy(traf_rec_tmp->ip_addr,ip_src);
   		
		//sojan start
		traf_rec_tmp->nw_interface = (char*)malloc(strlen(nw_interface) + 1); 
		strcpy(traf_rec_tmp->nw_interface,nw_interface);
		//sojan end

               traf_rec_tmp->pkts_frm = 1;
               traf_rec_tmp->pkts_to = 0;
               traf_rec_tmp->bytes_frm = header->len;
               traf_rec_tmp->bytes_to = 0;
               //printf("\n <- NEW**: [FROM Pkt] :");
			   //Currently setting byte rate as the default sort key
               //traf_rec_tmp->sort_on = BYTE_TOT;

               //Currently setting packet count as the default sort key
               traf_rec_tmp->sort_on = PKT_TOT;
		
               AddNode(ip_traf_list,NewNode(traf_rec_tmp));

               traf_rec_tmp = NULL;

            }

            delete traf_rec;
            traf_rec =NULL;

        }

        //Freeing Mems
        free(ip_src);
        free(ip_dst);
		free(header);
        //free(ethernet);
        //free(ip);
        //free(packet);


        //Assigning to NULL
        ip_src = NULL;
        ip_dst = NULL;
        header = NULL;
        //ethernet =NULL;
        //packet = NULL:

    }
    end_time = tm_buf.time;

    pcap_stats(handle,&ps);
    pcap_close(handle);

    //Sort IP traffic list.
    SortList(ip_traf_list);


    //printf("\nPacket Capture For Top10 IP Finished...");
    return 1;

}

void PrintDDReport(){
    short cnt = 1;
    IpTrafRec *traf_rec_tmp;

    printf("\n\n=============================================");
    printf("\nMonitored IP: %s",giv_ip);
    printf("\nMonitored Device: %s",dev);
    printf("\nInterval: %d",giv_int);
    printf("\nStart Time: %s",ctime(&start_time));
    printf("End Time: %s",ctime(&end_time));
    printf("\n*Capture Statistics*");
    printf("\n=====================");
    printf("\nTot Cap: %ld \t Tot Drop: %ld",ps.ps_recv,ps.ps_drop);
    printf("\nTotal packet count to/from the given IP: %ld",pkt_tot);
    printf("\nTotal Number of Alien IPs: %d",ip_traf_list->Size);


    //Print Top 10 Alien ip details.
    ip_traf_list->Current = ip_traf_list->Head;
    while((cnt<=10) && (ip_traf_list->Current != NULL)){
        traf_rec_tmp = (IpTrafRec*)ip_traf_list->Current->Data;

        if(traf_rec_tmp != NULL){
          //  printf("\nIP:%s\tPktsFrm:%lu\tPktsTo:%lu\tBytesFrm:%lu\tBytesTo:%lu", //tSortOn:%0x",
           // traf_rec_tmp->ip_addr,traf_rec_tmp->pkts_frm,traf_rec_tmp->pkts_to,
           // traf_rec_tmp->bytes_frm,traf_rec_tmp->bytes_to); //,traf_rec_tmp->sort_on );
        }

        ip_traf_list->Current = ip_traf_list->Current->Next;
        cnt++;
    }

    //End of printing.
    printf("\n");

}

#ifdef MAIN

int main(int args, char** argv)
{

    if(args != 3 || atoi(argv[2]) <= 0){
       printf("Found More Arguments/Incorrect Usage/Invalid IP Address/Zero Interval.");
       printf("\nUsage is:- ddtop10ip <ip(IPv4 format: XXX.XXX.XXX.XXX)> <interval in secs>\n");
       exit(1);
    }

    InitializeDD(argv[1],atoi(argv[2]));
    DoPktCapForTop10IP();
    PrintDDReport();
    Destroy();

    return(0);
}

#endif


//JNI Functions

JNIEXPORT int JNICALL Java_com_proactivenet_monitors_IPTop10_IPTop10_initialize
  (JNIEnv *env, jobject obj, jstring ip, jint interval, jint n, jstring path){

    char *ip_ = (char*)env->GetStringUTFChars(ip, 0);
    
    InitializeDD(ip_, interval, n);
    
    env->ReleaseStringUTFChars(ip, ip_);
    env->DeleteLocalRef(ip);
    
    return 1;

}

JNIEXPORT jobject JNICALL Java_com_proactivenet_monitors_IPTop10_IPTop10_DoDDForTop10TrafMakingIP
  (JNIEnv *env, jobject obj){

    mutex_lock(&itrMutex);

   // printf("\nEntered Do DD For Top10 Traf-Making IP");
    short cnt = 0;
    short no_of_ip = 0;
    IpTrafRec *traf_rec_tmp;

    jobjectArray iprecarr = NULL;
    jobject tmp_iprecobj = NULL;

    //printf("\nBefore Do Packet Capture For Top10 IP");
    if(!DoPktCapForTop10IP()){
        FreeAndReset();    
        mutex_unlock(&itrMutex);
        return NULL;    
    }
   // printf("\nAfter Do Packet Capture For Top10 IP"); 
    
    jclass iprecclass = env->FindClass("com/proactivenet/monitors/IPTop10/IPRec");

    jclass ddrptclass = env->FindClass("com/proactivenet/monitors/IPTop10/IPTop10Report");
    jobject ddrptobj = env->AllocObject(ddrptclass);


    jfieldID stid = env->GetFieldID(ddrptclass, "startTime","Ljava/lang/String;");
    jfieldID etid = env->GetFieldID(ddrptclass, "endTime","Ljava/lang/String;");
    jfieldID tpid = env->GetFieldID(ddrptclass, "totPkts","J");
    jfieldID noid = env->GetFieldID(ddrptclass, "numOfInterfaces","I");
    jfieldID liid = env->GetFieldID(ddrptclass, "listenedOn","Ljava/lang/String;");
    jfieldID tiplstid = env->GetFieldID(ddrptclass, "top10IPs","[Lcom/proactivenet/monitors/IPTop10/IPRec;");


    //sojan start
    jmethodID SetInterfaceID = env->GetMethodID(iprecclass, "setInterface", "(Ljava/lang/String;)V");
    //sojan end
    
    jmethodID SetIPID = env->GetMethodID(iprecclass, "setIP", "(Ljava/lang/String;)V");
    jmethodID SetPktsFrmID = env->GetMethodID(iprecclass, "setPktsFrom", "(J)V");
    jmethodID SetPktsToID = env->GetMethodID(iprecclass, "setPktsTo", "(J)V");
    jmethodID SetBytesFrmID = env->GetMethodID(iprecclass, "setBytesFrom", "(J)V");
    jmethodID SetBytesToID = env->GetMethodID(iprecclass, "setBytesTo", "(J)V");


    
    if(ip_traf_list->Size < top_n){
        no_of_ip = ip_traf_list->Size;
    }else{
        no_of_ip = top_n;
    }


    iprecarr = env->NewObjectArray((jsize)no_of_ip,iprecclass,NULL);

    //Fill Top N Alien ip details.
    ip_traf_list->Current = ip_traf_list->Head;
    while((cnt<top_n) && (ip_traf_list->Current != NULL)){
        traf_rec_tmp = (IpTrafRec*)ip_traf_list->Current->Data;

        if(traf_rec_tmp != NULL){

            tmp_iprecobj = env->AllocObject(iprecclass);

            env->CallVoidMethod(tmp_iprecobj, SetIPID, env->NewStringUTF(traf_rec_tmp->ip_addr));
	    //sojan start
            env->CallVoidMethod(tmp_iprecobj, SetInterfaceID, env->NewStringUTF(traf_rec_tmp->nw_interface));
	    //sojan end

            env->CallVoidMethod(tmp_iprecobj, SetPktsFrmID, (jlong)traf_rec_tmp->pkts_frm);
            env->CallVoidMethod(tmp_iprecobj, SetPktsToID, (jlong)traf_rec_tmp->pkts_to);
            env->CallVoidMethod(tmp_iprecobj, SetBytesFrmID, (jlong)traf_rec_tmp->bytes_frm);
            env->CallVoidMethod(tmp_iprecobj, SetBytesToID, (jlong)traf_rec_tmp->bytes_to);


            env->SetObjectArrayElement(iprecarr,cnt,tmp_iprecobj);

            env->DeleteLocalRef(tmp_iprecobj);
            tmp_iprecobj = NULL;

        }

        ip_traf_list->Current = ip_traf_list->Current->Next;
        cnt++;
    }


    env->SetObjectField(ddrptobj, stid, env->NewStringUTF(ctime(&start_time)));
    env->SetObjectField(ddrptobj, etid, env->NewStringUTF(ctime(&end_time)));
    env->SetLongField(ddrptobj, tpid, (jlong)pkt_tot);
    env->SetIntField(ddrptobj,noid,(jint)devCnt); 
    env->SetObjectField(ddrptobj, liid, env->NewStringUTF(lisDev));
    env->SetObjectField(ddrptobj, tiplstid, iprecarr);

    //Delete refs.
    env->DeleteLocalRef(iprecarr);
    env->DeleteLocalRef(iprecclass);
    env->DeleteLocalRef(ddrptclass);

    FreeAndReset();    

	mutex_unlock(&itrMutex); 

    return ddrptobj;

}


JNIEXPORT void JNICALL Java_com_proactivenet_monitors_IPTop10_IPTop10_destroy
  (JNIEnv *env, jobject obj){
  
    Destroy();

}


JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_IPTop10_IPTop10_getError (JNIEnv *env, jobject obj)
{
    return env->NewStringUTF(mErrorMessage);
}
