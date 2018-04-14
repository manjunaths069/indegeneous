#ifndef _PROC_INFO_MANAGER_H__
#define _PROC_INFO_MANAGER_H__
#include <iostream.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/param.h>  /* for HZ -- should be in <time.h> ? */
#include <sys/time.h>
#include <sys/types.h>
#include <procfs.h>
//#include <sys/procfs.h>
#include <dirent.h>
#include <rw/cstring.h>
#include <rw/tphdict.h>
#include <rw/tvslist.h>
#include "ProcInfo.h"
class ProcInfoManager
{
        private  :
                RWTPtrHashDictionary<int,ProcInfo> mProcIdDict ;
        public :
                static unsigned hash ( const int & i)
                {return i; }
                ProcInfoManager() : mProcIdDict(hash)
                {}
                // Modified by Ashish, now inserting the user name also
                //int add(int version, char * procName, char * commandLineMatch, JNIEnv *,jobject);
#ifndef  _STANDALONE_
                int add(int version, const char *, const char *, const char *, JNIEnv *,jobject);
                char * execute (JNIEnv *env, jobject obj, int id , int polltype);
                float get (int id, int infoId, JNIEnv *,jobject);
                char *  getErrorMsg (int id);
                void logDbg(JNIEnv *,jobject, char *);
                void logErr(JNIEnv *,jobject, char *);
                // Introduced by Ashish to support the polling at user level
                int executeAtUserLevel ( JNIEnv *env, jobject obj, int id , int polltype, int, RWCString procIds[]);

                int addForRegExp(int version, JNIEnv *env, jobject obj);
                int executeForRegExp(JNIEnv *env, jobject obj, int id, int polltype, jobjectArray procs, RWCString procIds[]);
#else
                int add(int version, const char *, const char *, const char *);
                char * execute (int id , int polltype);
        float get (int id, int infoId);
                char *  getErrorMsg (int id);
                void logDbg(char *);
                void logErr(char *);
                // Introduced by Ashish to support the polling at user level
                int executeAtUserLevel (int id , int polltype, int proc_id_size, RWCString procIds[]);

                int addForRegExp(int version);
        //int executeForRegExp(int id, int polltype, jobjectArray procs, RWCString procIds[]);
                int executeForRegExp(int id, int polltype, char **procs, int procslength, RWCString procIds[]);
#endif
                void remove (int id );



};
#endif// End ProcInfoManager class


