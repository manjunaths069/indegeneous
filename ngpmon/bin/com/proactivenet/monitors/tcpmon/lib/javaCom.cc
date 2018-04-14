#include "tcpmon.h"
#include "TCPStats.h"
TCPStats tcpStats;

// define some global attribute variables

static int   firstTime = 0;
/* --------------------- Changes for JNI logging START------------------------------*/

void logDbg(JNIEnv *env, jobject obj, char *logStr)
{
        if(logStr == NULL)
                return;
        char mlogStr[1280];
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
        char merrStr[1280];
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


JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_tcpmon_tcpmon_javaComInit(JNIEnv *, jobject)
{

/*
        tcpStats.getMibData(&tcp_struct);
        tcpStats.updataLast();
        tcpStats.printLastTcpStats();
*/
    return 0 ;
}

/*
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_tcpmon_tcpmon_javaComExit(JNIEnv *, jobject)
{
    if (tcpStats != 0 )
        delete tcpStats;

    return 0 ;
}
*/
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_tcpmon_tcpmon_javaComGetTCPData(JNIEnv *env, jobject obj)
{
        mib2_tcp_t tcp_struct;
        /* -------------------- JNI Logging changes  START-------------------- */
        char mLogString[255];
        sprintf(mLogString,"Native Logging: function javaComGetTCPData: START");
        logDbg(env,obj,mLogString);
        /* -------------------- JNI Logging changes  END-------------------- */


    if( firstTime == 0 )
    {

                tcpStats.getMibData(&tcp_struct,env,obj);
                tcpStats.updataLast(env,obj);
                // tcpStats.printLastTcpStats();

                firstTime = 1;
        }

        /* -------------------- JNI Logging changes  START-------------------- */
        sprintf(mLogString,"Native Logging: function javaComGetTCPData: calling function getMibData");
        logDbg(env,obj,mLogString);
        /* -------------------- JNI Logging changes  END-------------------- */

        tcpStats.getMibData(&tcp_struct,env,obj);

        /* -------------------- JNI Logging changes  START-------------------- */
        sprintf(mLogString,"Native Logging: function javaComGetTCPData: calling function getDiffTcpStats");
        logDbg(env,obj,mLogString);
        /* -------------------- JNI Logging changes  END-------------------- */


        tcpStats.getDiffTcpStats(env,obj);

        // tcpStats.printDiffTcpStats();

        /* -------------------- JNI Logging changes  START-------------------- */
        sprintf(mLogString,"Native Logging: function javaComGetTCPData: calling function updateLast");
        logDbg(env,obj,mLogString);
        /* -------------------- JNI Logging changes  END-------------------- */


        tcpStats.updataLast(env,obj);

        /* -------------------- JNI Logging changes  START-------------------- */
        sprintf(mLogString,"Native Logging: function javaComGetTCPData: END");
        logDbg(env,obj,mLogString);
        /* -------------------- JNI Logging changes  END-------------------- */

        return 0 ;
}

JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_tcpmon_tcpmon_javaCom(JNIEnv *env, jobject obj, jint attribID)
{
        // the following id is the same as attribID

        /* -------------------- JNI Logging changes  START-------------------- */
        char mLogString[255];
        sprintf(mLogString,"Native Logging: function javaCom: Entered with attribID %d",attribID);
        logDbg(env,obj,mLogString);
        /* -------------------- JNI Logging changes  END-------------------- */


        if (attribID == TcpActiveOpens)
                return tcpStats.GetTcpActiveOpens();

        if (attribID == TcpPassiveOpens)
                return tcpStats.GetTcpPassiveOpens();
        if (attribID == TcpCurrEstab)
                return tcpStats.GetTcpCurrEstab();
        if (attribID == TcpListenDrop)
                return tcpStats.GetTcpListenDrop();
        if (attribID == TcpHalfOpenDrop)
                return tcpStats.GetTcpHalfOpenDrop();
        if (attribID == TcpListenDropQ0)
                return tcpStats.GetTcpListenDropQ0();

        if (attribID == TcpInSegs )
                return tcpStats.GetTcpInSegs();
        if (attribID == TcpInDataInorderSegs )
                return tcpStats.GetTcpInDataInorderSegs();
        if (attribID == TcpInDataUnorderSegs)
                return tcpStats.GetTcpInDataUnorderSegs();
        if (attribID == TcpInDataDupSegs)
                return tcpStats.GetTcpInDataDupSegs();
        if (attribID == TcpInDataPartDupSegs)
                return tcpStats.GetTcpInDataPartDupSegs();

        if (attribID == TcpInBytes)
                return tcpStats.GetTcpInBytes();
        if (attribID == TcpInDataInorderBytes)
                return tcpStats.GetTcpInDataInorderBytes();
        if (attribID == TcpInDataUnorderBytes)
                return tcpStats.GetTcpInDataUnorderBytes();
        if (attribID == TcpInDataDupBytes )
                return tcpStats.GetTcpInDataDupBytes();
        if (attribID == TcpInDataPartDupBytes)
                return tcpStats.GetTcpInDataPartDupBytes();

        if (attribID == TcpOutSegs )
                return tcpStats.GetTcpOutSegs();
        if (attribID == TcpOutDataSegs)
                return tcpStats.GetTcpOutDataSegs();
        if (attribID == TcpRetransSegs)
                return tcpStats.GetTcpRetransSegs();
        if (attribID == TcpOutBytes)
                return tcpStats.GetTcpOutBytes();
        if (attribID == TcpOutDataBytes)
                return tcpStats.GetTcpOutDataBytes();

        if (attribID == TcpRetransBytes)
                return tcpStats.GetTcpRetransBytes();

        if (attribID == TcpAttemptFails)
                return tcpStats.GetTcpAttemptFails();

        if (attribID == TcpOutRsts)
                return tcpStats.GetTcpOutRsts();

}

