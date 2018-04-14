/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_proactivenet_monitors_TraceRouteMon_Computation */

#ifndef _Included_com_proactivenet_monitors_TraceRouteMon_Computation
#define _Included_com_proactivenet_monitors_TraceRouteMon_Computation
#ifdef __cplusplus
extern "C" {
#endif
/* Inaccessible static: threadInitNumber */
/* Inaccessible static: stopThreadPermission */
#undef com_proactivenet_monitors_TraceRouteMon_Computation_MIN_PRIORITY
#define com_proactivenet_monitors_TraceRouteMon_Computation_MIN_PRIORITY 1L
#undef com_proactivenet_monitors_TraceRouteMon_Computation_NORM_PRIORITY
#define com_proactivenet_monitors_TraceRouteMon_Computation_NORM_PRIORITY 5L
#undef com_proactivenet_monitors_TraceRouteMon_Computation_MAX_PRIORITY
#define com_proactivenet_monitors_TraceRouteMon_Computation_MAX_PRIORITY 10L
#undef com_proactivenet_monitors_TraceRouteMon_Computation_REPLY
#define com_proactivenet_monitors_TraceRouteMon_Computation_REPLY 0L
#undef com_proactivenet_monitors_TraceRouteMon_Computation_ERROR
#define com_proactivenet_monitors_TraceRouteMon_Computation_ERROR 11L
#undef com_proactivenet_monitors_TraceRouteMon_Computation_INVALID
#define com_proactivenet_monitors_TraceRouteMon_Computation_INVALID -1L
#undef com_proactivenet_monitors_TraceRouteMon_Computation_IPADDRESS
#define com_proactivenet_monitors_TraceRouteMon_Computation_IPADDRESS 0L
#undef com_proactivenet_monitors_TraceRouteMon_Computation_HOSTNAME
#define com_proactivenet_monitors_TraceRouteMon_Computation_HOSTNAME 1L
/* Inaccessible static: storevalues */
/* Inaccessible static: addressHolder */
/* Inaccessible static: Pingtable */
/*
 * Class:     com_proactivenet_monitors_TraceRouteMon_Computation
 * Method:    initializeNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_proactivenet_monitors_TraceRouteMon_Computation_initializeNative
  (JNIEnv *, jclass);

/*
 * Class:     com_proactivenet_monitors_TraceRouteMon_Computation
 * Method:    openRawSocket
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_TraceRouteMon_Computation_openRawSocket
  (JNIEnv *, jclass);

/*
 * Class:     com_proactivenet_monitors_TraceRouteMon_Computation
 * Method:    sendRequest
 * Signature: (JLjava/lang/String;II)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_TraceRouteMon_Computation_sendRequest
  (JNIEnv *, jclass, jlong, jstring, jint, jint);

/*
 * Class:     com_proactivenet_monitors_TraceRouteMon_Computation
 * Method:    setalldeleted
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_proactivenet_monitors_TraceRouteMon_Computation_setalldeleted
  (JNIEnv *, jclass);

/*
 * Class:     com_proactivenet_monitors_TraceRouteMon_Computation
 * Method:    startListening
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_proactivenet_monitors_TraceRouteMon_Computation_startListening
  (JNIEnv *, jobject);

/*
 * Class:     com_proactivenet_monitors_TraceRouteMon_Computation
 * Method:    verifyTarget
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_TraceRouteMon_Computation_verifyTarget
  (JNIEnv *, jclass, jstring, jint);

#ifdef __cplusplus
}
#endif
#endif