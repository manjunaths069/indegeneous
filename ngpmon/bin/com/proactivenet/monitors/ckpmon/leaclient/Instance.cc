
#include "Instance.h"


#include <strings.h>
#include "intoa.h"
#include "const.h"
#include "stdlog.h"
#include <rw/ctoken.h>
#include <rw/cstring.h>
#include <rw/tvhdict.h>

debug

static RWTValHashDictionary<RWCString, int> *mProtocolNumbers; // ( RWCString::hash );
static int isProtocolHashInitialized = 0;

Instance::Instance ( IpcMsg *instmsg )
{
_(Instance)

    mInstMsg = instmsg;
    mInstId = mInstMsg->readInt ( CKP_INSTID );
    mAccepted = 0;
    mDropped = 0;
    mRejected = 0;
    mEncrypted = 0;
    mAuthcrypted = 0;
    mAuthorized = 0;
    mDeauthorized = 0;
    mDecrypted = 0;
    mKeyinstalled = 0;

    mConnections = 0;
    gettimeofday ( &mFromTime, NULL );

}

Instance::~Instance ()
{

    if ( mInstMsg != NULL )
        delete mInstMsg;
    if ( mProtocolNumbers )
        delete mProtocolNumbers;
}

void Instance::resetFromTime ()
{
    gettimeofday ( &mFromTime, NULL );
}

void Instance::resetCounters ()
{
    mAccepted = 0;
    mDropped = 0;
    mRejected = 0;
    mEncrypted = 0;
    mAuthcrypted = 0;
    mAuthorized = 0;
    mDeauthorized = 0;
    mDecrypted = 0;
    mKeyinstalled = 0;

    mConnections = 0;
}

int Instance::createFilter ()
{
_(createFilter)

    int cnt = 0;

    DEBUG (( " filter for  %d ", mInstId ));
    LLOG (( " filter for  %d ", mInstId ));

    //RWCTokenizer *t;
    RWCString token;
    char *field;

    if ( ( field = mInstMsg->readStr ( CKP_IFNAME )) != NULL ) 
    {
        RWCTokenizer t = RWCTokenizer ( RWCString ( field ) );
        while ( !(token=t()).isNull() )  
        {
            mIfNames.append ( token );
            DEBUG (( "ifname %s ", token.data() ));
            LLOG (( "ifname %s ", token.data() ));
        }
    }

    if ( ( field = mInstMsg->readStr ( CKP_ORIGIN )) != NULL )
    {
        RWCTokenizer t = RWCTokenizer ( RWCString ( field ) );
        while ( !(token=t()).isNull() )
        {
            mOrigins.append ( token );
            DEBUG (( "origin %s ", token.data () ));
            LLOG (( "origin %s ", token.data () ));
        }
    }

    if ( ( field = mInstMsg->readStr ( CKP_SRCIP )) != NULL )
    {
        RWCTokenizer t = RWCTokenizer ( RWCString ( mInstMsg->readStr ( CKP_SRCIP ) ) );
        while ( !(token=t()).isNull() )
        {
            mSrcIps.append ( token );
            DEBUG (( "srcip %s ", token.data() ));
            LLOG (( "srcip %s ", token.data() ));
        }
    }

    if ( ( field = mInstMsg->readStr ( CKP_DESTIP )) != NULL )
    { 
        RWCTokenizer t = RWCTokenizer ( RWCString ( mInstMsg->readStr ( CKP_DESTIP ) ) );
        while ( !(token=t()).isNull() )
        {
            mDestIps.append ( token );
            DEBUG (( "destip %s ", token.data() ));
            LLOG (( "destip %s ", token.data() ));
        }
    }

    if ( ( field = mInstMsg->readStr ( CKP_RESOURCE)) != NULL )
    {
        RWCTokenizer t = RWCTokenizer ( RWCString ( mInstMsg->readStr ( CKP_RESOURCE ) ) );
        while ( !(token=t()).isNull() )
        {
            mResources.append ( token );
            DEBUG (( "ifname %s ", token.data() ));
            LLOG (( "ifname %s ", token.data() ));
        }
    }

    if ( ( field = mInstMsg->readStr ( CKP_PROTOCOL )) != NULL )
    {
         RWCTokenizer t = RWCTokenizer ( RWCString ( mInstMsg->readStr ( CKP_PROTOCOL ) ) );
         while ( !(token=t()).isNull() )
         {

             if ( token == "-1" )
                 mProtocols.append ( "All" );
             else
                 mProtocols.append ( token );
             DEBUG (( "protocol %s ", token.data() ));
             LLOG (( "protocol %s ", token.data() ));

/*
             if ( token == "ALL"  || token == "-1" )
             {
                 mProtocols.append ( "All" );
                 DEBUG (( "protocol All" ));
                 LLOG (( "protocol All" ));
             } 
             else if ( mProtocolNumbers->contains ( token ) )
             {
                 mProtocols.append ( (*mProtocolNumbers) [ token ] ); 
                 DEBUG (( "protocol %s", token.data() ));
                 LLOG (( "protocol %s", token.data() ));
             }
             else
             {
                 LOGERROR (( "Unknown protocol %s " , token.data () ));
                 return FALSE;
             }
*/
         }
    }

    if ( ( field = mInstMsg->readStr ( CKP_RULENO )) != NULL )
    { 
        RWCTokenizer t = RWCTokenizer ( RWCString ( mInstMsg->readStr ( CKP_RULENO ) ) );
        while ( !(token=t()).isNull() )
        {
            if ( token == "All" || token == "ALL" || token == "-1" )
                mRuleNos.append ( -1 );
            else
                mRuleNos.append ( atoi ( token.data () ) );
            DEBUG (( "rule number %s ", token.data() ));
            LLOG (( "rule number %s ", token.data() ));
        }
    }
    return TRUE;
}

int Instance::match ( char *record )
{
    return 1;
}
