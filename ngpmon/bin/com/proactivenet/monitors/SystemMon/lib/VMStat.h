#ifndef _VMSTAT_CPU_H__
#define _VMSTAT_CPU_H__
#include <unistd.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <kstat.h>
#include <jni.h>
//#include <iostream.h>
//#include <stdlib.h>
class VMStat
{
        private :
                longlong_t mPageSize;
                longlong_t mTotalMemeory;
                longlong_t mTotalSwap;
                float mPercentMemUsed;
                float mSwapUsed;
                time_t mPrevTime ;
                time_t mRecordTime ;
                char * mErrorMessage;
                longlong_t mPrevSwapResv;
                longlong_t mPrevSwapAvail;
                longlong_t mPrevFreemem;
                longlong_t mSwapResv;
                longlong_t mSwapAvail;
                longlong_t mFreemem;
        public :
                VMStat();
                int record( kstat_ctl_t    * kc,JNIEnv *,jobject );
                void logDbg(JNIEnv *,jobject,char *);
                void logErr(JNIEnv *,jobject,char *);
                float mem() {return mPercentMemUsed; }
                float swap() {return mSwapUsed; }
                long memTotal() {return mTotalMemeory /1024/1024; }
                long swapTotal() {return mTotalSwap /1024/1024; }
                char * getErrorMessage(){return mErrorMessage;}
};
#endif
