// NetFlow V5 Headers
#if ! defined(NETFLOWV5_HEADER)
#define NETFLOWV5_HEADER

#include <tools.h>
#include <NetFlowMsgBase.h>

using namespace std;

typedef struct _NetFlowHeaderV5 {
    /* 24 byte header */
    _ushort version;			/* 5 */
    _ushort count;				/* The number of records in the PDU */
    _uint sysUpTime;			/* Current time in millisecs since router booted */
    _uint unix_secs;			/* Current seconds since 0000 UTC 1970 */
    _uint unix_nsecs;			/* Residual nanoseconds since 0000 UTC 1970 */
    _uint flow_sequence;		/* Seq counter of total flows seen */
    _ubyte  engine_type;		/* Type of flow switching engine (RP,VIP,etc.) */
    _ubyte  engine_id;			/* Slot number of the flow switching */
    _ushort reserved;
} NetFlowHeaderV5;

typedef struct _NetFlowRecordV5 {
    /* 48 byte header */
    _uint srcaddr;		/* Source IP Address */
    _uint dstaddr;		/* Destination IP Address */
    _uint nexthop;		/* Input interface index */
    _ushort input;			/* Input interface index */
    _ushort output;			/* Output interface index */
    _uint dPkts;			/* Packets sent in Duration */
    _uint dOctets;		/* Octets sent in Duration */
    _uint First;			/* SysUptime at start of flow */
    _uint Last;			/* and of last packet of flow */
    _ushort srcport;		/* TCP/UDP source port number of equivalent */
    _ushort dstport;		/* TCP/UDP destination port number of equivalent */
    _ubyte pad;
    _ubyte tcp_flags;		/* Cumulative OR of tcp flags */
    _ubyte prot;			/* IP protocol, e.g., 6=TCP, 17=UDP, ... */
    _ubyte tos;				/* IP Type-of-Service */
    _ushort src_as;			/* Originating AS of source address */
    _ushort dst_as;			/* Originating AS of destination address */
    _ubyte src_mask;		/* Source address prefix mask bits */
    _ubyte dst_mask;		/* Destination address prefix mask bits */
    _ushort drops;
} NetFlowRecordV5;

class NetFlowMsgBase;
class NetFlowMsgV5: public NetFlowMsgBase
{
    friend class NetFlowMsgBase;

public:
    virtual ~NetFlowMsgV5();
    static NetFlowMsgValidity_e validateMsgContent(NFRowMsg* nfRowMsg);

    virtual void dumpContent()const;
private:
	NetFlowMsgV5(void*);
    NetFlowRecordV5 *m_records;
    NetFlowHeaderV5 *m_header;
};
#endif
