/*
* @(#)nfsmon.cc 
* Native function call used by nfsmon.java
* provide nfsRespone in JAVA code : int nfsResponse(String hostname)
*/
#include <stdlib.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <nfs/nfs.h>
#include <rpcsvc/mount.h>
#include <rpc/rpc.h>
#include <jni.h>
#include "nfsmon.h"

JNIEXPORT jint JNICALL 
Java_com_proactivenet_monitors_nfsmon_nfsmon_nfsResponse
    (JNIEnv *env, jobject obj, jstring hostname)
{
    clnt_stat rcode;
    const char *str = env->GetStringUTFChars(hostname, 0);
    char name[65];
    strcpy(name, str);
    env->ReleaseStringUTFChars(hostname, str);

    rcode = rpc_call(name, NFS_PROGRAM, NFS_VERSION, RFS_NULL, (xdrproc_t) xdr_void, 0, (xdrproc_t) xdr_void, 0, 0);
    return rcode;
}
