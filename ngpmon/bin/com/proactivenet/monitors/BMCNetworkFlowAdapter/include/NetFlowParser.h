/*
 * NetFlowParser.h
 *
 *  Created on: Apr 26, 2009
 *      Author: agamzu
 */

#ifndef NETFLOWPARSER_H_
#define NETFLOWPARSER_H_

#include <NetFlow/NetFlowMsgBase.h>

class NetFlowParser {
	friend class NetFlowListener;
public:
	void processNewMsgs();
	void startNetFlowParserThread();
	void handleNewRowMsgs();
private:

	static NetFlowParser* CreateNetFlowParser(void*);
	static NetFlowParser* deleteNetFlowParser();

	NetFlowParser(void*);
	virtual ~NetFlowParser();


	void parseMsg(NFRawQueue *deviceQueue);
	void waitForNewMsgs();

    static NetFlowParser *m_NetFlowParser;
    static int m_refCounter;

    NFCond m_cond;
    NFMutex m_mutex;

    pthread_t m_nfParserHandler;
};

#endif /* NETFLOWPARSER_H_ */
