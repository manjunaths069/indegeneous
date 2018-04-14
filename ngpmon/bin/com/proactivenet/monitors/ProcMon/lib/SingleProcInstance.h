#ifndef _SINGLEPROCINSTANCE_H__
#define _SINGLEPROCINSTANCE_H__
#include <iostream.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/param.h>  /* for HZ -- should be in <time.h> ? */
#include <sys/time.h>
#include <sys/types.h>
#include <procfs.h>
#include <dirent.h>
#include <rw/cstring.h>
#include "our_old_procfs.h"
class SingleProcInstance 
{
	private :
		RWCString * mKey  ; //pid
		//float mCPU  ; //%
		double mSize  ; //in kB
		double mRsSize  ; //in kB
		float mThreads  ; //Number of lwps
		float mFds  ; //open file descriptors
		time_t mPrevInCpuSec ; //In cpu time when last poll was called
		time_t mInCpuSec ; //In cpu time of now
		long mPrevCPUNenoSec ;
		long mCPUNenoSec ;
		time_t mPrevRecordTime; //Time stamp of the last poll
		time_t mRecordTime; //Time stamp of this poll
		char  mErrorMessage[256] ;
		int isRecorded; //0 or 1
		double mDeltaInCPUSec ;
		double mDeltaSec ;
		long mPageSize ;
		//Ashish 
		double totalSize;
		double totalRsSize;
        float mCpuPerc;
	public:
		SingleProcInstance(RWCString  * pid );
		~SingleProcInstance();
		int execute(psinfo ps);
		int execute(prpsinfo ps);
		double deltaMilliCPUSec()
		{ return mDeltaInCPUSec; }
		double deltaSecs()
		{return mDeltaSec ; }
		float getThreads ()
		{ return mThreads ;}
		float getFds ()
		{ return mFds ;}
		float getSize()
		{ return mSize;}
		float getRsSize()
		{ return mRsSize;}
		int getFdCount(char * pid );
		char * getErrorMsg()
		{ 
			return mErrorMessage ;
		}
		void print();
		//Ashish
		float getTotalSize() {
			return totalSize;
		}
		
		float getTotalRsSize() {
			return totalRsSize;
		}
		float getCpuPerc() {
			return mCpuPerc;
		}
}; // End SingleProcInstance class
#endif
