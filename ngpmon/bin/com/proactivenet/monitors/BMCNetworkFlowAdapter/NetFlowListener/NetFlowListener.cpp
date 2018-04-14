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

#include <tools.h>
#include <NFMap.h>
#include <NFRawQueue.h>
#include <NetFlowListener.h>

using namespace std;
extern NFMap g_devicesMap;
extern DeviceSet g_allDeviceSet;
extern DeviceSet g_filteredDeviceSet;

NetFlowListener::NetFlowListener(int port)
: m_servicePort(port),m_nfParser(0L)
{
    m_buffer = malloc(sizeof(char)*MAX_MSG_SIZE);
}

NetFlowListener::~NetFlowListener()
{
    free(m_buffer);
    close(m_fd);
}

void NetFlowListener::initListener()
{
    m_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    m_timeout = 1;
}

void NetFlowListener::startListening()
{
    int retVal = 0;
    fd_set fdSet, fdExpSet;
    FD_ZERO(&fdSet);
    FD_ZERO(&fdExpSet);
    struct timeval time_val; time_val.tv_sec = m_timeout; time_val.tv_usec = 0L;
    struct sockaddr_in sa;
    size_t fromlen, recsize;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons(m_servicePort);

    if (bind(m_fd,(struct sockaddr *)&sa, sizeof(struct sockaddr))==-1) {
        perror("error bind failed");
        close(m_fd);
        exit(EXIT_FAILURE);
    }
    else {
        int reuse = 1;
        int size = sizeof(reuse);
        retVal = setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, size);
    }
    m_running = true;
    NFRawQueue *deviceQueue = NULL;
    DeviceSet::iterator devIt;

	g_filteredDeviceSet.insert(1507739785L);
    g_filteredDeviceSet.insert(0L);
    g_filteredDeviceSet.insert(721359532L);		// linux-cmg-qa-m1.bmc.com
    g_filteredDeviceSet.insert(2303254493L);

    unsigned long addr;

    createNFParserThread();

    while(m_running) {
        FD_SET(m_fd, &fdSet);
        FD_SET(m_fd, &fdExpSet);
        retVal = select(m_fd+1, &fdSet, NULL, &fdExpSet, &time_val);
        if (retVal==-1) {
            fprintf(stderr,"Error occurred, while waiting on a socket.\n");
        }
        else if (retVal==0) {
            time_val.tv_sec = 1;
        }
        else {
            if(FD_ISSET(m_fd, &fdSet)){
                recsize = recvfrom(m_fd, m_buffer, MAX_MSG_SIZE, 0, (struct sockaddr *)&sa, &fromlen);
                if (recsize < 0)
                    fprintf(stderr, "%s\n", strerror(errno));
                else {
                    addr = ntohl((unsigned long)(sa.sin_addr.s_addr));
                    g_allDeviceSet.insert(addr);
                    devIt = g_filteredDeviceSet.find(addr);
					fprintf(stderr,"[NetFlowListener::startListening] addr=%u\n", addr);
                    // If the received device is also in enabled for monitoring in the g_filteredDeviceSet, do it.

                    if (devIt!=g_filteredDeviceSet.end()){
						fprintf(stderr,"[NetFlowListener::startListening] ok=%d\n", *devIt);
                        deviceQueue = g_devicesMap.getDeviceQueue(sa);
                        if (deviceQueue){
                            // Thread safe call
                            deviceQueue->push(m_buffer, recsize);
                        }
                        else {
                            // Thread safe call
                            deviceQueue = g_devicesMap.createNewDevice(sa);
                            // Thread safe call
                            deviceQueue->push(m_buffer, recsize);
                        }
                        m_nfParser->processNewMsgs();
                    }
                }
            }
            else if(FD_ISSET(m_fd, &fdExpSet)){
                fprintf(stderr,"\nSocket has generated an exception.\n");
            }
        }
    }
}

void NetFlowListener::stopListening()
{
    // It does not matter that its not thread safe.
    m_running = false;
}

void NetFlowListener::createNFParserThread()
{
	if (!m_nfParser)
	{
		fprintf(stderr,"[NetFlowListener::createNFParserThread]\n");
		m_nfParser = NetFlowParser::CreateNetFlowParser(NULL);
		m_nfParser->startNetFlowParserThread();

	}
}
