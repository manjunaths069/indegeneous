/*
 * tools.h
 *
 *  Created on: Apr 12, 2009
 *      Author: nyavin
 */

#ifndef TOOLS_H_
#define TOOLS_H_

#include <set>

using namespace std;

typedef set<unsigned long> DeviceSet;

#if defined(WINDOWS)
#else
typedef pthread_cond_t NFCond;
typedef pthread_mutex_t NFMutex;
#endif

typedef char               _byte;
typedef unsigned char      _ubyte;
typedef unsigned short     _ushort;
typedef unsigned int       _uint;
typedef unsigned long      _ulong;
typedef unsigned long long _ulonglong;

#endif /* TOOLS_H_ */
