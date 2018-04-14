// NetFlow Header V8
#if ! defined(NETFLOWV8_HEADER)
#define NETFLOWV8_HEADER

#include <tools.h>
#include <NetFlowMsgBase.h>

typedef struct _NetFlowHeaderV8 {
    /* 28 byte header */
    _ushort version;            /* 8 */
    _ushort count;                /* The number of records in the PDU */
    _uint sysUpTime;            /* Current time in millisecs since router booted */
    _uint unix_secs;            /* Current seconds since 0000 UTC 1970 */
    _uint unix_nsecs;            /* Residual nanoseconds since 0000 UTC 1970 */
    _uint flow_sequence;        /* Seq counter of total flows seen */
    _ubyte  engine_type;        /* Type of flow switching engine (RP,VIP,etc.) */
    _ubyte  engine_id;            /* Slot number of the flow switching */
    _ubyte  aggregation;        /* Aggregation method being used */
    _ubyte  agg_version;        /* Version of the aggregation export */
 //   _uint reserved;
} NetFlowHeaderV8;


typedef struct _NetFlowRecordV8 {
    /* 28 byte header */
    _uint dFlows;            /* Number of flows */
    _uint dPkts;            /* Packets sent in Duration */
    _uint dOctets;        /* Octets sent in Duration */
    _uint First;            /* SysUptime at start of flow */
    _uint Last;            /* and of last packet of flow */
    _ushort src_as;            /* Originating AS of source address */
    _ushort dst_as;            /* Originating AS of destination address */
    _ushort input;            /* Input interface index */
    _ushort output;            /* Output interface index */
} NetFlowRecordV8;

class NetFlowMsgV8: public NetFlowMsgBase
{
    friend class NetFlowMsgBase;
public:
    virtual ~NetFlowMsgV8();
    static NetFlowMsgValidity_e validateMsgContent(NFRowMsg* nfRowMsg);

    virtual void dumpContent()const;

private:
    NetFlowMsgV8(void*);
    NetFlowHeaderV8* m_header;
    NetFlowRecordV8 *m_records;
};
#endif

