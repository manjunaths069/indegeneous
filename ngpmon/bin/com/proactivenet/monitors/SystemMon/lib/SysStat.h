#ifndef _SYSSTAT_H__
#define _SYSSTAT_H__
#include <unistd.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <kstat.h>
#include <jni.h>
//#include <iostream.h>
//#include <stdlib.h>
// Added three new attributes - Rao  - 09/27/2K
// Number of Runnable processes
// Number of processes Swapped
// Number of processes waiting for I/O
// This file  is similar to VMStat.h

class SysStat
{
        private :
                char * mErrorMessage;
                ulong mPrevRunque;
                ulong mRunque;
                float mAveRunque;
                ulong mPrevSwpque;
                ulong mSwpque;
                float mAveSwpque;
                ulong mPrevWaiting;
                ulong mWaiting;
                float mAveWaiting;
                ulong mPrevUpdates;
                ulong mUpdates;
        public :
                SysStat();
                int record( kstat_ctl_t    * kc, JNIEnv *,jobject);
                void logDbg(JNIEnv *,jobject, char *);
                void logErr(JNIEnv *,jobject, char *);
                float runque() {return mAveRunque; }
                float swpque() {return mAveSwpque; }
                float waiting() {return mAveWaiting; }
                char * getErrorMessage(){return mErrorMessage;}
};
#endif
