#include <stdio.h>
#include "MiscStat.h"
MiscStat::MiscStat()
{
        mErrorMessage = new char[256];
        mBootTime = 0 ;
        mSysload15 = 0 ;
        mSysload1 = 0 ;
        mSysload5 = 0 ;
        mNumProcs = 0 ;
}

/* --------------------- Changes for JNI logging START------------------------------*/

void MiscStat::logDbg(JNIEnv *env, jobject obj, char *logStr)
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

void MiscStat::logErr(JNIEnv *env, jobject obj, char *errStr)
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
MiscStat::record( kstat_ctl_t    * kc,JNIEnv * env,jobject obj)
{
        kstat_t *k_space;
        void *val_ptr;
        /* --------------------- Changes for JNI logging START------------------------------*/
        char mLogMessage[1024];
        sprintf(mLogMessage,"Native Logging: MiscStat: function record: Start");
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        k_space = kstat_lookup(kc,"unix",0,"system_misc");
        if (k_space == NULL)
        {
                sprintf(mErrorMessage,"kstat lookup failed for module name = unix "
                                                          "and stat name = misc." );
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: MiscStat: function record: %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/

                return -1 ;
        }
        if(kstat_read(kc,k_space, NULL ) == -1 )
        {
                sprintf(mErrorMessage,"kstat read failed for module name = unix "
                                                                          "and stat name = misc." );
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: MiscStat: function record: %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/
                return -1;
        }
        if ((val_ptr = (void*)kstat_data_lookup(k_space,"avenrun_15min")) == NULL )
        {
                sprintf(mErrorMessage,"kstat data lookup error! for module name = unix "
                                                        "and stat name = misc. named stat name "                                                                         " avenrun_l5min" );
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: MiscStat: function record: %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/

                return -1 ;
        }
        mSysload15  = ((float)((kstat_named_t *) val_ptr)->value.i32)/256 ;
        if ((val_ptr = (void*)kstat_data_lookup(k_space,"avenrun_1min")) == NULL )
        {
                sprintf(mErrorMessage,"kstat data lookup error! for module name = unix "
                                                        "and stat name = misc. named stat name "                                                                         " avenrun_lmin" );
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: MiscStat: function record: %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/

                return -1 ;
        }
        mSysload1  = ((float)((kstat_named_t *) val_ptr)->value.i32)/256 ;
        if ((val_ptr = (void*)kstat_data_lookup(k_space,"avenrun_5min")) == NULL )
        {
                sprintf(mErrorMessage,"kstat data lookup error! for module name = unix "
                                                        "and stat name = misc. named stat name "                                                                         " avenrun_5min" );
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: MiscStat: function record: %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/
                return -1 ;
        }
        mSysload5  = ((float)((kstat_named_t *) val_ptr)->value.i32)/256 ;
        if ((val_ptr = (void*)kstat_data_lookup(k_space,"boot_time")) == NULL )
        {
                mBootTime = 0 ;
                //For some version for solaris we don't have stat boot time so
                // it is actually not a error, so we are not returning -1
                // The client program has to take care of it.
                // return -1 ;

        }
        else
                mBootTime = ((kstat_named_t *) val_ptr)->value.i32 ;
        if ((val_ptr = (void*)kstat_data_lookup(k_space,"nproc")) == NULL )
        {
                sprintf(mErrorMessage,"kstat data lookup error! for module name = unix "
                                                                          "and stat name = misc. named stat name "                                                                       " nproc" );
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: MiscStat: function record: %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/

                return -1 ;
        }
        mNumProcs = ((kstat_named_t *) val_ptr)->value.i32 ;
        /* --------------------- Changes for JNI logging START------------------------------*/
        sprintf(mLogMessage,"Native Logging: MiscStat: function record: END");
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        return 0 ;
}
void
MiscStat::print()
{
        /*
        cout << "\nSysload                      "  << mSysload15;
        cout << "\nBoot Time        "  << mBootTime << "\n";
        cout << "\nNumber of Procs   "  << mNumProcs << "\n";
        */
}
