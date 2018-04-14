/*
 * NetFlowMsgBase.cpp
 *
 *  Created on: Apr 26, 2009
 *      Author: agamzu
 */

#include <stdio.h>
#include <netinet/in.h>
#include <tools.h>
#include <NFRawQueue.h>
#include <NetFlowMsgBase.h>
#include <NetFlowV5.h>
#include <NetFlowV8.h>
#include <NetFlowV9.h>

NetFlowMsgBase* NetFlowMsgBase::CreateNetFlowMsg(void* msg)
{
	int ret;
	NetFlowMsgBase* netFlowMsgBase = NULL;
	NFRowMsg *nfMsg = (NFRowMsg*)msg;
	const _ushort* version = reinterpret_cast<const _ushort*>(nfMsg->m_msg);

	switch (ntohs(version[0]))
	{
		case NFV5:
		{
			fprintf(stderr,"\t\t[NetFlowMsgBase::CreateNetFlowMsg] version 5 is %d.\n", ntohs(version[0]));
			ret = NetFlowMsgV5::validateMsgContent(nfMsg);
			if (ret == MSG_VALID)
			{
				netFlowMsgBase = new NetFlowMsgV5(msg);
				netFlowMsgBase->dumpContent();
			}
			else
				fprintf(stderr,"\t\t[NetFlowMsgBase::CreateNetFlowMsg] invalid msg with error %d.\n", ret);

			break;
		}
		case NFV8:
		{
			fprintf(stderr,"\t\t[NetFlowMsgBase::CreateNetFlowMsg] version 8 is %d.\n", ntohs(version[0]));
			ret = NetFlowMsgV8::validateMsgContent(nfMsg);
			//if (ret == MSG_VALID)
			//{
				netFlowMsgBase = new NetFlowMsgV8(msg);
				netFlowMsgBase->dumpContent();
		//	}
		//	else
		//		fprintf(stderr,"\t\t[NetFlowMsgBase::CreateNetFlowMsg] invalid msg with error %d.\n", ret);

			break;
		}
		case NFV9:
		{
			fprintf(stderr,"\t\t[NetFlowMsgBase::CreateNetFlowMsg] version 9 is %d.\n", ntohs(version[0]));
			netFlowMsgBase = new NetFlowMsgV9(nfMsg);
			break;
		}
		default:
		{
			fprintf(stderr,"\t\t[NetFlowMsgBase::CreateNetFlowMsg] no version is %d.\n", ntohs(version[0]));
		}

	}
	return netFlowMsgBase;
}

NetFlowMsgBase::~NetFlowMsgBase()
{

}

void NetFlowMsgBase::dumpHexContent(const char* buffer, int size)const
{
	int i = 0;
	fprintf(stderr,"\t\t[NetFlowMsgBase::dumpHexContent]");
	while (i < size)
	{
		fprintf(stderr,"[0x%02lX] ", buffer[i]);
		i++;
	}
	fprintf(stderr,"\n");
}
