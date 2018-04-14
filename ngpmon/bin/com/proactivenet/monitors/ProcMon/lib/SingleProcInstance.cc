#include "SingleProcInstance.h"
#include <unistd.h>

#define FRC2PCT(pp) (((float)(pp))/0x8000*100)

SingleProcInstance::SingleProcInstance(RWCString * pid )
{
		mKey  = pid;
		double mSize  = 0 ; //in MB
		double mRsSize  = 0 ; //in MB
		mThreads   = 0; //Number of lwps
		mPrevInCpuSec  = -1 ; //In cpu time when last poll was called
		mInCpuSec  = -1 ; //In cpu time of now
		mPrevRecordTime = 0; //Time stamp of the last poll
		mRecordTime = 0; //Time stamp of this poll
		mPrevCPUNenoSec = 0 ;
		mCPUNenoSec = 0 ;
		mDeltaInCPUSec = -1 ;
		mDeltaSec = 0 ;
		mPageSize =  sysconf(_SC_PAGESIZE) /1024 ; //in kb
		//Ashish
		totalSize = 0;
		totalRsSize = 0;
                mCpuPerc = 0;
}
SingleProcInstance::~SingleProcInstance()
{
//        cout << "---" << *mKey << endl;
//        delete mKey;
}
int 
SingleProcInstance::execute(psinfo ps )
{
	//Reset attributes
	mSize = 0 ;
	mRsSize = 0 ;
	mThreads = 0 ;
	mPrevInCpuSec  = mInCpuSec ;
	mPrevRecordTime = mRecordTime ;
	mPrevCPUNenoSec = mCPUNenoSec ;
	mInCpuSec  = ps.pr_time.tv_sec;
	mCPUNenoSec  = ps.pr_time.tv_nsec;
	mRecordTime = time(0);
	//Ashish
	totalSize = 0;
	totalRsSize = 0;
	if ( mPrevInCpuSec != -1 )
	{
		mDeltaInCPUSec  = (1000000 * ( mInCpuSec - mPrevInCpuSec ) )
							+ ((mCPUNenoSec - mPrevCPUNenoSec ) / 1000 );
		mDeltaSec = mRecordTime - mPrevRecordTime ;
	}
	else
	{
		mDeltaInCPUSec = -1 ;
		mDeltaSec - 1 ;
	}
	mSize = ps.pr_size ; //Size in kb
	//totalSize += mSize; //in KB
	totalSize += ps.pr_size; //in KB
	mRsSize = ps.pr_rssize ; //Size in kb
	//totalRsSize += mRsSize;//in KB
	totalRsSize += ps.pr_rssize;//in KB
	mThreads = ps.pr_nlwp ;
	mFds = getFdCount((char * ) mKey->data());
	if ( mFds < 0 )
		mFds = 0;
        mCpuPerc = FRC2PCT(ps.pr_pctcpu);
	return 0;
}
int 
SingleProcInstance::execute(prpsinfo ps )
{
	//Reset attributes
	//ASHISH CHANGES START//
	totalSize = 0;
	totalRsSize = 0;
	//ASHISH CHANGES END//
	mSize = 0 ;
	mRsSize = 0 ;
	mThreads = 0 ;
	mPrevInCpuSec  = mInCpuSec ;
	mPrevRecordTime = mRecordTime ;
	mPrevCPUNenoSec = mCPUNenoSec ;
	mInCpuSec  = ps.pr_time.tv_sec;
	mCPUNenoSec  = ps.pr_time.tv_nsec;
	mRecordTime = time(0);
	if ( mPrevInCpuSec != -1 )
	{
		mDeltaInCPUSec  = (1000000 * ( mInCpuSec - mPrevInCpuSec ) )
							+ ((mCPUNenoSec - mPrevCPUNenoSec ) / 1000 );
		mDeltaSec = mRecordTime - mPrevRecordTime ;
	}
	else
	{
		mDeltaInCPUSec = -1 ;
		mDeltaSec - 1 ;
	}
	mSize = ps.pr_size * mPageSize; //Size in kb
	//totalSize += mSize;//in KB
	totalSize += ps.pr_size * mPageSize;//in KB
	mRsSize = ps.pr_rssize * mPageSize; //Size in kb
	//totalRsSize += mRsSize;//in KB
	totalRsSize += ps.pr_rssize * mPageSize;//in KB
	mThreads = 0 ;
        mCpuPerc = FRC2PCT(ps.pr_pctcpu);
	return 0 ;
}
void 
SingleProcInstance::print()
{
	cout << " mDetlaCp->" <<  mDeltaInCPUSec << "\n"
		<<  "SIZE ->" << mSize << "\n"
		<<  "Resident SIZE ->" << mRsSize << "\n"
		<<  "THREADS ->" << mThreads << "\n"
		<< endl ;
}
int 
SingleProcInstance::getFdCount(char * pid )
{
	struct dirent *direntP;
	char dirName[32] = {0};
// 	strcpy(dirName,"/proc/");
// 	strcat(dirName,pid);
// 	strcat(dirName,"/fd");
	DIR *  procDir;

        snprintf(dirName, sizeof(dirName), "/proc/%s/fd", pid);

	if (!(procDir = opendir (dirName)))
	{
	/*  Changes by Arvind For ARS - 5474
	 *  For any error in getting the Fd Count 
 	 *  we return -2, we are not logging it to
	 *  java.agent.stderr, as it will increase its size.
	 */
		if ( errno == 13 ) //13 == permission denied
		{
			snprintf(mErrorMessage,sizeof(mErrorMessage),"2102151 Unable to open Directory %s.\nError is %s", dirName,strerror(errno));
			return -2 ;
		}

		//snprintf(mErrorMessage,sizeof(mErrorMessage),"Unable to open to %s", dirName);
		//return -1;

		return -2;
	}

	int count = 0 ;
	for (rewinddir (procDir); (direntP = readdir (procDir));)
	{ 
		int fd;

		if (direntP->d_name[0] == '.')
		continue ;
		count++;
	}
	closedir(procDir);
	return count ;
}
