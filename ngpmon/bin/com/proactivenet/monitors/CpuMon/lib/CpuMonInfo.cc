#include <stdio.h>
#include "CpuMonInfo.h"
#include <errno.h>

#define  MAX_CPU_LEN  32

CpuMonInfo::CpuMonInfo()
{
}
/* --------------------- Changes for JNI logging START------------------------------*/
#ifndef  _STANDALONE_
void CpuMonInfo::logDbg(JNIEnv *env, jobject obj, char *logStr)
#else
void CpuMonInfo::logDbg(char *logStr)
#endif
{
        if(logStr == NULL)
                return;
#ifndef  _STANDALONE_
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL)
        {
              mid = env->GetMethodID(cls,"logDbg","(Ljava/lang/String;)V");
              if(mid != NULL)
              {
                      jstring tString = env->NewStringUTF(logStr);
                      env->CallVoidMethod(obj,mid,tString);
                      env->DeleteLocalRef(tString);
              }
        }
#else
        fprintf(stdout, "%s\n", logStr);
#endif        
}

#ifndef  _STANDALONE_
void CpuMonInfo::logErr(JNIEnv *env, jobject obj, char *errStr)
#else
void CpuMonInfo::logErr(char *errStr)
#endif
{
        if(errStr == NULL)
                return;
#ifndef  _STANDALONE_
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL)
        {
              mid = env->GetMethodID(cls,"logErr","(Ljava/lang/String;)V");
              if(mid != NULL)
              {
                      jstring tString = env->NewStringUTF(errStr);
                      env->CallVoidMethod(obj,mid,tString);
                      env->DeleteLocalRef(tString);
              }
        }
#else
        fprintf(stdout, "%s\n", errStr);
#endif
}

/* --------------------- Changes for JNI logging END------------------------------*/


#ifndef  _STANDALONE_
int CpuMonInfo::checkAvailability (int instId,JNIEnv * env,jobject obj)
#else
int CpuMonInfo::checkAvailability (int instId)
#endif
{
        kstat_t *k_space;
        void *val_ptr;

        char mLogString[256] = {0};
        snprintf(mLogString,sizeof(mLogString),"Native Logging: Function checkAvailability: Start");
#ifndef  _STANDALONE_
        logDbg(env,obj,mLogString);
#else
        logDbg(mLogString);
#endif        

        errno = 0;
        kstat_ctl_t *kc = kstat_open ();
        if (kc == NULL)
        {
                snprintf(mLogString,sizeof(mLogString),"Native Logging: Function checkAvailability: kstat_open fail. Error code %d : END",errno);
#ifndef  _STANDALONE_
                logErr(env,obj,mLogString);
#else
                logErr(mLogString);
#endif
                return -1;
        }

        for (k_space = kc->kc_chain; k_space != NULL; k_space = k_space->ks_next)
        {
                        if(k_space->ks_type == KSTAT_TYPE_RAW &&
                                !strcmp(k_space->ks_module,"cpu_stat") )
                        {
                                if (k_space->ks_instance == instId)
                                {
                                        kstat_close (kc);
                                        snprintf(mLogString,sizeof(mLogString),"Native Logging: Function checkAvailability: Successful : END");
#ifndef  _STANDALONE_
                                        logDbg(env,obj,mLogString);
#else
                                        logDbg(mLogString);
#endif        
                                        return 0;
                                }
                        }
        }

        kstat_close (kc);
        snprintf(mLogString,sizeof(mLogString),"Native Logging: Function checkAvailability: Could not find 'cpu_stat' pattern : END");
#ifndef  _STANDALONE_
                logErr(env,obj,mLogString);
#else
                logErr(mLogString);
#endif
        return -1;
}

#ifndef  _STANDALONE_
int CpuMonInfo::getCpuStatName (int instId,char *CpuStatName, JNIEnv * env, jobject obj)
#else
int CpuMonInfo::getCpuStatName (int instId,char *CpuStatName)
#endif
{
        kstat_t *k_space;
        void *val_ptr;
        char mLogString[256];
        snprintf(mLogString,sizeof(mLogString),"Native Logging: Function getCpuStatName: Start");
#ifndef  _STANDALONE_
        logDbg(env,obj,mLogString);
#else
        logDbg(mLogString);
#endif        
        errno =0;
        kstat_ctl_t *kc = kstat_open ();
        if (kc == NULL)
        {
                snprintf(mLogString,sizeof(mLogString),"Native Logging: Function getCpuStatName: kstat_open fail. Error code %d : END",errno);
#ifndef  _STANDALONE_
                logErr(env,obj,mLogString);
#else
                logErr(mLogString);
#endif
                return -1;
        }

        for (k_space = kc->kc_chain; k_space != NULL; k_space = k_space->ks_next)
        {
                        if(k_space->ks_type == KSTAT_TYPE_RAW &&
                                !strcmp(k_space->ks_module,"cpu_stat") )
                        {

                                if (k_space->ks_instance == instId && k_space->ks_name)
                                {
					snprintf(CpuStatName, MAX_CPU_LEN, "%s", k_space->ks_name);
                                        kstat_close (kc);
                                        snprintf(mLogString,sizeof(mLogString),"Native Logging: Function getCpuStatName: %s: Successful : END", CpuStatName);
#ifndef  _STANDALONE_
        logDbg(env,obj,mLogString);
#else
        logDbg(mLogString);
#endif        
                                        return 0;
                                }
                        }
        }
        kstat_close (kc);
        snprintf(mLogString,sizeof(mLogString),"Native Logging: Function getCpuStatName: Could not find 'cpu_stat' pattern : END");
#ifndef  _STANDALONE_
                logErr(env,obj,mLogString);
#else
                logErr(mLogString);
#endif
        return -1;
}

//Assumes that the kstat_open is called for kc just before the call of record
#ifndef  _STANDALONE_
int CpuMonInfo::getData(int instId,float *vals, JNIEnv * env, jobject obj)
#else
int CpuMonInfo::getData(int instId,float *vals)
#endif
{
        int mInstId = instId;
        char mCPUStatName[MAX_CPU_LEN+1] = {0};
        char mLogString[256];
        snprintf(mLogString,sizeof(mLogString),"Native Logging: Function getData: Start");
#ifndef  _STANDALONE_
        logDbg(env,obj,mLogString);
#else
        logDbg(mLogString);
#endif        

#ifndef  _STANDALONE_
                if ( getCpuStatName(mInstId,mCPUStatName,env,obj) != 0 )
#else                        
                if ( getCpuStatName(mInstId,mCPUStatName) != 0 )
#endif
                {

                        snprintf(mLogString,sizeof(mLogString),"Native Logging: Function getData: call to getCpuStatName failed : END");
#ifndef  _STANDALONE_
                logErr(env,obj,mLogString);
#else
                logErr(mLogString);
#endif
                        return -1;
                }

        kstat_t *k_space;
        void *val_ptr;
        kstat_ctl_t *kc = kstat_open ();
        if (kc == NULL)
        {
                snprintf(mLogString,sizeof(mLogString),"Native Logging: Function getData: kstat_open failed. returned NULL : END");
#ifndef  _STANDALONE_
                logErr(env,obj,mLogString);
#else
                logErr(mLogString);
#endif
                return -1;
        }

        k_space = kstat_lookup(kc,"cpu_stat",mInstId,mCPUStatName);
        if (k_space == NULL)
        {
                kstat_close (kc);
                snprintf(mLogString,sizeof(mLogString),"Native Logging: Function getData: kstat_lookup failed. returned NULL : END");
#ifndef  _STANDALONE_
                logErr(env,obj,mLogString);
#else
                logErr(mLogString);
#endif
                return -1;
        }

        if (kstat_read(kc,k_space,NULL) == -1 )
        {
                snprintf(mLogString,sizeof(mLogString),"Native Logging: Function getData: kstat_read failed. returned -1 : END");
#ifndef  _STANDALONE_
                logErr(env,obj,mLogString);
#else
                logErr(mLogString);
#endif
                kstat_close (kc);
                return -1;
        }
        if ( k_space->ks_data ==  NULL )
        {
                snprintf(mLogString,sizeof(mLogString),"Native Logging: Function getData: After read kstat_data is NULL for modulename cpu_stat and stat name = cpu_stat0 : END");
#ifndef  _STANDALONE_
                logErr(env,obj,mLogString);
#else
                logErr(mLogString);
#endif
                kstat_close (kc);
                return -1;
        }
        cpu_stat myStat ; //Change it to pinter
        memcpy(&myStat, k_space->ks_data,sizeof(cpu_stat ));

        vals[0] = ( myStat.cpu_sysinfo).cpu[0] ;
        vals[1] = ( myStat.cpu_sysinfo).cpu[1] ;
        vals[2] = ( myStat.cpu_sysinfo).cpu[2] ;

        vals[3] = ( myStat.cpu_sysinfo).cpu[3] ;
        vals[4] = (myStat.cpu_sysinfo).xcalls;
        vals[5] =  (myStat.cpu_sysinfo).cpumigrate; //Fix-Raghu
        vals[6] = (myStat.cpu_sysinfo).pswitch;
        vals[7] = (myStat.cpu_sysinfo).syscall;
        vals[8] = (myStat.cpu_sysinfo).intrthread;
        vals[9] = (myStat.cpu_sysinfo).inv_swtch;
        vals[10]= ( myStat.cpu_sysinfo).intr ;
        vals[11] = ( myStat.cpu_vminfo).scan ;

        kstat_close (kc);

        snprintf(mLogString,sizeof(mLogString),"Native Logging: Function getData: END");
#ifndef  _STANDALONE_
        logDbg(env,obj,mLogString);
#else
        logDbg(mLogString);
#endif        

        return 0 ;
}

/*void
CpuMonInfo::print(float *vals)
{
        cout << "User CPU = " << vals[0] << " \n Sysstem CPU = " << vals[1] << "\n  IO Wait = " << vals[2] << endl<< "Interrupt = " << vals[3]<< endl << "ScanRate = " << vals[4]<< endl << "Context Switches = " << vals[5]<< endl << "Involuntory cntxt switches = "<< vals[6]<< endl << "System calls = "<< vals[7]<< endl << "Interrupts as threads = "<< vals[8]<< endl << "Cross calls to CPUs = " << vals[9]<< endl << "CPU migrations by threads = " << vals[10]<< endl;
}*/


/*int main (int argc, char **argv)
{
        if (argc <= 1)
        {
                cout << "Usage ./a.out <Processor ID>" << endl;
                return 0;
        }
        CpuMonInfo myCpuStat;
        while (1)
        {
                float vals[12];
                if (myCpuStat.checkAvailability (atoi(argv[1])))
                {
                        cout << "Processor does not exist." << endl;
                        return 0;
                }
                if (myCpuStat.getData (atoi(argv[1]),vals) == -1)
                {
                        cout << "ERROR in getting data" << endl;
                        return 0;
                }

                myCpuStat.print (vals);
                sleep (5);
        }

        return (0);
}*/
int CpuMonInfo::getProcessors ()
{
        kstat_t *k_space;
        int i =0;

        kstat_ctl_t *kc = kstat_open ();
        if (kc == NULL)
        {
                //cout << "Error in kstat" << endl;
                //kstat_close (kc);
                return -1;
        }

        for (k_space = kc->kc_chain; k_space != NULL; k_space = k_space->ks_next)
        {
                        if(k_space->ks_type == KSTAT_TYPE_RAW &&
                                !strcmp(k_space->ks_module,"cpu_stat") )
                        {
                                i++;
                        }
                }

        kstat_close (kc);
        return i;
}

