#ifndef LEA_CLIENT_H
#define LEA_CLIENT_H

#include <rw/tvhdict.h>

#include "LeaFiltSession.h"
#include "Instance.h"

class LeaClient
{

  public:

    LeaClient ( char *ip, char *port, int authflag );
    ~LeaClient ();

    int startLeaClient ( int *argc, char **argv ); 
    int createLeaSession ();
    void exitLeaClient ();

    int mLeaSessStatus;
    int  mCkpIpcid;
    RWTValHashDictionary < unsigned, Instance *> *mInstances;

  private:

    //int readCkpmon ( int event, void *data );
    char *mIp;
    char *mPortStr;
    int  mPort;
    OpsecEnv     *mEnv;
    OpsecSession *mSession;

};

#endif // LEA_CLIENT_H
