/*
 * NFMap.h
 *
 *  Created on: Apr 12, 2009
 *      Author: nyavin
 */

#ifndef NFMAP_H_
#define NFMAP_H_
using namespace std;
#include <map>

#include <NFRawQueue.h>

class NFRawQueue;
typedef map<unsigned long, NFRawQueue*> NFDeviceMap;


class NFMap {
public:
    NFMap();
    virtual ~NFMap();
    NFRawQueue* createNewDevice(struct sockaddr_in);
    NFRawQueue* getDeviceQueue(struct sockaddr_in);
	NFRawQueue* getDeviceQueue(unsigned long deviceAddr);
    void dumpMapContent();
    NFRawQueue* getCopyOfDeviceQueue(unsigned long deviceAddr);
private:
    NFDeviceMap m_deviceMap;
    NFMutex m_mutex;
};

#endif /* NFMAP_H_ */
