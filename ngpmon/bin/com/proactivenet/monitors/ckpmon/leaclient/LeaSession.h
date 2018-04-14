#ifndef LEA_SESSION_HPP_INC
#define LEA_SESSION_HPP_INC

#include "opsec/lea.h"

class LeaRecord;

class LeaFile {
	friend class LeaSession;
public :
	enum LeaLogType {
		LOG_NORMAL,
		LOG_ACCOUNT
	};
	enum LeaLogSelect {
		LOG_FIRST,
		LOG_CURRENT
	};
private :
	int	f_logtrack;
	char	*f_filename;
	long	f_fileid;
public :
	LeaFile (char *filename);
	LeaFile (LeaLogType which, long fileid);
	LeaFile (LeaLogType which, LeaLogSelect when);
};

class LeaSession {
public :
	enum LeaFieldID {
		TIME_ID,
		SERVICE_ID,
		IF_NAME_ID,
		IF_DIR_ID,
		ORIGIN_ID,
		ACTION_ID,
		USER_ID,
		SRC_ID,
		XLATESRC_ID,
		DST_ID,
		XLATEDST_ID,
		S_PORT_ID,
		XLATESPORT_ID,
		XLATEDPORT_ID,
		PROTOCOL_ID,
		ICMP_TYPE_ID,
		ICMP_CODE_ID,
		H_LEN_ID,
		IP_VERS_ID,
		ALERT_ID,
		RULE_ID,
		RESOURCE_ID,
		RES_ACTION_ID,
		FILE_ID,
		LEN_ID,
		ELAPSED_ID,
		BYTES_ID,
		REASON_ID,
		SYS_MSGS_ID,
		ENC_FAILURE_ID,
		DEC_FAILURE_ID,
		SUCCESS_REASON_ID,
		AGENT_ID,
		FROM_ID,
		TO_ID,
		ORIG_FROM_ID,
		ORIG_TO_ID,
		ERR_NOTIFICATION_ID,
		MESSAGE_ID,
		RPC_PROG_ID,
		START_TIME_ID,
		PACKETS_ID,
		SCHEME_ID,
		METHODS_ID,
		SPI_ID,
		KEY_UPDATE_ID,
		SIGNED_BY_ID,
		LICENCE_VIOLATION_ID,
		CATEGORY_ID,
		CAT_SERVER_ID,
		REQUEST_ID,
		TARGET_ID,
		EXPIRE_ID,
		SRCKEY_ID,
		DSTKEY_ID,
		ISAKMP_LOG_ID,
		NEGOTIATION_ID,
		K_REASON_ID,
		K_HOST_ID,
		K_PORT_ID,
		HAS_ACCOUNTING_ID
	};

private :
	static int	s_startHandler (OpsecSession *session);
	static void	s_endHandler (OpsecSession *session);

	static int	s_recordHandler (OpsecSession *session,
			lea_record *rec, int attrib_perm[]);
	static int	s_dictionaryHandler (OpsecSession *session,
			int dict_id, LEA_VT val_type, int n_d_entries);
	static int	s_switchHandler (OpsecSession *session);
	static int	s_eofHandler (OpsecSession *session);

	void	attrDictHandler ();

protected :
	static char	*s_fieldNames[];
	static int	s_fieldCount;

	OpsecSession	*s_session;

    int s_lealogtype;

	int	*s_attrib;
	int	s_valid;
	int	s_position;
	int	s_deleteOnEnd;

public :
	LeaSession (OpsecEnv *env, char *server_conf, int mode,
			const LeaFile & lf, int startat, int position = 0);

	virtual	~LeaSession ();

	int	isValid () { return s_valid; }
	operator OpsecSession * () { return s_session; }

	static int	fieldCount () { return s_fieldCount; }
	static char	*fieldName (LeaFieldID id) { return s_fieldNames[id]; }
	int	fieldAttr (LeaFieldID id) { return s_attrib[id]; }
	int	position () { return s_position; }
	void	deleteOnEnd (int t) { s_deleteOnEnd = t; }
	int	deleteOnEnd () { return s_deleteOnEnd; }

	virtual void	endHandler () {}

	virtual int	recordHandler (LeaRecord *record) = 0;
	virtual int	dictionaryHandler ( int dict_id, LEA_VT val_type,
			int n_d_entries) { return OPSEC_SESSION_OK; }
	virtual int	switchHandler () { return OPSEC_SESSION_OK; }
	virtual int	eofHandler () { return OPSEC_SESSION_OK; }
};

class LeaRecord {
	friend class LeaSession;

	LeaSession	*r_session;
	lea_record	*r_record;
	int	*r_attrib_perm;

	LeaRecord (LeaSession *ses, lea_record *rec, int attrib_perm[]) :
			r_session(ses), r_record(rec),
			r_attrib_perm(attrib_perm) {}
public :
	int	fieldIndex (LeaSession::LeaFieldID id);
	char	*fieldStr (LeaSession::LeaFieldID id);
	char	*fieldRawStr (LeaSession::LeaFieldID id);
	lea_field	*getField (LeaSession::LeaFieldID id);

    int fieldInt ( LeaSession::LeaFieldID id, int *value );
};


char	*formatField (LeaSession::LeaFieldID id, LeaRecord *record,
		char *title = 0, char sep = '=', char delim = ';');
char	*formatRawField (LeaSession::LeaFieldID id, LeaRecord *record,
		char *title = 0, char sep = '=', char delim = ';');

#endif

