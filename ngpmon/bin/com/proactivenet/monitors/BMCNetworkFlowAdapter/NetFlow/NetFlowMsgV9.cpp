#include <netinet/in.h>
#include <tools.h>
#include <NFRawQueue.h>
#include <NetFlowV9.h>

NFV9RecordMap NetFlowMsgV9::m_recordMap;

#define NFV9_HEADER_SIZE                20
#define NFV9_RECORD_MAX_RECORDS_COUNT   30   /* Number of flows exported in this packet (1-30) */
#define NFV9_RECORD_MIN_RECORDS_COUNT   1    /* Number of flows exported in this packet (1-30) */

NetFlowMsgValidity_e  NetFlowMsgV9::validateMsgContent(NFRowMsg* nfRowMsg)
{
    if( (nfRowMsg->m_size) < NFV9_HEADER_SIZE ) return ERROR_SIZE_SHORT;
    NetFlowHeaderV9 *hdr = (NetFlowHeaderV9*)(nfRowMsg->m_msg);
    _ushort count = ntohs(hdr->count);
    if( (count < NFV9_RECORD_MIN_RECORDS_COUNT) || (count > NFV9_RECORD_MAX_RECORDS_COUNT) ) return ERROR_SIZE_SPLIT;
    return MSG_VALID;
}

NetFlowMsgV9::NetFlowMsgV9(void* msg)
{
    NFRowMsg *rowMsg = (NFRowMsg*)msg;

    fprintf(stderr,"[NetFlowMsgV9::NetFlowMsgV9] message size = %d\n", rowMsg->m_size);

    m_header = (NetFlowHeaderV9*)(rowMsg->m_msg);
    m_version = ntohs(m_header->version);

    // Number of FlowSet records (both template and data) contained within the packet.
    m_recordCount = ntohs(m_header->count);

    fprintf(stderr,"[NetFlowMsgV9::NetFlowMsgV9] message size = %d, record count = %u\n", rowMsg->m_size, m_recordCount);

    m_engineID = (_ushort)ntohs(m_header->source_id) >> 16;

    ParseV9(rowMsg);
}

NetFlowMsgV9::~NetFlowMsgV9()
{
}

_uint NetFlowMsgV9::createKey(_ushort engineID, _ushort templateID)
{
    _ushort *s;
    _uint key = 0;
    s = (_ushort*)(&key);
    *s = engineID;
    *(s+1) = templateID;
    return key;
}


void NetFlowMsgV9::ParseV9(NFRowMsg *msg)
{
    _ushort size_left = msg->m_size;

    CommonHeaderV9 *ch;
    _ushort nf9Len = sizeof(NetFlowHeaderV9);
    size_left -= nf9Len;
    _ushort flowset_length = 0;
    _ushort total_flowset_length = 0;
    _ushort flowset_id;

    while (size_left) {
            ch = (CommonHeaderV9*)((char*)msg->m_msg + nf9Len + total_flowset_length);

            flowset_id = ntohs(ch->flowset_id);
            flowset_length = ntohs(ch->length);
            fprintf(stderr, "\t\tNetFlowMsgV9::ParseV9 flowset ID = %u, flowset length = %u\n", flowset_id, flowset_length);

            total_flowset_length += flowset_length;

            if ( flowset_length <= 4 ) {
                fprintf(stderr,"[NetFlowMsgV9::ParseV9] error flowset_length=%d is too small\n", flowset_length);
                return;
            }

            if ( flowset_length > size_left ) {
                fprintf(stderr,"[NetFlowMsgV9::ParseV9] error flowset_length=%d is too big\n", flowset_length);
                size_left = 0;
                continue;
            }

            switch( flowset_id )
            {
                case 0:
                    fprintf(stderr, "\t\tNetFlowMsgV9::Process_v9_templates...\n");
                    Processv9Templates((TemplateFlowsetV9*)ch);
                    break;
                case 1:
                    fprintf(stderr, "\t\tNetFlowMsgV9::Process_v9_option_templates...\n");
                    //ProcessV9OptionTemplates((OptionTemplateFlowsetV9*)ch);
                    break;
                default:
                    fprintf(stderr, "\t\tNetFlowMsgV9::Process_v9_data...\n");
                    ProcessV9Data((DataFlowsetV9*)ch);
            }

            // next flowset
            size_left -= flowset_length;
    }
}

void NetFlowMsgV9::Processv9Templates(TemplateFlowsetV9 *template_flowset)
{
    TemplateRecordV9 *rec;
    uint16_t id, count;
    uint32_t size_left, template_size, key;

    map<_uint,TemplateRecordV9*>::iterator it;

    size_left = ntohs(template_flowset->length) - 4; // -4 for flowset header -> id and length
    rec = template_flowset->fields;

    // process all templates in flowset, as long as any bytes are left
    template_size = 0;
    while (size_left) {
        rec = (TemplateRecordV9*)(rec + template_size);

        id    = ntohs(rec->templateId);
        count = ntohs(rec->count);
        fprintf(stderr, "\t\tNetFlowMsgV9::Processv9Templates id = %u, count = %u\n", id, count);
        template_size = 4 + 4 * count;  // id + count = 4 bytes, and 2 x 2 bytes for each entry
        fprintf(stderr, "\t\tNetFlowMsgV9::Processv9Templates template_size = %d\n", template_size);

        key = createKey(m_engineID, id);
        it = m_recordMap.find(key);
        if( it == m_recordMap.end() )
        {
            fprintf(stderr, "\t\tNetFlowMsgV9::Processv9Templates insert new record to map\n");
            m_recordMap.insert(NFV9RecordMap::value_type((uint32_t)(key), rec));
        }
        else    // template already exist - refresh it.
        {
            fprintf(stderr, "\t\tNetFlowMsgV9::Processv9Templates template already exist in map - refresh it\n");
            //delete(m_recordMap.find(key)->second);
            m_recordMap[key] = rec;
        }

        if ( size_left < template_size ) {
            fprintf(stderr,"[NetFlowMsgV9::Processv9Templates] error template_size=%d is too big\n", template_size);
            size_left = 0;
            continue;
        }

        size_left -= template_size;
    }
}

void NetFlowMsgV9::ProcessV9OptionTemplates(OptionTemplateFlowsetV9 *option_template_flowset)
{
    uint32_t    size_left, nr_scopes, nr_options, i;
    uint16_t    id, scope_length, option_length;

    i = 0;
    size_left = ntohs(option_template_flowset->length) - 4; // -4 for flowset header -> id and length
    id = ntohs(option_template_flowset->flowset_id);
    scope_length = ntohs(option_template_flowset->option_scope_length);
    option_length = ntohs(option_template_flowset->option_length);

    if ( scope_length & 0x3 ) {
        // scope length error: not multiple of 4
        return;
    }

    if ( option_length & 0x3 ) {
        // option length error: not multiple of 4
        return;
    }

    if ( (scope_length + option_length) > size_left ) {
        // option template length error: size left too small for scopes length and options length
        return;
    }

    nr_scopes  = scope_length >> 2;
    nr_options = option_length >> 2;
}

void NetFlowMsgV9::ProcessV9Data(DataFlowsetV9 *data_flowset)
{
    uint8_t *data;
    uint16_t field_type, field_length, offset, count;
    uint32_t key;
    int i;
    map<_uint,TemplateRecordV9*>::iterator it;
    TemplateRecordV9 *rec;

    data_flowset->flowsetId = ntohs(data_flowset->flowsetId);
    fprintf(stderr, "\t\tNetFlowMsgV9::ProcessV9Data flowset ID = %u\n", data_flowset->flowsetId);
    // look for the template ID that match this flowset ID.
    key = createKey(m_engineID, data_flowset->flowsetId);
    it = m_recordMap.find(key);
    if( it == m_recordMap.end() )   // data does not match any template - ignore.
    {
        fprintf(stderr,"[NetFlowMsgV9::ProcessV9Data] data does not match any template in map - ignore\n");
        return;
    }

    fprintf(stderr, "\t\tNetFlowMsgV9::ProcessV9Data key found in map\n");

    offset = 0;
    rec = (TemplateRecordV9*)(it->second);
    count = ntohs(rec->count);
    for( i = 0; i < count; i++ )
    {
        field_type   = ntohs(rec->record[i].type);
        field_length = ntohs(rec->record[i].length);
        fprintf(stderr, "\t\tNetFlowMsgV9::ProcessV9Data count %d : type %u, length %u\n", i+1, field_type, field_length);

        // make sure field < 128
        if ( field_type > 127 )
        {
            fprintf(stderr,"[NetFlowMsgV9::ProcessV9Data] error field type=%d\n", field_type);
            continue;
        }

        data = (uint8_t*)(data_flowset->data);
        fprintf(stderr, "\t\tNetFlowMsgV9::ProcessV9Data data: ");
        printf("data: ");
        for( i = offset; i < (field_length+offset); i++ )
        {
            fprintf(stderr, "\t\tNetFlowMsgV9::ProcessV9Data %u ", data[i]);
        }
        fprintf(stderr, "\t\tNetFlowMsgV9::ProcessV9Data \n");
        offset += field_length;
    }
}


