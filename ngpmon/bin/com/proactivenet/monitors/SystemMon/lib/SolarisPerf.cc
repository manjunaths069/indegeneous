#include "SolarisPerf.h"
#include <iostream.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream.h>
#include <utmp.h>
#include <unistd.h>




SolarisPerf::SolarisPerf(JNIEnv * env, jobject obj)
{
        mErrorMessage = new char[256];
        mErrorState = 0 ;

        /* --------------------- Changes for JNI logging START------------------------------*/
        char mLogMessage[255];
        sprintf(mLogMessage,"Native Logging: constructor SolarisPerf: Start");
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        open_kstat();

        /* --------------------- Changes for JNI logging START------------------------------*/
        sprintf(mLogMessage,"Native Logging: constructor SolarisPerf: initialized kstat");
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        mErrorState = mCpustat.init(mKstatCntrl,env,obj);
        //Following two line of the code is commented becase
        //we do no want to record data for a few milliseconds
        //if ( ! mErrorState )
        //      mErrorState = mCpustat.record(mKstatCntrl);
        if ( ! mErrorState )
                mErrorState = mSysstat.record(mKstatCntrl,env,obj);
        if ( ! mErrorState )
                mErrorState = mVmstat.record(mKstatCntrl,env,obj);
        if ( ! mErrorState )
                mErrorState = mMicsStat.record(mKstatCntrl,env,obj);
        if ( ! mErrorState )
                sleep(1);
        //Called again to call the swap data
        mErrorState = mVmstat.record(mKstatCntrl,env,obj);
        close_kstat();
        //For solaris 2.5.1 boot time cannot be obtained for
        // kstat, so we have to get it from /var/adm/utmp file
        mBootTime = mMicsStat.bootTime() ;
        if ( mBootTime == 0 )
                mBootTime = computeBootTimeFromVarAdmUtmp();
        /* --------------------- Changes for JNI logging START------------------------------*/
        sprintf(mLogMessage,"Native Logging: constructor SolarisPerf: Error State %d: END",mErrorState);
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/


}
/* --------------------- Changes for JNI logging START------------------------------*/

void SolarisPerf::logDbg(JNIEnv *env, jobject obj, char *logStr)
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

void SolarisPerf::logErr(JNIEnv *env, jobject obj, char *errStr)
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

int SolarisPerf::open_kstat()
{
        mKstatCntrl = kstat_open();
        if (mKstatCntrl == NULL )
        {
                sprintf(mErrorMessage,"Could not open ksat ");
                return -1;
        }
        return 0;
}
int SolarisPerf::record(JNIEnv * env,jobject obj)
{
        /* --------------------- Changes for JNI logging START------------------------------*/
        char mLogMessage[2048];
        sprintf(mLogMessage,"Native Logging: function record: Start");
        logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        if ( open_kstat() == -1 )
        {
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: function record: Failed to open kstat");
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/
                return -1 ;
        }
        mErrorState=0;
        strcpy(mErrorMessage,"");
        if ( ! mErrorState )
        {
                mErrorState = mMicsStat.record(mKstatCntrl,env,obj);
                strcpy(mErrorMessage, mMicsStat.getErrorMessage() );
        }
        if ( ! mErrorState )
        {
                mErrorState = mVmstat.record(mKstatCntrl,env,obj);
                strcpy(mErrorMessage,mVmstat.getErrorMessage() );
        }
        if ( ! mErrorState )
        {
                mErrorState = mSysstat.record(mKstatCntrl,env,obj);
                strcpy(mErrorMessage,mSysstat.getErrorMessage() );
        }
        if ( ! mErrorState )
        {
                mErrorState = mCpustat.record(mKstatCntrl,env,obj);
                strcpy(mErrorMessage,mCpustat.getErrorMessage() );
                //Padmini's code change for Walmart fix begin
                //The kstat closed is being called, because the pointer to kstat control structure mKstatCntrl may be out of date with the kernal kstat. It is reopend, and the values are reintialised.
                if(mErrorState != 0)
                {
                        mCpustat.clean_up();
                        close_kstat();
                        if(open_kstat() == -1)
                        {
                                return -1;
                        }
                        mCpustat.init(mKstatCntrl,env,obj);
                }
                //Padmini's code change for Walmart fix end
        }
        close_kstat();
        if(strlen(mErrorMessage) > 1)
        {
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: function record: Error message is : %s. END",mErrorMessage);
                logErr(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/
        }
        else
        {
                /* --------------------- Changes for JNI logging START------------------------------*/
                sprintf(mLogMessage,"Native Logging: function record:  END");
                logDbg(env,obj,mLogMessage);
                /* --------------------- Changes for JNI logging END------------------------------*/

        }

        return  mErrorState ;
}
long
SolarisPerf::computeBootTimeFromVarAdmUtmp()
{
    struct stat buf; //contains the attribute of the file
    cout << "Size of the struct is " << sizeof(utmp) << endl ;
    int fd = open(UTMP_FILE,O_RDONLY);
    if (fd == -1 )
    {
                sprintf(mErrorMessage,"Cannot open file %s",UTMP_FILE) ;
                mErrorState = -1 ;
        return -1 ;
    }
    if (fstat(fd,&buf) != 0 )
    {
        sprintf(mErrorMessage,"Failed fstat  file %s",UTMP_FILE) ;
        mErrorState = -1 ;

        //Padmini's code change for ARS 7839 - 15/09/2003 - Begin
        //fd needs to be closed.

        close(fd);
        //Padmini's code change for ARS 7839 - 15/09/2003 - End

        return -1 ;
    }
    int sizeOfFile = buf.st_size ;
    char *  fileContent = new char [sizeOfFile + 1 ] ;
    int bytes = read(fd,fileContent,sizeOfFile);
    int times = bytes/ sizeof(utmp) ;
    char * st = new char [sizeof(utmp)] ;
    for (int i = 0 ; i < times ; i ++ )
    {
        memcpy(st,(fileContent + ( i * sizeof(utmp))), sizeof(utmp));
        utmp * ut = (utmp *)st;
        if ( ut->ut_type == 2)
                {
                        delete st ;
                        delete fileContent ;

                        //Padmini's code change for ARS 7839 - 15/09/2003 - Begin
                        //fd needs to be closed.

                        close(fd);
                        //Padmini's code change for ARS 7839 - 15/09/2003 - End

                        return ut->ut_time;
                }
    }
    delete st ;
        delete fileContent ;

        //Padmini's code change for ARS 7839 - 15/09/2003 - Begin
        //fd needs to be closed.

        close(fd);
        //Padmini's code change for ARS 7839 - 15/09/2003 - End

}
