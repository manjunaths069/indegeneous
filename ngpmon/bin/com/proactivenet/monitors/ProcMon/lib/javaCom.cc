#include "ProcMon.h"
#include "ProcMonAttrib.h"
#include "ProcInfoManager.h"
#include <thread.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>

static mutex_t  itrMutex;
ProcInfoManager mgr  ;
ProcInfo info;

#define   PRINT(i,x)  cout << "__func__" << ":: " << i << ": " << x << endl
#define   PRINT_L(i,x)  cout << "__func__" << ":: " << i << ": " << x.length() << endl
#define   SF_P    PRINT(0, "Entering..")
#define   EF_P    PRINT(0, "Leaving..")

/* --------------------- Changes for JNI logging START------------------------------*/

#ifndef  _STANDALONE_
void logDbg(JNIEnv *env, jobject obj, char *logStr) {
	if(logStr == NULL)
                return;
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL) {
                mid = env->GetMethodID(cls,"logDbg","(Ljava/lang/String;)V");
                if(mid != NULL) {
                        jstring tString = env->NewStringUTF(logStr);
                        env->CallVoidMethod(obj,mid,tString);
                        env->DeleteLocalRef(tString);
		}
	}
}

void logErr(JNIEnv *env, jobject obj, char *errStr) {
	if(errStr == NULL)
		return;
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL) {
                mid = env->GetMethodID(cls,"logErr","(Ljava/lang/String;)V");
                if(mid != NULL) {
                        jstring tString = env->NewStringUTF(errStr);
                        env->CallVoidMethod(obj,mid,tString);
                        env->DeleteLocalRef(tString);
		}
	}
}
#else
void logDbg(char *logStr)
{
        if (!logStr) {
                return;
        }
        fprintf(stdout, "DBG: %s\n", logStr);
}
void logErr(char *errStr)
{
        if (!errStr) {
                return;
        }
        fprintf(stdout, "ERR: %s\n", errStr);
}
#endif


/* --------------------- Changes for JNI logging END------------------------------*/

#ifndef  _STANDALONE_
JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_execute
(JNIEnv *env, jobject obj, jint ourProcId, jint polltype)
#else
        char *execute(int ourProcId, int polltype)
#endif        
{
        /* --------------------- Changes for JNI logging START------------------------------*/
        //char mLogMessage[255];
        //snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: javaCom: function ProcInfo_execute: Start");
        //logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        mutex_lock(&itrMutex);
#ifndef  _STANDALONE_
        char * procid = mgr.execute(env, obj, ourProcId, polltype);
#else
        char * procid = mgr.execute(ourProcId, polltype);
#endif
        mutex_unlock(&itrMutex);
        /* --------------------- Changes for JNI logging START------------------------------*/
        //snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: javaCom: function ProcInfo_execute: END ");
        //logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
        //void gcFixPrematureFrees();
#ifndef  _STANDALONE_
        jstring tmp = env->NewStringUTF(procid);
        free(procid);
        return tmp;
#else
        return procid;
#endif
}

#ifndef  _STANDALONE_
JNIEXPORT jfloat JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_get
(JNIEnv * env, jobject obj, jint ourProc, jint infoId)
#else
        float get(int ourProc, int infoId)
#endif        
{
        float ret = 0 ;
        /* --------------------- Changes for JNI logging START------------------------------*/
        //char mLogMessage[255];
        //snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: javaCom: function ProcInfo_get. InfoID %d : Start",infoId);
        //logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        mutex_lock(&itrMutex);
#ifndef  _STANDALONE_
        ret = mgr.get(ourProc,infoId,env,obj);
#else
        ret = mgr.get(ourProc,infoId);
#endif
        mutex_unlock(&itrMutex);
        /* --------------------- Changes for JNI logging START------------------------------*/
        //snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: javaCom: function ProcInfo_get: END");
        //logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
        ////void gcFixPrematureFrees();
        return  ret ;
}
#ifndef  _STANDALONE_
JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_getErrMessage
(JNIEnv * env, jobject obj, jint procId)
#else
        char *getErrMessage(int procId)
#endif        
{
        mutex_lock(&itrMutex);
    	char * msg = mgr.getErrorMsg(procId) ;
        mutex_unlock(&itrMutex);
        //void gcFixPrematureFrees();
#ifndef  _STANDALONE_        
        return env->NewStringUTF(msg);
#else
        return msg;
#endif
}

#ifndef  _STANDALONE_
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_load
(JNIEnv * env, jobject obj, jint version, jstring procName, jstring commandLineMatch, jstring userName)
#else
        int load(int version, char *procName, char *commandLineMatch, char *userName)
#endif        
{
        int ret = 0 ;
        mutex_lock(&itrMutex);
#ifndef  _STANDALONE_
        const char *str = env->GetStringUTFChars(procName, 0);
//         char p_name[1024];
//         strcpy(p_name, str);
        /* --------------------- Changes for JNI logging START------------------------------*/
        //char mLogMessage[255];
        //snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: javaCom: function ProcInfo_load: Start");
        //logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        const char *str1 = env->GetStringUTFChars(commandLineMatch, 0);
        //Desai char c_name[256];
        //CHAVA NANDAKUMAR
        //char c_name[1024];
//         char c_name[51200];
//         //CHAVA NANDAKUMAR
//         strcpy(c_name, str1);
        // From 71SP2 onward, we are listing the processes based on user also.
        // so now storing the user name also along with process name and command line
        // inserted by Ashish
        const char *str2 = env->GetStringUTFChars(userName, 0);
        //CHAVANANDA
        //char u_name[256];
//         char u_name[1024];
//         //CHAVANANDA
//         strcpy(u_name, str2);
        //ret = mgr.add(version,p_name,c_name,env, obj);
//         ret = mgr.add(version,p_name,c_name,u_name,env, obj);
        ret = mgr.add(version, str, str1, str2,env, obj);

        env->ReleaseStringUTFChars(procName, str);
        env->ReleaseStringUTFChars(commandLineMatch, str1);
        env->ReleaseStringUTFChars(userName, str2);
#else
        ret = mgr.add(version, procName, commandLineMatch, userName);
#endif
        mutex_unlock(&itrMutex);
        /* --------------------- Changes for JNI logging START------------------------------*/
        //snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: javaCom: function ProcInfo_load: END ret== %d", ret);
        //logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
        //void gcFixPrematureFrees();
        return  ret ;
}

#ifndef  _STANDALONE_
JNIEXPORT void JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_unLoad
(JNIEnv *, jobject, jint ourProcId)
#else
        void unLoad(int ourProcId)
#endif
{
        mutex_lock(&itrMutex);
        mgr.remove(ourProcId);
        mutex_unlock(&itrMutex);
        //void gcFixPrematureFrees();
}

#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getListofProcs
(JNIEnv *env, jobject jobj, jint osType)
#else
        void getListofProcs(int osType)
#endif
{
        int iCount = 0;
        RWCString *strProcsName = NULL;
#ifndef  _STANDALONE_
        jstring utf_str;
        jclass clazz;
        clazz = env->FindClass ("java/lang/String");
        jobjectArray newArr = NULL;
#endif

        mutex_lock(&itrMutex);
        iCount = info.getProcessCount (osType);
        if (iCount <= 0) {
                goto out;
        }
        // iCount is increased by 10 to take care of new instances of processes.
        // This need to be modified.
        iCount = iCount+10;

        strProcsName = new RWCString [iCount];
        if (!strProcsName) {
                goto out;
        }
        iCount = info.getProcessNames (iCount, osType, strProcsName);

#ifndef  _STANDALONE_
        newArr = (jobjectArray)env->NewObjectArray (iCount, clazz, NULL);
        if (!newArr) {
                goto out;
        }
        for (int i = 0; i < iCount; i++)
        {
                utf_str = env->NewStringUTF (strProcsName[i]);
                env->SetObjectArrayElement (newArr, i, utf_str);
                env->DeleteLocalRef (utf_str);
        }
#else
        for (int i = 0; i < iCount; i++) {
                PRINT_L(i,strProcsName[i]);
        }
#endif
out:
        if (strProcsName) {
                delete []strProcsName;
        }
        mutex_unlock(&itrMutex);
        //void gcFixPrematureFrees();
#ifndef  _STANDALONE_        
	if (!newArr) {
		newArr = (jobjectArray)env->NewObjectArray (0, clazz, NULL);
	}
        return (newArr);
#endif
}

#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getCmdLines
(JNIEnv *env, jobject jobj, jstring procName, jint osType)
#else
        void getCmdLines(char *procName, int osType)
#endif
{
	/* ---------------------- JNI Logging Code START-----------------------------*/
	//char mLogString[1280];
	//snprintf(mLogString,sizeof(mLogString),"Native Logging:getCmdLines(javaCom.cc) : Start");
        //logDbg(env,jobj,mLogString);
        /* ---------------------- JNI Logging Code END -----------------------------*/
        int iCount = 0;
        RWCString *strCmdMatch = NULL;
        mutex_lock(&itrMutex);
#ifndef  _STANDALONE_
        jstring utf_str;
        jclass clazz;
        RWCString strProcName;
        clazz = env->FindClass ("java/lang/String");
        jobjectArray newArr = NULL;

        //get String from jstring procName
        char* processName = (char*) env->GetStringUTFChars(procName,0);
	strProcName = (RWCString)processName;

        iCount = info.getCmdMatchCount (osType, strProcName);
        if (iCount <= 0) {
                goto out;
        }
        // iCount is increased by 10 to take care of new instances of processes.
        // This need to be modified.
        iCount = iCount+10;

        strCmdMatch = new RWCString [iCount];
        if (!strCmdMatch) {
                goto out;
        }
        iCount = info.getCmdMatch (iCount, osType, strProcName, strCmdMatch);

        // Release the STRING
        env->ReleaseStringUTFChars(procName, processName);

	newArr = (jobjectArray)env->NewObjectArray (iCount, clazz, NULL);
        if (!newArr) {
                goto out;
        }
	for (int i = 0; i < iCount; i++) {
                utf_str = env->NewStringUTF (strCmdMatch[i]);
                env->SetObjectArrayElement (newArr, i, utf_str);
                env->DeleteLocalRef (utf_str);
	}

#else
        iCount = info.getCmdMatchCount (osType, procName);
        iCount = iCount+10;
        strCmdMatch = new RWCString [iCount];
        if (!strCmdMatch) {
                goto out;
        }
        iCount = info.getCmdMatch (iCount, osType, procName, strCmdMatch);

        PRINT(0, procName);
        for (int i = 0; i < iCount; i++) {
                PRINT_L(i, strCmdMatch[i]);
        }
#endif
out:
        if (strCmdMatch) {
                delete []strCmdMatch;
        }
        mutex_unlock(&itrMutex);
        //void gcFixPrematureFrees();
#ifndef  _STANDALONE_
	if (!newArr) {
		newArr = (jobjectArray)env->NewObjectArray (0, clazz, NULL);
	}
	return (newArr);
#endif
}

//************************** ASHISH ************************************//
// Following functios have been written to support the enhancement.
// Now processes are getting listed based on user also.
//************************** ASHISH ************************************//

// Function to get the available users name and IDs.
#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getUserIDs
(JNIEnv *env, jobject jobj, jint osType)
#else
        void getUserIDs(int osType)
#endif        
{
	/* ---------------------- JNI Logging Code START-----------------------------*/
	//char mLogString[1280];
        //snprintf(mLogString,sizeof(mLogString),"Native Logging:getListofUserIDs(javaCom.cc) : Start");
        //logDbg(env,jobj,mLogString);
        /* ---------------------- JNI Logging Code END -----------------------------*/
        int iCount = 0;
  	RWCString *usrIDArr = NULL;
#ifndef  _STANDALONE_
  	jstring utf_str;
        jclass clazz;
        clazz = env->FindClass ("java/lang/String");
        jobjectArray newArr = NULL;
#endif
        struct passwd *ret;
	char tmpUsrName[512] = {0};
	long uid;

        mutex_lock(&itrMutex);
	for (int iii = 0; ret = getpwent(); iii++) {
		if(ret != NULL) {
			iCount ++;
		}
	}
	endpwent();

	if (iCount == 0) {
                iCount = info.getProcessCount (osType);
                //snprintf(mLogString,sizeof(mLogString),"Process Count value %d", iCount);
                //logDbg(env,jobj,mLogString);
                iCount = iCount + 10;
	 	usrIDArr = new RWCString [iCount];
                if (!usrIDArr) {
                        goto out;
                }
#ifndef  _STANDALONE_
	 	iCount = info.getUsrIDs (env, jobj, iCount, usrIDArr, osType);
#else
	 	iCount = info.getUsrIDs (iCount, usrIDArr, osType);
#endif
	} else {
                int i = 0;
                usrIDArr = new RWCString [iCount];
                if (!usrIDArr) {
                        goto out;
                }
                while ((ret = getpwent()) != NULL) {
                        snprintf(tmpUsrName, sizeof(tmpUsrName), "%s/%d",
                                 ret->pw_name, ret->pw_uid);
// 			strcpy(logname, ret->pw_name);
// 			uid = ret->pw_uid;
// 			snprintf(usrids,sizeof(usrids),"%ld",uid);

// 			strcpy(tmpUsrName, logname);
// 			strcat(tmpUsrName, "/");
// 			strcat(tmpUsrName, usrids);
			usrIDArr[i] = (RWCString) tmpUsrName;
			i++;
  		}
		endpwent();
                iCount = i;
	}

#ifndef  _STANDALONE_
        newArr = (jobjectArray)env->NewObjectArray (iCount, clazz, NULL);
        if (!newArr) {
                goto out;
        }

        for (int i = 0; i < iCount; i++) {
                utf_str = env->NewStringUTF (usrIDArr[i]);
                env->SetObjectArrayElement (newArr, i, utf_str);
                env->DeleteLocalRef (utf_str);
	}
#else
        for (int i = 0; i < iCount; i++) {
                PRINT(i, usrIDArr[i]);
        }
#endif
out:
	if (usrIDArr) {
                delete []usrIDArr;
        }
        mutex_unlock(&itrMutex);
        //snprintf(mLogString,sizeof(mLogString),"Native Logging:getUserIDs(javaCom.cc):Ends");
        //logDbg(env,jobj,mLogString);
	//void gcFixPrematureFrees();
#ifndef  _STANDALONE_
	if (!newArr) {
		newArr = (jobjectArray)env->NewObjectArray (0, clazz, NULL);
	}
        return (newArr);
#endif
}


// Following function is for getting the list of processes running under specific user
#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getUserProcesses
(JNIEnv *env, jobject jobj, jint osType, jstring strUid)
#else
        void getUserProcesses(int osType, char *strUid)
#endif
{
        int iCount = 0;
        RWCString *strProcsName = NULL;

        mutex_lock(&itrMutex);
#ifndef  _STANDALONE_
        jstring utf_str;
        jclass clazz;
        clazz = env->FindClass ("java/lang/String");
        jobjectArray newArr = NULL;

	long uid = atol((char*) env->GetStringUTFChars(strUid,0));

	iCount = info.getUserProcessCount (env, jobj, osType, uid);
#else
        long uid = atol(strUid);
	iCount = info.getUserProcessCount (osType, uid);
#endif
        if (iCount <= 0) {
                goto out;
        }
        iCount = iCount + 10;

        strProcsName = new RWCString [iCount];
        if (!strProcsName) {
                goto out;
        }
#ifndef  _STANDALONE_
        iCount = info.getUserProcessNames (env, jobj, iCount, osType, uid, strProcsName);

        newArr = (jobjectArray)env->NewObjectArray (iCount, clazz, NULL);
        if (!newArr) {
                goto out;
        }
        for (int i = 0; i < iCount; i++) {
                utf_str = env->NewStringUTF (strProcsName[i]);
                env->SetObjectArrayElement (newArr, i, utf_str);
                env->DeleteLocalRef (utf_str);
	}
#else
        iCount = info.getUserProcessNames (iCount, osType, uid, strProcsName);
        for (int i = 0; i < iCount; i++) {
                PRINT_L(i, strProcsName[i]);
        }
#endif
out:
        if (strProcsName) {
                delete []strProcsName;
        }
        mutex_unlock(&itrMutex);
#ifndef  _STANDALONE_
	if (!newArr) {
		newArr = (jobjectArray)env->NewObjectArray (0, clazz, NULL);
	}
        return (newArr);
#endif
}

// Following function is for getting the command line for process running under specific user
#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getUserCmdLines
(JNIEnv *env, jobject jobj, jstring procName, jint osType, jstring strUid)
#else
        void getUserCmdLines(char *procName, int osType, char *strUid)
#endif
{
	int iCount = 0;
        RWCString *strCmdMatch = NULL;
        mutex_lock(&itrMutex);
#ifndef  _STANDALONE_
        jstring utf_str;
        jclass clazz;
        RWCString strProcName;
        clazz = env->FindClass ("java/lang/String");
        jobjectArray newArr = NULL;
        long uid = atol((char*) env->GetStringUTFChars(strUid,0));

        char* processName = (char*) env->GetStringUTFChars(procName,0);
        strProcName = (RWCString)processName;

        iCount = info.getUsrCmdMatchCount (env, jobj, osType, strProcName, uid);
#else
        RWCString strProcName = (RWCString)procName;
        long uid = atol(strUid);
        iCount = info.getUsrCmdMatchCount (osType, strProcName, uid);
#endif
        if (iCount <= 0) {
                goto out;
        }
        iCount = iCount + 10;

        strCmdMatch = new RWCString [iCount];
        if (!strCmdMatch) {
                goto out;
        }
#ifndef  _STANDALONE_
        iCount = info.getUsrCmdMatch (env, jobj, iCount, osType, strProcName, strCmdMatch, uid);

        // Release the STRING
        env->ReleaseStringUTFChars(procName, processName);
	newArr = (jobjectArray)env->NewObjectArray (iCount, clazz, NULL);
        if (!newArr) {
                goto out;
        }
        for (int i = 0; i < iCount; i++) {
                utf_str = env->NewStringUTF (strCmdMatch[i]);
                env->SetObjectArrayElement (newArr, i, utf_str);
                env->DeleteLocalRef (utf_str);
	}
#else
        iCount = info.getUsrCmdMatch (iCount, osType, strProcName, strCmdMatch, uid);
        for (int i = 0; i < iCount; i++) {
                PRINT_L(i, strCmdMatch[i]);
        }
#endif
  out:
        if (strCmdMatch) {
                delete []strCmdMatch;
        }
        mutex_unlock(&itrMutex);
#ifndef  _STANDALONE_
	if (!newArr) {
		newArr = (jobjectArray)env->NewObjectArray (0, clazz, NULL);
	}
        return newArr;
#endif
}

// Following function is for validating the user's name or ID
#ifndef  _STANDALONE_
JNIEXPORT jlong JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_ValidateUserID
(JNIEnv *env, jobject jobj, jstring name, jstring id, jint flag)
#else
long ValidateUserID(char *name, char *id, int flag)
#endif
{
#ifndef  _STANDALONE_
	char* usrName = NULL;
        if (name != NULL) {
		usrName = (char*) env->GetStringUTFChars(name,0);
	}
	char* usrID = (char*) env->GetStringUTFChars(id,0);
	long uid = -857320316;
        if (flag == 0) {
                uid = atol(usrID);
	}
#else
        char *usrName = name;
        char *usrID = id;
        long uid = -857320316;
        if (flag == 0) {
                uid = atol(id);
        }
#endif
        struct passwd *ret = NULL;
        char* logname = NULL;

        mutex_lock(&itrMutex);
        if (usrName != NULL) {
                if ((ret = getpwuid(uid))!= NULL) {
                        logname = ret->pw_name;
                        if (strcmp (logname, usrName) == 0) {
#ifndef  _STANDALONE_
                                env->ReleaseStringUTFChars(id, usrID);
                                env->DeleteLocalRef(id);
                                env->ReleaseStringUTFChars(name, usrName);
                                env->DeleteLocalRef(name);
                                endpwent();
#endif
                                mutex_unlock(&itrMutex);
                                return uid;
                        } else {
#ifndef  _STANDALONE_
                                env->ReleaseStringUTFChars(id, usrID);
                                env->DeleteLocalRef(id);
                                env->ReleaseStringUTFChars(name, usrName);
                                env->DeleteLocalRef(name);
#endif
                                endpwent();
                                uid = NULL;
                                mutex_unlock(&itrMutex);
                                return -857320316;
                        }
                } else {
#ifndef  _STANDALONE_
                        env->ReleaseStringUTFChars(id, usrID);
                        env->DeleteLocalRef(id);
                        env->ReleaseStringUTFChars(name, usrName);
                        env->DeleteLocalRef(name);
#endif
                        endpwent();
                        uid = NULL;
                        mutex_unlock(&itrMutex);
                        return -857320316;
                }
        } else if (flag == 1) {
                if ((ret = getpwnam(usrID))!= NULL) {
                        uid = ret->pw_uid;
#ifndef  _STANDALONE_
                        env->ReleaseStringUTFChars(id, usrID);
                        env->DeleteLocalRef(id);
                        env->ReleaseStringUTFChars(name, usrName);
                        env->DeleteLocalRef(name);
#endif
                        endpwent();
                        return uid;
                } else {
#ifndef  _STANDALONE_
                        env->ReleaseStringUTFChars(id, usrID);
                        env->DeleteLocalRef(id);
                        env->ReleaseStringUTFChars(name, usrName);
                        env->DeleteLocalRef(name);
#endif
                        endpwent();
                        uid = NULL;
                        mutex_unlock(&itrMutex);
                        return -857320316;
                }
        } else {
                if ((ret = getpwuid(uid))!= NULL) {
#ifndef  _STANDALONE_
                        env->ReleaseStringUTFChars(id, usrID);
                        env->DeleteLocalRef(id);
                        env->ReleaseStringUTFChars(name, usrName);
                        env->DeleteLocalRef(name);
#endif
                        endpwent();
                        mutex_unlock(&itrMutex);
                        return uid;
                } else {
#ifndef  _STANDALONE_
                        env->ReleaseStringUTFChars(id, usrID);
                        env->DeleteLocalRef(id);
                        env->ReleaseStringUTFChars(name, usrName);
                        env->DeleteLocalRef(name);
#endif
                        endpwent();
                        uid = NULL;
                        mutex_unlock(&itrMutex);
                        return -857320316;
                }
        }
#ifndef  _STANDALONE_
        env->ReleaseStringUTFChars(id, usrID);
        env->DeleteLocalRef(id);
        env->ReleaseStringUTFChars(name, usrName);
        env->DeleteLocalRef(name);
#endif
        endpwent();
        uid = NULL;
        mutex_unlock(&itrMutex);
        return -857320316;
}


// Polling at user level
#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_executeAtUserLevel
(JNIEnv *env, jobject obj, jint ourProcId, jint polltype, jint osType)
#else
        void executeAtUserLevel(int ourProcId, int polltype, int osType)
#endif                
{
        int iCount = 0;
        int i;
        RWCString *TempstrProcsName = NULL;
#ifndef  _STANDALONE_
        jstring utf_str;
        jclass clazz;
        clazz = env->FindClass ("java/lang/String");
        jobjectArray newArr = NULL;
#endif    

        mutex_lock(&itrMutex);
        iCount = info.getProcessCount(osType);
        PRINT(iCount, " ProcessCount");
        if (iCount <= 0) {
                goto out;
        }
        TempstrProcsName = new RWCString [iCount];
        if (!TempstrProcsName) {
                goto out;
        }
#ifndef  _STANDALONE_
        iCount = mgr.executeAtUserLevel(env, obj, ourProcId, polltype, iCount, TempstrProcsName);
#else
        iCount = mgr.executeAtUserLevel(ourProcId, polltype, iCount, TempstrProcsName);
#endif
        /* --------------------- Changes for JNI logging START------------------------------*/
        //snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: javaCom: function ProcInfo_executeAtUserLevel: END");
        //logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
	//void gcFixPrematureFrees();
#ifndef  _STANDALONE_
        newArr = (jobjectArray)env->NewObjectArray (iCount, clazz, NULL);
        if (!newArr) {
                goto out;
        }
        for (i = 0; i < iCount; i++) {
                utf_str = env->NewStringUTF (TempstrProcsName[i]);
                env->SetObjectArrayElement (newArr, i, utf_str);
                env->DeleteLocalRef (utf_str);
        } 
#else
        for (int i = 0; i < iCount; i++) {
                PRINT(i, TempstrProcsName[i]);
        }
        PRINT(iCount, "iCount (pidCount)");
#endif
out:
        if (TempstrProcsName) {
                delete []TempstrProcsName;
        }
        mutex_unlock(&itrMutex);
#ifndef  _STANDALONE_
	if (!newArr) {
		newArr = (jobjectArray)env->NewObjectArray (0, clazz, NULL);
	}
        return(newArr);
#endif
}

#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getListofFilteredProcs
(JNIEnv *env, jobject jobj, jobjectArray FUserNames, jint osType, jint Flength)
#else
        void getListofFilteredProcs(char **FUserNames, int osType, int Flength)
#endif
{
	/* --------------------- Changes for JNI logging START------------------------------*/
        //char mLogMessage[255];
        //snprintf(mLogMessage,sizeof(mLogMessage),"** Native Logging: javaCom: function getListofFilteredProcs Start **");
        // logDbg(env,jobj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

	int length = Flength;
	char **FilterUsers = NULL;
	int iCount = 0;
	int index=0;
        int i = 0;
	RWCString *strProcsName = NULL;
	RWCString *TempstrProcsName = NULL;

#ifndef  _STANDALONE_
	jstring utf_str;
	jclass clazz;
	clazz = env->FindClass ("java/lang/String");
	jobjectArray newArr = NULL;
#endif

        mutex_lock(&itrMutex);
	FilterUsers = (char**) calloc(length, sizeof(char*));
        if (!FilterUsers) {
                goto end_loop;
        }
	for (i=0;i<length;i++)
	{
#ifndef  _STANDALONE_
                jstring jstr = (jstring)env->  GetObjectArrayElement(FUserNames,i);
                char * username =  (char*) env->GetStringUTFChars(jstr,0);
                if (username) {
                        FilterUsers[i] = strdup(username);
                }
	   	env->DeleteLocalRef (jstr);
#else
                if (FUserNames[i]) {
                        FilterUsers[i] = strdup(FUserNames[i]);
                }
#endif
	}
 	iCount = info.getProcessCount(osType);
        if (iCount <= 0) {
                goto end_loop;
        }
	iCount = iCount + 10;
	TempstrProcsName = new RWCString [iCount];
        if (!TempstrProcsName) {
                goto end_loop;
        }
	for (i=0;i<length;i++)
	{
                long uid;
                struct passwd *ret;
                if(FilterUsers[i] && (ret = getpwnam(FilterUsers[i]))!=NULL)
                {
                        uid = ret->pw_uid;
#ifndef  _STANDALONE_
                        int TempiCount = info.getUserProcessCount (env, jobj, osType, uid);
#else
                        int TempiCount = info.getUserProcessCount (osType, uid);
#endif
                        if (TempiCount <= 0) {
                                goto end_loop;
                        }
                        TempiCount = TempiCount + 10;
                        strProcsName = new RWCString [TempiCount];
                        if (!strProcsName) {
                                goto end_loop;
                        }
#ifndef  _STANDALONE_
                        TempiCount = info.getUserProcessNames (env, jobj, TempiCount, osType, uid, strProcsName);
#else
                        TempiCount = info.getUserProcessNames (TempiCount, osType, uid, strProcsName);
#endif
                        for (int k=0;k< TempiCount;k++)
                        {
                                if (index >= iCount) {
                                        delete []strProcsName;
                                        goto end_loop;
                                }
                                TempstrProcsName[index]=strProcsName[k];
                                index++;
                        }
                        delete []strProcsName;
                }
                endpwent();
	}
end_loop:
	//NANDAMEM
	for(i = 0; i < length; i++)
        {
                if (FilterUsers[i]) {
                        free (FilterUsers[i]);
                }
        }
        if (FilterUsers != NULL)
        {
                free (FilterUsers);
        }
#ifndef  _STANDALONE_
	newArr = (jobjectArray)env->NewObjectArray (index, clazz, NULL);
        if (newArr) {
                for ( i = 0; i < index; i++) {
                        utf_str = env->NewStringUTF (TempstrProcsName[i]);
                        env->SetObjectArrayElement (newArr, i, utf_str);
                        env->DeleteLocalRef (utf_str);
                }
        }
#else
	for ( i = 0; i < index; i++) {
                PRINT_L(i, TempstrProcsName[i]);
        }
#endif
	if (TempstrProcsName) {
                delete []TempstrProcsName;
        }
	/* --------------------- Changes for JNI logging START------------------------------*/
        //snprintf(mLogMessage,sizeof(mLogMessage),"** Native Logging: javaCom: function getListofFilteredProcs Ends** ");
        //logDbg(env,jobj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
        mutex_unlock(&itrMutex);
#ifndef  _STANDALONE_
	if (!newArr) {
		newArr = (jobjectArray)env->NewObjectArray (0, clazz, NULL);
	}
	return (newArr);
#endif
}

#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getListOfFilteredCmdLines
(JNIEnv *env, jobject jobj, jobjectArray FUserNames, jstring procName, jint osType, jint Flength, jint PFlag)
#else
        void getListOfFilteredCmdLines(char **FUserNames, char *procName, int osType, int Flength, int PFlag)
#endif        
{
	/* --------------------- Changes for JNI logging START------------------------------*/
        //char mLogMessage[255];
        //snprintf(mLogMessage,sizeof(mLogMessage),"** Native Logging: javaCom: function getListOfFilteredCmdLines STARTS **");
        //logDbg(env,jobj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
        int iCount = 0;
        int i = 0;
        int TempiCount = 0;
        int index = 0;
        RWCString *strCmdMatch = NULL;
        RWCString strProcName;
        RWCString *TempstrCmdMatch = NULL;
#ifndef  _STANDALONE_
        jstring utf_str;
        jclass clazz;
        clazz = env->FindClass ("java/lang/String");
        jobjectArray newArr = NULL;
#endif
        int length=Flength;
        int ProcFlag = PFlag;
        char **FilterUsers = NULL;
        RWCString *TempstrCmdName = NULL;

        mutex_lock(&itrMutex);
        FilterUsers = (char**) calloc(length, sizeof(char*));
        if (!FilterUsers) {
                goto end_loop;
        }
	for (i=0;i<length;i++)
	{
#ifndef  _STANDALONE_
                jstring jstr = (jstring)env->  GetObjectArrayElement(FUserNames,i);
                char * username =  (char*) env->GetStringUTFChars(jstr,0);
                if (username) {
                        FilterUsers[i] = strdup(username);
                }
	   	env->DeleteLocalRef (jstr);
#else
                if (FUserNames[i]) {
                        FilterUsers[i] = strdup(FUserNames[i]);
                }
#endif
	}

#ifndef  _STANDALONE_
        char* processName = (char*) env->GetStringUTFChars(procName,0);
	strProcName = (RWCString)processName;
#else
        strProcName = (RWCString)procName;
#endif
	TempiCount = info.getCmdMatchCountNOProcsNoUser (osType);
        if (TempiCount <= 0) {
                goto end_loop;
        }
	TempiCount = TempiCount+10;
	TempstrCmdName = new RWCString [TempiCount];
        if (!TempstrCmdName) {
                goto end_loop;
        }
	for(i=0;i<length;i++)
	{
                long uid;
                struct passwd *ret;
                if(FilterUsers[i] && (ret = getpwnam(FilterUsers[i]))!=NULL)
                {
                        uid = ret->pw_uid;
                        if(ProcFlag ==0)
                        {
#ifndef  _STANDALONE_
                                iCount = info.getUsrCmdMatchCount (env, jobj, osType, strProcName, uid);
#else
                                iCount = info.getUsrCmdMatchCount (osType, strProcName, uid);
#endif
                        }
                        else
                        {
                                iCount = info.getUserCmdMatchCountNOProcs (osType, uid);
                        }
                        iCount = iCount+10;
                        if (iCount <= 0) {
                                goto end_loop;
                        }
                        strCmdMatch = new RWCString [iCount];
                        if (!strCmdMatch) {
                                goto end_loop;
                        }
                        if(ProcFlag ==0)
                        {
#ifndef  _STANDALONE_
                                iCount = info.getUsrCmdMatch (env, jobj, iCount, osType, strProcName, strCmdMatch, uid);
#else
                                iCount = info.getUsrCmdMatch (iCount, osType, strProcName, strCmdMatch, uid);
#endif
                        }
                        else
                        {
                                iCount = info.getUsrCmdMatchNoProcess (iCount, osType, strCmdMatch, uid);
                        }
                        for (int k=0;k< iCount;k++)
                        {
                                if (index >= iCount) {
                                        delete []strCmdMatch;
                                        goto end_loop;
                                }
                                TempstrCmdName[index]=strCmdMatch[k];
                                index++;
                        }
                        delete []strCmdMatch;
                } //end of If
                endpwent();

	} // end of for

  end_loop:
        for(i = 0; i < length; i++)
        {
                if (FilterUsers[i]) {
                        free (FilterUsers[i] );
                }
        }
        if (FilterUsers != NULL)
        {
                free (FilterUsers);
        }

#ifndef  _STANDALONE_
	env->ReleaseStringUTFChars(procName, processName);
	newArr = (jobjectArray)env->NewObjectArray (index, clazz, NULL);
        if (newArr) {
                for (i = 0; i < index; i++) {
                        utf_str = env->NewStringUTF (TempstrCmdName[i]);
                        env->SetObjectArrayElement (newArr, i, utf_str);
                        env->DeleteLocalRef (utf_str);
                }
        }
#else
	for (i = 0; i < index; i++) {
                PRINT_L(i, TempstrCmdName[i]);
        }
#endif
        if (TempstrCmdName) {
                delete []TempstrCmdName;
        }
        mutex_unlock(&itrMutex);
#ifndef  _STANDALONE_
	if (!newArr) {
		newArr = (jobjectArray)env->NewObjectArray (0, clazz, NULL);
	}
        return newArr;
#endif
}

#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getListOfFilteredCmdLinesProcReg
(JNIEnv *env, jobject jobj, jstring userName, jobjectArray FProcNames, jint osType, jint Flength)
#else
        void getListOfFilteredCmdLinesProcReg(char *userName, char **FProcNames, int osType, int Flength)
#endif        
{
	/* --------------------- Changes for JNI logging START------------------------------*/
        //char mLogMessage[255];
        //snprintf(mLogMessage,sizeof(mLogMessage),"** Native: javaCom: function getListOfFilteredCmdLines PROC REG STARTS **");
        //logDbg(env,jobj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
	int iCount = 0;
	int TempiCount=0;
	int length= Flength;
        RWCString *strCmdMatch = NULL;
#ifndef  _STANDALONE_
        jstring utf_str, tmp_str;
        jclass clazz;
        clazz = env->FindClass ("java/lang/String");
        jobjectArray newArr;
#endif
        RWCString strProcName;
        RWCString *TempstrCmdName = NULL;
        char **FilterProcess = NULL;
        int index=0;
        int i = 0;
        long uid;
        char * all="< ALL USERS >";

#ifndef  _STANDALONE_
        char* TuserName = (char*) env->GetStringUTFChars(userName,0);
        if(strcmp(TuserName, all) != 0)
        {
                uid = atol((char*) env->GetStringUTFChars(userName,0));
        }
#else
        char* TuserName = userName;
        if(strcmp(TuserName, all) != 0)
        {
                uid = atol(userName);
        }
#endif
        mutex_lock(&itrMutex);
        FilterProcess = (char**) malloc(length * sizeof(char*));
        if (!FilterProcess) {
                goto end_loop;
        }
	for (i=0;i<length;i++)
	{
#ifndef  _STANDALONE_
                jstring jstr = (jstring)env->  GetObjectArrayElement(FProcNames,i);
                char * processName =  (char*) env->GetStringUTFChars(jstr,0);
                FilterProcess[i] = strdup(processName);
	   	env->DeleteLocalRef (jstr);
#else
                FilterProcess[i] = strdup(FProcNames[i]);
#endif
	}
	for(i=0;i<length;i++)
	{
                strProcName = (RWCString)FilterProcess[i];
                int ExCount = info.getCmdMatchCount (osType, strProcName);
                TempiCount = TempiCount+ExCount;
	}
        if (TempiCount <= 0) {
                goto end_loop;
        }
	TempiCount= TempiCount+10;
	TempstrCmdName = new RWCString [TempiCount];
        if (!TempstrCmdName) {
                goto end_loop;
        }
        for(i=0;i<length;i++)
        {
                strProcName = (RWCString)FilterProcess[i];
                if(strcmp(TuserName, all) == 0)
                {
                        iCount = info.getCmdMatchCount (osType, strProcName);
                }
                else
                {
#ifndef  _STANDALONE_
                        iCount = info.getUsrCmdMatchCount (env, jobj, osType, strProcName, uid);
#else
                        iCount = info.getUsrCmdMatchCount (osType, strProcName, uid);
#endif
                }
                if (iCount <= 0) {
                        goto end_loop;
                }
                iCount = iCount + 10;
                strCmdMatch = new RWCString [iCount];
                if (!strCmdMatch) {
                        goto end_loop;
                }
                if(strcmp(TuserName, all) == 0)
                {
                        iCount = info.getCmdMatch (iCount, osType, strProcName, strCmdMatch);
                }
                else
                {
#ifndef  _STANDALONE_
                        iCount = info.getUsrCmdMatch (env, jobj, iCount, osType, strProcName, strCmdMatch, uid);
#else
                        iCount = info.getUsrCmdMatch (iCount, osType, strProcName, strCmdMatch, uid);
#endif
                }
                for(int k=0;k< iCount;k++)
                {
                        if (index >= TempiCount) {
                                delete []strCmdMatch;
                                goto end_loop;
                        }
                        TempstrCmdName[index]=strCmdMatch[k];
                        index++;
                }
                delete []strCmdMatch;
        }

  end_loop:
        for (int No = 0; No < length; No++) {
                if (FilterProcess[No]) {
                        free(FilterProcess[No]);
                }
        }
        if (FilterProcess)
                free (FilterProcess);
#ifndef  _STANDALONE_
        newArr = (jobjectArray)env->NewObjectArray (index, clazz, NULL);
        if (newArr) {
                for (i = 0; i < index; i++) {
                        utf_str = env->NewStringUTF (TempstrCmdName[i]);
                        env->SetObjectArrayElement (newArr, i, utf_str);
                        env->DeleteLocalRef (utf_str);
                }
        }
#else
        for (i = 0; i < index; i++) {
                PRINT_L(i, TempstrCmdName[i]);
        }
#endif
        if (TempstrCmdName) {
                delete []TempstrCmdName;
        }
        mutex_unlock(&itrMutex);
        /* --------------------- Changes for JNI logging START------------------------------*/
        //snprintf(mLogMessage,sizeof(mLogMessage),"** Native: javaCom: function getListOfFilteredCmdLines PROC REG ENDS **");
        //logDbg(env,jobj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
#ifndef  _STANDALONE_
	if (!newArr) {
		newArr = (jobjectArray)env->NewObjectArray (0, clazz, NULL);
	}
        return newArr;
#endif
}

#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getListOfFilteredCmdLinesUserRegProcReg
(JNIEnv *env, jobject jobj, jobjectArray FUserNames, jobjectArray FProcNames, jint osType, jint FUlength, jint FPlength)
#else
void getListOfFilteredCmdLinesUserRegProcReg(char **FUserNames, char **FProcNames, int osType, int FUlength, int FPlength)
#endif
{
	/* --------------------- Changes for JNI logging START------------------------------*/
        //char mLogMessage[255];
        //snprintf(mLogMessage,sizeof(mLogMessage),"** Native: javaCom: getListOfFilteredCmdLinesUSER REG AND PROC REG STARTS **");
        //logDbg(env,jobj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
        int iCount = 0;
#ifndef  _STANDALONE_
        jstring utf_str;
        jclass clazz;
        clazz = env->FindClass ("java/lang/String");
        jobjectArray newArr = NULL;
#endif
        int Ulength=FUlength;
        int Plength=FPlength;
        char **FilterUsers = NULL;
        char **FilterProcess = NULL;
        int TempiCount=0;
        int index=0;
        int i = 0;
        int No = 0;
        RWCString *TempstrCmdName = NULL;
        RWCString strProcName;
        RWCString *strCmdMatch = NULL;

        mutex_lock(&itrMutex);
        FilterUsers = (char**) calloc(Ulength, sizeof(char*));
        if (!FilterUsers) {
                goto end_loop;
        }
	FilterProcess = (char**) calloc(Plength, sizeof(char*));
        if (!FilterProcess) {
                goto end_loop;
        }
	for (i=0;i<Ulength;i++)
	{
#ifndef  _STANDALONE_
                jstring jstr = (jstring)env->  GetObjectArrayElement(FUserNames,i);
                char * username =  (char*) env->GetStringUTFChars(jstr,0);
                if (username) {
                        FilterUsers[i] = strdup(username);
                }
	   	env->DeleteLocalRef (jstr);
#else
                if (FUserNames[i]) {
                        FilterUsers[i] = strdup(FUserNames[i]);
                }
#endif
	}
	for (i=0;i<Plength;i++)
	{
#ifndef  _STANDALONE_
                jstring jstr = (jstring)env->  GetObjectArrayElement(FProcNames,i);
                char * processName =  (char*) env->GetStringUTFChars(jstr,0);
                if (processName) {
                        FilterProcess[i] = strdup(processName);
                }
	   	env->DeleteLocalRef (jstr);
#else
                if (FProcNames[i]) {
                        FilterProcess[i] = strdup(FProcNames[i]);
                }
#endif
	}
        for(int temp=0;temp<Ulength;temp++)
        {
                long tempuid;
                struct passwd *pwd;
                if(FilterUsers[temp] && (pwd = getpwnam(FilterUsers[temp]))!=NULL)
                {
                        tempuid = pwd->pw_uid;
                        for(i=0;i<Plength;i++)
                        {
                                strProcName = (RWCString)FilterProcess[i];
                                //int ExCount = info.getCmdMatchCount (osType, strProcName);
#ifndef  _STANDALONE_
                                int ExCount = info.getUsrCmdMatchCount (env, jobj, osType, strProcName, tempuid);
#else
                                int ExCount = info.getUsrCmdMatchCount (osType, strProcName, tempuid);
#endif
                                TempiCount = TempiCount+ExCount;
                        }
                }
                endpwent();
        }
        if (TempiCount <= 0) {
                goto end_loop;
        }
	TempiCount = TempiCount + 10;
	TempstrCmdName = new RWCString [TempiCount];
        if (!TempstrCmdName) {
                goto end_loop;
        }
	for(i=0;i<Ulength;i++)
	{
                long uid;
                struct passwd *ret;
                if(FilterUsers[i] && (ret = getpwnam(FilterUsers[i]))!=NULL)
                {
                        uid = ret->pw_uid;
                        for(int j=0;j<Plength;j++)
                        {
                                strProcName = (RWCString)FilterProcess[j];
#ifndef  _STANDALONE_
                                iCount = info.getUsrCmdMatchCount (env, jobj, osType, strProcName, uid);
#else
                                iCount = info.getUsrCmdMatchCount (osType, strProcName, uid);
#endif
                                if (iCount <=0 ) {
                                        goto end_loop;
                                }
                                iCount = iCount+10;
                                strCmdMatch = new RWCString [iCount];
                                if (!strCmdMatch) {
                                        goto end_loop;
                                }
#ifndef  _STANDALONE_
                                iCount = info.getUsrCmdMatch (env, jobj, iCount, osType, strProcName, strCmdMatch, uid);
#else
                                iCount = info.getUsrCmdMatch (iCount, osType, strProcName, strCmdMatch, uid);
#endif
                                for (int k=0;k< iCount;k++)
                                {
                                        if (index >= TempiCount) {
                                                delete []strCmdMatch;
                                                goto end_loop;
                                        }
                                        TempstrCmdName[index]=strCmdMatch[k];
                                        index++;
                                }
                                delete []strCmdMatch;
                        } // end of process for
                } //end of If
                endpwent();
	} // end of user for

  end_loop:
        for (No = 0; No < Ulength; No++) {
                if (FilterUsers[No]) {
                        free(FilterUsers[No]);
                }
        }
        if (FilterUsers != NULL)
                free (FilterUsers);
        for (No = 0; No < Plength; No++) {
                if (FilterProcess[No]) {
                        free(FilterProcess[No]);
                }
        }
        if (FilterProcess != NULL)
                free (FilterProcess);
#ifndef  _STANDALONE_
	newArr = (jobjectArray)env->NewObjectArray (index, clazz, NULL);
        if (newArr) {
                for (i = 0; i < index; i++) {
                        utf_str = env->NewStringUTF (TempstrCmdName[i]);
                        env->SetObjectArrayElement (newArr, i, utf_str);
                        env->DeleteLocalRef (utf_str);
                }
        }
#else
        for (i = 0; i < index; i++) {
                PRINT_L(i, TempstrCmdName[i]);
        }
#endif
        if (TempstrCmdName) {
                delete []TempstrCmdName;
        }
        mutex_unlock(&itrMutex);
        /* --------------------- Changes for JNI logging START------------------------------*/
        //snprintf(mLogMessage,sizeof(mLogMessage),"** Native: javaCom: getListOfFilteredCmdLinesUSER REG AND PROC REG ENDS **");
        //logDbg(env,jobj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
#ifndef  _STANDALONE_
	if (!newArr) {
		newArr = (jobjectArray)env->NewObjectArray (0, clazz, NULL);
	}
        return newArr;
#endif
}


#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getListofAllCmdLines
(JNIEnv *env, jobject jobj, jint osType, jstring User)
#else
        void getListofAllCmdLines(int osType, char *User)
#endif
{
	/* --------------------- Changes for JNI logging START------------------------------*/
        //char mLogMessage[255];
        //snprintf(mLogMessage,sizeof(mLogMessage),"** Native: javaCom: getListofAllCmdLines With No User No Process Starts  **");
        //logDbg(env,jobj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
        int iCount = 0;
        RWCString *strCmdMatch = NULL;
        char * all="< ALL USERS >";
        long uid;
#ifndef  _STANDALONE_
        jstring utf_str;
        jclass clazz;
        clazz = env->FindClass ("java/lang/String");
        jobjectArray newArr = NULL;
        char* TuserName = (char*) env->GetStringUTFChars(User,0);
        if(strcmp(TuserName, all) != 0)
        {
                uid = atol((char*) env->GetStringUTFChars(User,0));
        }
#else
        char* TuserName = User;
        if(strcmp(TuserName, all) != 0)
        {
                uid = atol(User);
        }
#endif

        mutex_lock(&itrMutex);
        if(strcmp(TuserName, all) == 0)
        {
                iCount = info.getCmdMatchCountNOProcsNoUser (osType);
        }
        else
        {
                iCount = info.getUserCmdMatchCountNOProcs (osType, uid);
        }
        if (iCount <= 0) {
                goto end_loop;
        }
        iCount = iCount + 10;
        strCmdMatch = new RWCString [iCount];
        if (!strCmdMatch) {
                goto end_loop;
        }
	if(strcmp(TuserName, all) == 0)
	{
                iCount = info.getCmdMatchNoUserNoProcess (iCount, osType, strCmdMatch);
        }
        else
        {
                iCount = info.getUsrCmdMatchNoProcess (iCount, osType, strCmdMatch, uid);
        }
#ifndef  _STANDALONE_
        newArr = (jobjectArray)env->NewObjectArray (iCount, clazz, NULL);
        if (newArr) {
                for (int i = 0; i < iCount; i++) {
                        utf_str = env->NewStringUTF (strCmdMatch[i]);
                        env->SetObjectArrayElement (newArr, i, utf_str);
                        env->DeleteLocalRef (utf_str);
                }
        }
#else
        for (int i = 0; i < iCount; i++) {
                PRINT_L(i, strCmdMatch[i]);
        }
#endif
  end_loop:
        mutex_unlock(&itrMutex);
        if (strCmdMatch) {
                delete []strCmdMatch;
        }
#ifndef  _STANDALONE_
	if (!newArr) {
		newArr = (jobjectArray)env->NewObjectArray (0, clazz, NULL);
	}
        return newArr;
#endif
}

//*********************************************************************************************************//
#ifndef  _STANDALONE_
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_loadForRegExp
(JNIEnv *env, jobject obj, jint version)
#else
        int loadForRegExp(int version)
#endif
{
	int ret = 0 ;
	mutex_lock(&itrMutex);
#ifndef  _STANDALONE_
	ret = mgr.addForRegExp(version, env, obj);
#else
	ret = mgr.addForRegExp(version);
#endif
	mutex_unlock(&itrMutex);
	return  ret ;
}

#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_executeForRegExp
(JNIEnv *env, jobject obj, jint ourProcId, jint polltype, jobjectArray procs, jint procslength)
#else
        void executeForRegExp(int ourProcId, int polltype, char **procs, int procslength)
#endif
{
	int i;
	int iCount = 0;
        char mLogMessage[255];
	RWCString *TempstrProcsName = NULL;
#ifndef  _STANDALONE_
	jstring utf_str;
	jclass clazz;
	clazz = env->FindClass ("java/lang/String");
        jobjectArray newArr = NULL;
#endif
        mutex_lock(&itrMutex);
        if (procslength <= 0) {
                goto out;
        }
        TempstrProcsName = new RWCString [procslength];
        if (!TempstrProcsName) {
                goto out;
        }
#ifndef  _STANDALONE_
	iCount = mgr.executeForRegExp(env, obj, ourProcId, polltype, procs, TempstrProcsName);
	env->DeleteLocalRef(procs);
	procs = NULL;
#else
	iCount = mgr.executeForRegExp(ourProcId, polltype, procs, procslength, TempstrProcsName);
#endif
        snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: javaCom: ENDS [%d], iCount= %d",
                 ourProcId, iCount);
#ifndef  _STANDALONE_
        logDbg(env,obj,mLogMessage);
#else
        logDbg(mLogMessage);
#endif

#ifndef  _STANDALONE_
        newArr = (jobjectArray)env->NewObjectArray (iCount, clazz, NULL);
        if (!newArr) {
                goto out;
        }
	for (i = 0; i < iCount; i++) {
                utf_str = env->NewStringUTF (TempstrProcsName[i]);
                env->SetObjectArrayElement (newArr, i, utf_str);
                env->DeleteLocalRef (utf_str);
        }
#else
	for (i = 0; i < iCount; i++) {
                PRINT(i, TempstrProcsName[i]);
        }
#endif
  out:
        if (TempstrProcsName) {
                delete []TempstrProcsName;
        }
        mutex_unlock(&itrMutex);
#ifndef  _STANDALONE_
	if (!newArr) {
		newArr = (jobjectArray)env->NewObjectArray (0, clazz, NULL);
	}
        return(newArr);
#endif
}


#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_getUserNameAndID
(JNIEnv *env, jobject obj, jint procID)
#else
        void getUserNameAndID(int procID)
#endif
{
	mutex_lock(&itrMutex);
	char mLogMessage[255];
        RWCString *strUserName = NULL;
	snprintf(mLogMessage,sizeof(mLogMessage),"Native javaCom: getUserName: STARTS [%d]", procID);
#ifndef  _STANDALONE_
        logDbg(env,obj,mLogMessage);
	int iCount = 0;
	jstring utf_str;
	jclass clazz;
	clazz = env->FindClass ("java/lang/String");
        jobjectArray newArr = NULL;
	iCount = info.getUserCount(env, obj);

	snprintf(mLogMessage,sizeof(mLogMessage),"Native javaCom: getUserName: [%d] [Count %d]", procID, iCount);
        logDbg(env,obj,mLogMessage);
#else
        logDbg(mLogMessage);
	int iCount = 0;
	iCount = info.getUserCount();

	snprintf(mLogMessage,sizeof(mLogMessage),"Native javaCom: getUserName: [%d] [Count %d]", procID, iCount);
        logDbg(mLogMessage);
#endif

	if(iCount == 0) {
		snprintf(mLogMessage,sizeof(mLogMessage),"Native javaCom: getUserName: IF PART [%d]", procID);
#ifndef  _STANDALONE_
                logDbg(env,obj,mLogMessage);
		newArr = (jobjectArray)env->NewObjectArray (1, clazz, NULL);
                if (!newArr) {
                        goto out;
                }
		utf_str = env->NewStringUTF ("<<error>>");
		env->SetObjectArrayElement (newArr, 0, utf_str);
		env->DeleteLocalRef (utf_str);
#else
                logDbg(mLogMessage);
                PRINT(0, "<<error>>");
#endif
	} else {
		//snprintf(mLogMessage,sizeof(mLogMessage),"Native javaCom: getUserName: ELSE PART [%d]", procID);
                //logDbg(env,obj,mLogMessage);
		strUserName = new RWCString[iCount];
                if (!strUserName) {
                        goto out;
                }
#ifndef  _STANDALONE_
		iCount = info.getUID(env, obj, iCount, strUserName);
		//snprintf(mLogMessage,sizeof(mLogMessage),"Native javaCom: getUserName: MARKER 1 [%d] [Count %d]", procID, iCount);
                //logDbg(env,obj,mLogMessage);
		newArr = (jobjectArray)env->NewObjectArray (iCount, clazz, NULL);
                if (!newArr) {
                        goto out;
                }
		for (int i = 0; i < iCount; i++) {
			utf_str = env->NewStringUTF (strUserName[i]);
			env->SetObjectArrayElement (newArr, i, utf_str);
			env->DeleteLocalRef (utf_str);
		}
		//snprintf(mLogMessage,sizeof(mLogMessage),"Native javaCom: getUserName: MARKER 2 [%d]", procID);
#else
		iCount = info.getUID(iCount, strUserName);
		for (int i = 0; i < iCount; i++) {
                        PRINT(i, strUserName[i]);
                }
#endif
		snprintf(mLogMessage,sizeof(mLogMessage),"Native javaCom: getUserName: MARKER 3 [%d]", procID);
#ifndef  _STANDALONE_
                logDbg(env,obj,mLogMessage);
#else
                logDbg(mLogMessage);
#endif
	}
out:
	if (strUserName) {
                delete [] strUserName;
        }
        mutex_unlock(&itrMutex);
	snprintf(mLogMessage,sizeof(mLogMessage),"Native javaCom: getUserName: ENDS [%d]", procID);
#ifndef  _STANDALONE_
        logDbg(env,obj,mLogMessage);
	if (!newArr) {
		newArr = (jobjectArray)env->NewObjectArray (0, clazz, NULL);
	}
	return (newArr);
#else
        logDbg(mLogMessage);
#endif
}

#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_getProcInfo
(JNIEnv *env, jobject obj, jint procId, jint osType)
#else
        void getProcInfo(int procId, int osType)
#endif
{
	mutex_lock(&itrMutex);
	char mLogMessage[255];
	int iCount = 0;
        RWCString *strProcsName = NULL;
	snprintf(mLogMessage,sizeof(mLogMessage),"Native javaCom: getProcInfo: STARTS [%d]", procId);
#ifndef  _STANDALONE_
	jstring utf_str;
	jclass clazz;
	clazz = env->FindClass ("java/lang/String");
        jobjectArray newArr = NULL;

        logDbg(env,obj,mLogMessage);
	iCount = info.getFullProcsInfoCount(env, obj, osType);
#else

        logDbg(mLogMessage);
	iCount = info.getFullProcsInfoCount(osType);
#endif

	if(iCount == 0) {
#ifndef  _STANDALONE_
		newArr = (jobjectArray)env->NewObjectArray (1, clazz, NULL);
                if (!newArr) {
                        goto out;
                }
		utf_str = env->NewStringUTF ("<<error>>");
		env->SetObjectArrayElement (newArr, 0, utf_str);
		env->DeleteLocalRef (utf_str);
#else
                PRINT(0, "<<error>>");
#endif
	} else {
		strProcsName = new RWCString [iCount];
#ifndef  _STANDALONE_
		iCount = info.getFullProcsInfo(env, obj, strProcsName, iCount, osType);
                snprintf(mLogMessage,sizeof(mLogMessage),"number of  procs: %d\n", iCount);
                logDbg(env, obj, mLogMessage);
		newArr = (jobjectArray)env->NewObjectArray (iCount, clazz, NULL);
		for (int i = 0; i < iCount; i++) {
			utf_str = env->NewStringUTF (strProcsName[i]);
			env->SetObjectArrayElement (newArr, i, utf_str);
			env->DeleteLocalRef (utf_str);
		}
#else
		iCount = info.getFullProcsInfo(strProcsName, iCount, osType);
		for (int i = 0; i < iCount; i++) {
                        PRINT_L(i, strProcsName[i]);
                }
#endif
	}
  out:
        if (strProcsName) {
                delete [] strProcsName;
        }
        mutex_unlock(&itrMutex);
	snprintf(mLogMessage,sizeof(mLogMessage),"Native javaCom: getProcInfo: ENDS [%d]", procId);
#ifndef  _STANDALONE_
        logDbg(env,obj,mLogMessage);
	if (!newArr) {
		newArr = (jobjectArray)env->NewObjectArray (0, clazz, NULL);
	}
        return (newArr);
#else
        logDbg(mLogMessage);
#endif
}

#ifndef  _STANDALONE_
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_test(JNIEnv *env)
#else
        void test(void)
#endif        
{
        int test;
#ifndef  _STANDALONE_
	jobjectArray newArr = NULL;
        return newArr;
#else
	return;
#endif
}


