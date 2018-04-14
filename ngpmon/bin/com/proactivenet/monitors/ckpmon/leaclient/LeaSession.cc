#include <stdio.h>

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/types.h>
#include <strings.h>
#include <netinet/in.h>
#endif

#include "LeaSession.h"
//#include "opsec/opsec.h"


#include "stdlog.h"
debug


//////////////////////// LeaSession ///////////////////////////

char	*LeaSession::s_fieldNames[] = {
	"time",
	"service",
	"i/f_name",
	"i/f_dir",
	"orig",
	"action",
	"user",
	"src",
	"xlatesrc",
	"dst",
	"xlatedst",
	"s_port",
	"xlatesport",
	"xlatedport",
	"proto",
	"icmp-type",
	"icmp-code",
	"h_len",
	"ip_vers",
	"alert",
	"rule",
	"resource",
	"res_action",
	"file",
	"len",
	"elapsed",
	"bytes",
	"reason",
	"sys_msgs",
	"encryption failure:",
	"decryption failure:",
	"success reason:",
	"agent",
	"from",
	"to",
	"orig_from",
	"orig_to",
	"error notification:",
	"message",
	"rpc_prog",
	"start_time",
	"packets",
	"scheme:",
	"methods:",
	"SPI:",
	"key update for",
	"signed by",
	"licensce violation detected",
	"category",
	"cat_server",
	"request",
	"target",
	"expire",
	"srckeyid",
	"dstkeyid",
	"ISAKMP Log:",
	"Negotiation Id:",
	"reason:",
	"host:",
	"port:",
	"has_accounting",
	// Added for Ckp NG
	"Sys_message:",
	"IKE Log:",
	"CookieI",
	"CookieR"
};

int	LeaSession::s_fieldCount = sizeof(LeaSession::s_fieldNames) /
		sizeof(char *);

int	LeaSession::s_startHandler (OpsecSession *session)
{
//	fprintf(stderr, "Start Handler\n");

	return OPSEC_SESSION_OK;
}

void	LeaSession::s_endHandler (OpsecSession *session)
{
//	fprintf(stderr, "End Handler\n");

	LeaSession	*ses = (LeaSession *) SESSION_OPAQUE(session);

	ses->endHandler();

	if (ses->deleteOnEnd())
		delete ses;
}

int	LeaSession::s_recordHandler (OpsecSession *session,
		lea_record *rec, int attrib_perm[])
{
//	fprintf(stderr, "Record Handler\n");

	LeaSession	*ses = (LeaSession *) SESSION_OPAQUE(session);
	LeaRecord	record(ses, rec, attrib_perm);

	ses->s_position = lea_get_record_pos(session);

	return ses->recordHandler(&record);
}

int	LeaSession::s_dictionaryHandler (OpsecSession *session,
		int dict_id, LEA_VT val_type, int n_d_entries)
{
//	fprintf(stderr, "Dictionary Handler\n");

	LeaSession	*ses = (LeaSession *) SESSION_OPAQUE(session);

	if (dict_id == LEA_ATTRIB_ID)
		ses->attrDictHandler();

	return ses->dictionaryHandler(dict_id, val_type, n_d_entries);
}

int	LeaSession::s_switchHandler (OpsecSession *session)
{
//	fprintf(stderr, "Switch Handler\n");

	LeaSession	*ses = (LeaSession *) SESSION_OPAQUE(session);

	return ses->switchHandler();
}

int	LeaSession::s_eofHandler (OpsecSession *session)
{
//	fprintf(stderr, "Eof Handler\n");

	LeaSession	*ses = (LeaSession *) SESSION_OPAQUE(session);

	return ses->eofHandler();
}

void	LeaSession::attrDictHandler ()
{
_(attrDictHandler)

	lea_value_t	d_value;

	for (int i = 0 ; i < s_fieldCount ; i++) {
		if (s_attrib[i] != LEA_NOT_FOUND)
			continue;

		if (lea_reverse_dictionary_lookup(s_session, LEA_ATTRIB_ID,
				s_fieldNames[i], &d_value) != LEA_NOT_FOUND)
        {
			s_attrib[i] = d_value.i_value;
            //LOG (( "%s %d", s_fieldNames[i], d_value.i_value ));
        }
	}
}
 
LeaSession::LeaSession (OpsecEnv *env, char *server_conf, int mode,
		const LeaFile & lf, int startat, int position)
{
	s_valid = 0;
	s_session = 0;
	s_attrib = 0;
	s_position = 0;
	s_deleteOnEnd = 0;

	OpsecEntity *client, *server;

    s_lealogtype = lf.f_logtrack;

	server = opsec_init_entity(env, LEA_SERVER,
			OPSEC_ENTITY_NAME, server_conf,
			OPSEC_EOL);
	if (server == 0)
		return;

	client = opsec_init_entity(env, LEA_CLIENT,
			OPSEC_SESSION_START_HANDLER, &LeaSession::s_startHandler,
			LEA_RECORD_HANDLER, &LeaSession::s_recordHandler,
			LEA_DICT_HANDLER, &LeaSession::s_dictionaryHandler,
			LEA_EOF_HANDLER, &LeaSession::s_eofHandler,
			LEA_SWITCH_HANDLER, &LeaSession::s_switchHandler,
			OPSEC_SESSION_END_HANDLER, &LeaSession::s_endHandler,
			OPSEC_EOL);
	if (client == 0)
		return;

    
	if (lf.f_filename != 0)
		s_session = lea_new_session(client, server, mode,
				lf.f_logtrack, lf.f_filename,
				startat, position);
	else if (lf.f_fileid < 0)
		s_session = lea_new_session(client, server, mode,
				lf.f_logtrack,
				startat, position);
	else
		s_session = lea_new_session(client, server, mode,
				lf.f_logtrack, lf.f_fileid,
				startat, position);

	if (s_session == 0)
		return;

    opsec_start_keep_alive ( s_session, 10000 );

	SESSION_OPAQUE(s_session) = this;

	s_attrib = new int[s_fieldCount];
	for (int i = 0 ; i < s_fieldCount ; i++)
		s_attrib[i] = LEA_NOT_FOUND;

	s_valid = 1;
}

LeaSession::~LeaSession ()
{
	if (s_attrib != 0)
		delete [] s_attrib;
	s_session = 0;
	s_valid = 0;
}


//////////////////////// LeaFile ///////////////////////////

LeaFile::LeaFile (char *filename)
{
	f_logtrack = LEA_FILENAME;
	f_filename = filename;
	f_fileid = -1;
}

LeaFile::LeaFile (LeaLogType which, long fileid)
{
	f_logtrack = (which == LOG_NORMAL) ? LEA_NORMAL_FILEID :
			LEA_ACCOUNT_FILEID;
	f_filename = 0;
	f_fileid = fileid;
}

LeaFile::LeaFile (LeaLogType which, LeaLogSelect when)
{
	if (when == LOG_FIRST)
		f_logtrack = (which == LOG_NORMAL) ? LEA_FIRST_NORMAL_FILEID :
				LEA_FIRST_ACCOUNT_FILEID;
	else
		f_logtrack = (which == LOG_NORMAL) ? LEA_CURRENT_NORMAL_FILEID :
				LEA_CURRENT_ACCOUNT_FILEID;
	f_filename = 0;
	f_fileid = -1;
}


//////////////////////// LeaRecord ///////////////////////////

int	LeaRecord::fieldIndex (LeaSession::LeaFieldID id)
{
	int	attr;

	if ((attr = r_session->fieldAttr(id)) != LEA_NOT_FOUND)
		return r_attrib_perm[attr];

	return LEA_NOT_FOUND;
}

char	*LeaRecord::fieldStr (LeaSession::LeaFieldID id)
{
_(fieldStr)

	int	fldIndex;

	if ((fldIndex = fieldIndex(id)) != LEA_NOT_FOUND)
    {
        char *str = lea_resolve_field(*r_session, r_record->fields[fldIndex]);
        //LOG (("%s", str ));
        return str;
    }
		//return lea_resolve_field(*r_session, r_record->fields[fldIndex]);

	return 0;
}

int LeaRecord::fieldInt ( LeaSession::LeaFieldID id, int *value )
{

    lea_field       *field;

    if ((field = getField(id)) == 0)
        return 0;

    if ( field->lea_val_type != LEA_VT_INT  || field->lea_val_type != LEA_VT_IP_PROTO )
        return 0;

    *value = field->lea_int;
    return 1;
}

char	*LeaRecord::fieldRawStr (LeaSession::LeaFieldID id)
{
	static char	buf[512];
        lea_field       *field;

        if ((field = getField(id)) == 0)
		return 0;
		
	switch (field->lea_val_type) {
	case LEA_VT_ACTION :
	case LEA_VT_INTERFACE :
	case LEA_VT_ALERT :
	case LEA_VT_RULE :
	case LEA_VT_INT :
		sprintf(buf, "%d", field->lea_int);
		break;
	case LEA_VT_DIRECTION :
	case LEA_VT_IP_PROTO :
		sprintf(buf, "%u", field->lea_u_char);
		break;
	case LEA_VT_TCP_PORT :
	case LEA_VT_UDP_PORT :
	case LEA_VT_USHORT :
		sprintf(buf, "%u", ntohs(field->lea_u_short));
		break;
	case LEA_VT_RPC_PROG :
	case LEA_VT_TIME :
	case LEA_VT_DURATION_TIME :
		sprintf(buf, "%lu", field->lea_u_long);
		break;
	case LEA_VT_HEX :
		sprintf(buf, "%lX", field->lea_u_long);
		break;
	case LEA_VT_IP_ADDR :
	case LEA_VT_MASK : {
		unsigned long	addr = ntohl(field->lea_u_long);

		sprintf(buf, "%d.%d.%d.%d",
                        (addr >> 24) & 0xff, (addr >> 16) & 0xff,
                        (addr >>  8) & 0xff, addr & 0xff);
		break;
		}
	case LEA_VT_STRING :
		sprintf(buf, "%s", field->lea_string);
		break;
	}

	return buf;
}

lea_field	*LeaRecord::getField (LeaSession::LeaFieldID id)
{
	int	fldIndex;

	if ((fldIndex = fieldIndex(id)) != LEA_NOT_FOUND)
		return &r_record->fields[fldIndex];

	return 0;
}


//////////////////////// Auxiliary ///////////////////////////

char	*formatField (LeaSession::LeaFieldID id, LeaRecord *record,
		char *title, char sep, char delim )
{
_(formatField)

	static char	buf[512];
	char	*str;

	*buf = '\0';

	if (title == 0)
		title = LeaSession::fieldName(id);

    //LOG (( "title %s %d", title, id ));

	if ((str = record->fieldStr(id)) != 0)
    {
        sprintf(buf, "%s", str );
        //LOG (( "%s buf %s", str, buf ));
        return buf;
/*
        if ( strcmp ( title, "" ) == 0 )
            sprintf(buf, "%s", str );
        else
		    sprintf(buf, "%s%c%s%c", title, sep, str, delim);
*/
    }
	return buf;
}
	
char	*formatRawField (LeaSession::LeaFieldID id, LeaRecord *record,
		char *title, char sep, char delim )
{
	static char	buf[512];
	char	*str;

	*buf = '\0';

	if (title == 0)
		title = LeaSession::fieldName(id);

	if ((str = record->fieldRawStr(id)) != 0)
		sprintf(buf, "%s%c%s%c", title, sep, str, delim); 

	return buf;
}


