/*
* @(#)nodemon.cpp
* Native function call used by nodemon.java
* provide NTPing, NTPingStartUp, NTPingDone in JAVA code :
*    int NTPing(int control, int ip, int timeout)
*    int NTPingDone()
*    int NTPingStartUp()
*/

#include <jni.h>
#include "nodemon.h"
#include "nticmp.h"

JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_nodemon_nodemon_NTPingStartUp
  (JNIEnv *env, jobject obj)
{
  return NTPingStartUp();
}

JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_nodemon_nodemon_NTPingDone
  (JNIEnv *env, jobject obj, jint control);
{
  return NTPingDone((NTPINGCONTROL)control);
}

JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_nodemon_nodemon_NTPing
  (JNIEnv *env, jobject obj, jint control, jstring ip, jint timeout)
{
  int responsetime;
  const char *ipstring = env->GetStringUTFChars(ip, 0);
  responsetime = NTPing((NTPINGCONTROL)control, ipstring, (int)timeout);
  env->ReleaseStringUTFChars(ip, ipstring);
  
  return responsetime;
}
