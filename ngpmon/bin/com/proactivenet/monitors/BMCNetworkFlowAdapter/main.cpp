/*
 * main.cpp
 *
 *  Created on: Apr 20, 2009
 *      Author: nyavin
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#include <tools.h>
#include <NFMap.h>
#include <NFRawQueue.h>
#include <NetFlowListener.h>

extern DeviceSet g_filteredDeviceSet;

NetFlowListener *g_nfListener = NULL;

extern "C" void* NetFlowListenerThread(void *arg)
{
    g_nfListener->startListening();
    return NULL;
}

int main(int argc, char *argv[])
{
    int port = 7654, sleepTime = 0;
    //unsigned long s_addr;
    if (argc!=7){
        fprintf(stderr,"Usage:\n\t%s -p <port number> -d <router IP> -t <life time in sec>\n", argv[0]);
    }
    else {
        port = atoi(argv[2]);
        //s_addr = (unsigned long)(inet_addr(argv[4]));
        sleepTime = atoi(argv[6]);
    }

    g_nfListener = new NetFlowListener(port);
    g_nfListener->initListener();
    //g_filteredDeviceSet.insert(s_addr);

    pthread_t nfHandler = 0L;
    pthread_create(&nfHandler, NULL, NetFlowListenerThread, NULL);
    sleep(sleepTime);
    g_nfListener->stopListening();
    delete g_nfListener;
    return 0;
}

