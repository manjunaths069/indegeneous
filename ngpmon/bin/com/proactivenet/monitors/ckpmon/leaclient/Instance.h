
#ifndef INSTANCE_H
#define INSTANCE_H

#include "apps/ipclib/app_ipc.h"
#include <sys/time.h>
#include <rw/tvslist.h>
#include <rw/cstring.h>
//#include <rw/tvhdict.h>

class Instance
{

   public:


       static  unsigned hash (const Instance & inst) { return inst.mInstId; };

       unsigned mAccepted;
       unsigned mRejected;
       unsigned mDropped;
       unsigned mEncrypted;
       unsigned mAuthcrypted;
       unsigned mAuthorized;
       unsigned mDeauthorized;
       unsigned mDecrypted;
       unsigned mKeyinstalled;

       unsigned mConnections;
       struct timeval mFromTime;

       IpcMsg *mInstMsg;

       int mInstId;

/*
       char   *mIfName;
       char   *mOrigin;
       char   *mSrcIp;
       char   *mDestIp;
       int    mProtocol;
       int    mRuleNo;
       char   *mResource;
*/

       //RWTValHashDictionary<RWCString, int> mProtocolNumbers; 

       RWTValSlist <RWCString> mIfNames;
       RWTValSlist <RWCString> mOrigins;
       RWTValSlist <RWCString> mSrcIps;
       RWTValSlist <RWCString> mDestIps;
       RWTValSlist <RWCString> mProtocols;
       RWTValSlist <int> mRuleNos;
       RWTValSlist <RWCString> mResources;
       
       int match ( char *record );
       void resetCounters ();
       void resetFromTime ();

       Instance ( IpcMsg *instmsg );
       ~Instance ();

       int createFilter ();

  private:
       void readProtocolNumbers ();
       char matchStr [512];

};

#endif // INSTANCE_H
