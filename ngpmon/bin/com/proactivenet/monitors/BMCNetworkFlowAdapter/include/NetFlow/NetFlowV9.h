// NetFlowV9.h headers
#if ! defined(NETFLOWV9_HEADER)
#define NETFLOWV9_HEADER

#include <map>
#include <NetFlowMsgBase.h>
using namespace std;

typedef struct _NetFlowHeaderV9 {
    _ushort  version;
    _ushort  count;
    _uint  SysUptime;
    _uint  unix_secs;
    _uint  sequence;
    _uint  source_id;
} NetFlowHeaderV9;

typedef struct _TemplateRecordV9 {
    _ushort templateId; // Each record has a template id
    _ushort count; // Number of fields in the template record
    struct {
        _ushort type; // type of field
        _ushort length; // length (size) of the corresponding field (to that type)
    } record[1];
} TemplateRecordV9;

typedef struct _TemplateFlowsetV9 {
    _ushort flowsetId;
    _ushort length;
    TemplateRecordV9 fields[1];
} TemplateFlowsetV9;

typedef struct _DataFlowsetV9 {
    _ushort flowsetId;
    _ushort length;
    _ubyte	data[4];
} DataFlowsetV9;

typedef struct _OptionTemplateFlowsetV9 {
    _ushort    flowset_id;
    _ushort    length;
    _ushort    template_id;
    _ushort    option_scope_length;
    _ushort    option_length;
    struct {
        _ushort  type;
        _ushort  length;
    } record[1];
} OptionTemplateFlowsetV9;

typedef struct _CommonHeaderV9 {
    _ushort flowset_id;
    _ushort length;
} CommonHeaderV9;

typedef map<_uint, TemplateRecordV9*> NFV9RecordMap;

class NetFlowMsgV9:public NetFlowMsgBase
{
    friend class NetFlowMsgBase;
public:
    virtual ~NetFlowMsgV9();
    static NetFlowMsgValidity_e validateMsgContent(NFRowMsg* nfRowMsg);

private:
    NetFlowMsgV9(void*);
    void ParseV9(NFRowMsg *msg);
    void Processv9Templates(TemplateFlowsetV9*);
    void ProcessV9OptionTemplates(OptionTemplateFlowsetV9*);
    void ProcessV9Data(DataFlowsetV9*);
    _uint createKey(_ushort, _ushort);

    NetFlowHeaderV9 *m_header;
    TemplateFlowsetV9 *m_flowSets[];
    OptionTemplateFlowsetV9 *m_optFlowSets;
    DataFlowsetV9 *m_dataFlowSets;

    static NFV9RecordMap m_recordMap;
};
#endif
