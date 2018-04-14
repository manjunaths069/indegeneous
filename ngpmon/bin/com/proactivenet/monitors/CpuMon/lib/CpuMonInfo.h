#ifndef _CpuMonInfo_H__
#define _CpuMonInfo_H__
#include <unistd.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <kstat.h>
#include <iostream.h>
#include <stdlib.h>
#ifndef  _STANDALONE_
#include <jni.h>
#endif

class CpuMonInfo
{
        private :
        public :
#ifndef  _STANDALONE_
                CpuMonInfo();
                int checkAvailability (int instId, JNIEnv * env, jobject obj);
                int getCpuStatName(int instId,char *CpuStatName,JNIEnv * env, jobject obj );
                int getData(int instId,float val[11],JNIEnv * env, jobject obj);
                int getProcessors ();
                void logDbg(JNIEnv *, jobject, char *);
                void logErr(JNIEnv *, jobject, char *);
                //void print(float *vals);
#else
        CpuMonInfo();
        int checkAvailability (int instId);
        int getCpuStatName(int instId,char *CpuStatName);
        int getData(int instId,float val[11]);
        int getProcessors ();
        void logDbg(char *);
        void logErr(char *);
#endif
};
#endif
