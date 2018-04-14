/*
 * NetFlowParser.cpp
 *
 *  Created on: Apr 26, 2009
 *      Author: agamzu
 */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <tools.h>
#include <NFMap.h>
#include <NetFlowParser.h>

using namespace std;

extern NFMap g_devicesMap;
extern DeviceSet g_filteredDeviceSet;

#define WAIT_TIME_SECONDS       1

NetFlowParser* NetFlowParser::m_NetFlowParser = NULL;
int NetFlowParser::m_refCounter = 0;

extern "C" void* NetFlowParserThread(void *arg)
{

	NetFlowParser* nfParser = (NetFlowParser*)arg;
	nfParser->handleNewRowMsgs();
    return NULL;
}

void NetFlowParser::startNetFlowParserThread()
{
	pthread_create(&m_nfParserHandler, NULL, NetFlowParserThread, this);
	//nfParser->handleNewRowMsgs(NULL);
}

NetFlowParser* NetFlowParser::CreateNetFlowParser(void* arg)
{
    if (m_refCounter == 0)
    {
        m_NetFlowParser = new NetFlowParser(arg);
    }
    ++m_refCounter;
    return m_NetFlowParser;
}

NetFlowParser* NetFlowParser::deleteNetFlowParser()
{
    --m_refCounter;
    if (m_refCounter == 0)
    {
        delete m_NetFlowParser;  m_NetFlowParser = NULL;
    }
    return m_NetFlowParser;
}

NetFlowParser::NetFlowParser(void* arg)
:m_nfParserHandler(0L)
{
    int retVal = 0;
    retVal = pthread_cond_init(&m_cond,NULL);
    retVal = pthread_mutex_init(&m_mutex,NULL);
}

NetFlowParser::~NetFlowParser() {
    int retVal = 0;
    // Free whatever might be waiting for this
    pthread_mutex_lock(&m_mutex);
    pthread_cond_signal(&m_cond);
    pthread_mutex_unlock(&m_mutex);

    retVal = pthread_cond_destroy(&m_cond);
    retVal = pthread_mutex_destroy(&m_mutex);

}

void NetFlowParser::handleNewRowMsgs()
{
	DeviceSet::iterator filteredDeviceSetIter;
	while(1)
	{

		filteredDeviceSetIter = g_filteredDeviceSet.begin();

	    NFRawQueue *deviceQueue = NULL;
		NFRawQueue *copyOfDeviceQueue = NULL;

	    while(filteredDeviceSetIter!=g_filteredDeviceSet.end())
	    {

			deviceQueue = g_devicesMap.getDeviceQueue(*filteredDeviceSetIter);
			if (deviceQueue != NULL)
			{
				fprintf(stderr,"\t\t[NetFlowParser::start] data of %d\n", *filteredDeviceSetIter);
				//deviceQueue->dumpQueueContent();
				copyOfDeviceQueue = deviceQueue->popRevQueue();
				copyOfDeviceQueue->dumpQueueContent();
				parseMsg(copyOfDeviceQueue);
			}
	        ++filteredDeviceSetIter;
		}
	    waitForNewMsgs();
	}
}


void NetFlowParser::parseMsg(NFRawQueue *deviceQueue)
{
    NFRowMsg *firstMsg;

    while (!(deviceQueue->empty()))
    {
    	firstMsg = (NFRowMsg*)(deviceQueue->pop());
		fprintf(stderr,"\t\tpop queue %d.\n", firstMsg->m_size);
		NetFlowMsgBase::CreateNetFlowMsg(firstMsg);
    }
}

void NetFlowParser::waitForNewMsgs()
{
	int retVal;
	struct timeval now;
	struct timespec timeout;
	retVal = pthread_mutex_lock(&m_mutex);
	gettimeofday(&now,NULL);
	timeout.tv_sec = now.tv_sec + WAIT_TIME_SECONDS;
	timeout.tv_nsec = now.tv_usec * 1000; // Convert micro seconds to nana.
	retVal = pthread_cond_timedwait(&m_cond, &m_mutex, &timeout);
	retVal = pthread_mutex_unlock(&m_mutex);

}

void NetFlowParser::processNewMsgs()
{
	int retVal;
	retVal = pthread_mutex_lock(&m_mutex);
	retVal = pthread_cond_signal(&m_cond);
	retVal = pthread_mutex_unlock(&m_mutex);
}



