#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <kstat.h>
#include <iostream.h>
#include <stdlib.h>
#include <thread.h>
#include "CpuMonInfo.h"
#include "CpuMon.h"
#include "CpuMonAttributeChoice.h"
#include <errno.h>
static mutex_t  itrMutex;


jfieldID mUsr, //%User CPU utilization
                mSys, //%System CPU utilization
                mIOw, //%IO Wait
                mIntrsRate, //Interrupts rate
                mSrt, //Scan rate
                mIntrThread, //Rate of intr as thread
                mSwtchRt, //Switch rate
                mInvolSwtchRt, //Involuntory switch rate
                mSysyCallRt, //System Call rate
                mXcallRt, //Cross calls to CPUs
                mCpumigrateRt; //CPU migration by threads
int minstId;
CpuMonInfo cpInfo;

/* --------------------- Changes for JNI logging START------------------------------*/

void logDbg(JNIEnv *env, jobject obj, char *logStr)
{
        if(logStr == NULL)
                return;
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL)
        {
              mid = env->GetMethodID(cls,"logDbg","(Ljava/lang/String;)V");
              if(mid != NULL)
              {
                      jstring tString = env->NewStringUTF(logStr);
                      env->CallVoidMethod(obj,mid,tString);
                      env->DeleteLocalRef(tString);
              }
        }
}

void logErr(JNIEnv *env, jobject obj, char *errStr)
{
        if(errStr == NULL)
                return;
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL)
        {
              mid = env->GetMethodID(cls,"logErr","(Ljava/lang/String;)V");
              if(mid != NULL)
              {
                      jstring tString = env->NewStringUTF(errStr);
                      env->CallVoidMethod(obj,mid,tString);
                      env->DeleteLocalRef(tString);
              }
        }
}

/* --------------------- Changes for JNI logging END------------------------------*/




JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_CpuMon_CpuMon_getAvailability
  (JNIEnv *env , jobject obj, jint instId)
{
        //cout<<"javaCom: getAvailability"<<endl;
    char mLogString[256];

    /* ---------------------- JNI Logging Code START-----------------------------*/

    snprintf(mLogString,sizeof(mLogString),"Native Logging: Function CpuMon_getAvailability: Start");
    logDbg(env,obj,mLogString);

    /* ---------------------- JNI Logging Code END -----------------------------*/

        mutex_lock(&itrMutex);
        minstId=instId;
        jint ret=cpInfo.checkAvailability(instId,env,obj);
        mutex_unlock(&itrMutex);
    /* ---------------------- JNI Logging Code START-----------------------------*/

    snprintf(mLogString,sizeof(mLogString),"Native Logging: Function CpuMon_getAvailability: END");
    logDbg(env,obj,mLogString);

    /* ---------------------- JNI Logging Code END -----------------------------*/

        return ret;

}


JNIEXPORT void JNICALL Java_com_proactivenet_monitors_CpuMon_CpuMon_nativeInit
  (JNIEnv * env, jclass clazz)
{
        mutex_lock(&itrMutex);
        mUsr            = env->GetFieldID( clazz, "usrCpu",             "F" );
        mSys            = env->GetFieldID( clazz, "sysCpu",             "F" );
        mIOw            = env->GetFieldID( clazz, "ioWait",             "F" );
        mIntrsRate      = env->GetFieldID( clazz, "ntrRate",            "F" );
        mSrt            = env->GetFieldID( clazz, "scanRate",           "F" );
        mSwtchRt        = env->GetFieldID( clazz, "csw",                "F" );
        mInvolSwtchRt   = env->GetFieldID( clazz, "iCsw",               "F" );
        mIntrThread     = env->GetFieldID( clazz, "intrByThreads",      "F" );
        mSysyCallRt     = env->GetFieldID( clazz, "sysCalls",           "F" );
        mXcallRt        = env->GetFieldID( clazz, "crossCalls",         "F" );
        mCpumigrateRt   = env->GetFieldID( clazz, "threadMigrated",     "F" );
        mutex_unlock(&itrMutex);
}


JNIEXPORT jfloatArray JNICALL Java_com_proactivenet_monitors_CpuMon_CpuMon_recordData
  (JNIEnv *env, jobject obj)
{
        mutex_lock(&itrMutex);
        int i=0;
        jfloatArray newArr = NULL;
        float tmpArr[13];
        int iCount = 13;
        newArr = (jfloatArray)env->NewFloatArray (iCount);
	if (!newArr) {
		return NULL;
	}


    /* ---------------------- JNI Logging Code START-----------------------------*/
    char mLogString[256];

    snprintf(mLogString,sizeof(mLogString),"Native Logging: Function CpuMon_recordData: Start");
    logDbg(env,obj,mLogString);

    /* ---------------------- JNI Logging Code END -----------------------------*/



        //cout<< "the Instance ID is " << minstId;
        //if(cpInfo.checkAvailability(minstId,env,obj)==0 )
         //       cout<<"The Procerssor  available";

        if(cpInfo.getData(minstId,tmpArr,env,obj)==0)
        {

                /* ---------------------- JNI Logging Code START-----------------------------*/

                snprintf(mLogString,sizeof(mLogString),"Native Logging: Function CpuMon_recordData: Assigning default values");
                logDbg(env,obj,mLogString);

                /* ---------------------- JNI Logging Code END -----------------------------*/


                //tmpArr[0] = cpInfo.getUserTime();
                //cout<< "The value of tmpArr[0] is " << tmpArr[0] <<endl;
                env->SetFloatArrayRegion(newArr, 0 , 0 , (jfloat*)tmpArr);

                //tmpArr[1] = cpInfo.getSysTime();
                //cout<< "The value of tmpArr[1] is " << tmpArr[1] <<endl;
                env->SetFloatArrayRegion( newArr, 0 , 1,   (jfloat*)tmpArr);

                //tmpArr[2] = cpInfo.getIOWaitTime();
                //cout<< "The value of tmpArr[2] is " << tmpArr[2] <<endl;
                env->SetFloatArrayRegion( newArr, 0 , 2 ,    (jfloat*)tmpArr);

                //tmpArr[3]=cpInfo.getIntrRate();
                //cout<< "The value of tmpArr[3] is " << tmpArr[3] <<endl;
                env->SetFloatArrayRegion( newArr, 0 , 3 ,(jfloat*)tmpArr);

                //tmpArr[4]=cpInfo.getScanRate();
                //cout<< "The value of tmpArr[4] is " << tmpArr[4] <<endl;
                env->SetFloatArrayRegion( newArr, 0 , 4 ,   (jfloat*)tmpArr);

                //tmpArr[5]=cpInfo.getSwtchRate();
                //cout<< "The value of tmpArr[5] is " << tmpArr[5] <<endl;
                env->SetFloatArrayRegion( newArr, 0 , 5 ,   (jfloat*)tmpArr);

                //tmpArr[6]=cpInfo.getInvolSwtchRate();
                //cout<< "The value of tmpArr[6] is " << tmpArr[6] <<endl;
                env->SetFloatArrayRegion( newArr, 0 , 6 ,   (jfloat*)tmpArr);

                //tmpArr[7]= cpInfo.getSysCallRate();
                //cout<< "The value of tmpArr[7] is " << tmpArr[7] <<endl;
                env->SetFloatArrayRegion( newArr, 0 , 7 ,   (jfloat*)tmpArr);

                //tmpArr[8]=cpInfo.getIntrThreadRate();
                //cout<< "The value of tmpArr[8] is " << tmpArr[8] <<endl;
                env->SetFloatArrayRegion( newArr, 0 , 8 ,   (jfloat*)tmpArr);


                //tmpArr[9]=cpInfo.getXcallRate();
                //cout<< "The value of tmpArr[9] is " << tmpArr[9] <<endl;
                env->SetFloatArrayRegion( newArr, 0 , 9,  (jfloat*)tmpArr);

                //tmpArr[10] = cpInfo.getCpumigrateRate();
                //cout<< "The value of tmpArr[10] is " << tmpArr[10] <<endl;
                env->SetFloatArrayRegion( newArr, 0 , 10 ,   (jfloat*)tmpArr);


                //cout<< "The value of tmpArr[11] is " << tmpArr[11] <<endl;
                env->SetFloatArrayRegion( newArr, 0 , 11 ,   (jfloat*)tmpArr);
                env->SetFloatArrayRegion( newArr, 0 , 12 ,   (jfloat*)tmpArr);


                mutex_unlock(&itrMutex);
                /* ---------------------- JNI Logging Code START-----------------------------*/

                snprintf(mLogString,sizeof(mLogString),"Native Logging: Function CpuMon_recordData: END");
                logDbg(env,obj,mLogString);

                /* ---------------------- JNI Logging Code END -----------------------------*/
                return(newArr);

        }

        else{
                mutex_unlock(&itrMutex);
                /* ---------------------- JNI Logging Code START-----------------------------*/

                snprintf(mLogString,sizeof(mLogString),"Native Logging: Function CpuMon_recordData: error : END");
                logErr(env,obj,mLogString);

                /* ---------------------- JNI Logging Code END -----------------------------*/

                return(newArr);
        }


}

JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_CpuMon_CpuMonAttributeChoice_getProcessors
  (JNIEnv * env, jobject obj)
{
        jmethodID setdata;
        jclass cls = env->GetObjectClass(obj);
        setdata = env->GetMethodID(cls, "setCPUS","(I)I" );
        mutex_lock(&itrMutex);
        //int i = cpInfo.getProcessors();

        kstat_t *k_space;
        void *val_ptr;
        /* ---------------------- JNI Logging Code START-----------------------------*/

        char mLogString[1024];
        snprintf(mLogString,sizeof(mLogString),"Native Logging: Function CpuMonAttributeChoice_getProcessors: Start");
        logDbg(env,obj,mLogString);

        /* ---------------------- JNI Logging Code END -----------------------------*/

        errno =0;
        kstat_ctl_t *kc = kstat_open ();

        if (kc == NULL)
        {
                /* ---------------------- JNI Logging Code START-----------------------------*/

                snprintf(mLogString,sizeof(mLogString),"Native Logging: Function CpuMonAttributeChoice_getProcessors: kstat_open fail. Error code %d. : END ",errno);
                logErr(env,obj,mLogString);

                /* ---------------------- JNI Logging Code END -----------------------------*/

        cout << "Error in kstat" << endl;
        return -1;
        }
        jint val = -1;

        for (k_space = kc->kc_chain; k_space != NULL; k_space = k_space->ks_next
)
        {
                        if(k_space->ks_type == KSTAT_TYPE_RAW &&
                                !strcmp(k_space->ks_module,"cpu_stat") )
                        {

                                val = k_space->ks_instance;
                                env->CallIntMethod(obj,setdata, val);

                        }
       }

        kstat_close (kc);
        mutex_unlock(&itrMutex);

        /* ---------------------- JNI Logging Code START-----------------------------*/

        snprintf(mLogString,sizeof(mLogString),"Native Logging: Function CpuMonAttributeChoice_getProcessors: END");
        logDbg(env,obj,mLogString);

        /* ---------------------- JNI Logging Code END -----------------------------*/

        return -1;
        //return i;
}
