// NetFlowMsgBase.h
#if ! defined(__NetFlowMsgBase__)
#define __NetFlowMsgBase__

#include <tools.h>

using namespace std;

enum NetFlowVersions_e { NFV5=5, NFV8=8,  NFV9=9 };
enum NetFlowMsgValidity_e { MSG_VALID, ERROR_SIZE_SHORT, ERROR_SIZE_LONG, ERROR_SIZE_SPLIT,  MSG_WITHOUT_HEADER  };

class NetFlowMsgBase
{
public:
    static NetFlowMsgBase* CreateNetFlowMsg(void*);

    NetFlowMsgBase():m_version(0),m_recordCount(0),m_engineID(0){};
    NetFlowMsgBase(_ushort version):m_version(version),m_recordCount(0),m_engineID(0){};
    virtual ~NetFlowMsgBase()=0;
    virtual _ushort getVersion() const{return m_version;}
    virtual _ushort getRecordCount() const{return m_recordCount;}
    virtual _uint getEngineID() const{return m_engineID;}

    virtual void dumpContent()const{};
protected:
	virtual void dumpHexContent(const char* buffer, int size)const;

    _ushort m_version;
    _ushort m_recordCount;
    _uint m_engineID;				/* _ubyte  engine_type, _ubyte  engine_id */
};
#endif
