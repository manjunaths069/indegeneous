/*
 * NetFlowV5.cpp
 *
 *  Created on: May 3, 2009
 *      Author: agamzu
 */

#include <netinet/in.h>
#include <NetFlowMsgBase.h>
#include <NFRawQueue.h>
#include <NetFlowV5.h>

#define NFV5_HEADER_SIZE       24
#define NFV5_RECORD_SIZE       48
#define NFV5_RECORD_MAX_RECORDS_COUNT  30	/* Number of flows exported in this packet (1-30) */
#define NFV5_RECORD_MIN_RECORDS_COUNT  1	/* Number of flows exported in this packet (1-30) */

NetFlowMsgValidity_e  NetFlowMsgV5::validateMsgContent(NFRowMsg* nfRowMsg)
{
	if (nfRowMsg->m_size < NFV5_HEADER_SIZE + NFV5_RECORD_SIZE) return ERROR_SIZE_SHORT;
	if (nfRowMsg->m_size > (NFV5_HEADER_SIZE + NFV5_RECORD_SIZE * NFV5_RECORD_MAX_RECORDS_COUNT)) return ERROR_SIZE_LONG;
	if (((nfRowMsg->m_size - NFV5_HEADER_SIZE) % NFV5_RECORD_SIZE) != 0) return ERROR_SIZE_SPLIT;
	const int calcRecCount = ((nfRowMsg->m_size - NFV5_HEADER_SIZE) / NFV5_RECORD_SIZE);
	const _ushort* recCount = reinterpret_cast<const _ushort*>(nfRowMsg->m_msg);
	if (ntohs(recCount[1])  != calcRecCount) return ERROR_SIZE_SPLIT;
	return MSG_VALID;
}


NetFlowMsgV5::NetFlowMsgV5(void* msg)
:NetFlowMsgBase(NFV5)
{
    NFRowMsg *rowMsg = (NFRowMsg*)msg;
    m_recordCount = (rowMsg->m_size - NFV5_HEADER_SIZE) / NFV5_RECORD_SIZE;
    m_header = (NetFlowHeaderV5*)(rowMsg->m_msg);
    m_records = new NetFlowRecordV5[m_recordCount];
    int recCount = 0;
    while(recCount < m_recordCount)
    {
    	m_records[recCount] =	*(((NetFlowRecordV5*)(rowMsg->m_msg))+NFV5_HEADER_SIZE+NFV5_RECORD_SIZE*recCount);
    	recCount++;
    }
}

NetFlowMsgV5::~NetFlowMsgV5()
{
	delete m_records;
}

void NetFlowMsgV5::dumpContent()const
{
	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  version = %d.\n", ntohs(m_header->version));
	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  count = %d.\n", ntohs(m_header->count));
	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  sysUpTime = %ld.\n", ntohl(m_header->sysUpTime));
	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  unix_secs = %d.\n", ntohl(m_header->unix_secs));
	//fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  unix_secs = %ld.\n", time(NULL));
	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  flow_sequence = %d.\n", ntohl(m_header->flow_sequence));
	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  engine_type = %d.\n", m_header->engine_type);
	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  engine_id = %d.\n", m_header->engine_id);
	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  reserved = %d.\n", ntohs(m_header->reserved));

	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  m_recordCount = %d.\n", m_recordCount);
    int recCount = 0;

    //while(recCount < m_recordCount)
    //{
    	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  srcaddr = %d.\n", ntohl(m_records[recCount].srcaddr));
    	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  dstaddr = %d.\n", ntohl(m_records[recCount].dstaddr));
    	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  nexthop = %d.\n", ntohl(m_records[recCount].nexthop));
    	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  input = %d.\n", ntohs(m_records[recCount].input));
    	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  output = %d.\n", ntohs(m_records[recCount].output));
    	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  dPkts = %d.\n", ntohl(m_records[recCount].dPkts));
    	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  First = %d.\n", ntohl(m_records[recCount].First));
    	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  Last = %d.\n", ntohl(m_records[recCount].Last));
    	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  srcport = %d.\n", ntohs(m_records[recCount].srcport));
    	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  dstport = %d.\n", ntohs(m_records[recCount].dstport));
    	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  pad = %d.\n", m_records[recCount].pad);
    	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  tcp_flags = %d.\n", m_records[recCount].tcp_flags);

    	fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  prot = %d.\n", m_records[recCount].prot);
		fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  tos = %d.\n", m_records[recCount].tos);
		fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  src_as = %d.\n", ntohs(m_records[recCount].src_as));
		fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  dst_as = %d.\n", ntohs(m_records[recCount].dst_as));
		fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  src_mask = %d.\n", m_records[recCount].src_mask);
		fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  dst_mask = %d.\n", m_records[recCount].dst_mask);
		fprintf(stderr,"\t\t[NetFlowMsgV5::dumpContent]  drops = %d.\n", ntohs(m_records[recCount].drops));
    	recCount++;
   // }

}
