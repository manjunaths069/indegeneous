#include <sys/time.h>
#include <memory.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <strings.h>
#include <wait.h>

#include "apps/include/app_fw.h"
#include "apps/ipclib/app_ipc.h"

#include "LeaClient.h"
#include "opsec/opsec_error.h"

#include "intoa.h"
#include "const.h"
#include "stdlog.h"
debug

#ifdef __cplusplus
extern "C" {
#endif
time_t mktime (struct tm *);
struct tm *localtime_r(const time_t *, struct tm *);
#ifdef __cplusplus
}
#endif

unsigned hash (const unsigned & instid) { return instid; };

int readCkpmon ( int event, void *data )
{
_( readCkpmon )

    LeaClient *lc = ( LeaClient * )data;

    if ( opsec_errno != EO_OK )
    {
        //LOG (( "%s", opsec_errno_str ( opsec_errno ) ));
        LLOG (( "%s", opsec_errno_str ( opsec_errno ) ));
        if ( opsec_errno == EO_SYS_SOCK )
        {
            //LOG (( "%s", opsec_errno_str ( opsec_errno ) ));
            LLOG (( "%s", opsec_errno_str ( opsec_errno ) ));
            lc->exitLeaClient ();
        }
    }

    MON_MSG m;
    int msgtype = recv_next_msg ( lc->mCkpIpcid, &m );
    //LOG (( "**** received msgtype %d msg: %s ", msgtype ));
    LLOG (( "**** received msgtype %d msg: %s ", msgtype ));

    if ( msgtype == ERROR )
    {
        //LOG (( "**** Error on the Agent connection , exiting ****" ));
        LLOG (( "**** Error on the Agent connection , exiting ****" ));
        exit (0);
    }
 
    IpcMsg *ipcmsg = decodeGenericMsg (&m);

    switch (msgtype)
    {
        case CKP_STATUS_MSG:
        {
            IpcMsg *msg = new IpcMsg ();

            msg->add ( ERROR_CODE, opsec_errno );
            msg->add ( ERROR_STRING, opsec_errno_str ( opsec_errno ) );
            msg->setMessageType ( CKP_STATUS_MSG );

            sndMessage ( lc->mCkpIpcid, msg, CKP_STATUS_MSG, getip () );
            delete msg;
        }
        break;

        case CKP_GET_STATS_MSG:
        {
            if ( ! lc->mLeaSessStatus )
            {
                IpcMsg *msg = new IpcMsg ();

                msg->add ( ERROR_CODE, opsec_errno );
                msg->add ( ERROR_STRING, opsec_errno_str ( opsec_errno ) );
                msg->setMessageType ( CKP_STATUS_MSG );

                sndMessage ( lc->mCkpIpcid, msg, CKP_STATUS_MSG, getip () );
                delete msg;
                //LOG (( opsec_errno_str ( opsec_errno ) ));
                LLOG (( opsec_errno_str ( opsec_errno ) ));
                //LOG (( "restarting leasession" ));
                LLOG (( "restarting leasession" ));
                lc->createLeaSession ();
            }
            else
            {
                int instid = ipcmsg->readInt ( CKP_INSTID );
                if ( !(* lc->mInstances).contains ( instid ) )
                {
                    //LOG (( "CKP_STATS_MSG instid %d not yet in the list", instid ));
                    LLOG (( "CKP_STATS_MSG instid %d not yet in the list", instid ));
                    IpcMsg *msg = new IpcMsg ();
                    msg->add ( CKP_INST_STATUS, CKPMON_INST_ABSENT );
                    msg->setMessageType ( CKP_INST_STATUS_MSG);
                    sndMessage ( lc->mCkpIpcid, msg, CKP_INST_STATUS_MSG, getip () );
                    delete msg; 
                    break;
                }
                Instance *inst = (* lc->mInstances)[ instid ];

                if ( inst != NULL )
                { 
                    //LOG (( "CKP_STATS_MSG instid %d", instid ));
                    LLOG (( "CKP_STATS_MSG instid %d", instid ));
                    struct timeval totime;
                    gettimeofday ( &totime, NULL );

                    IpcMsg *msg = new IpcMsg ();
                    msg->add ( CKP_TOTIME, totime.tv_sec );
                    msg->add ( CKP_FROMTIME, (inst->mFromTime).tv_sec );
                    inst->resetFromTime ();
                    msg->add ( CKP_ACCEPTED, inst->mAccepted );
                    msg->add ( CKP_DROPPED, inst->mDropped );
                    msg->add ( CKP_REJECTED, inst->mRejected );
                    msg->add ( CKP_ENCRYPTED, inst->mEncrypted );
                    msg->add ( CKP_AUTHCRYPTED, inst->mAuthcrypted );
                    msg->add ( CKP_AUTHORIZED, inst->mAuthorized  );
                    msg->add ( CKP_DEAUTHORIZED, inst->mDeauthorized );
                    msg->add ( CKP_DECRYPTED, inst->mDecrypted );
                    msg->add ( CKP_KEYINSTALLED, inst->mKeyinstalled );
                    msg->add ( CKP_CONNACCEPTED, inst->mConnections );
                    msg->setMessageType ( CKP_STATS_MSG );
                    sndMessage ( lc->mCkpIpcid, msg, CKP_STATS_MSG, getip () ); 

                    LLOG (( "sent stats: instid %d: A %d D %d R %d E %d AUTHCRY %d AUTH %d DEAUTH %d DECRYP %d KI %d CONNS %d", 
                          instid, inst->mAccepted, inst->mDropped, inst->mRejected, inst->mEncrypted, 
                          inst->mAuthcrypted, inst->mAuthorized, inst->mDeauthorized, inst->mDecrypted,
                          inst->mKeyinstalled, inst->mConnections ));
                    inst->resetCounters ();
                    delete msg;
                }
            } 
        }
        break;

        case CKP_INSTANCE_MSG:
        {
            //LOG (( "CKP_INSTANCE_MSG instid %d", ipcmsg->readInt ( CKP_INSTID ) ));
            LLOG (( "CKP_INSTANCE_MSG instid %d", ipcmsg->readInt ( CKP_INSTID ) ));
            int instid = ipcmsg->readInt ( CKP_INSTID );

            IpcMsg *msg = new IpcMsg ();
            msg->setMessageType ( CKP_INST_STATUS_MSG );

            if ( (lc->mInstances)->contains ( instid ) )
            {
                msg->add ( CKP_INST_STATUS, CKPMON_INST_PRESENT );
                sndMessage ( lc->mCkpIpcid, msg, CKPMON_INST_PRESENT, getip () );
                //LOG (( "%d already present", instid ));
                LLOG (( "%d already present", instid ));
            } 
            else
            {
                Instance *inst = new Instance ( ipcmsg );
                if ( inst->createFilter () == TRUE )
                {
                    (lc->mInstances)->insertKeyAndValue ( ipcmsg->readInt ( CKP_INSTID ), inst );
                    msg->add ( CKP_INST_STATUS, CKPMON_INST_PRESENT );
                }
                else
                {
                    //LOG (( "Unable to create instance" ));
                    LLOG (( "Unable to create instance" ));
                    msg->add ( CKP_INST_STATUS, CKPMON_INST_FAILED );
                }
                sndMessage ( lc->mCkpIpcid, msg, CKP_INST_STATUS_MSG, getip () );
            }
            delete msg;

        }
        break;

        case CKP_DELETEINST_MSG:
        {
            int instid = ipcmsg->readInt ( CKP_INSTID );
            if ( ( lc->mInstances )->remove ( instid ) )
            {
                //LOG (( "removed inst %d", instid ));
                LLOG (( "removed inst %d", instid ));
            }
            else
            {
                //LOG (( "inst %d not found in list", instid ));
                LLOG (( "inst %d not found in list", instid ));
            }

            if ( ( lc->mInstances )->isEmpty () )
            {
                IpcMsg *msg = new IpcMsg ();
                msg->setMessageType ( CKP_EXIT_MSG );
                sndMessage ( lc->mCkpIpcid, msg, CKP_EXIT_MSG, getip () );
                delete msg;
                lc->exitLeaClient ();
            }
            else
            {
                IpcMsg *msg = new IpcMsg ();

                msg->add ( ERROR_CODE, opsec_errno );
                msg->add ( ERROR_STRING, opsec_errno_str ( opsec_errno ) );
                msg->setMessageType ( CKP_STATUS_MSG );

                sndMessage ( lc->mCkpIpcid, msg, CKP_STATUS_MSG, getip () );
                delete msg;
            }
        } 
        break;

        case CKP_EXIT_MSG:
            lc->exitLeaClient ();
            break;

        default:
            //LOG (( "unknown message type %d", msgtype ));
            LLOG (( "unknown message type %d", msgtype ));
            break;
    }
 
    if ( ipcmsg != NULL )
         delete ipcmsg;

    return TRUE;
}

LeaClient::LeaClient ( char *ip, char *portstr, int port )
{
    mIp = ip;
    mPortStr = portstr;
    mPort = port;
    mLeaSessStatus = 0;
    mInstances = new RWTValHashDictionary <unsigned  , Instance * > (hash);
}

LeaClient::~LeaClient ()
{

    unsigned key;
    Instance *inst;
    RWTValHashDictionaryIterator<unsigned, Instance*> itr(*mInstances);
    while ( key = itr () )
    {
        inst =  itr.value();
        delete inst;
    }

}


int LeaClient::startLeaClient ( int *argc, char **argv )
{
_( startLeaClient )

    mEnv = opsec_init ( OPSEC_CONF_ARGV, argc, argv, OPSEC_EOL );
    if ( mEnv == 0 )
    {
         LOGERROR (( "failed to initialize OPSEC environment" ));
         LOGERROR (( "opsec_errno %d, opsec_errno_str %s", opsec_errno, opsec_errno_str ( opsec_errno ) ));
         exit ( 1 );
    }

    createLeaSession ();
    mCkpIpcid = connect_to_remote_proc ( getip (), atoi ( argv[2] ), TRUE );
    opsec_set_socket_event ( mEnv, OPSEC_SK_INPUT, mCkpIpcid, readCkpmon, (void *)this );

    opsec_mainloop ( mEnv );
    return 0;
 

}

int LeaClient::createLeaSession ()
{
_( createLeaSession )


    LeaSession  *mNormalSession = new LeaFiltSession ( &mLeaSessStatus, mInstances, mEnv, "lea_server", LEA_ONLINE,
                                                     LeaFile(LeaFile::LOG_NORMAL, LeaFile::LOG_CURRENT),
                                                     LEA_AT_END );
    if ( !mNormalSession->isValid () )
    {
        LOGERROR (( "failed to create log LEA session" ));
        LOGERROR (( "opsec_errno %d, opsec_errno_str %s", opsec_errno, opsec_errno_str ( opsec_errno ) ));
        mLeaSessStatus = 0;
        return ERROR;
    }

    LeaSession  *mAccountSession = new LeaFiltSession ( &mLeaSessStatus, mInstances, mEnv, "lea_server", LEA_ONLINE,
                                                     LeaFile(LeaFile::LOG_ACCOUNT, LeaFile::LOG_CURRENT),
                                                     LEA_AT_END );
    if ( !mAccountSession->isValid () )
    {
        LOGERROR (( "failed to create log LEA session" ));
        LOGERROR (( "opsec_errno %d, opsec_errno_str %s", opsec_errno, opsec_errno_str ( opsec_errno ) ));
        mLeaSessStatus = 0;
        return ERROR;
    }

    mLeaSessStatus = 1;
    //LOG (( "mLeaSessStatus %d", mLeaSessStatus ));
    LLOG (( "mLeaSessStatus %d", mLeaSessStatus ));
    mAccountSession->deleteOnEnd(1);
    mNormalSession->deleteOnEnd(1);
    return TRUE;
}

void LeaClient::exitLeaClient ()
{
_( exitLeaClient )

    //LOG (( "** Leaclient exiting **" ));
    LLOG (( "** Leaclient exiting **" ));
    exit ( 1 );
}

void InitDebugSettings(char *longexecname, char *pronethomedir)
{
_(InitDebugSettings)

    char debugsettingfile[256], debugoutputfile[256], logoutputfile[256], execname[256] ; 
    char debug_file_dir[256], log_file_dir[256];
    
    char  pronethome [256];
	strcpy(pronethome, pronethomedir);
    
   	sprintf (debugsettingfile, "%s/%s\0", pronethome , "monitors/monitors/ckpmon/leaclient_debug");
   	//printf ("log file : %s \n" , debugsettingfile ); 

    //printf ("before - reading leaclient_debug\n"); 
    ReadDebugFile (debugsettingfile);
    //printf ("after - reading leaclient_debug\n"); 
    
    {
        char  buf [256];
        const char *ptr; int i;
        ptr = longexecname;
        strcpy(execname, "?\0");
        if (ptr)
        {
            strcpy(buf, ptr);
            for (i=strlen(buf)-1; i>=0; i--)
            {
                if (buf[i]=='/') break; else ptr = &buf[i];
            }
            strcpy (execname, ptr);
        }
    }

	sprintf (debug_file_dir, "%s/%s\0", pronethome , "pronto/tmp");
	sprintf (log_file_dir, "%s/%s\0", pronethome , "pronto/logs");

    //printf ("%s\n", debug_file_dir); 
    //printf ("%s\n", log_file_dir); 

    sprintf (debugoutputfile, "%s/sm_%s_%d.debug\0", debug_file_dir, execname, getpid());
    sprintf (logoutputfile,   "%s/sm_%s_%d.log\0",   log_file_dir,   execname, getpid());
    
    //printf ("%s\n", debugoutputfile); 
	//printf ("%s\n", logoutputfile); 

    SetDebugFile (debugoutputfile, logoutputfile, execname);

    LOG(("========================= %s ========================= \n", execname));
    LOG(("===== DEBUG: %s ===== \n", debugoutputfile));
    LOG(("===== LOG  : %s ===== \n", logoutputfile));

}

// Input format: 
// </pnp/pw> 12124 -v lea_server ip <IP> -v lea_server auth_port <18184> -v lea_server auth_type auth_opsec ... etc 

main ( int argc, char **argv )
{
	//printf ("before main \n"); 
_(main)
	//printf ("after _main \n"); 
    InitDebugSettings ( argv[0] , argv[1]);
    
    LLOG (( "%d %s %s %s", argc, argv[2], argv[1], argv[6] ));

    if ( argc < 10 )
    {
        LOG (( "Not enough args" ));
        exit (0);
    }

    char *ip = argv[6];
    char *portstr = argv[9];
    int port = atoi ( argv[10] );

	//printf ( "%s %s %d \n" , ip, portstr, port  ) ;
	LeaClient *leac = new LeaClient ( ip, portstr, port );
    
    char **argument = (char**)( malloc(  sizeof(char*) * (argc)) ) ;
    
    /*
    for (int count = 0 ; count < argc ; count++)
    {
		printf ( "%s\n" , argv[count] ) ;
	}
	*/

    leac->startLeaClient ( &argc, argv );
    return 0;
}

