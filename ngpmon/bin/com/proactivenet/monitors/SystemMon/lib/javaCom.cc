#include <time.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <kstat.h>
#include <iostream.h>
#include <stdlib.h>
#include <jni.h>
#include "SystemInfo.h"
#include "SolarisPerf.h"
#include <sys/utsname.h>
SolarisPerf *  perf  = 0;
/* --------------------- Changes for JNI logging START------------------------------*/

void logDbg(JNIEnv *env, jobject obj, char *logStr)
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

void logErr(JNIEnv *env, jobject obj, char *errStr)
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

JNIEXPORT jdouble JNICALL Java_com_proactivenet_monitors_SystemMon_SystemInfo_javacom (JNIEnv * env, jobject obj, jint type)
{

/* --------------------- Changes for JNI logging START------------------------------*/
        char mLogMessage[255];
        sprintf(mLogMessage,"Native Logging: function javacom: int type = %d. Entered",type);
        logDbg(env,obj,mLogMessage);
/* --------------------- Changes for JNI logging END------------------------------*/

        if  ( type == 1 )
        return perf->memTotal();
    else if  ( type == 2 )
        return perf->swapTotal();
    else if  ( type == 3 )
        return perf->us();
    else if  ( type == 4 )
        return perf->sy();
    else if  ( type == 5 )
        return perf->io();
    else if  ( type == 6 )
        return perf->rate();
    else if  ( type == 7 )
        return perf->sysload15();
    else if  ( type == 8 )
        return perf->processes();
    else if  ( type == 9 )
        return perf->mem();
    else if  ( type == 10 )
        return perf->swap();
    else if  ( type == 11 )
        return perf->bootTime();
    else if  ( type == 12 )
        return perf->sr();
    else if  ( type ==  13 )
        return perf->sysload1();
    else if  ( type == 14 )
        return perf->sysload5();
    else if  ( type == 15 )
        return perf->runque();
    else if  ( type == 16 )
        return perf->swpque();
    else if  ( type == 17 )
        return perf->waiting();
    else if  ( type == 18 )
          return perf->noCPUS();
}

JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_SystemMon_SystemInfo_javacomError (JNIEnv * env, jobject obj)
{
/* --------------------- Changes for JNI logging START------------------------------*/
        char mLogMessage[2048];
        sprintf(mLogMessage,"Native Logging: function javacomError: Start") ;
        logDbg(env,obj,mLogMessage);
/* --------------------- Changes for JNI logging END------------------------------*/

        char * msg = perf->getErrorMessage() ;

/* --------------------- Changes for JNI logging START------------------------------*/
        sprintf(mLogMessage,"Native Logging: function javacomError: Error Message:%s. Function END",msg) ;
        logDbg(env,obj,mLogMessage);
/* --------------------- Changes for JNI logging END------------------------------*/

    return env->NewStringUTF(msg);
}
JNIEXPORT jint JNICALL JNICALL Java_com_proactivenet_monitors_SystemMon_SystemInfo_javacomInit (JNIEnv * env, jobject obj)
{
/* --------------------- Changes for JNI logging START------------------------------*/
        char mLogMessage[2048];
        sprintf(mLogMessage,"Native Logging: function javacomInit: Start") ;
        logDbg(env,obj,mLogMessage);
/* --------------------- Changes for JNI logging END------------------------------*/


    perf = new SolarisPerf(env,obj);
    if (perf->errorState() != 0 )
    {
        /* --------------------- Changes for JNI logging START------------------------------*/
        sprintf(mLogMessage,"Native Logging: function javacomInit: Error creating perf object. END") ;
        logErr(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
        return -1 ;
    }
    /* --------------------- Changes for JNI logging START------------------------------*/
    sprintf(mLogMessage,"Native Logging: function javacomInit: Successfully creating perf object. END") ;
    logDbg(env,obj,mLogMessage);
    /* --------------------- Changes for JNI logging END------------------------------*/

    return 0 ;
}
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_SystemMon_SystemInfo_javacomRecord (JNIEnv * env, jobject obj)
{
        /* --------------------- Changes for JNI logging START------------------------------*/
        char mLogMessage[255];
        sprintf(mLogMessage,"Native Logging: function javacomRecord: Entered") ;
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        return perf->record(env,obj) ;
}
/* Since the config attribute 'Kernel Version is removed for reverting back the Config Change Detection,
   The following navtive method definition is also commented - Kannan
   
JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_SystemMon_SystemInfo_nKernelVer (JNIEnv * env, jobject obj)
{
	char result[100];
	struct utsname u;

	int i = uname(&u);

	if(i != -1)
	{
    	    strcpy(result,u.version);
	}
    	
	return env->NewStringUTF(result);
}
*/
