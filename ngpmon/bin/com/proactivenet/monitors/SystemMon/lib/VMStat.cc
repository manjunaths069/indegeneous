#include <stdio.h>
#include "VMStat.h"
VMStat::VMStat()
{
        mErrorMessage = new char[256];
        mTotalSwap = 0 ;
        mPercentMemUsed = 0 ;
        mSwapUsed = 0 ;
        mPrevTime  = 0 ;
        mRecordTime  = 0 ;
        mPageSize =  sysconf(_SC_PAGESIZE) ;
        longlong_t pages = sysconf(_SC_PHYS_PAGES);
        mTotalMemeory = mPageSize *  pages;
        mPrevSwapResv = 0 ;
        mPrevSwapAvail = 0 ;
        mPrevFreemem = 0 ;
        mSwapResv = 0 ;
        mSwapAvail = 0 ;
        mFreemem = 0 ;
}
/* --------------------- Changes for JNI logging START------------------------------*/

void VMStat::logDbg(JNIEnv *env, jobject obj, char *logStr)
{
        if(logStr == NULL)
                return;
        char mlogStr[2048];
        strcpy(mlogStr,logStr);
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL)
        {
              mid = env->GetMethodID(cls,"logDbg","(Ljava/lang/String;)V");
              if(mid != NULL)
              {
                      jstring tString = env->NewStringUTF(mlogStr);
                      env->CallVoidMethod(obj,mid,tString);
                      env->DeleteLocalRef(tString);
              }
        }
}

void VMStat::logErr(JNIEnv *env, jobject obj, char *errStr)
{
        if(errStr == NULL)
                return;
        char merrStr[2048];
        strcpy(merrStr,errStr);
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL)
        {
              mid = env->GetMethodID(cls,"logErr","(Ljava/lang/String;)V");
              if(mid != NULL)
              {
                      jstring tString = env->NewStringUTF(merrStr);
                      env->CallVoidMethod(obj,mid,tString);
                      env->DeleteLocalRef(tString);
              }
        }
}

/* --------------------- Changes for JNI logging END------------------------------*/

//Assumes that the kstat_open is called for kc just before the call of record
int
VMStat::record( kstat_ctl_t    * kc,JNIEnv * env,jobject obj )
{
        /* --------------------- Changes for JNI logging START------------------------------*/
        char mLogMessage[1024];
        sprintf(mLogMessage,"Native Logging: VMStat: function record: Start");
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        kstat_t *k_space;
        void *val_ptr;
        k_space = kstat_lookup(kc,"unix",0,"vminfo");
        if (k_space == NULL)
        {
                sprintf(mErrorMessage,"kstat lookup failed for module name = unix "
                                                          "and stat name = vminfo." );
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: VMStat: function record: %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/

                return -1;
        }
        //Difference in the statistics
        if (kstat_read(kc,k_space,NULL) == -1 )
        {
                sprintf(mErrorMessage,"kstat read failed for module name = unix "
                                                          "and stat name = vminfo." );
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: VMStat: function record: %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/

                return -1;
        }
        if ( k_space->ks_data ==  NULL )
        {
                sprintf(mErrorMessage,"kstat returned null data after read "
                                                        "for module name = unix "
                                                          "and stat name = vminfo." );
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: VMStat: function record: %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/

                return -1;
        }
        vminfo myStat ; //Change it to pinter
        memcpy(&myStat, k_space->ks_data,sizeof(vminfo ));
        mPrevSwapResv =  mSwapResv ;
        mPrevSwapAvail = mSwapAvail;
        mPrevFreemem = mFreemem;
        mSwapResv = myStat.swap_resv ;
        mSwapAvail = myStat.swap_avail ;
        mFreemem = myStat.freemem ;
        mPrevTime = mRecordTime ;
        mRecordTime = time(0);
        long deltaTime = mRecordTime - mPrevTime ;
        if ( !deltaTime )
        {
                //ignore this funciton is called in succession
                return 0;
        }

        //Padmini's code change for ARS 7839 - 15/09/2003 - Begin

        //Check for division by zero

        if(mPageSize == 0)
        {
                sprintf(mErrorMessage,"Page Size if zero.");
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: VMStat: function record: %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/
                return -1;
        }//End of if
        //Padmini's code change for ARS 7839 - 15/09/2003 - End

        longlong_t deltaMem = mFreemem - mPrevFreemem ;
        longlong_t deltaSwapResv = mSwapResv - mPrevSwapResv ;
        longlong_t deltaSwapAvail = mSwapAvail - mPrevSwapAvail ;

        //Padmini's code change for ARS 7839 - 15/09/2003 - Begin

        //Check for division by zero

        if((deltaSwapResv + deltaSwapAvail)  == 0)
        {
                sprintf(mErrorMessage,"Error while calculating swap used.");
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: VMStat: function record: %s",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/
                return -1;
        }//End of if
        //Padmini's code change for ARS 7839 - 15/09/2003 - End

        if ( mPrevFreemem ) //If any of the prev != 0, means no first data point
        {
/* --------  ARS 13033 - Pecent Mem Used records Negative Value  --------- */

		if(mTotalMemeory > 0)
		{
               		mTotalSwap = ((deltaSwapAvail + deltaSwapResv )/ deltaTime) * mPageSize;
                	mPercentMemUsed = (1 - (
                                                                (double)((deltaMem/deltaTime)*mPageSize)
                                                                ) / mTotalMemeory) * 100;


/*
The old formula given above was very readable, although the calculations are correct.
Changed the formula to a 2 step process. 
First we calculate the Free memory by divinding it with time and multiplying with page size.
This is the same way in which total Memory is calculated.
To avoid negative values, we compare free memory with total memory.
If free memory is greater than total memory then we record NO_DATA
else
We calculate used memory by subtracting: -> m = total - free.
To get the percentage this is again divided by total memory and then multiplied by 100. :-> r = (m/totalMem) * 100
*/

                	sprintf(mLogMessage,"Native Logging: VMSTAT:Percent Memory used using OLD FORMULA %f",mPercentMemUsed);
                	logDbg(env,obj,mLogMessage);
			double mFreeMemory = (double)((deltaMem/deltaTime)*mPageSize);
			if(mFreeMemory > mTotalMemeory)
                		mPercentMemUsed  = -1 ;
			else
				mPercentMemUsed = (( mTotalMemeory - mFreeMemory  ) / mTotalMemeory ) * 100;
                	sprintf(mLogMessage,"Native Logging: VMSTAT:Percent Memory used using NEW FORMULA %f",mPercentMemUsed);
                	logDbg(env,obj,mLogMessage);

		}
		else
		{
			mTotalMemeory = -1;
                	mPercentMemUsed  = -1 ;
		}
/* -----------  ARS Chagnes End ---------------- */


                //The expression is divided by 1024 and then multiplied by 1024
                //so the when we typecast deltaSwapResv(longlong_t) to double
                //we don't loose any thing.
                //We have to type cast it to double so that we can show that data
                //in percentage
                mSwapUsed = ((double)(deltaSwapResv /1024)  / (deltaSwapResv + deltaSwapAvail) ) * 100 * 1024;
        }
        else
        {
                // "Fisrt data point " << endl ;
                mPercentMemUsed  = -1 ;
                mSwapUsed = -1 ;
        }
        /* --------------------- Changes for JNI logging START------------------------------*/
        sprintf(mLogMessage,"Native Logging: VMStat: function record: END");
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        return 0 ;
}
