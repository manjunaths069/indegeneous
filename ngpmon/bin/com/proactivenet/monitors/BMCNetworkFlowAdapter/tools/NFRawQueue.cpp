/*
 * NFRawQueue.cpp
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

NFRawQueue::NFRawQueue()
: m_maxSize(1000)
{
    int retVal = 0;
    retVal = pthread_mutex_init(&m_mutex,NULL);
}

NFRawQueue::NFRawQueue(int maxSize)
: m_maxSize(maxSize)
{
    int retVal = 0;
    retVal = pthread_mutex_init(&m_mutex,NULL);
}

NFRawQueue::~NFRawQueue()
{
    int retVal = 0;
    // Free whatever might be waiting for this
    pthread_mutex_lock(&m_mutex);
    pthread_mutex_unlock(&m_mutex);

    retVal = pthread_mutex_destroy(&m_mutex);
}

int NFRawQueue::push(void *nfRowMsg, int msgSize)
{
    //void *newBuffer;
    int retVal = 0;
    fprintf(stderr,"NFRawQueue::push CAlled with %d bytes for message\n",msgSize);
    if (nfRowMsg && msgSize>0){
    	retVal = pthread_mutex_lock(&m_mutex);

        m_msgQueue.push(new NFRowMsg(msgSize, nfRowMsg));
        pthread_mutex_unlock(&m_mutex);
    }
    else {
        retVal = -1;
    }
   // fprintf(stderr,"NFRawQueue::push returned with %d value\n",retVal);
    return retVal;
}

int NFRawQueue::push(NFRowMsg *nfRowMsg)
{
    //void *newBuffer;
    int retVal = 0;
    fprintf(stderr,"NFRawQueue::push CAlled with %d bytes for message\n",nfRowMsg->m_size);
    if (nfRowMsg && nfRowMsg->m_msg && (nfRowMsg->m_size > 0)){
    	retVal = pthread_mutex_lock(&m_mutex);
        m_msgQueue.push(nfRowMsg);
        pthread_mutex_unlock(&m_mutex);
    }
    else {
        retVal = -1;
    }
    // fprintf(stderr,"NFRawQueue::push returned with %d value\n",retVal);
    return retVal;
}

void* NFRawQueue::pop()
{
    NFRowMsg *firstMsg;
    pthread_mutex_lock(&m_mutex);
    firstMsg = m_msgQueue.front();
    m_msgQueue.pop();
    pthread_mutex_unlock(&m_mutex);
    //memcpy(m_buffer, firstMsg->m_msg,firstMsg->m_size);
    return firstMsg;
}

NFRawQueue* NFRawQueue::popRevQueue()
{
    NFRowMsg *firstMsg;
    NFRawQueue* copyNFRawQueue = new NFRawQueue();
    pthread_mutex_lock(&m_mutex);
    while (!m_msgQueue.empty())
    {
    	firstMsg = m_msgQueue.front();
    	copyNFRawQueue->push(firstMsg);
		fprintf(stderr,"\t\tpop queue %d.\n", firstMsg->m_size);
    	 m_msgQueue.pop();
    }

    pthread_mutex_unlock(&m_mutex);
    return copyNFRawQueue;
}

void NFRawQueue::dumpQueueContent()
{
    pthread_mutex_lock(&m_mutex);
    fprintf(stderr,"\t\tCurrently %d messages in the queue.\n", m_msgQueue.size());
    pthread_mutex_unlock(&m_mutex);
}


