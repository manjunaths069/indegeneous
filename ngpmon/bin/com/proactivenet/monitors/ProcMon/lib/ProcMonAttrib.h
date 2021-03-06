/*
 *
 */
#ifndef  _STANDALONE_
/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo */

#ifndef _Included_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo
#define _Included_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo
#ifdef __cplusplus
extern "C" {
#endif
/* Inaccessible static: mOSType */
/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo
 * Method:    getCmdLines
 * Signature: (Ljava/lang/String;I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getCmdLines
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo
 * Method:    getListofProcs
 * Signature: (I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getListofProcs
  (JNIEnv *, jobject, jint);

//********************* A S H I S H *************************
/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo
 * Method:    getUserIDs
 * Signature: (I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getUserIDs
	(JNIEnv *, jobject, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo
 * Method:    getUserProcesses
 * Signature: (I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getUserProcesses
  (JNIEnv *, jobject, jint, jstring);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo
 * Method:    getUserCmdLines
 * Signature: (I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getUserCmdLines
  (JNIEnv *, jobject, jstring, jint, jstring);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo
 * Method:    ValidateUserID
 * Signature: (I)[Ljava/lang/String;
 */
JNIEXPORT jlong JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_ValidateUserID
	(JNIEnv *, jobject, jstring, jstring, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo
 * Method:    getListofFilteredProcs
 * Signature: (I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getListofFilteredProcs
	(JNIEnv *, jobject, jobjectArray, jint, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo
 * Method:    getListOfFilteredCmdLines
 * Signature: (I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getListOfFilteredCmdLines
	(JNIEnv *, jobject, jobjectArray, jstring, jint, jint, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo
 * Method:    getListOfFilteredCmdLinesProcReg
 * Signature: (I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getListOfFilteredCmdLinesProcReg
	(JNIEnv *, jobject, jstring, jobjectArray, jint, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo
 * Method:    getListOfFilteredCmdLinesUserRegProcReg
 * Signature: (I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getListOfFilteredCmdLinesUserRegProcReg
	(JNIEnv *, jobject, jobjectArray, jobjectArray, jint, jint, jint);

/*
 * Class:     com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo
 * Method:    getListofAllCmdLines
 * Signature: (I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_ProcMon_ProcMonAttributeInfo_getListofAllCmdLines
	(JNIEnv *, jobject, jint, jstring);

#ifdef __cplusplus
}
#endif
#endif
#else  /* _STANDALONE_ */

/* return: objectarray*/
void getCmdLines(char *, int);
/* return: objectarray*/
void getListofProcs(int);
/* return: objectarray */
void getUserIDs(int);
/* return: objectarray */
void getUserProcesses(int, char *);
/* return: objectarray */
void getUserCmdLines(char *, int, char *);
long ValidateUserID(char *, char *, int);
/* return: objectarray */
void getListofFilteredProcs(char **, int, int);
/* return: objectarray */
void getListOfFilteredCmdLines(char **, char *, int, int, int);
/* return: objectarray */
void getListOfFilteredCmdLinesProcReg(char *, char **, int, int);
/* return: objectarray */
void getListOfFilteredCmdLinesUserRegProcReg(char **, char **, int, int, int);
/* return: objectarray */
void getListofAllCmdLines(int, char *);

#endif
