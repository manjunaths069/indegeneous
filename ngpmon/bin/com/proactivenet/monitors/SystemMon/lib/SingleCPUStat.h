#ifndef _SINGLE_CPU_H__
#define _SINGLE_CPU_H__
#include <unistd.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <kstat.h>
#include <iostream.h>
#include <stdlib.h>
class SingleCPUStat
{
	private :
		char * mCPUStatName ; 
		int mInstanceId;
		float mUs;
		float mSy;
		float mIO;
		unsigned long mSum;
		unsigned long mPrevStateCounters[4];
		unsigned long mCurrentStateCounter[4] ;
		unsigned long mPrevInterrupts ;
		unsigned long mInterrupts ;
		unsigned long mScans ;
		unsigned long mPrevScans ;
		float mIntrRate;
		float mSr; //Scan rate
		time_t mPrevTime ;
		time_t mRecordTime ;
		char * mErrorMessage;
	public :
		SingleCPUStat(char * name , int instanceid);
		int record( kstat_ctl_t    * kc );
		float us() {return mUs; }
		float sy() {return mSy; }
		float io() {return mIO; }
		float rate() {return mIntrRate; }
		float sr() {return mSr; }
		long getCPUCouterSum() { return mSum; }
		void print();
		char * getErrorMessage(){return mErrorMessage;}
};
#endif
