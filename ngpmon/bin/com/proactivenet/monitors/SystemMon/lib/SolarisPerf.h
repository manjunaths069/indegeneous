#ifndef __SOLARIS_PERF_H_
#define _SLOARIS_PERF_H__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "VMStat.h"
#include "SysStat.h"
#include "CPUStat.h"
#include "MiscStat.h"
#include <kstat.h>
#include <jni.h>

class SolarisPerf
{
	private :
		SysStat mSysstat;
		VMStat mVmstat;
		CPUStat mCpustat;
		MiscStat mMicsStat;
		kstat_ctl_t    *mKstatCntrl;
		int open_kstat();
		void close_kstat ()
		{ kstat_close(mKstatCntrl); }
		char * mErrorMessage;
		int mErrorState ;
		long mBootTime ;
	public :
		SolarisPerf(JNIEnv *,jobject);
		int errorState() {return mErrorState ;}
		float us() {return mCpustat.us() ; }
		float sy() {return mCpustat.sy() ; } 
		float io() {return mCpustat.io() ; }
		float rate(){return mCpustat.rate() ; }
		long memTotal(){return  mVmstat.memTotal(); }
		long swapTotal() {return mVmstat.swapTotal(); }
		float mem() {return mVmstat.mem() ; }
		float swap() {return mVmstat.swap() ; }
		float sysload15(){return mMicsStat.sysload15() ; }
		float sysload1(){return mMicsStat.sysload1() ; }
		float sysload5(){return mMicsStat.sysload5() ; }
		long bootTime(){return mBootTime; }
		int noCPUS() { return mCpustat.numCPUS(); }
		float processes() { return mMicsStat.getNumOfProcesses() ; }
		float sr() { return mCpustat.sr() ; }
		int record(JNIEnv *, jobject);
		char * getErrorMessage() { return mErrorMessage; }
		long computeBootTimeFromVarAdmUtmp();
		void logDbg(JNIEnv *,jobject, char *);
		void logErr(JNIEnv *,jobject, char *);

// Added three new attributes - Rao  - 09/27/2K 
// Number of Runnable processes
// Number of processes Swapped
// Number of processes waiting for I/O

		float runque() {return mSysstat.runque() ; }
		float swpque() {return mSysstat.swpque() ; }
		float waiting() {return mSysstat.waiting() ; }
};
#endif 

