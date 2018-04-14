/*
 * NetFlowListener.h
 *
 *  Created on: Apr 20, 2009
 *      Author: nyavin
 */

#ifndef NETFLOWLISTENER_H_
#define NETFLOWLISTENER_H_

#include <NetFlowParser.h>

class NetFlowListener
{
public:
    NetFlowListener(int servicePort);
    virtual ~NetFlowListener();
    void initListener();
    void startListening();
    void stopListening();
    void createNFParserThread();
private:
    int m_servicePort;
    long m_fd;
    long m_timeout;
    void *m_buffer;
    bool m_running;

    NetFlowParser* m_nfParser;
};

#endif /* NETFLOWLISTENER_H_ */
