
#ifndef LEA_FILTSESSION_H
#define LEA_FILTSESSION_H

#include "apps/ipclib/app_ipc.h"
#include "LeaSession.h"
#include "Instance.h"
#include <rw/tvhdict.h>

class LeaFiltSession : public LeaSession
{
public:
    LeaFiltSession ( int *sessstatus, RWTValHashDictionary < unsigned, Instance *> *insts, 
                     OpsecEnv *env, char *server_conf, int mode,
                     const LeaFile & lf, int startat, int position =0 ) ;

    ~LeaFiltSession ();

    int recordHandler (LeaRecord *record);
    int switchHandler ();
    int eofHandler ();

    void    endHandler ();
    int *mSessStatus;

    RWTValHashDictionary < unsigned, Instance *> *mInstances;

   // MON_MSG getInstStats ( int instid );


};

#endif // LEA_FILTSESSION_H
