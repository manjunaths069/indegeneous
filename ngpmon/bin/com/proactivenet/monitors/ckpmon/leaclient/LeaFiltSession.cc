#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "opsec/lea.h"
#include "opsec/opsec.h"

#include "LeaFiltSession.h"

#include "stdc.h"
debug


LeaFiltSession::LeaFiltSession ( int *sessstatus, RWTValHashDictionary < unsigned, Instance *> *insts,
                                 OpsecEnv *env, char *server_conf, int mode,
                                 const LeaFile & lf, int startat, int position ) : 
                               LeaSession ( env, server_conf, mode,lf, startat, position )
{
    mSessStatus = sessstatus;
    mInstances = insts;
}

LeaFiltSession::~LeaFiltSession ()
{}

int LeaFiltSession::recordHandler (LeaRecord *record)
{
_(recordHandler)
  
    char *buf;
    RWCString src_raw;
    RWCString dest_raw;
    RWCString ifname;
    RWCString ifname_raw;
    RWCString origin_raw;

    RWCString all ( "All" );

    ifname = RWCString( formatField( IF_NAME_ID, record, "" ) );

    buf = record->fieldRawStr ( SRC_ID );
    if ( buf != NULL ) 
        src_raw = RWCString ( buf );   // raw value gives the ip address 
    else
        DEBUG (( "SRC_ID is null" ));

    buf = record->fieldRawStr ( DST_ID );
    if ( buf != NULL )
        dest_raw = RWCString ( buf );
    else
        DEBUG (( "DST_ID is null" ));

    buf = record->fieldRawStr ( ORIGIN_ID );
    if ( buf != NULL )
        origin_raw = RWCString ( buf );
    else
        DEBUG (( "ORIGIN_ID is null" ));

    
    RWCString proto ( formatField( PROTOCOL_ID, record, "" ) );
    int       ruleno = atoi ( formatField( RULE_ID, record, "" ) );
    RWCString resource ( formatField( RESOURCE_ID, record, "" ) );

    DEBUG (( "\n++ RECORD received ++" ));

    DEBUG (( "raw origin %s", origin_raw.data() ));
    DEBUG (( "ifname %s", ifname.data() ));
    DEBUG (( "raw src %s", src_raw.data() ));
    DEBUG (( "raw dest %s", dest_raw.data() ));
    DEBUG (( "proto %s", proto.data() ));
    DEBUG (( "ruleno %d", ruleno ));
    DEBUG (( "resource %s", resource.data() ));

    RWTValHashDictionaryIterator< unsigned, Instance * > itr( *mInstances );

    while(itr())
    {
        Instance *inst = itr.value();

        DEBUG (("?? Matching instance %d ", inst->mInstId ));

        if ( inst == NULL )
        {
            DEBUG (( "Null instance in the hash" ));
            continue;
        }

        if ( (inst->mIfNames).contains ( all ) == FALSE && (inst->mIfNames).contains ( ifname ) == FALSE ) 
        {
            DEBUG (("ifname not found"));
            continue;
        }
        if ( (inst->mOrigins).contains ( all ) == FALSE && (inst->mOrigins).contains ( origin_raw ) == FALSE ) 
        {
            DEBUG (("origin not found"));
            continue;
        }

        if ( (inst->mSrcIps).contains ( all ) == FALSE && (inst->mSrcIps).contains ( src_raw ) == FALSE ) 
        {
            DEBUG (("src not found"));
            continue;
        }

        if ( (inst->mDestIps).contains ( all ) == FALSE && (inst->mDestIps).contains ( dest_raw ) == FALSE ) 
        {
            DEBUG (("dest not found"));
            continue;
        }

        if ( (inst->mProtocols).contains ( all ) == FALSE && (inst->mProtocols).contains ( proto ) == FALSE ) 
        {
            DEBUG (("proto not found"));
            continue;
        }

        if ( (inst->mRuleNos).contains ( -1 ) == FALSE && (inst->mRuleNos).contains ( ruleno ) == FALSE )
        {
            DEBUG (("ruleno not found"));
            continue;
        }

        if ( (inst->mResources).contains ( all ) == FALSE && (inst->mResources).contains ( resource ) == FALSE ) 
        {
            DEBUG (("resource not found"));
            continue;
        }


        DEBUG (("!!! Matched instance %d ", inst->mInstId ));

        char *action = formatField( ACTION_ID, record, "" );
        int packets = 0;

        if ( s_lealogtype == LeaFile::LOG_ACCOUNT )
            packets = record->fieldInt ( PACKETS_ID, &packets );
        else
            packets = 1;


        if ( strcmp ( action, "accept" ) == 0 )
            inst->mAccepted += packets;
        else if ( strcmp ( action, "reject" ) == 0 )
            inst->mRejected += packets;
        else if ( strcmp ( action, "drop" ) == 0 )
            inst->mDropped += packets;
        else if ( strcmp ( action, "encrypt" ) == 0 )
            inst->mEncrypted += packets;
        else if ( strcmp ( action, "authcrypt" ) == 0 )
            inst->mAuthcrypted += packets;
        else if ( strcmp ( action, "authorize" ) == 0 )
            inst->mAuthorized += packets;
        else if ( strcmp ( action, "decrypt" ) == 0 )
            inst->mDecrypted += packets;
        else if ( strcmp ( action, "keyinst" ) == 0 )
            inst->mKeyinstalled += packets;
        else if ( strcmp ( action, "deauthorize" ) == 0 )
            inst->mDeauthorized += packets;
        else
            LOGERROR (("unknown action %s", action ));

        inst->mConnections++; 
    }

    return OPSEC_SESSION_OK;
}

int LeaFiltSession::eofHandler ()
{
    lea_logdesc *desc;
/*
    if ((desc = lea_get_logfile_desc(*this)) != 0) {
        fprintf(stderr, "Note: logfile EOF reached\n");
        fprintf(stderr, "Note: Filename: %s, Fileid: %ld\n",
                desc->filename, desc->fileid);
    }
*/
    return OPSEC_SESSION_OK;
}

int LeaFiltSession::switchHandler ()
{
    lea_logdesc *desc;

    if ((desc = lea_get_logfile_desc(*this)) != 0) {
        fprintf(stderr, "Note: logfile switched\n");
        fprintf(stderr, "Note: Filename: %s, Fileid: %ld\n",
                desc->filename, desc->fileid);
    }
    return OPSEC_SESSION_OK;
}

void    LeaFiltSession::endHandler ()
{
_(endHandler)

    *mSessStatus = 0;
    LLOG (("Received session end event, sessStatus %d", *mSessStatus ));
    fprintf(stderr, "Note: session terminated\n");
}

