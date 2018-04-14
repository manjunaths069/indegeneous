#include <stdio.h>
#include "SysStat.h"
// Added three new attributes - Rao  - 09/27/2K
// Number of Runnable processes
// Number of processes Swapped
// Number of processes waiting for I/O
// This file  is similar to VMStat.h

SysStat::SysStat()
{
        mErrorMessage = new char[256];
        mPrevRunque = 0 ;
        mRunque = 0 ;
        mAveRunque = 0 ;
        mPrevSwpque = 0 ;
        mSwpque = 0 ;
        mAveSwpque = 0 ;
        mPrevWaiting = 0 ;
        mWaiting = 0 ;
        mAveWaiting = 0 ;
}
/* --------------------- Changes for JNI logging START------------------------------*/

void SysStat::logDbg(JNIEnv *env, jobject obj, char *logStr)
{
        if(logStr == NULL)
                return;
        char mlogStr[2048];
        strcpy(mlogStr,logStr);
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL)
        {
              mid = env->GetMethodID(cls,"logDbg","(Ljava/lang/String;)V");
              if(mid != NULL)
              {
                      jstring tString = env->NewStringUTF(mlogStr);
                      env->CallVoidMethod(obj,mid,tString);
                      env->DeleteLocalRef(tString);
              }
        }
}

void SysStat::logErr(JNIEnv *env, jobject obj, char *errStr)
{
        if(errStr == NULL)
                return;
        char merrStr[2048];
        strcpy(merrStr,errStr);
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL)
        {
              mid = env->GetMethodID(cls,"logErr","(Ljava/lang/String;)V");
              if(mid != NULL)
              {
                      jstring tString = env->NewStringUTF(merrStr);
                      env->CallVoidMethod(obj,mid,tString);
                      env->DeleteLocalRef(tString);
              }
        }
}

/* --------------------- Changes for JNI logging END------------------------------*/

//Assumes that the kstat_open is called for kc just before the call of record
int
SysStat::record( kstat_ctl_t    * kc, JNIEnv * env, jobject obj )
{
        kstat_t *k_space;
        void *val_ptr;
        /* --------------------- Changes for JNI logging START------------------------------*/
        char mLogMessage[2048];
        sprintf(mLogMessage,"Native Logging: SysStat: function record: Start");
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        k_space = kstat_lookup(kc,"unix",0,"sysinfo");
        if (k_space == NULL)
        {
                sprintf(mErrorMessage,"kstat lookup failed for module name = unix "
                                                          "and stat name = sysinfo." );

                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: SysStat: function record: %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/

                return -1;
        }
        //Difference in the statistics
        if (kstat_read(kc,k_space,NULL) == -1 )
        {
                sprintf(mErrorMessage,"kstat read failed for module name = unix "
                                                          "and stat name = sysinfo." );
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: SysStat: function record: %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/

                return -1;
        }
        if ( k_space->ks_data ==  NULL )
        {
                sprintf(mErrorMessage,"kstat returned null data after read "
                                                        "for module name = unix "
                                                          "and stat name = sysinfo." );
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: SysStat: function record: %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/

                return -1;
        }
        sysinfo myStat ; //Change it to pinter
        memcpy(&myStat, k_space->ks_data,sizeof(sysinfo ));
        mPrevUpdates =  mUpdates ;
        mPrevRunque =  mRunque ;
        mPrevSwpque =  mSwpque ;
        mPrevWaiting =  mWaiting ;
        mUpdates = myStat.updates ;
        mRunque = myStat.runque ;
        mSwpque = myStat.swpque ;
        mWaiting = myStat.waiting ;
        long deltaUpdates =  mUpdates - mPrevUpdates ;
        if ( !deltaUpdates )
        {
                //ignore this funciton is called in succession
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: SysStat: function record: END");
                logDbg(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/

                return 0;
        }
        ulong deltaRunque = mRunque - mPrevRunque ;
        ulong deltaSwpque = mSwpque - mPrevSwpque ;
        ulong deltaWaiting = mWaiting - mPrevWaiting ;
        if ( mPrevUpdates ) //If any of the prev != 0, means no first data point
        {
                mAveRunque = ( mRunque - mPrevRunque ) / deltaUpdates ;
                mAveSwpque = ( mSwpque - mPrevSwpque ) / deltaUpdates ;
                //mAveWaiting = ( mWaiting - mWaiting) / deltaUpdates ;
                mAveWaiting = ( mWaiting - mPrevWaiting) / deltaUpdates ; 
        }
        else
        {
                // "Fisrt data point " << endl ;
                mAveRunque = -1 ;
                mAveSwpque = -1 ;
                mAveWaiting = -1 ;
        }
        /* --------------------- Changes for JNI logging START------------------------------*/
        sprintf(mLogMessage,"Native Logging: SysStat: function record: END");
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
        return 0 ;
}
