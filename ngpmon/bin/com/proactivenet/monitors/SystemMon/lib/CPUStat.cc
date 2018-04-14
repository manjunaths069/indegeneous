#include <stdio.h>
#include "CPUStat.h"
CPUStat::CPUStat()
{
        mErrorMessage = new char[256];
        mUs = 0;
        mSy =0 ;
        mIO = 0;
        mNumberOfCPUs = 0 ;
        mRate = 0 ;
        mSr = 0 ;
        for (int i = 0 ; i < 256 ; i ++ )
                cpuArray[i] = 0 ;
}
/* --------------------- Changes for JNI logging START------------------------------*/

void CPUStat::logDbg(JNIEnv *env, jobject obj, char *logStr)
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

void CPUStat::logErr(JNIEnv *env, jobject obj, char *errStr)
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
CPUStat::init( kstat_ctl_t    * kc,JNIEnv * env, jobject obj )
{
        int i = 0 ;
        /* --------------------- Changes for JNI logging START------------------------------*/
        char mLogMessage[255];
        sprintf(mLogMessage,"Native Logging: CPUStat : function init : Start");
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        for (kstat * ksp = kc->kc_chain; ksp != NULL; ksp = ksp->ks_next)
        {
                if(ksp->ks_type == KSTAT_TYPE_RAW &&
                                                        !strcmp(ksp->ks_module,"cpu_stat") )
                {
                        SingleCPUStat * stat = new SingleCPUStat(ksp->ks_name,
                                ksp->ks_instance);
                        cpuArray[i] = stat ;
                        i++;
                }
        }
        mNumberOfCPUs = i ;
        /* --------------------- Changes for JNI logging START------------------------------*/
        sprintf(mLogMessage,"Native Logging: CPUStat : function init : Number of CPU %d. END",i);
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        return 0;
}
//Assumes that the kstat_open is called for kc just before the call of record
int
CPUStat::record( kstat_ctl_t    * kc, JNIEnv * env,jobject obj )
{
        /* --------------------- Changes for JNI logging START------------------------------*/
        char mLogMessage[1024];
        sprintf(mLogMessage,"Native Logging: CPUStat : function record : Start");
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        if ( mNumberOfCPUs == 0 ) //means initialization is not done
        {
                sprintf(mErrorMessage,"Initialization of the CPUStat class in not done"
                        " OR it was failed");
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: CPUStat : function record : %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/
                return -1;
        }
        //Ask all SingleCPU object to call record
        for (int i =0 ; i < mNumberOfCPUs; i++ )
        {
                int ret = cpuArray[i]->record(kc) ;
                if (ret != 0 ) //Error case
                {
                        sprintf(mErrorMessage,cpuArray[i]->getErrorMessage());
                        /* --------------------- Changes for JNI logging START------------------------------*/
                        sprintf(mLogMessage,"Native Logging: CPUStat : function record : %s",mErrorMessage);
                        logErr(env,obj,mLogMessage);
                        /* --------------------- Changes for JNI logging END------------------------------*/
                        return ret ;
                }
        }
        //Computes the CPU
        float us = 0 ;
        float sy = 0 ;
        float io = 0 ;
        long tempSum = 0;
        long sum = 0 ;
        float rate = 0 ;
        float sr = 0 ;
        for (i = 0 ; i < mNumberOfCPUs; i++ )
        {
                long tempSum = cpuArray[i]->getCPUCouterSum() ;
                //cpuArray[i]->print() ;
                if ( tempSum == - 1) //First time
                {
                        mUs = -1 ;
                        mSy = -1 ;
                        mIO = -1 ;
                        /* --------------------- Changes for JNI logging START------------------------------*/
                        sprintf(mLogMessage,"Native Logging: CPUStat : function record : getCPUCounterSum function failed");
                        logDbg(env,obj,mLogMessage);
                        /* --------------------- Changes for JNI logging END------------------------------*/

                        return 0 ;
                }
                sum += tempSum ;
                us  += (cpuArray[i]->us() * tempSum);
                sy  += (cpuArray[i]->sy() * tempSum);
                io  += (cpuArray[i]->io() * tempSum);
                rate += cpuArray[i]->rate( );
                sr += cpuArray[i]->sr() ;
        }
        mUs = us / sum ;
        mSy = sy / sum ;
        mIO = io / sum ;
        mRate = rate ;
        mSr = sr ;
        /* --------------------- Changes for JNI logging START------------------------------*/
        sprintf(mLogMessage,"Native Logging: CPUStat : function record : END");
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        return 0 ;
}
void
CPUStat::print()
{
        printf("%0.2f   %0.2f   %0.2f %0.2f  %0.2f\n",mUs,mSy,mIO,mRate,mSr);
}

void
CPUStat::clean_up()
{
        for (int i =0 ; i < mNumberOfCPUs; i++ )
        {
                if(cpuArray[i] != 0)
                {
                        if(cpuArray[i] != NULL)
                        {
                                delete cpuArray[i];
                                cpuArray[i] = NULL;
                        }
                }
        }

}
