/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation */

#ifndef _Included_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation
#define _Included_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation
#ifdef __cplusplus
extern "C" {
#endif
/* Inaccessible static: threadInitNumber */
/* Inaccessible static: stopThreadPermission */
#undef com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_MIN_PRIORITY
#define com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_MIN_PRIORITY 1L
#undef com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_NORM_PRIORITY
#define com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_NORM_PRIORITY 5L
#undef com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_MAX_PRIORITY
#define com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_MAX_PRIORITY 10L
#undef com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_REPLY
#define com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_REPLY 0L
#undef com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_ERROR
#define com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_ERROR 11L
#undef com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_UNREACHABLE
#define com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_UNREACHABLE 3L
#undef com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_PORT_UNREACHABLE
#define com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_PORT_UNREACHABLE 3L
#undef com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_INVALID
#define com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_INVALID -1L
#undef com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_IPADDRESS
#define com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_IPADDRESS 0L
#undef com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_HOSTNAME
#define com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_HOSTNAME 1L
/* Inaccessible static: storevalues */
/* Inaccessible static: addressHolder */
/* Inaccessible static: Pingtable */
/* Inaccessible static: UDPReqtable */
/* Inaccessible static: UDPPingtable */
/*
 * Class:     com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation
 * Method:    initializeNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_initializeNative
  (JNIEnv *, jclass);

/*
 * Class:     com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation
 * Method:    openRawSocket
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_openRawSocket
  (JNIEnv *, jclass);

/*
 * Class:     com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation
 * Method:    sendRequest
 * Signature: (JLjava/lang/String;II)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_sendRequest
  (JNIEnv *, jclass, jlong, jstring, jint, jint);

/*
 * Class:     com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation
 * Method:    sendUDPRequest
 * Signature: (JLjava/lang/String;IIS)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_sendUDPRequest
  (JNIEnv *, jclass, jlong, jstring, jint, jint, jshort);

/*
 * Class:     com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation
 * Method:    setalldeleted
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_setalldeleted
  (JNIEnv *, jclass);

/*
 * Class:     com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation
 * Method:    startListening
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_startListening
  (JNIEnv *, jobject);

/*
 * Class:     com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation
 * Method:    verifyTarget
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_ICMPUDPTraceRouteMon_Computation_verifyTarget
  (JNIEnv *, jclass, jstring, jint);

#ifdef __cplusplus
}
#endif
#endif
