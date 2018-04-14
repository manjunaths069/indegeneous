/*
 * NetFlowV8.cpp
 *
 *  Created on: May 5, 2009
 *      Author: agamzu
 */

#include <netinet/in.h>
#include <NetFlowMsgBase.h>
#include <NFRawQueue.h>
#include <NetFlowV8.h>

#define NFV8_HEADER_SIZE       24
#define NFV8_RECORD_SIZE       28
#define NFV8_RECORD_MAX_RECORDS_COUNT  30	/* Number of flows exported in this packet (1-30) */
#define NFV8_RECORD_MIN_RECORDS_COUNT  1	/* Number of flows exported in this packet (1-30) */

NetFlowMsgValidity_e  NetFlowMsgV8::validateMsgContent(NFRowMsg* nfRowMsg)
{
	fprintf(stderr,"\t\t[NetFlowMsgV8::validateMsgContent1]  nfRowMsg->m_size = %d.\n", nfRowMsg->m_size);
	if (nfRowMsg->m_size < NFV8_HEADER_SIZE + NFV8_RECORD_SIZE) return ERROR_SIZE_SHORT;
	fprintf(stderr,"\t\t[NetFlowMsgV8::validateMsgContent2]  nfRowMsg->m_size = %d.\n", nfRowMsg->m_size);
	if (nfRowMsg->m_size > (NFV8_HEADER_SIZE + NFV8_RECORD_SIZE * NFV8_RECORD_MAX_RECORDS_COUNT)) return ERROR_SIZE_LONG;
	fprintf(stderr,"\t\t[NetFlowMsgV8::validateMsgContent3]  nfRowMsg->m_size = %d.\n", nfRowMsg->m_size);
	if (((nfRowMsg->m_size - NFV8_HEADER_SIZE) % NFV8_RECORD_SIZE) != 0) return ERROR_SIZE_SPLIT;
	fprintf(stderr,"\t\t[NetFlowMsgV8::validateMsgContent4]  nfRowMsg->m_size = %d.\n", nfRowMsg->m_size);
	const int calcRecCount = ((nfRowMsg->m_size - NFV8_HEADER_SIZE) / NFV8_RECORD_SIZE);
	const _ushort* recCount = reinterpret_cast<const _ushort*>(nfRowMsg->m_msg);
	if (ntohs(recCount[1])  != calcRecCount) return ERROR_SIZE_SPLIT;
	return MSG_VALID;
}


NetFlowMsgV8::NetFlowMsgV8(void* msg)
:NetFlowMsgBase(NFV8)
{
    NFRowMsg *rowMsg = (NFRowMsg*)msg;
    m_recordCount = (rowMsg->m_size - NFV8_HEADER_SIZE) / NFV8_RECORD_SIZE;
    m_header = (NetFlowHeaderV8*)(rowMsg->m_msg);

    m_records = new NetFlowRecordV8[2];
    m_records[0] =	*(((NetFlowRecordV8*)(rowMsg->m_msg))+NFV8_HEADER_SIZE+NFV8_RECORD_SIZE);
    dumpHexContent((const char*)rowMsg->m_msg,50);
/*    m_records = new NetFlowRecordV8[m_recordCount];
    int recCount = 0;
    while(recCount < m_recordCount)
    {
    	m_records[recCount] =	*(((NetFlowRecordV8*)(rowMsg->m_msg))+NFV8_HEADER_SIZE+NFV8_RECORD_SIZE*recCount);
    	recCount++;
    }
*/}

NetFlowMsgV8::~NetFlowMsgV8()
{
	delete m_records;
}

void NetFlowMsgV8::dumpContent()const
{

	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  version = %d.\n", ntohs(m_header->version));
	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  count = %d.\n", ntohs(m_header->count));
	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  sysUpTime = %ld.\n", ntohl(m_header->sysUpTime));
	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  unix_secs = %d.\n", ntohl(m_header->unix_secs));
	//fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  unix_secs = %ld.\n", time(NULL));
	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  flow_sequence = %d.\n", ntohl(m_header->flow_sequence));
	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  engine_type = %d.\n", m_header->engine_type);
	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  engine_id = %d.\n", m_header->engine_id);
	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  aggregation = %d.\n", m_header->aggregation);
	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  agg_version = %d.\n", m_header->agg_version);
//	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  reserved = %d.\n", ntohl(m_header->reserved));

	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  m_recordCount = %d.\n", m_recordCount);

	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  dFlows = %d.\n", ntohl(m_records[0].dFlows));

    int recCount = 0;
/*
    //while(recCount < m_recordCount)
    //{
    	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  dFlows = %d.\n", ntohl(m_records[recCount].dFlows));
    	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  dPkts = %d.\n", ntohl(m_records[recCount].dPkts));
    	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  dOctets = %d.\n", ntohl(m_records[recCount].dOctets));
    	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  First = %d.\n", ntohl(m_records[recCount].First));
    	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  Last = %d.\n", ntohl(m_records[recCount].Last));
		fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  src_as = %d.\n", ntohs(m_records[recCount].src_as));
		fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  dst_as = %d.\n", ntohs(m_records[recCount].dst_as));
    	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  input = %d.\n", ntohs(m_records[recCount].input));
    	fprintf(stderr,"\t\t[NetFlowMsgV8::dumpContent]  output = %d.\n", ntohs(m_records[recCount].output));
*/
    	//recCount++;
   // }

}
