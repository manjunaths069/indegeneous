/*
 * NFMap.cpp
 *
 *  Created on: Apr 12, 2009
 *      Author: nyavin
 */
#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;

#include <tools.h>
#include <NFRawQueue.h>
#include <NFMap.h>

NFMap::NFMap()
{
    int retVal = 0;
    retVal = pthread_mutex_init(&m_mutex,NULL);
}

NFMap::~NFMap()
{
}

NFRawQueue* NFMap::createNewDevice(struct sockaddr_in deviceAddr)
{
	int val;
	NFRawQueue* newQueue = new NFRawQueue();
    pthread_mutex_lock(&m_mutex);
    m_deviceMap.insert(NFDeviceMap::value_type(ntohl((unsigned long)(deviceAddr.sin_addr.s_addr)), newQueue));
    val = pthread_mutex_unlock(&m_mutex);
    return newQueue;
}

NFRawQueue* NFMap::getDeviceQueue(struct sockaddr_in deviceAddr)
{
    NFRawQueue *queue = NULL;
    pthread_mutex_lock(&m_mutex);
    NFDeviceMap::iterator it = m_deviceMap.find((unsigned long)(deviceAddr.sin_addr.s_addr));
    if(it!=m_deviceMap.end()){
        queue = it->second;
    }
    pthread_mutex_unlock(&m_mutex);

    return queue;
}

NFRawQueue* NFMap::getDeviceQueue(unsigned long deviceAddr)
{
    NFRawQueue *queue = NULL;
    pthread_mutex_lock(&m_mutex);
    NFDeviceMap::iterator it = m_deviceMap.find(deviceAddr);
    if(it!=m_deviceMap.end()){
        queue = it->second;
    }
    pthread_mutex_unlock(&m_mutex);

    return queue;
}

void NFMap::dumpMapContent()
{
    fprintf(stderr,"Currently %d devices monitored.\n", m_deviceMap.size());
    pthread_mutex_lock(&m_mutex);
    NFDeviceMap::iterator it = m_deviceMap.begin();
    NFRawQueue *queue = NULL;
    while(it!=m_deviceMap.end()){
        queue = it->second;
        fprintf(stderr,"dumpQueueContent of device %d .\n", it->first);
        queue->dumpQueueContent();
        ++it;
    }
    pthread_mutex_unlock(&m_mutex);
}

NFRawQueue* NFMap::getCopyOfDeviceQueue(unsigned long deviceAddr)
{
	NFRawQueue *queue = NULL;
	NFRawQueue *copyQueue = NULL;
	pthread_mutex_lock(&m_mutex);
	NFDeviceMap::iterator it = m_deviceMap.find(deviceAddr);
	if(it!=m_deviceMap.end()){
		queue = it->second;
		copyQueue = queue->popRevQueue();
	}

	pthread_mutex_unlock(&m_mutex);
	return copyQueue;
}
