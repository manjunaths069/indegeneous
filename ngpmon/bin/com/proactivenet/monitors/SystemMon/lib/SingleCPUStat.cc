#include <stdio.h>
#include "SingleCPUStat.h"
SingleCPUStat::SingleCPUStat(char * name , int instance)
{
	mCPUStatName = new char[256];
	strcpy(mCPUStatName,name) ; 
	mInstanceId = instance ;
	mErrorMessage = new char[256];
	for (int i = 0 ; i < 4  ; i++ )
	{
		mPrevStateCounters[i] = 0 ;
		mCurrentStateCounter[i] = 0 ;
	}
	mUs = 0;
	mSy =0 ;
	mIO = 0;
	mSum = 0;
	mScans = 0 ;
	mPrevScans = 0 ;
	mPrevInterrupts = 0   ;
	mInterrupts = 0   ;
	mIntrRate = 0 ;
	mSr = 0 ;
	mPrevTime = 0  ;
	mRecordTime  = 0 ;
}
//Assumes that the kstat_open is called for kc just before the call of record
int 
SingleCPUStat::record( kstat_ctl_t    * kc )
{
	kstat_t *k_space;
	void *val_ptr;
	k_space = kstat_lookup(kc,"cpu_stat",mInstanceId,mCPUStatName);
	//k_space = kstat_lookup(kc,"cpu_stat",mInstanceId,"cpu_stat0");
	if (k_space == NULL)
	{
		sprintf(mErrorMessage,"kstat lookup failed for module name cpu_stat "
							  " Instance Id = %d and stat name = %s.", 
							  mInstanceId,mCPUStatName );
		return -1;
	}
	//Difference in the statistics
	int diff [4] ;
	/*  Do we need to update every time. We are going to have kstat_open call 
		before call of reacord
	if (kstat_chain_update(kc) == -1 )
	{
		cout << "Update failed" << endl;
		return -1;
	}
	*/
	if (kstat_read(kc,k_space,NULL) == -1 )
	{
		sprintf(mErrorMessage,"kstat read failed for module name cpu_stat "
							  "and stat name = %s and ks_instance = %d"
							  , mCPUStatName ,k_space->ks_instance);
		return -1;
	}
	if ( k_space->ks_data ==  NULL )
	{
		sprintf(mErrorMessage,"After read kstat_data is NULL for module name cpu_stat "
							  "and stat name = %s and ks_instance = %d"
							  , mCPUStatName ,k_space->ks_instance);
		return -1;
	}
	cpu_stat myStat ; //Change it to pinter
	memcpy(&myStat, k_space->ks_data,sizeof(cpu_stat ));
	//cout << "size " <<  sizeof(cpu_stat) << endl ;

	mPrevStateCounters[0] = mCurrentStateCounter[0] ;
	mPrevStateCounters[1] = mCurrentStateCounter[1] ;
	mPrevStateCounters[2] = mCurrentStateCounter[2] ;
	mPrevStateCounters[3] = mCurrentStateCounter[3] ;
	mCurrentStateCounter[0] = ( myStat.cpu_sysinfo).cpu[0] ;
	mCurrentStateCounter[1] = ( myStat.cpu_sysinfo).cpu[1] ;
	mCurrentStateCounter[2] = ( myStat.cpu_sysinfo).cpu[2] ;
	mCurrentStateCounter[3] = ( myStat.cpu_sysinfo).cpu[3] ;
	diff[0] = mCurrentStateCounter[0] - mPrevStateCounters[0] ;
	diff[1] = mCurrentStateCounter[1] - mPrevStateCounters[1] ;
	diff[2] = mCurrentStateCounter[2] - mPrevStateCounters[2] ;
	diff[3] = mCurrentStateCounter[3] - mPrevStateCounters[3] ;
	mSum = diff[0] + diff[1] + diff[2] + diff[3] ;
	//handling rate
	mPrevTime = mRecordTime ;
	mRecordTime = time(0) ;

	mPrevInterrupts = mInterrupts ;
	mInterrupts = ( myStat.cpu_sysinfo).intr ;

	mPrevScans =  mScans ;
	mScans = ( myStat.cpu_vminfo).scan ;
	if ( mRecordTime == mPrevTime )
	{
		//Means that the funciton is called in quick successioin
		return 0 ;
	}
	mIntrRate = ((double)( mInterrupts - mPrevInterrupts ))/(mRecordTime - mPrevTime ) ;
	mSr = ((double)(mScans - mPrevScans )) / (mRecordTime - mPrevTime ) ;
	if ( mPrevStateCounters[0] > 0 )
	{
		mUs = ((float)( diff[1] ) / mSum ) * 100 ;
		mSy = ((float)( diff[2] ) / mSum  ) * 100;
		mIO = ((float)( diff[3] ) / mSum  ) * 100 ;
	}
	else
	{
		// "Fisrt data point " << endl ;
		mUs = -1 ;
		mSy = -1 ;
		mIO = -1 ;
		mSum = -1 ;
		mIntrRate = -1 ;
		mSr = -1 ;
	}			
	return 0 ;
}
void 
SingleCPUStat::print()
{
	printf("%0.2f   %0.2f   %0.2f	sum = %d	%0.2f %0.2f\n",	
	mUs,mSy,mIO,mSum,mIntrRate, mSr);
}
