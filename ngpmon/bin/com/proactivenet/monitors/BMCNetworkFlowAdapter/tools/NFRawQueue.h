/*
 * NFRawQueue.h
 *
 *  Created on: Apr 12, 2009
 *      Author: nyavin
 */

#ifndef NFRAWQUEUE_H_
#define NFRAWQUEUE_H_

#include <queue>

#define MAX_MSG_SIZE 65536

/*typedef struct _NFMsg {
	int size;
	char *msg;
} NFMsg;
*/
class NFRowMsg
{
public:
	NFRowMsg(int size, void* msg){
		m_size= size;
    	m_msg = malloc(sizeof(char) *m_size);
        memcpy(m_msg, msg, m_size);
	}
	~NFRowMsg(){
		free(m_msg);
	}
	int m_size;
	void *m_msg;
};

typedef queue<NFRowMsg*> NFRowMsgQueue;

class NFRawQueue {
public:
    NFRawQueue();
    NFRawQueue(int);
    virtual ~NFRawQueue();
    int push(void*, int);
    int push(NFRowMsg*);
    void* pop();
	bool empty()const {return m_msgQueue.empty();}
    void dumpQueueContent();
    NFRawQueue* popRevQueue();
private:
    int m_maxSize;
    NFRowMsgQueue m_msgQueue;
    NFMutex m_mutex;
    char m_buffer[MAX_MSG_SIZE];
};

#endif /* NFRAWQUEUE_H_ */
