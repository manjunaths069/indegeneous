#ifndef _MISCSTAT_CPU_H__
#define _MISCSTAT_CPU_H__
#include <unistd.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <kstat.h>
#include <stdlib.h>
#include <iostream.h>
#include <jni.h>
class MiscStat
{
        private :
                long mBootTime;
                long mNumProcs ;
                float mSysload15;
                float mSysload1;
                float mSysload5;
                char * mErrorMessage;
        public :
                MiscStat();
                void logDbg(JNIEnv *,jobject obj,char *);
                void logErr(JNIEnv *,jobject obj,char *);
                int record( kstat_ctl_t    * kc, JNIEnv *,jobject);
                float sysload15() {return mSysload15; }
                float sysload1() {return mSysload1; }
                float sysload5() {return mSysload5; }
                long bootTime() {return mBootTime; }
                float getNumOfProcesses ()

                { return mNumProcs ; }
                void print();
                char * getErrorMessage(){return mErrorMessage;}
};
#endif
