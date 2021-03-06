/*
 * ProcMon.h
 */
#ifndef  _STANDALONE_
/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_proactivenet_monitors_ProcMon_ProcInfo */

#ifndef _Included_com_proactivenet_monitors_ProcMon_ProcInfo
#define _Included_com_proactivenet_monitors_ProcMon_ProcInfo
#ifdef __cplusplus
extern "C" {
#endif
/* Inaccessible static: mClassName */
/* Inaccessible static: mOSType */
/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcInfo
 * Method:    execute
 * Signature: (II)V
 */
JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_execute
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcInfo
 * Method:    get
 * Signature: (II)F
 */
JNIEXPORT jfloat JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_get
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcInfo
 * Method:    getErrMessage
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_getErrMessage
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcInfo
 * Method:    load
 * Signature: (ILjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_load
  (JNIEnv *, jobject, jint, jstring, jstring, jstring);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcInfo
 * Method:    unLoad
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_unLoad
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcInfo
 * Method:    execute
 * Signature: (II)V
 */
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_executeAtUserLevel
  (JNIEnv *, jobject, jint, jint, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcInfo
 * Method:    loadForRegExp
 * Signature: (ILjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_loadForRegExp
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcInfo
 * Method:    executeForRegExp
 * Signature: (II)V
 */
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_executeForRegExp
 (JNIEnv *, jobject, jint, jint, jobjectArray, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcInfo
 * Method:    getUserNamdAndID
 * Signature: (II)V
 */
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_getUserNameAndID
 (JNIEnv *, jobject, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcInfo
 * Method:    getProcInfo
 * Signature: (II)V
 */
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcInfo_getProcInfo
 (JNIEnv *, jobject, jint, jint);


#ifdef __cplusplus
}
#endif
#endif

#else  /* _STANDALONE_ */

char *execute(int, int);
float get(int, int);
char *getErrMessage(int i);
int load(int, char *, char *, char *);
void unLoad(int);

/* return: objectarray...*/
void executeAtUserLevel(int, int, int);

int loadForRegExp(int);
/* return: objectarray*/
void executeForRegExp(int, int, char **, int);
/* return: objectarray*/
void getUserNameAndID(int);
/* return: objectarray*/
void getProcInfo(int, int);

#endif
