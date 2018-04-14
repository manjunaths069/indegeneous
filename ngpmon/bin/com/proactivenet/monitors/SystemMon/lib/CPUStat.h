#ifndef _STAT_CPU_H__
#define _STAT_CPU_H__
#include "SingleCPUStat.h"
#include <jni.h>
class CPUStat
{
	private :
		SingleCPUStat * cpuArray[256];
		int mNumberOfCPUs;
		float mUs;
		float mSy;
		float mIO;
		float mRate ; //interrupt rate
		float mSr ; //Scan rate
		char * mErrorMessage;
	public :
		CPUStat();
		int init(kstat_ctl_t    *kc, JNIEnv *, jobject);
		int record(kstat_ctl_t    * kc, JNIEnv *, jobject);
		float us() {return mUs; }
		float sy() {return mSy; }
		float io() {return mIO; }
		float rate() {return mRate; }
		int numCPUS() { return mNumberOfCPUs;}
		float sr() {return mSr; }
		void print();
		void logDbg(JNIEnv *,jobject,char *);
		void logErr(JNIEnv *,jobject,char *);
		char * getErrorMessage(){return mErrorMessage;}
		void clean_up();
};
#endif
