#include "ProcInfo.h"
#include "our_old_procfs.h"
#include <rw/ctoken.h>
#include <stdlib.h>
#include <kstat.h>
#include  <strings.h>

#define  ZERO(x)  bzero(x, sizeof(x))
#define  NEED_PID(model) (model != PR_MODEL_NATIVE && model == PR_MODEL_LP64)
#define  FRC2PCT(pp) (((float)(pp))/0x8000*100)

/********ARS 8894*******************/
#include "LongArgs.h"
/***************************/

//*********LOGGING START*********************************

#ifndef  _STANDALONE_
void logDbg1(JNIEnv *env, jobject obj, char *logStr) {
	if(logStr == NULL)
		return;
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL) {
                mid = env->GetMethodID(cls, "logDbg", "(Ljava/lang/String;)V");
                if(mid != NULL) {
                        jstring tString = env->NewStringUTF(logStr);
                        env->CallVoidMethod(obj, mid, tString);
                        env->DeleteLocalRef(tString);
		}
	}
}

void logErr1(JNIEnv *env, jobject obj, char *errStr) {
	if(errStr == NULL)
                return;
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL) {
                mid = env->GetMethodID(cls, "logErr", "(Ljava/lang/String;)V");
                if(mid != NULL) {
                        jstring tString = env->NewStringUTF(errStr);
                        env->CallVoidMethod(obj, mid, tString);
                        env->DeleteLocalRef(tString);
		}
	}
}
#else
void logDbg1(char *logStr)
{
        if (!logStr) {
                return;
        }
        fprintf(stdout, "DBG: %s\n", logStr);
}
void logErr1(char *errStr)
{
        if (!errStr) {
                return;
        }
        fprintf(stdout, "ERR: %s\n", errStr);
}
#endif

//***********LOGGING ENDS*********************************

ProcInfo::ProcInfo(int osVersion, RWCString processName,
                RWCString commandLineMatch, RWCString userName)
        : mProcIdDict(RWCString::hash)
        , mNewProcIdDict(RWCString::hash) {
    mProcName = processName ;
    ZERO(mErrorMessage);
    mCommandLineMatch = commandLineMatch ;
    mNumberOfInstances = -1 ;
    mNumberOfNewInstances = -1;
    mSumCPUUtillization = -1 ;
    mMaxSize = -1 ;
    mMaxRsSize = -1 ;
    mSumThreads = 0 ;
    mSumFds = 0 ;
    mErrorState = 0 ;
    mOSVersionFlag = osVersion ; //0 = 5.5 and 1 = 5.6
    mAvailableFlag = 0 ;
    // Ashish modified for supporting the User specific polling
    mAvailableFlagAtUserLvl = 0;
    mUserName = userName;
    // Ashish modified for getting the value for new stats attributes
    mTotalRsSize = -1;
	mTotalSize = -1;
    // ENDS

    mNumCpus = getNuberOfCPUs() ;
    // cout << "Number of CPU = " << mNumCpus << endl ;
}

//*************For RegExp
ProcInfo::ProcInfo(int osVersion)
        : mProcIdDict(RWCString::hash)
        , mNewProcIdDict(RWCString::hash) {
	char mLogString[1280];
	mNumberOfInstances = -1 ;
    mNumberOfNewInstances = -1;
    mSumCPUUtillization = -1 ;
    mMaxSize = -1 ;
    mMaxRsSize = -1 ;
    mSumThreads = 0 ;
    mSumFds = 0 ;
    mErrorState = 0 ;
    mOSVersionFlag = osVersion ; //0 = 5.5 and 1 = 5.6
    mAvailableFlag = 0 ;
    mAvailableFlagAtUserLvl = 0;
    mTotalRsSize = -1;
	mTotalSize = -1;
    mNumCpus = getNuberOfCPUs() ;
    //mUserIDName;
    ZERO(mErrorMessage);
}
//ASHISH/
//extern "C" dirent *readdir_r(DIR *__dp, struct dirent *__ent);



//*************************

ProcInfo::ProcInfo()
        : mProcIdDict(RWCString::hash)
        , mNewProcIdDict(RWCString::hash) {
    ZERO(mErrorMessage);
    mErrorState = 0 ;
    mAvailableFlag = 0 ;
}

//void ProcInfo::clean_up()
ProcInfo::~ProcInfo()
{
        mProcIdDict.clearAndDestroy();
        mNewProcIdDict.clearAndDestroy();
}

#ifndef  _STANDALONE_
char * ProcInfo::execute(JNIEnv *env, jobject jobj, int pollType) {
#else
char * ProcInfo::execute(int pollType) {
#endif
        char *out = NULL;
	if (pollType == 0)
	{
#ifndef  _STANDALONE_
	  out = executeRegular(env, jobj);
#else
	  out = executeRegular();
#endif
        }
	else
	{
#ifndef  _STANDALONE_
	  executeFast(env, jobj);
#else
	  executeFast();
#endif
	}
        return out;
}

#ifndef  _TRY_OLD_EXECUTION_ 
#ifndef  _STANDALONE_
char * ProcInfo::executeRegular(JNIEnv *env, jobject jobj)
#else
char * ProcInfo::executeRegular(void)
#endif
{
#define   PID_MAX_LEN   8
        char ProcPid[PID_MAX_LEN+1] = {0} ;
        int result  = 0;
        mNumberOfInstances = 0 ;
        mNumberOfNewInstances = 0 ;
        mSumCPUUtillization = -1 ;
        mAvailableFlag = 0 ;
        double cpuSecMillis = 0 ;
        double mRealSec  = 0;
        float tmpSize = 0 ;
        mMaxSize = 0 ;
        mMaxRsSize = 0 ;
        mSumThreads = 0 ;
        mSumFds = 0 ;
        mErrorState = 0 ;
        int countOfContrinutingProcs = 0 ;
        // Ashish modified for supporting the User specific polling
        mTotalSize = 0;
        mTotalRsSize = 0;
        char mLogString[128];
        int userFlag = 0;
        long uid = -857320316;
        if (strcmp(mUserName, "< ALL USERS >") == 0) {
                userFlag = 1;
        } else {
                uid = atol(mUserName);
        }
        snprintf(mLogString,sizeof(mLogString),"UID(Execute Reg):%d", uid);
#ifndef  _STANDALONE_
        logDbg1(env,jobj,mLogString);
#else
        logDbg1(mLogString);
#endif
        psinfo currproc ;
        struct dirent *direntp;
        static DIR *procdir;
	int fd = 0;
	char name[32] = {0};
        int isProcNotMatched = 0;
        int psinfo_args_len;
        RWCString full_cmd;
        RWCString pid_arr;
        seteuid(0);

        ZERO(mErrorMessage);

        if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
                mErrorState = -1 ;
                return NULL;
	}

        /**
         * Store pid's from mNewProcIdDict.
         */
        PidHash pidHash(RWCString::hash);
        PidPtrHashItr itr(mNewProcIdDict);
        for(;itr();) {
                pidHash.insert(*itr.key());
        }
	for (rewinddir (procdir); (direntp = readdir (procdir));) {
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
		fd = open (name, O_RDONLY);
		if (fd <= -1 ) {
                        if ((!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
                                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
                                         name,strerror(errno));
                                mErrorState = 100 ;
			}
			continue ;
		}
                int byteRead = read(fd, &currproc, sizeof(currproc));
                if ( byteRead <= 0 ) {
                        close(fd);
                        continue;
                }
                (void) close (fd);
                if (userFlag == 0 && currproc.pr_uid != uid) {
                        continue;
                }
                psinfo_args_len = strlen(currproc.pr_psargs);
                full_cmd = currproc.pr_psargs;
                if (psinfo_args_len > 70) {
                        full_cmd = longargs(currproc);
                        if (full_cmd.length() < psinfo_args_len) {
                                full_cmd = currproc.pr_psargs;
                        }
                }
                isProcNotMatched = procNameMatched(full_cmd);
                if (isProcNotMatched) {
                        isProcNotMatched = procNameMatched(currproc.pr_fname);
                }

                if(mCommandLineMatch != "" && mProcName == "" )
                {
                        isProcNotMatched=0;
                }
                if ( !isProcNotMatched ) {
                        if (mCommandLineMatch != ""  && !commandLineMatchFound(full_cmd)) {
                                continue;
                        }
                        RWCString  pid = direntp->d_name;
                        snprintf(mLogString,sizeof(mLogString),"<< PROCESS ADDED >> %d", currproc.pr_pid);
#ifndef  _STANDALONE_
                        logDbg1(env,jobj,mLogString);
#else
                        logDbg1(mLogString);
#endif
                        updateProcStats(currproc);
                	pidHash.remove(direntp->d_name);
                        if (NEED_PID(currproc.pr_dmodel)) {
                                snprintf(ProcPid, sizeof(ProcPid), "%s", direntp->d_name);
                                pid_arr += direntp->d_name;
                                pid_arr += " ";
                        }
                }
        }
        closedir(procdir);
        /**
         * Remove pid's from mNewProcIdDict that do not exist.
         */
        RWTValHashTableIterator<RWCString> pidItr(pidHash);
        for (; pidItr();) {
               RWCString tmpPid = pidItr.key (); 
                RWCString *oldPid = mNewProcIdDict.remove(&tmpPid);
                if (oldPid) {
                        delete oldPid;
                }
        }
        pidHash.clear();
//                 mAvailableFlagAtUserLvl = 1;
//                 if (mSumCPUUtillization < 0) {
//                         mSumCPUUtillization = 0.0;
//                 }
//                 RWCString *pid = new RWCString(direntp->d_name);
//                 SingleProcInstance proc_inst(pid);
//                 proc_inst.execute(currproc);
//                 tmpSize = proc_inst.getSize();
//                 if (NEED_PID(currproc.pr_dmodel)) {
//                         snprintf(ProcPid,sizeof(ProcPid),"%s", direntp->d_name);
//                 }
//                 if (mMaxSize < tmpSize) {
//                         mMaxSize = tmpSize;
//                 }
//                 mTotalSize += proc_inst.getTotalSize();
//                 tmpSize = proc_inst.getRsSize();
//                 if (mMaxRsSize < tmpSize) {
//                         mMaxRsSize = tmpSize;
//                 }
//                 mTotalRsSize += proc_inst.getTotalRsSize();
//                 mSumThreads += proc_inst.getThreads () ;
//                 mSumFds += proc_inst.getFds() ;
//                 mSumCPUUtillization += proc_inst.getCpuPerc();
//                 mAvailableFlag = 100 ;
//                 mNumberOfInstances++;
//                 if (!mNewProcIdDict.find(pid)) {
//                         mNewProcIdDict.insertKeyAndValue(pid,NULL);
//                 } else {
//                         delete pid;
//                 }
//         }
//         closedir(procdir);
//                         SingleProcInstance * inst = mProcIdDict.findValue(&pid) ;
//                         if ( inst ==  0 ) {
//                                 RWCString * npid = new RWCString(pid) ;
//                                 inst = new SingleProcInstance(npid);
//                                 result = inst->execute(currproc);

//                                 if ( result == -1 ) {
//                                         delete inst;
//                                         delete npid;
//                                         continue;
//                                 }
//                                 mProcIdDict.insertKeyAndValue(npid,inst);
//                                 mNumberOfNewInstances ++ ;
		 
//                                 if(inst->getSize() == 0)
//                                 {
//                                         strcpy(ProcPid,pid.data());
//                                 }
//                         } else {
//                                 result = inst->execute(currproc);
//                                 if ( result == -1 ) {
//                                         sprintf (mErrorMessage, inst->getErrorMsg());
//                                         continue;
// //                                         mErrorState = -1 ;
// //                                         closedir(procdir);
// //                                         (void) oldPids.clearAndDestroy();
// //                                         uid = NULL;
// //                                         return NULL;
//                                 }

//                                 //We just want to record data for not-new processes
//                                 if ( inst->deltaMilliCPUSec() > -1 ) {
//                                         cpuSecMillis += inst->deltaMilliCPUSec()  ;
//                                         mRealSec  += inst->deltaSecs() ;
//                                         tmpSize = inst->getSize();
//                                         //Nanda to support 64 bit process Starts
//                                         if(tmpSize == 0)
//                                         {
//                                                 strcpy(ProcPid,pid.data());
//                                         }
//                                         // Nanda to support 64 bit process Ends
	            
//                                         if (mMaxSize < tmpSize)
//                                                 mMaxSize = tmpSize ;
//                                         // Ashish: Getting the value for Total Virtual Size
//                                         mTotalSize += inst->getTotalSize();
//                                         tmpSize = inst->getRsSize();
//                                         if (mMaxRsSize < tmpSize)
//                                                 mMaxRsSize = tmpSize ;
//                                         // Ashish: Geting the value for Total Resident Size
//                                         mTotalRsSize += inst->getTotalRsSize();
//                                         mSumThreads += inst->getThreads () ;
//                                         mSumFds += inst->getFds () ;
//                                         countOfContrinutingProcs ++ ;
//                                 }
//                                 RWCString  * tmpPid = oldPids.remove(&pid); //Remove if from old
//                                 delete tmpPid ;
//                         }
//                 }
//         }
//         closedir(procdir);
//         if (procFound != 0) {
//                 if ( countOfContrinutingProcs > 0  ) {//Means first time
//                         // half second is substracted from cpuSecMillis to compensate for its
//                         // being in nano second
//                         mSumCPUUtillization =  cpuSecMillis  /
//                                 (( mRealSec * 10000  * mNumCpus ) / countOfContrinutingProcs );
//                         // cpuSecMillis has the precision of millisec and real sec has precision of sec
//                         // So it is probable we can get percentage more that 100
//                         if ( mSumCPUUtillization > 100 ) {
//                                 mSumCPUUtillization = 100 ;
//                         }
//                         if ( mSumCPUUtillization < 0 )
//                                 mSumCPUUtillization = 0 ;
//                 }
//                 while ( oldPids.entries()) {
//                         RWCString * opid = oldPids.get();
//                         SingleProcInstance * killedProc = mProcIdDict.findValue(opid);
//                         mProcIdDict.remove(opid);
//                         delete opid ;
//                         delete killedProc ;
//                 }

//                 mNumberOfInstances =  mProcIdDict.entries();
//                 if ( mNumberOfInstances )
//                         mAvailableFlag = 100 ;
//         }
//         (void) oldPids.clearAndDestroy();
//         uid = NULL;

	char *tmp = NULL;
	if (pid_arr.length()) {
		tmp = (char *)calloc(pid_arr.length()+1, sizeof(char));
		snprintf(tmp, pid_arr.length(), "%s", pid_arr.data());
	} else {
		tmp = (char *)calloc(PID_MAX_LEN+1, sizeof(char));
		snprintf(tmp,PID_MAX_LEN,"null");
	}
        return tmp;
}
#else  // _TRY_OLD_EXECUTION_
#ifndef  _STANDALONE_
char * ProcInfo::executeRegular(JNIEnv *env, jobject jobj)
#else
char * ProcInfo::executeRegular(void)
#endif
{
        char ProcPid[8] = {0} ;
        int result  = 0;
        mNumberOfInstances = 0 ;
        mNumberOfNewInstances = 0 ;
        mSumCPUUtillization = -1 ;
        mAvailableFlag = 0 ;
        double cpuSecMillis = 0 ;
        double mRealSec  = 0;
        float tmpSize = 0 ;
        mMaxSize = 0 ;
        mMaxRsSize = 0 ;
        mSumThreads = 0 ;
        mSumFds = 0 ;
        mErrorState = 0 ;
        int countOfContrinutingProcs = 0 ;
        
        // Ashish modified for supporting the User specific polling
        mTotalSize = 0;
        mTotalRsSize = 0;
        char mLogString[128];
        int userFlag = 0;
        long uid = -857320316;
        if (strcmp(mUserName, "< ALL USERS >") == 0) {
                userFlag = 1;
        } else {
                uid = atol(mUserName);
        }
        snprintf(mLogString,sizeof(mLogString),"UID(Execute Reg):%d", uid);
#ifndef  _STANDALONE_
        logDbg1(env,jobj,mLogString);
#else
        logDbg1(mLogString);
#endif
        psinfo currproc ;
        struct dirent *direntp;
        static DIR *procdir;
        seteuid(0);

        char errStr[384];
        RWCString longargs_name;
        int procFound = 0;
        ZERO(mErrorMessage);

        if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
                mErrorState = -1 ;
                return NULL;
	}
        RWTPtrSlist <RWCString>  oldPids;
        RWTPtrHashDictionaryIterator<RWCString,SingleProcInstance>
                itr(mProcIdDict);
        for(;itr();)
                oldPids.append(new RWCString(*itr.key()));

	int fd = 0;
	char name[32] = {0};
        int isProcNotMatched = 0;
        int psinfo_args_len;
        RWCString full_cmd;

	for (rewinddir (procdir); (direntp = readdir (procdir));) {
		fd = 0;
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
		fd = open (name, O_RDONLY);
		if (fd == -1 ) {
                        if ((!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
                                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
                                         name,strerror(errno));
                                mErrorState = 100 ;
			}
			continue ;
		}
                int byteRead = read(fd, &currproc, sizeof(currproc));
                if ( byteRead <= 0 ) {
                        close(fd);
                        continue;
                }
                (void) close (fd);
                if (userFlag == 0 && currproc.pr_uid != uid) {
                        continue;
                }
                psinfo_args_len = strlen(currproc.pr_psargs);
                full_cmd = currproc.pr_psargs;
                if (psinfo_args_len > 70) {
                        full_cmd = longargs(currproc);
                        if (full_cmd.length() < psinfo_args_len) {
                                full_cmd = currproc.pr_psargs;
                        }
                }
                isProcNotMatched = procNameMatched(full_cmd);
                if (isProcNotMatched) {
                        isProcNotMatched = procNameMatched(currproc.pr_fname);
                }

                if(mCommandLineMatch != "" && mProcName == "" )
                {
                        isProcNotMatched=0;
                }
                // NandaKumar Modification Ends
                if ( !isProcNotMatched ) {
                        if (mCommandLineMatch != ""  && !commandLineMatchFound(full_cmd)) {
                                continue;
                        }
                        RWCString  pid = direntp->d_name;
                        snprintf(mLogString,sizeof(mLogString),"<< PROCESS ADDED >> %d", currproc.pr_pid);
#ifndef  _STANDALONE_
                        logDbg1(env,jobj,mLogString);
#else
                        logDbg1(mLogString);
#endif
                        procFound = 1;
                        SingleProcInstance * inst = mProcIdDict.findValue(&pid) ;
                        if ( inst ==  0 ) {
                                RWCString * npid = new RWCString(pid) ;
                                inst = new SingleProcInstance(npid);
                                result = inst->execute(currproc);

                                if ( result == -1 ) {
                                        delete inst;
                                        delete npid;
                                        continue;
                                }
                                mProcIdDict.insertKeyAndValue(npid,inst);
                                mNumberOfNewInstances ++ ;
		 
                                if(inst->getSize() == 0)
                                {
                                        strcpy(ProcPid,pid.data());
                                }
                        } else {
                                result = inst->execute(currproc);
                                if ( result == -1 ) {
                                        sprintf (mErrorMessage, inst->getErrorMsg());
                                        continue;
//                                         mErrorState = -1 ;
//                                         closedir(procdir);
//                                         (void) oldPids.clearAndDestroy();
//                                         uid = NULL;
//                                         return NULL;
                                }

                                //We just want to record data for not-new processes
                                if ( inst->deltaMilliCPUSec() > -1 ) {
                                        cpuSecMillis += inst->deltaMilliCPUSec()  ;
                                        mRealSec  += inst->deltaSecs() ;
                                        tmpSize = inst->getSize();
                                        //Nanda to support 64 bit process Starts
                                        if(tmpSize == 0)
                                        {
                                                strcpy(ProcPid,pid.data());
                                        }
                                        // Nanda to support 64 bit process Ends
	            
                                        if (mMaxSize < tmpSize)
                                                mMaxSize = tmpSize ;
                                        // Ashish: Getting the value for Total Virtual Size
                                        mTotalSize += inst->getTotalSize();
                                        tmpSize = inst->getRsSize();
                                        if (mMaxRsSize < tmpSize)
                                                mMaxRsSize = tmpSize ;
                                        // Ashish: Geting the value for Total Resident Size
                                        mTotalRsSize += inst->getTotalRsSize();
                                        mSumThreads += inst->getThreads () ;
                                        mSumFds += inst->getFds () ;
                                        countOfContrinutingProcs ++ ;
                                }
                                RWCString  * tmpPid = oldPids.remove(&pid); //Remove if from old
                                delete tmpPid ;
                        }
                }
        }
        closedir(procdir);
        if (procFound != 0) {
                if ( countOfContrinutingProcs > 0  ) {//Means first time
                        // half second is substracted from cpuSecMillis to compensate for its
                        // being in nano second
                        mSumCPUUtillization =  cpuSecMillis  /
                                (( mRealSec * 10000  * mNumCpus ) / countOfContrinutingProcs );
                        // cpuSecMillis has the precision of millisec and real sec has precision of sec
                        // So it is probable we can get percentage more that 100
                        if ( mSumCPUUtillization > 100 ) {
                                mSumCPUUtillization = 100 ;
                        }
                        if ( mSumCPUUtillization < 0 )
                                mSumCPUUtillization = 0 ;
                }
                while ( oldPids.entries()) {
                        RWCString * opid = oldPids.get();
                        SingleProcInstance * killedProc = mProcIdDict.findValue(opid);
                        mProcIdDict.remove(opid);
                        delete opid ;
                        delete killedProc ;
                }

                mNumberOfInstances =  mProcIdDict.entries();
                if ( mNumberOfInstances )
                        mAvailableFlag = 100 ;
        }
        (void) oldPids.clearAndDestroy();
        uid = NULL;
        char *tmp = (char *)calloc(sizeof(char), strlen(ProcPid)+1);
        snprintf(tmp,sizeof(tmp),"%s", ProcPid);
        return tmp;
}
#endif  // _TRY_OLD_EXECUTION_

#ifndef  _STANDALONE_
void ProcInfo::executeFast(JNIEnv *env, jobject obj)
#else
void ProcInfo::executeFast(void)
#endif
{
	//Reset the parameters
    int result  = 0;
    mAvailableFlag = 0 ; //if one instance of the process found, this flag is set to 1
    mErrorState = 0 ;
    psinfo currproc ;
    prpsinfo old_currproc ;
    struct dirent *direntp;
    static DIR *procdir;
    seteuid(0);
    int fd = 0 ;
    int procFound = 0;
    // Ashish modified for supporting the User specific polling
    char mLogString[1280];
    int userFlag = 0;
    long uid = -857320316;
    if (strcmp(mUserName, "< ALL USERS >") == 0) {
            userFlag = 1;
    } else {
            uid = atol(mUserName);
    }
    snprintf(mLogString,sizeof(mLogString),"UID(Execute Fast):%d", uid);
#ifndef  _STANDALONE_
    logDbg1(env,obj,mLogString);
#else
    logDbg1(mLogString);
#endif
    ZERO(mErrorMessage);

    if (!(procdir = opendir (PROCFS))) {
	    snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
            mErrorState = -1 ;
            return;
    }

    PidPtrHashItr itr(mNewProcIdDict);
    /**********************ARS 8894***********************************/
    char args_name[128];
    char errStr[384];
    RWCString longargs_name;
    /***********************************************************/
    char name[32] = {0};
    int psinfo_args_len;
    int isProcNotMatched = 0;
    RWCString full_cmd;

    snprintf(mLogString,sizeof(mLogString),"Existing instances: %d", mNewProcIdDict.entries());
#ifndef  _STANDALONE_
    logDbg1(env,obj,mLogString);
#else
    logDbg1(mLogString);
#endif

    for ( ; itr() ; ) {
	    RWCString pid = *itr.key();
            snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, pid.data());
            fd = open (name, O_RDONLY);
            if (fd == -1 ) {
                    if ( (!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
                            snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
                                     name,strerror(errno));
                            mErrorState = 100 ;
                    }
                    continue ;
            }
            int byteRead = read(fd, &currproc, sizeof(currproc));
            if ( byteRead <= 0 ) {
                    snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s\n",
                             name,strerror(errno));
                    mErrorState = -1 ;
                    (void) close(fd);
                    continue;
            }
            (void) close (fd);
            if (userFlag == 0 && currproc.pr_uid != uid) {
                    continue;
            }

            psinfo_args_len = strlen(currproc.pr_psargs);
            full_cmd = currproc.pr_psargs;
            if (psinfo_args_len > 70) {
                    full_cmd = longargs(currproc);
                    if (full_cmd.length() < psinfo_args_len) {
                            full_cmd = currproc.pr_psargs;
                    }
            }
            isProcNotMatched = procNameMatched(full_cmd);
            if (isProcNotMatched) {
                    isProcNotMatched = procNameMatched(currproc.pr_fname);
            }

//             RWCString  rwName= args_name;
//             RWCString rwProc;
//             RWCTokenizer next(rwName);
//             rwProc =  next();
//             int isProcNotMatched = 0;
//             if(rwProc.length() > 70) {
//                     longargs_name = longargs(currproc.pr_pid,errStr);
//                     if(longargs_name.isNull())
//                             continue;
//                     isProcNotMatched = procNameMatched(longargs_name);
//             } else
//                     isProcNotMatched = procNameMatched(args_name);

//             /**ASHISH CHANGES TO IMPLEMENT ARS-13176 and ARS-23559**/
//             if (isProcNotMatched) {
//                     isProcNotMatched = procNameMatched(proc_name);
//             }

            if ( !isProcNotMatched ) {
                    if (mCommandLineMatch != ""  && !commandLineMatchFound(full_cmd)) {
                            continue;
                    }
                    mAvailableFlag = 100 ;
                    closedir(procdir);
                    return ; // one Process found no More Need to iterate
            }
    }  // for itr()

    //If the process is not found in the list of the Pids stored
    //from the last poll, we have to search entire /proc dir
    //It is expensive
    //char proc_name[100];
    for (rewinddir (procdir); (direntp = readdir (procdir));) {
            if (direntp->d_name[0] == '.')
                    continue;
            snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
            fd = open (name, O_RDONLY);
            if (fd == -1 ) {
                    if ( (!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
                            snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
                                     name,strerror(errno));
                            mErrorState = 100 ;
                    }
                    continue ;
            }
            int byteRead = read(fd, &currproc, sizeof(currproc));
            if ( byteRead <= 0 ) {
                    snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s",
                             name,strerror(errno));
                    mErrorState = -1 ;
                    (void) close(fd);
                    continue;
            }
            (void) close (fd);
            if (userFlag == 0 && currproc.pr_uid != uid) {
                    continue;
            }

            psinfo_args_len = strlen(currproc.pr_psargs);
            full_cmd = currproc.pr_psargs;
            if (psinfo_args_len > 70) {
                    full_cmd = longargs(currproc);
                    if (full_cmd.length() < psinfo_args_len) {
                            full_cmd = currproc.pr_psargs;
                    }
            }
            isProcNotMatched = procNameMatched(full_cmd);
            if (isProcNotMatched) {
                    isProcNotMatched = procNameMatched(currproc.pr_fname);
            }

//             RWCString  rwName= args_name;
//         RWCString rwProc;
//         RWCTokenizer next(rwName);
//         rwProc =  next();
//         int isProcNotMatched = 0;
// 		if(rwProc.length() > 70) {
// 	    	longargs_name = longargs(currproc.pr_pid,errStr);
//             if(longargs_name.isNull())
// 				continue;
// 			isProcNotMatched = procNameMatched(longargs_name);
// 		} else
//         	isProcNotMatched = procNameMatched(args_name);

//         /**ASHISH CHANGES TO IMPLEMENT ARS-13176 and ARS-23559**/
// 		if (isProcNotMatched) {
// 			isProcNotMatched = procNameMatched(proc_name);
// 		}

        if ( !isProcNotMatched ) {
	        if (mCommandLineMatch != ""  && !commandLineMatchFound(full_cmd)) {
                        if( ! commandLineMatchFound(full_cmd))
                                continue;
// /**************************************ARS 8894******************************************/
// 				//longargs_name = longargs(currproc.pr_pid,errStr);
// 				if (userFlag == 0) {
// 					if (currproc.pr_uid == uid) {
// 						longargs_name = longargs(currproc.pr_pid,errStr);
// 					}
// 				} else {
// 					longargs_name = longargs(currproc.pr_pid,errStr);
// 				}
// 				if(longargs_name.isNull())
//                 	continue;
// 				else if( ! commandLineMatchFound(longargs_name))
//                 	continue;
/********************************************************************************/
                }
                mAvailableFlag = 100 ;
                procFound = 1;
                break ; // one Process found no More Need to iterate
        }
    }
    closedir(procdir);
}

int ProcInfo::commandLineMatchFound(RWCString progNameWithArgument) {
	RWCTokenizer next(progNameWithArgument);
    RWCString token = next(); //Advance to first token
    int len = token.length();
    RWCString args = progNameWithArgument(len,
                                (progNameWithArgument.length() - len) );
                                                                        //String without the process name


    // cout << "Proc NAme " << token << endl ;
    // cout << "ARga " << args << endl ;
    /*
    while(!(token = next() ).isNull()) {
                args += token ;
	}
    */

    if(args.contains(mCommandLineMatch) ) {
	    return 1;
	}
    return 0 ;
}

int ProcInfo::procNameMatched(RWCString procArg) {
	int fullCompare = 0 ;
    if ( mProcName.contains("/") ) {
	    fullCompare = 1 ;
	}
    RWCString arg;
    RWCTokenizer next(procArg);
    arg =  next();
    //Padmini's Code change for 6353 begin
    RWCString nextArgu;
    nextArgu = next();
    //Padmini's Code change for 6353 end

    if ( fullCompare ) {
	    if ( arg == mProcName  )
        	return 0 ;
	} else {
		RWCString slashedProcArg ;
        RWCTokenizer nextArg(arg);
        RWCString temp ;
        while (!(temp = nextArg("/")).isNull()) {
	        slashedProcArg = temp ;
		}
		if ( slashedProcArg == mProcName )
        	return 0 ;

		//Padmini's Code change for 6353 begin
        if (!nextArgu.isNull()) {
	        RWCString nextslashedProcArg ;
            RWCTokenizer nextArg(nextArgu);
            RWCString nexttemp ;

            while (!(nexttemp = nextArg("/")).isNull()) {
	            nextslashedProcArg = nexttemp ;
			}//End of while
            if ( nextslashedProcArg == mProcName ) {
	            return 0 ;
			}//End of if
		}//End of if
                //Padmini's Code change for 6353 end

	}
    return 1 ;
}

int ProcInfo::getNuberOfCPUs()
{
        kstat_ctl_t    * kc = kstat_open();
        int cpus = -1;
        if (kc == NULL )
        {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102155 kstat open (Error).\nFailed to initialize kernel  statistics");
                mErrorState = -1 ;
                goto out;
        }
        kstat_t *k_space;
        void *val_ptr;
        k_space = kstat_lookup(kc,"unix",0,"system_misc");
        if (k_space == NULL)
        {
                snprintf(mErrorMessage,sizeof(mErrorMessage),
                         "2102156 kstat lookup (Error).\nFailed to find a kstat by name unix\n"
                         "and stat name = misc." );
                mErrorState = -1 ;
                goto out;
        }
        if(kstat_read(kc,k_space, NULL ) == -1 )
        {
                snprintf(mErrorMessage,sizeof(mErrorMessage),
                         "2102157 kstat read (Error).\nFailed to read kstat data for module name unix\n"
                         "and stat name = misc." );
                mErrorState = -1 ;
                goto out;
        }
        if ((val_ptr = (void*)kstat_data_lookup(k_space,"ncpus")) == NULL )
        {
                snprintf(mErrorMessage,sizeof(mErrorMessage),
                         "2102158 kstat data lookup (Error).\nFailed to find kstat by name = unix "
                         ".\nStat name = misc and Named Stat name ncpus" );
                mErrorState = -1 ;
                goto out;
        }
        cpus = ((kstat_named_t *) val_ptr)->value.i32 ;
  out:
        if (kc) {
                kstat_close(kc);
        }
        return cpus;
}

int
ProcInfo::getProcessNames(int size, int osType, RWCString procNames[]) {
        //Reset the parameters
        int result  = 0;
        mOSVersionFlag = osType;
        mErrorState = 0 ;
        psinfo currproc ;
        prpsinfo old_currproc ;
        struct dirent *direntp;
        static DIR *procdir;
        int iCounter = 0;
        RWCString proc_Name;
        RWCString processName= (RWCString)"oracle";

        seteuid(0);
        int fd = 0 ;
        ZERO(mErrorMessage);

        if (!(procdir = opendir (PROCFS)))
        {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
                mErrorState = -1;
                return -1;
        }

        char name[32] = {0};
        //If the process is not found in the list of the Pids stored
        //from the last poll, we have to search entire /proc dir
        //It is expensive
        for (rewinddir (procdir); (direntp = readdir (procdir)); )
        {
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
                fd = open (name, O_RDONLY);
                if (fd < 0)
                {
                        snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
                                        name, strerror(errno));
                        mErrorState = -1 ;
                        continue;
                }
                int byteRead = read(fd,&currproc, sizeof(currproc));
                if ( byteRead <= 0 )
                {
                        snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s",
                                 name,strerror(errno));
                        mErrorState = -1 ;
                        (void) close(fd);
                        continue;
                }
//                         strcpy(proc_name,currproc.pr_fname);
//                         strcpy(proc_args,currproc.pr_psargs);
//                 }
//                 else
//                 {
//                         int ioCtl;
//                         ioCtl = ioctl (fd, PIOCPSINFO, &old_currproc);
//                         if (ioCtl < 0)
//                         {
//                                 snprintf(mErrorMessage,sizeof(mErrorMessage),"2102154 IOCTL FAILURE for PID %s.\nError is %s\n",
//                                 name,strerror(errno));
//                                 mErrorState = -1 ;
//                                 (void) close(fd);
//                                 closedir(procdir);
//                                 return -1;
//                         }
//                         strcpy(proc_name,old_currproc.pr_fname);
//                         strcpy(proc_args,old_currproc.pr_psargs);
//                 }

                (void) close (fd);
                if (iCounter >= size) {
                        break;
                }
                proc_Name = currproc.pr_fname;
                if (proc_Name.index ("oracle", 0, RWCString::ignoreCase) != RW_NPOS)
                {
                        procNames[iCounter] = oracleProc(currproc.pr_psargs);
                }
                else if (proc_Name.index ("libhttpd.ep", 0, RWCString::ignoreCase) != RW_NPOS)
                {
                        procNames[iCounter] = "httpd";
                }
                else
                        procNames[iCounter] = (RWCString)currproc.pr_fname;
                iCounter++;
        }
        closedir(procdir);
        return iCounter;
}

int ProcInfo::getProcessCount(int osType)
{
        static DIR *dir;
        struct dirent *dp;
        int iCounter = 0;

        dir = opendir(PROCFS);
	if (!dir) {
		snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is %s", PROCFS, strerror(errno));
                return -1;
	}
	while ((dp = readdir(dir))) {
		if (dp->d_name[0] == '.') {
			continue;
		}
                iCounter++;
        }
        closedir(dir);
        return iCounter;
#if 0
	//Reset the parameters
    int result  = 0;
    mOSVersionFlag = osType;
    mErrorState = 0 ;
    psinfo currproc ;
    prpsinfo old_currproc ;
    struct dirent *direntp;
    static DIR *procdir;
    char proc_name[128];
    int iCounter = 0;

    seteuid(0);
    int fd = 0 ;
    if (!(procdir = opendir (PROCFS))) {
		snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
        mErrorState = -1;
        return 0;
	}

	if (chdir (PROCFS)) {
		snprintf(mErrorMessage,sizeof(mErrorMessage),"2102151 Unable to chdir to %s.\nError is %s", PROCFS,strerror(errno));
        mErrorState = -1 ;
        closedir(procdir);
        return 0;
	}

	char name[256];
    //If the process is not found in the list of the Pids stored
    //from the last poll, we have to search entire /proc dir
    //It is expensive
    for (rewinddir (procdir); (direntp = readdir (procdir)); ) {
		//char buf[30];
        if (direntp->d_name[0] == '.')
        	continue;
        if ( mOSVersionFlag == 1 ) {//5.6 and above
			strcpy(name,PROCFS);
			strcat(name,"/");
			strcat(name,direntp->d_name);
			strcat(name,"/psinfo");
		} else
			strcpy(name,direntp->d_name);
                fd = open (name, O_RDONLY);
		if (fd == -1 ) {
			snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
                                        name,strerror(errno));
            mErrorState = -1 ;
            closedir(procdir);
            return 0;
		}

		if ( mOSVersionFlag == 1 ) {//5.6 and above
			int byteRead = read(fd,&currproc, sizeof(currproc));
			if ( byteRead == 0 ) {
				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s\n",
                                        name,strerror(errno));
				mErrorState = -1 ;
				(void) close(fd);
				closedir(procdir);
				return 0;
			}
			iCounter++;
		} else {
			int ioCtl;
			ioCtl = ioctl (fd, PIOCPSINFO, &old_currproc);
			if (ioCtl < 0) {
				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102154 IOCTL FAILURE for PID %s.\nError is %s",
                                name,strerror(errno));
                mErrorState = -1 ;
                (void) close(fd);
                closedir(procdir);
                return 0;
    		}
    		iCounter++;
		}
		(void) close (fd);
	}
	closedir(procdir);
	return iCounter;
#endif
}

int ProcInfo::getCmdMatchCount (int osType, RWCString process)
{
	//Reset the parameters
        int result  = 0;
//        const RWCString procName = process;
        mOSVersionFlag = osType;
        mAvailableFlag = 0 ; //if one instance of the process found, this flag                                 is set to 1
        mErrorState = 0 ;
        psinfo currproc ;
        prpsinfo old_currproc ;
        struct dirent *direntp;
        static DIR *procdir;
        char proc_cmd[128];
        int iCounter = 0;
        RWCString proc_Cmd;

        seteuid(0);
        int fd = 0 ;
        if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
                mErrorState = -1;
                return 0;
	}
	char name[32] = {0};
        //If the process is not found in the list of the Pids stored
        //from the last poll, we have to search entire /proc dir
        //It is expensive
        for (rewinddir (procdir); (direntp = readdir (procdir)); ) {
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"%s/%s/psinfo", PROCFS, direntp->d_name);
		fd = open (name, O_RDONLY);
                if (fd < 0) {
                        snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file %s.\nError is %s",
                                 name,strerror(errno));
                        mErrorState = -1 ;
                        continue;
		}
// 		if ( mOSVersionFlag == 1 ) {//5.6 and above
                int byteRead = read(fd,&currproc , sizeof(currproc));
                if ( byteRead <= 0 ) {
                        snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s",
                                 name,strerror(errno));
                        mErrorState = -1 ;
                        (void) close(fd);
                        continue;
                }
		(void) close (fd);
		proc_Cmd = (RWCString) currproc.pr_psargs;
		if ( proc_Cmd.index (process, 0, RWCString::ignoreCase) == RW_NPOS) {
			continue;
		}
		iCounter++;
	}
        closedir(procdir);
        return iCounter;
}

int ProcInfo::getCmdMatch(int size, int osType, RWCString process, RWCString procCmd[]) {
	//Reset the parameters
        const RWCString procName = process;
        int result  = 0;
        mOSVersionFlag = osType;
        mAvailableFlag = 0 ; //if one instance of the process found, this flag is set to 1
        mErrorState = 0 ;
        psinfo currproc ;
        prpsinfo old_currproc ;
        struct dirent *direntp;
        static DIR *procdir;
        char proc_cmd[128];
        RWCString proc_Cmd;
        int iCounter = 0;
        procCmd[0] = " ";
        seteuid(0);
        int fd = 0 ;
    
        const char* PArgs;
        RWCString RPArgs;

        RWCString full_cmd;
        int psinfo_args_len;

        //Desai
        char errStr[384];
        if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s\n", PROCFS, strerror(errno));
                mErrorState = -1;
                return -1;
        }

	char name[32] = {0};
        //If the process is not found in the list of the Pids stored
        //from the last poll, we have to search entire /proc dir
        //It is expensive
        for (rewinddir (procdir); (direntp = readdir (procdir)); ) {
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
		fd = open (name, O_RDONLY);
                if (fd < 0 ) {
                        continue;
		}

                int byteRead = read(fd,&currproc, sizeof(currproc));
                if ( byteRead <= 0 ) {
                        close(fd);
                        continue;
                }
                (void) close (fd);

                if (iCounter >= size) {
                        break;
                }
                psinfo_args_len = strlen(currproc.pr_psargs);
                full_cmd = currproc.pr_psargs;
                if (full_cmd.index (procName, 0, RWCString::ignoreCase) == RW_NPOS) {
                        continue;
                }
                if (psinfo_args_len > 70) {
                        full_cmd = longargs(currproc);
                        if (full_cmd.length() < psinfo_args_len) {
                                full_cmd = currproc.pr_psargs;
                        }
                }
                procCmd[iCounter] = full_cmd;

#if 0
		/* Desaiproc_Cmd = (RWCString) proc_cmd;*/
        if( strlen(proc_cmd) < 70 ) {
	        proc_Cmd = (RWCString) proc_cmd;
		}
		 else
		{
			proc_Cmd = longargs(currproc.pr_pid,errStr);

			if (proc_Cmd.index ("ioctl failed", 0, RWCString::ignoreCase) == RW_NPOS)
			{
	       // snprintf(mLogString,sizeof(mLogString),"Native: Got LONGARGS:");
    	 // 	logDbg1(env,obj,mLogString);
			}
			else
			{
			//snprintf(mLogString,sizeof(mLogString),"Native: didnt Get LONGARGS:");
    	  	//logDbg1(env,obj,mLogString);
			 proc_Cmd=(RWCString) proc_cmd;
			}

		 }

      		PArgs = proc_cmd;
		RPArgs = (RWCString) proc_cmd;
								 		
				
      		if (RPArgs.index (procName, 0, RWCString::ignoreCase) == RW_NPOS) {
	      	continue;
		}
		if (iCounter < size) {
			if(proc_Cmd.isNull())
			    procCmd[iCounter] = RPArgs;
			else
			   procCmd[iCounter] = proc_Cmd;
			//procCmd[iCounter] = (RWCString)proc_Cmd;
		} else {
			break;
		}
#endif
		iCounter++;
    }
    closedir(procdir);
    return iCounter;
}

/**
 * This function modifies the string that is passed to it.
 * It just returns the absolute file-name from the input.
 * e.g: /a/b/c arg1 arg2 --> c
 */
char * ProcInfo:: oracleProc(char *input)
{
        char *t, *n;
        t = strchr(input, ' ');
        if (t) {
                *t = '\0';
        }
        t = strrchr(input, '/');
        if (t) {
                n = t+1;
        } else {
                n = input;
        }
        return n;
//         char t_procName[128];
//         int i = 0;
//         for(i=0;t_proc[i] != '\0'; i++)
//         {
//                 t_procName[i] = t_proc[i];
//                 if(t_proc[i] == ' ')
//                         break;
//         }
//         t_procName[i] = '\0';

//         return t_procName;
}
/********************************************* Prakash changes end - ARS 6595 *******************************/

/**
* The following function is called during polling only if the Process is not found using regular logic.
* These function matches the process name with the "pr_fname" elemnet of the psinfo structure.
* This is done as a part of bug fix for ARS 13176.
* It was found that 'pr_fname' element of psinfo structure gives the name of the executable file
* 'pr_psargs' element gives the initial arguments of the running process.
* If the name of the process cannot be determined using 'pr_psargs' element (as in executeRegular/executeFast)
* then the process name is compared with 'pr_fname' argument' - Prakash
**/
/*
void ProcInfo::executeProcRegular(int userFlag, long uid) {
	int result  = 0;
    mNumberOfInstances = 0 ;
    mNumberOfNewInstances = 0 ;
    mSumCPUUtillization = -1 ;
    mAvailableFlag = 0 ;
    double cpuSecMillis = 0 ;
    double mRealSec  = 0;
    float tmpSize = 0 ;
    mMaxSize = 0 ;
    mMaxRsSize = 0 ;
    mSumThreads = 0 ;
    mSumFds = 0 ;
    mErrorState = 0 ;
    mTotalSize = 0;
    mTotalRsSize = 0;
    int countOfContrinutingProcs = 0 ;
    psinfo currproc ;
    prpsinfo old_currproc ;
    struct dirent *direntp;
    static DIR *procdir;
    seteuid(0);
    char args_name[100];
    char errStr[300];
    RWCString longargs_name;
    RWTPtrSlist <RWCString>  oldPids;
    RWTPtrHashDictionaryIterator<RWCString,SingleProcInstance>
    itr(mProcIdDict);
    for(;itr();)
    	oldPids.append(new RWCString(*itr.key()));

	if (!(procdir = opendir (PROCFS))) {
		snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
                // cout << " Error number is " << errno <<" error msg is " << strerror(errno) << endl;
        mErrorState = -1 ;
        (void) oldPids.clearAndDestroy();
        return ;
	}
	if (chdir (PROCFS)) {
		snprintf(mErrorMessage,sizeof(mErrorMessage),"2102151 Unable to chdir to %s.\nError is %s", PROCFS,strerror(errno));
        mErrorState = -1 ;
        closedir(procdir);
        (void) oldPids.clearAndDestroy();
        return ;
	}

	int fd;
    //char buf[30];
	char name[256];
	char proc_name[100];
	for (rewinddir (procdir); (direntp = readdir (procdir));) {
		//int fd;
        ////char buf[30];
        if (direntp->d_name[0] == '.')
        	continue;
		//char name[256];
        if ( mOSVersionFlag == 1 ) {//5.6 and above
			strcpy(name,PROCFS);
            strcat(name,"/");
            strcat(name,direntp->d_name);
            strcat(name,"/psinfo");
		} else
        	strcpy(name,direntp->d_name);
		fd = open (name, O_RDONLY);
        if (fd == -1 ) {
	        //errorno == 2 means process not found.
            if ((!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
	            snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
                                        name,strerror(errno));
                mErrorState = 100 ;
			}
			continue ;
		}
		//char proc_name[100];
        if ( mOSVersionFlag == 1 ) {//5.6 and above
			int byteRead = read(fd,&currproc, sizeof(currproc));
			if ( byteRead == 0 ) {
				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s",
                                        name, strerror(errno));
                mErrorState = -1 ;
                (void) close(fd);
                closedir(procdir);
                (void) oldPids.clearAndDestroy();
                return ;
			}
			// Ashish modified for supporting the User specific polling
            if (userFlag == 0) {
	            if (currproc.pr_uid == uid) {
		            strcpy(proc_name,currproc.pr_fname);
             		strcpy(args_name,currproc.pr_psargs);
             	} else {
		            strcpy(proc_name,"");
             		strcpy(args_name,"");
	            }
            } else {
	            strcpy(proc_name,currproc.pr_fname);
             	strcpy(args_name,currproc.pr_psargs);
         	}
        } else {
			int ioCtl;
            ioCtl = ioctl (fd, PIOCPSINFO, &old_currproc);
			if (ioCtl < 0) {
				sprintf (mErrorMessage,
                                        "2102154 IOCTL FAILURE for PID %s.\nError is %s\n",
                name, strerror(errno));
                mErrorState = -1 ;
                (void) close(fd);
                closedir(procdir);
                (void) oldPids.clearAndDestroy();
                return ;
			}
			// Ashish modified for supporting the User specific polling
			if (userFlag == 0) {
				if (old_currproc.pr_uid == uid) {
					strcpy(proc_name,old_currproc.pr_fname);
            		strcpy(args_name,old_currproc.pr_psargs);
            	} else {
	        		strcpy(proc_name,"");
             		strcpy(args_name,"");
        		}
    		} else {
	    		strcpy(proc_name,old_currproc.pr_fname);
            	strcpy(args_name,old_currproc.pr_psargs);
        	}
		}
		(void) close (fd);
		int isProcNotMatched = 0;
        isProcNotMatched = procNameMatched(proc_name);

        if ( !isProcNotMatched ) {
	        if (mCommandLineMatch != ""  && !commandLineMatchFound(args_name)) {
		        //longargs_name = longargs(currproc.pr_pid,errStr);
		        if (userFlag == 0) {
			        if (currproc.pr_uid == uid) {
				        longargs_name = longargs(currproc.pr_pid,errStr);
			        }
		        } else {
			        longargs_name = longargs(currproc.pr_pid,errStr);
		        }
                if(longargs_name.isNull())
					continue;
				else if( ! commandLineMatchFound(longargs_name))
					continue;
			}
			char pid_name[100];
            if ( mOSVersionFlag == 1 ) { //5.6 and above
            	if (userFlag == 0) {
	            	if (currproc.pr_uid == uid) {
		            	snprintf( pid_name,sizeof( pid_name),"%d",currproc.pr_pid);
	            	}
            	} else {
	            	snprintf( pid_name,sizeof( pid_name),"%d",currproc.pr_pid);
            	}
	        } else {
	        	if (userFlag == 0) {
		            	if (old_currproc.pr_uid == uid) {
			            	snprintf( pid_name,sizeof( pid_name),"%d",old_currproc.pr_pid);
		            	}
            	} else {
	            	snprintf( pid_name,sizeof( pid_name),"%d",old_currproc.pr_pid);
	            }
	        }

			RWCString  pid (pid_name);
			SingleProcInstance * inst = mProcIdDict.findValue(&pid) ;
            if ( inst ==  0 ) {
	            RWCString * npid = new RWCString(pid) ;
                inst = new SingleProcInstance(npid);
                mProcIdDict.insertKeyAndValue(npid,inst);
                mNumberOfNewInstances ++ ;
				if ( mOSVersionFlag == 1 ) //5.6 and above
                	result = inst->execute(currproc);
                else {//5.5
					result = inst->execute(old_currproc);
				}

                if ( result == -1 ) {
	                sprintf (mErrorMessage, inst->getErrorMsg());
                    mErrorState = -1 ;
                    closedir(procdir);
                    mProcIdDict.remove(npid);
                    delete inst ;
                    delete npid ;
                    (void) oldPids.clearAndDestroy();
                    return ;
				}
			} else {
				if ( mOSVersionFlag == 1 ) //5.6 and above
                	result = inst->execute(currproc);
                else {//5.5
					result = inst->execute(old_currproc);
				}
				if ( result == -1 ) {
					sprintf (mErrorMessage, inst->getErrorMsg());
                    mErrorState = -1 ;
                    closedir(procdir);
                    (void) oldPids.clearAndDestroy();
                    return ;
				}
				//We just want to record data for not-new processes

                if ( inst->deltaMilliCPUSec() > -1 ) {
	                cpuSecMillis += inst->deltaMilliCPUSec()  ;
                    mRealSec  += inst->deltaSecs() ;
                    tmpSize = inst->getSize();
                    if (mMaxSize < tmpSize)
						mMaxSize = tmpSize ;
					// Getting the value for Total Virtual Size
					mTotalSize += inst->getTotalSize();
					tmpSize = inst->getRsSize();
					if (mMaxRsSize < tmpSize)
                    	mMaxRsSize = tmpSize ;
                    // Getting the value for Total Resident Size
                    mTotalRsSize += inst->getTotalRsSize();
                    mSumThreads += inst->getThreads () ;
                    mSumFds += inst->getFds () ;
                    countOfContrinutingProcs ++ ;
				}
				RWCString  * tmpPid = oldPids.remove(&pid); //Remove if from old
                delete tmpPid ;
			}
		}
	}
    closedir(procdir);

    if ( countOfContrinutingProcs > 0  ) {//Means first time

        // half second is substracted from cpuSecMillis to compensate for its
        // being in nano second
        mSumCPUUtillization =  cpuSecMillis  /
        		(( mRealSec * 10000  * mNumCpus ) / countOfContrinutingProcs );
		// cpuSecMillis has the precision of millisec and real sec has precision of sec
        // So it is probable we can get percentage more that 100
        if ( mSumCPUUtillization > 100 ) {
	        mSumCPUUtillization = 100 ;
		}
		if ( mSumCPUUtillization < 0 )
        	mSumCPUUtillization = 0 ;

    	}
        while ( oldPids.entries()) {
	        RWCString * opid = oldPids.get();
            SingleProcInstance * killedProc = mProcIdDict.findValue(opid);
            mProcIdDict.remove(opid);
            delete opid ;
            delete killedProc ;
        }
        (void) oldPids.clearAndDestroy();
        mNumberOfInstances =  mProcIdDict.entries();
        if ( mNumberOfInstances )
        	mAvailableFlag = 100 ;
}
*/
/*
void ProcInfo::executeProcFast(int userFlag, long uid, JNIEnv *env, jobject jobj) {
	char mLogString[1280];
	int result  = 0;
    mAvailableFlag = 0 ;
    mAvailableFlagAtUserLvl = 0;
    mErrorState = 0 ;
    psinfo currproc ;
    prpsinfo old_currproc ;
    struct dirent *direntp;
    static DIR *procdir;
    seteuid(0);
    int fd = 0 ;
    if (!(procdir = opendir (PROCFS))) {
	    snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
        mErrorState = -1 ;
        return ;
	}
	if (chdir (PROCFS)) {
		snprintf(mErrorMessage,sizeof(mErrorMessage),"2102151 Unable to chdir to %s.\nError is %s\n", PROCFS,strerror(errno));
        mErrorState = -1 ;
        closedir(procdir);
        return ;
	}
	RWTPtrHashDictionaryIterator<RWCString,SingleProcInstance> itr(mProcIdDict);
    char args_name[100];
    char errStr[300];
    RWCString longargs_name;
    char name[256];
    char proc_name[100];
    for ( ; itr() ; ) {
	    RWCString pid = *itr.key();
        if ( mOSVersionFlag == 1 ) {//5.6 and above
			strcpy(name,PROCFS);
            strcat(name,"/");
            strcat(name,pid.data());
            strcat(name,"/psinfo");
		} else
			strcpy(name,pid.data());
		fd = open (name, O_RDONLY);
        if (fd == -1 ) {
	        if ( (!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
		        snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
                                        name,strerror(errno));
                mErrorState = 100 ;
			}
			continue ;
		}
		//char proc_name[100];
        if ( mOSVersionFlag == 1 ) {//5.6 and above
			int byteRead = read(fd,&currproc, sizeof(currproc));
            if ( byteRead == 0 ) {
	            snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s\n",
                                        name,strerror(errno));
                mErrorState = -1 ;
                (void) close(fd);
                closedir(procdir);
                return ;
			}
			// Ashish modified for supporting the User specific polling
			if (userFlag == 0) {
				if (currproc.pr_uid == uid) {
					strcpy(proc_name,currproc.pr_fname);
            		strcpy(args_name,currproc.pr_psargs);
            		mAvailableFlagAtUserLvl = 1;
            	}
			} else {
				strcpy(proc_name,currproc.pr_fname);
            	strcpy(args_name,currproc.pr_psargs);
            	mAvailableFlagAtUserLvl = 1;
        	}
		} else {
			int ioCtl;
            ioCtl = ioctl (fd, PIOCPSINFO, &old_currproc);
			if (ioCtl < 0) {
				sprintf (mErrorMessage,
                                        "2102154 IOCTL FAILURE for PID %s.\nError is %s",
                				name,strerror(errno));
                mErrorState = -1 ;
				(void) close(fd);
                closedir(procdir);
                return ;
			}
			// Ashish modified for supporting the User specific polling
			if (userFlag == 0) {
				if (old_currproc.pr_uid == uid) {
					strcpy(proc_name,old_currproc.pr_fname);
            		strcpy(args_name,old_currproc.pr_psargs);
            		mAvailableFlagAtUserLvl = 1;
            	}
			} else {
				strcpy(proc_name,old_currproc.pr_fname);
            	strcpy(args_name,old_currproc.pr_psargs);
            	mAvailableFlagAtUserLvl = 1;
        	}
		}
		(void) close (fd);
        int isProcNotMatched = 0;
        isProcNotMatched = procNameMatched(proc_name);
        if ( !isProcNotMatched ) {
	        if (mCommandLineMatch != ""  && !commandLineMatchFound(args_name)) {
		        //longargs_name = longargs(currproc.pr_pid,errStr);
		        if (userFlag == 0) {
			        if (currproc.pr_uid == uid) {
				        longargs_name = longargs(currproc.pr_pid,errStr);
			        }
		        } else {
			        longargs_name = longargs(currproc.pr_pid,errStr);
		        }
                if(longargs_name.isNull())
                	continue;
				else if( ! commandLineMatchFound(longargs_name))
					continue;
			}
			mAvailableFlag = 100 ;
            closedir(procdir);
            return ;
		}
	}
	//If the process is not found in the list of the Pids stored
    //from the last poll, we have to search entire /proc dir
    //It is expensive
    for (rewinddir (procdir); (direntp = readdir (procdir));) {
	    //char buf[30];
        if (direntp->d_name[0] == '.')
        	continue;
		if ( mOSVersionFlag == 1 ) {//5.6 and above
			strcpy(name,PROCFS);
            strcat(name,"/");
            strcat(name,direntp->d_name);
            strcat(name,"/psinfo");
		} else
			strcpy(name,direntp->d_name);
		fd = open (name, O_RDONLY);
		if (fd == -1 ) {
	        if ( (!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
		        snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
                                        name,strerror(errno));
                mErrorState = 100 ;
			}
			continue ;
		}
		//char proc_name[100];
        //char args_name[100];
        if ( mOSVersionFlag == 1 ) {//5.6 and above
			int byteRead = read(fd,&currproc ,(sizeof(currproc) + 1));
        	if ( byteRead == 0 ) {
	        	snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s",
                                        name,strerror(errno));
            	mErrorState = -1 ;
            	(void) close(fd);
            	closedir(procdir);
            	return ;
			}
			// Ashish modified for supporting the User specific polling
			if (userFlag == 0) {
	 			if (currproc.pr_uid == uid) {
		 			strcpy(proc_name,currproc.pr_fname);
        			strcpy(args_name,currproc.pr_psargs);
        			mAvailableFlagAtUserLvl = 1;
        		}
 			} else {
	 			strcpy(proc_name,currproc.pr_fname);
        		strcpy(args_name,currproc.pr_psargs);
        		mAvailableFlagAtUserLvl = 1;
    		}
        } else {
			int ioCtl;
            ioCtl = ioctl (fd, PIOCPSINFO, &old_currproc);

            if (ioCtl < 0) {
	            sprintf (mErrorMessage,
                                        "2102154 IOCTL FAILURE for PID %s.\nError is %s",
				name,strerror(errno));
                mErrorState = -1 ;
                (void) close(fd);
                closedir(procdir);
                return ;
			}
			// Ashish modified for supporting the User specific polling
			if (userFlag == 0) {
				if (old_currproc.pr_uid == uid) {
					strcpy(proc_name,old_currproc.pr_fname);
            		strcpy(args_name,old_currproc.pr_psargs);
            		mAvailableFlagAtUserLvl = 1;
            	}
			} else {
				strcpy(proc_name,old_currproc.pr_fname);
            	strcpy(args_name,old_currproc.pr_psargs);
            	mAvailableFlagAtUserLvl = 1;
        	}
		}
		(void) close (fd);
        int isProcNotMatched = 0;
        isProcNotMatched = procNameMatched(proc_name);
        if ( !isProcNotMatched ) {
	    	if (mCommandLineMatch != ""  && !commandLineMatchFound(args_name)) {
		        //longargs_name = longargs(currproc.pr_pid,errStr);
		        if (userFlag == 0) {
			        if (currproc.pr_uid == uid) {
				        longargs_name = longargs(currproc.pr_pid,errStr);
			        }
		        } else {
			        longargs_name = longargs(currproc.pr_pid,errStr);
		        }
                if(longargs_name.isNull())
                	continue;
				else if( ! commandLineMatchFound(longargs_name))
					continue;
			}
			mAvailableFlag = 100 ;
			break ;
		}
	}
	closedir(procdir);
}
*/

//************************** ASHISH ************************************//
// Following functios have been written to support the enhancement.
// Now processes are getting listed based on user also.
//************************** ASHISH ************************************//

// Function to get the User's ID. It is in the case of LDAP server.
#ifndef  _STANDALONE_
int ProcInfo::getUsrIDs (JNIEnv *env, jobject obj, int iCount, RWCString usrIDArr[], jint osType)
#else
int ProcInfo::getUsrIDs (int iCount, RWCString usrIDArr[], int osType)
#endif
{	int result = 0;
	mOSVersionFlag = osType;
        mErrorState = 0;
	psinfo currproc;
        struct dirent *direntp;
        static DIR *procdir;
        int iCounter = 0;
	seteuid(0);
        int fd = 0;
        char uid[64] = {0};
	char name[32] = {0};

	if (!(procdir = opendir (PROCFS))) {
		snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s\n", PROCFS);
                mErrorState = -1;
                return 0;
	}
        for (rewinddir(procdir); (direntp = readdir(procdir));) {
		if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
        	fd = open(name, O_RDONLY);
        	if (fd == -1 ) {
	        	snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.  errono is = %d", name, errno);
                        mErrorState = -1 ;
                        continue;
                }
                int byteRead = read(fd, &currproc, sizeof(currproc));
                if (byteRead <= 0) {
                        snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.  errono is = %d", name, errno);
                        mErrorState = -1 ;
                        (void) close(fd);
                        continue;
                }
                close(fd);
                if (iCounter >= iCount) {
                        break;
                }
                snprintf(uid, sizeof(uid), "%d", currproc.pr_uid);
                usrIDArr[iCounter++] = (RWCString) uid;
//                         uid = currproc.pr_uid;
//                         snprintf(usrids,sizeof(usrids),"%ld",uid);
//                         strcpy(tmpUsrName, usrids);
//                         iCounter++;
//                         memset (usrids, '\0', sizeof(char) * 64);
//                 } else {
//                         int ioCtl;
//                         ioCtl = ioctl(fd, PIOCPSINFO, &old_currproc);
//                         if (ioCtl < 0) {
//                                 snprintf(mErrorMessage,sizeof(mErrorMessage),"2102154 IOCTL FAILURE for P ID %s.  errono is = %d", name, errno);
//                                 mErrorState = -1 ;
//                                 (void) close(fd);
//                                 closedir(procdir);
//                                 if (usrids != NULL) {
//                                         free (usrids);
//                                         usrids = NULL;
//                                 }
//                                 return 0;
//                         }
//                         uid = old_currproc.pr_uid;
//                         snprintf(usrids,sizeof(usrids),"%ld",uid);
//                         strcpy(tmpUsrName, usrids);
//                         usrIDArr[iCounter] = (RWCString) tmpUsrName;
//                         iCounter++;
//                         memset (usrids, '\0', sizeof(char) * 64);
//                 }
//                 (void) close(fd);
        }
        closedir(procdir);
// 	if (usrids != NULL)	{
// 		(void) free (usrids);
// 		usrids = NULL;
// 	}

	return iCounter;
}

// Following function is to get the count of processes running under specific user
#ifndef  _STANDALONE_
int ProcInfo::getUserProcessCount (JNIEnv *env, jobject obj, int osType, long uid) {
#else
int ProcInfo::getUserProcessCount (int osType, long uid) {
#endif
	int result = 0;
	mOSVersionFlag = osType;
        mErrorState = 0;
	psinfo currproc;
        struct dirent *direntp;
        static DIR *procdir;
        int iCounter = 0;
	seteuid(0);
        int fd = 0;

        if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s\n", PROCFS);
                mErrorState = -1;
                return 0;
	}

	char name[32] = {0};
        //If the process is not found in the list of the Pids stored
        //from the last poll, we have to search entire /proc dir
        //It is expensive
        for (rewinddir(procdir); (direntp = readdir(procdir));) {
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);

                fd = open(name, O_RDONLY);
                if (fd == -1 ) {
                        snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.  errono is = %d", name, errno);
                        mErrorState = -1 ;
                        continue;
		}

                int byteRead = read(fd,&currproc, sizeof(currproc));
                if (byteRead <= 0) {
                        snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.  errono is = %d", name, errno);
                        mErrorState = -1 ;
                        (void) close(fd);
                        continue;
                }
                close(fd);
                if (currproc.pr_uid == uid) {
                        iCounter++;
                }
// 		} else {
// 			int ioCtl;
//                         ioCtl = ioctl(fd, PIOCPSINFO, &old_currproc);
//                         if (ioCtl < 0) {
//                                 snprintf(mErrorMessage,sizeof(mErrorMessage),"2102154 IOCTL FAILURE for P ID %s.  errono is = %d", name, errno);
//                                 mErrorState = -1 ;
//                                 (void) close(fd);
//                                 closedir(procdir);
//                                 return 0;
// 			}
// 			if (old_currproc.pr_uid == uid) {
// 				iCounter++;
// 			}
// 		}
// 		(void) close(fd);
	}
	closedir(procdir);
	return iCounter;
}


// Getting the list of processes for specific user
#ifndef  _STANDALONE_
int ProcInfo::getUserProcessNames(JNIEnv *env, jobject obj, int iCount, int osType, long uid, RWCString strProcsName[])
#else
int ProcInfo::getUserProcessNames(int iCount, int osType, long uid, RWCString strProcsName[])
#endif
{
#ifndef  _STANDALONE_
	RWCString pName;
	jstring utf_str;
	jclass clazz;
	clazz = env->FindClass ("java/lang/String");
	jobjectArray newArr;

        iCount = getProcName (env, obj, iCount, strProcsName, uid, osType);
/*
	//newArr = (jobjectArray)env->NewObjectArray (iCount, clazz, env->NewStringUTF (""));
	newArr = (jobjectArray)env->NewObjectArray (iCount, clazz, NULL);

	for (int i = 0; i < iCount; i++) {
		utf_str = env->NewStringUTF (strProcsName[i]);
		env->SetObjectArrayElement (newArr, i, utf_str);
                env->DeleteLocalRef (utf_str);
	}
*/
#else
        iCount = getProcName (iCount, strProcsName, uid, osType);
#endif
	//snprintf(mLogString,sizeof(mLogString),"Native Logging: Function getUserProcessNames: Ends");
	//logDbg1(env,obj,mLogString);

	return iCount;
}

#ifndef  _STANDALONE_
int ProcInfo::getProcName (JNIEnv *env, jobject obj, int iSize, RWCString procNames[],long uid, int osType) {
#else
int ProcInfo::getProcName (int iSize, RWCString procNames[],long uid, int osType) {
#endif

	//Reset the parameters
        int result  = 0;
        mOSVersionFlag = osType;
        mErrorState = 0 ;
        psinfo currproc ;
        struct dirent *direntp;
        static DIR *procdir;
        int procCounter = 0;
        char name[32] = {0};
        RWCString tempProcName;
        RWCString processName= (RWCString)"oracle";

        seteuid(0);
        int fd = 0 ;
        if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s\n", PROCFS);
		mErrorState = -1;
		return -1;
	}

        //If the process is not found in the list of the Pids stored
        //from the last poll, we have to search entire /proc dir
        //It is expensive
        for (rewinddir (procdir); (direntp = readdir (procdir));) {
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);

                fd = open (name, O_RDONLY);
                if (fd < 0) {
                        continue;
		}

                int byteRead = read(fd,&currproc , sizeof(currproc));
                if (byteRead <= 0) {
                        close(fd);
                        continue;
                }
                close(fd);
                if (currproc.pr_uid != uid) {
                        continue;
                }
                if (procCounter >= iSize) {
                        break;
                }
                tempProcName = (RWCString) currproc.pr_fname;
                if (tempProcName.index ("oracle", 0, RWCString::ignoreCase) != RW_NPOS) {
                         procNames[procCounter] = oracleProc(currproc.pr_psargs);
                }
                else if (tempProcName.index ("libhttpd.ep", 0, RWCString::ignoreCase) != RW_NPOS) {
                        procNames[procCounter] = "httpd";
                } else
                        procNames[procCounter] = tempProcName;
                procCounter++;
        }
        closedir(procdir);
        return procCounter;
}

// To get the number command line for process running under specific user
#ifndef  _STANDALONE_
int ProcInfo::getUsrCmdMatchCount (JNIEnv *env, jobject obj, int osType, RWCString strProcName, long uid) {
#else
int ProcInfo::getUsrCmdMatchCount (int osType, RWCString strProcName, long uid) {
#endif
        //Reset the parameters
        int result  = 0;
        mOSVersionFlag = osType;
        mAvailableFlag = 0; //if one instance of the process found, this flag is set to 1
        mErrorState = 0;
        psinfo currproc;
        struct dirent *direntp;
        static DIR *procdir;
        int iCounter = 0;
	seteuid(0);
        int fd = 0;
        char errStr[384] = {0};
        char name[32] = {0};
        RWCString full_cmd;
        int psinfo_args_len;

        if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s\n", PROCFS);
                mErrorState = -1;
                return 0;
	}

        //If the process is not found in the list of the Pids stored
        //from the last poll, we have to search entire /proc dir
        //It is expensive
        for (rewinddir(procdir); (direntp = readdir(procdir));) {
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
		fd = open (name, O_RDONLY);
                if (fd == -1) {
                        continue;
		}
// 		if (mOSVersionFlag == 1) { //5.6 and above
                int byteRead = read(fd, &currproc, sizeof(currproc));
                if (byteRead <= 0) {
                        close(fd);
                        continue;
                }
//			strcpy(proc_cmd,currproc.pr_psargs);
// 		} else {
// 			int ioCtl;
//                         ioCtl = ioctl(fd, PIOCPSINFO, &old_currproc);
//                         if (ioCtl < 0) {
//                                 close(fd);
//                                 continue;
// // 	            snprintf(mErrorMessage,sizeof(mErrorMessage),"2102154 IOCTL FAILURE for P ID %s.  errono is = %d",name, errno);
// // 				mErrorState = -1 ;
// // 				(void) close(fd);
// // 				closedir(procdir);
// // 				return 0;
// 			}
// //			strcpy(proc_cmd,old_currproc.pr_psargs);
// 		}
		(void) close(fd);
		if (currproc.pr_uid != uid ) {
                        continue;
                }

                psinfo_args_len = strlen(currproc.pr_psargs);
                full_cmd = currproc.pr_psargs;
                if (psinfo_args_len > 70) {
                        full_cmd = longargs(currproc);
                        if (full_cmd.length() < psinfo_args_len) {
                                full_cmd = currproc.pr_psargs;
                        }
                }
                if (full_cmd.index (strProcName, 0, RWCString::ignoreCase) == RW_NPOS) {
                        continue;
                }
                iCounter++;
        }
        closedir(procdir);
        return iCounter;
}

// To get the command line for process running under specific user
#ifndef  _STANDALONE_
int ProcInfo::getUsrCmdMatch(JNIEnv *env, jobject obj, int size, int osType, RWCString process, RWCString procCmd[], long uid) {
#else
int ProcInfo::getUsrCmdMatch(int size, int osType, RWCString process, RWCString procCmd[], long uid) {
#endif

	//Reset the parameters
//	const RWCString procName = process;
        int result  = 0;
        mOSVersionFlag = osType;
        mAvailableFlag = 0 ; //if one instance of the process found, this flag is set to 1
        mErrorState = 0 ;
        psinfo currproc ;
        struct dirent *direntp;
        static DIR *procdir;
        int iCounter = 0;
        char errStr[384];
        seteuid(0);
        
        RWCString full_cmd;
        int psinfo_args_len = 0;
        char name[32] = {0};

        int fd = 0 ;
    
        if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s\n", PROCFS);
                mErrorState = -1;
                return -1;
	}
    
        //If the process is not found in the list of the Pids stored
        //from the last poll, we have to search entire /proc dir
        //It is expensive
        for (rewinddir (procdir); (direntp = readdir (procdir));) {
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
// 		if (mOSVersionFlag == 1) {//5.6 and above
// 			strcpy(name, PROCFS);
//             strcat(name, "/");
//             strcat(name, direntp->d_name);
//             strcat(name, "/psinfo");
// 		} else
//                         snprintf(name,sizeof(name),"/%s/%s", PROCFS, direntp->d_name);
//                 //strcpy(name, direntp->d_name);
		fd = open (name, O_RDONLY);
                if (fd < 0) {
                        continue;
		}

// 		if (mOSVersionFlag == 1) {//5.6 and above
                int byteRead = read(fd,&currproc, sizeof(currproc));
            	if (byteRead <= 0) {
                        close(fd);
                        continue;
                }
		
// 		strcpy(proc_cmd, currproc.pr_psargs);
// 		} else {
// 			int ioCtl;
//             		ioCtl = ioctl (fd, PIOCPSINFO, &old_currproc);
// 			if (ioCtl < 0) {
//                                 close(fd);
//                                 continue;
// // 				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102154 IOCTL FAILURE for P ID %s.  errono is =  %d", name, errno);
// //                 	mErrorState = -1 ;
// //                 	(void) close(fd);
// //                		 closedir(procdir);
// //                 	return -1;
// 			}
//             		strcpy(proc_cmd, old_currproc.pr_psargs);
// 		}
		(void) close (fd);
                if (currproc.pr_uid != uid) {
                        continue;
                }
                if (iCounter >= size) {
                        break;
                }
                full_cmd = currproc.pr_psargs;
                if (full_cmd.index (process, 0, RWCString::ignoreCase) == RW_NPOS)  {
                        continue;
                }

                psinfo_args_len = strlen(currproc.pr_psargs);
                if (psinfo_args_len > 70) {
                        full_cmd = longargs(currproc);
                        if (full_cmd.length() < psinfo_args_len) {
                                full_cmd = currproc.pr_psargs;
                        }
                }
                procCmd[iCounter++] = full_cmd;

#if 0
		if (strlen(proc_cmd) < 70) {
                        proc_Cmd = (RWCString) proc_cmd;
                } else {
			proc_Cmd = longargs(currproc.pr_pid,errStr);
			if (proc_Cmd.index ("ioctl failed", 0, RWCString::ignoreCase) == RW_NPOS) {
				//snprintf(mLogString,sizeof(mLogString),"Native: Got LONGARGS:");
				//logDbg1(env,obj,mLogString);
			} else {
				//snprintf(mLogString,sizeof(mLogString),"Native: didnt Get LONGARGS:");
#ifndef  _STANDALONE_
				//logDbg1(env,obj,mLogString);
#else
				//logDbg1(LogString);
#endif
				proc_Cmd=(RWCString) proc_cmd;
                        }
		}
		PArgs = proc_cmd;
		RPArgs = (RWCString) proc_cmd;
		if (RPArgs.index (procName, 0, RWCString::ignoreCase) == RW_NPOS)  {
                        continue;
		}
		if (iCounter < size) {
			if (currproc.pr_uid == uid) {
                                if(proc_Cmd.isNull())
                                        procCmd[iCounter] = RPArgs;
                                else
                                        procCmd[iCounter] = proc_Cmd;
			} else {
				continue;
			}
		} else {
			break;
		}
#endif
	}
	closedir(procdir);

        return iCounter;
}

// Execution at user level, i.e.
// userName = Supplied
// Process Name = NULL
// Command Line = NULL
#ifndef  _STANDALONE_
int ProcInfo::executeAtUserLevel(JNIEnv *env, jobject jobj,
                                 int pollType, int proc_id_size, RWCString procIds[]) {
#else
int ProcInfo::executeAtUserLevel(int pollType, int proc_id_size, RWCString procIds[]) {
#endif
	int PCount=0;
	if (pollType == 0) {
		//Regular Poll
#ifndef  _STANDALONE_
		PCount= executeRegularAtUserLevel(env, jobj, proc_id_size, procIds);
#else
		PCount= executeRegularAtUserLevel(proc_id_size, procIds);
#endif
		return PCount;

	} else {
		// Fast Poll
#ifndef  _STANDALONE_
		executeFastAtUserLevel(env, jobj);
#else
		executeFastAtUserLevel();
#endif
		return PCount;
	}
}

#ifndef  _TRY_OLD_EXECUTION_ 
int getFdCount(const char *pid)
{
        struct dirent *dp = NULL;
        char name[32] = {0};
        DIR *dir = NULL;
        int num_fds = 0;
        snprintf(name, sizeof(name), "/proc/%s/fd", pid);
        dir = opendir(name);
        if (dir) {
                while ((dp = readdir(dir))) {
                        if (dp->d_name[0] == '.')
                                continue;
                        num_fds++;
                }
                closedir(dir);
        }
        return num_fds;
}

void ProcInfo::updateProcStats(const psinfo & psi)
{
        char pid[8] = {0};
        float tmpSize;

        snprintf(pid, sizeof(pid), "%d", psi.pr_pid);
        mAvailableFlagAtUserLvl = 1;
        if (mSumCPUUtillization < 0) {
                mSumCPUUtillization = 0.0;
        }
        RWCString *npid = new RWCString(pid);
//                 SingleProcInstance proc_inst(pid);
//                 proc_inst.execute(currproc);
//                 tmpSize = proc_inst.getSize();
        tmpSize = psi.pr_size;
        if (mMaxSize < tmpSize) {
                mMaxSize = tmpSize;
        }
//                 mTotalSize += proc_inst.getTotalSize();
//                 tmpSize = proc_inst.getRsSize();
        mTotalSize += tmpSize;
        tmpSize = psi.pr_rssize;
        if (mMaxRsSize < tmpSize) {
                mMaxRsSize = tmpSize;
        }
//                 mTotalRsSize += proc_inst.getTotalRsSize();
//                 mSumThreads += proc_inst.getThreads () ;
        mTotalRsSize += tmpSize;
        mSumThreads += psi.pr_nlwp;
//                 mSumFds += proc_inst.getFds();
//                 mSumCPUUtillization += proc_inst.getCpuPerc();
        mSumFds += getFdCount(pid);
        mSumCPUUtillization += FRC2PCT(psi.pr_pctcpu);
        mAvailableFlag = 100;
        mNumberOfInstances++;
//                 mNewProcIdDict.insertKeyAndValue(pid, NULL);
//                delete pid;
        if (!mNewProcIdDict.find(npid)) {
                mNewProcIdDict.insertKeyAndValue(npid, NULL);
                mNumberOfNewInstances++;
        } else {
                delete npid;
        }
}
 
// Regular polling at User level
#ifndef  _STANDALONE_
int ProcInfo::executeRegularAtUserLevel(JNIEnv *env, jobject jobj,
                                        int proc_id_size, RWCString procIds[])
#else
int ProcInfo::executeRegularAtUserLevel(int proc_id_size, RWCString procIds[])
#endif
{
        int pidCount=0;
        int result  = 0;
        mNumberOfInstances = 0 ;
        mNumberOfNewInstances = 0 ;
        mSumCPUUtillization = -1 ;
        mAvailableFlag = 0 ;
        mAvailableFlagAtUserLvl = 0;
        double cpuSecMillis = 0 ;
        double mRealSec  = 0;
        float tmpSize = 0 ;
        mMaxSize = 0 ;
        mMaxRsSize = 0 ;
        mSumThreads = 0 ;
        mSumFds = 0 ;
        mTotalSize = 0;
        mTotalRsSize = 0;
        mErrorState = 0 ;
        int countOfContrinutingProcs = 0 ;
        char mLogString[1280];
        long uid = -857320316;
        int usrProcessAvail = 0;
        uid = atol(mUserName);
        snprintf(mLogString,sizeof(mLogString),"UID(Execute Reg AtUserLevel):%d", uid);
#ifndef  _STANDALONE_
        logDbg1(env,jobj,mLogString);
#else
        logDbg1(mLogString);
#endif
        psinfo currproc ;
        struct dirent *direntp;
        static DIR *procdir;
        int fd = 0;
        char name[32] = {0};
        seteuid(0);

        if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
                mErrorState = -1;
                uid = NULL;
                return 0;
        }

        /**
         * Store pid's from mNewProcIdDict.
         */
        PidHash pidHash(RWCString::hash);
        PidPtrHashItr itr(mNewProcIdDict);
        for(;itr();) {
                pidHash.insert(*itr.key());
        }

        for (rewinddir (procdir); (direntp = readdir (procdir));) {
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
                fd = open (name, O_RDONLY);
                if (fd < 0) {
                        if ((!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
                                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
                                         name,strerror(errno));
                                mErrorState = 100 ;
                        }
                        continue ;
                }
                int byteRead = read(fd, &currproc, sizeof(currproc));
                if ( byteRead <= 0 ) {
                        snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s",
                                 name, strerror(errno));
                        mErrorState = -1 ;
                        (void) close(fd);
                        continue;
                }
                (void) close (fd);
                if (currproc.pr_uid != uid) {
                        continue;
                }
                if (pidCount >= proc_id_size) {
                        break;
                }
                snprintf(mLogString, sizeof(mLogString), "<Process Name>%s\n<< Process Added >> %d",
                        currproc.pr_fname, currproc.pr_pid);
#ifndef  _STANDALONE_
                logDbg1(env,jobj,mLogString);
#else
                logDbg1(mLogString);
#endif
                updateProcStats(currproc);
                if (NEED_PID(currproc.pr_dmodel)) {
                        procIds[pidCount++]= (RWCString)direntp->d_name;
                }
                pidHash.remove(direntp->d_name);
        }
        closedir(procdir);
        /**
         * Remove pid's from mNewProcIdDict that do not exist.
         */
        RWTValHashTableIterator<RWCString> pidItr(pidHash);
        for (; pidItr();) {
              RWCString tmpPid = pidItr.key ();
               RWCString *oldPid = mNewProcIdDict.remove(&tmpPid);
                if (oldPid) {
                        delete oldPid;
                }
        }
        pidHash.clear();
//
//                 RWCString  pid = (RWCString)direntp->d_name;
//                 procFound = 1;
//                 SingleProcInstance * inst = mProcIdDict.findValue(&pid) ;
//                 if ( inst ==  0 ) {
//                         RWCString * npid = new RWCString(pid) ;
//                         inst = new SingleProcInstance(npid);
//                         result = inst->execute(currproc);
//                         mProcIdDict.insertKeyAndValue(npid,inst);
//                         mNumberOfNewInstances ++ ;
//                         if(inst->getSize() == 0)
//                         {
//                                 procIds[pidCount]= pid;
//                                 pidCount++;
//                         }
//                 } else {
//                         result = inst->execute(currproc);
//                         if ( inst->deltaMilliCPUSec() > -1 ) {
//                                 cpuSecMillis += inst->deltaMilliCPUSec()  ;
//                                 mRealSec  += inst->deltaSecs() ;
//                                 tmpSize = inst->getSize();

//                                 if(tmpSize == 0)
//                                 {
//                                         procIds[pidCount]= pid;
//                                         pidCount++;
//                                 }
//                                 if (mMaxSize < tmpSize)
//                                         mMaxSize = tmpSize ;
//                                 mTotalSize += inst->getTotalSize();
//                                 tmpSize = inst->getRsSize();
                  	    
//                                 if (mMaxRsSize < tmpSize)
//                                         mMaxRsSize = tmpSize ;
//                                 mTotalRsSize += inst->getTotalRsSize();
//                                 mSumThreads += inst->getThreads () ;
//                                 mSumFds += inst->getFds () ;
//                                 countOfContrinutingProcs ++ ;
//                         }
//                         RWCString  * tmpPid = oldPids.remove(&pid); //Remove if from old
// //                         fprintf(stderr, "-tmp: %p; %s\n", tmpPid, tmpPid->data());
//                         delete tmpPid ;
//                 }
//                 usrProcessAvail = 0;
//         }
//         closedir(procdir);
//         if (procFound != 0) {
//                 if ( countOfContrinutingProcs > 0  ) {//Means first time
//                         // half second is substracted from cpuSecMillis to compensate for its
//                         // being in nano second
//                         mSumCPUUtillization =  cpuSecMillis  /
//                                 (( mRealSec * 10000  * mNumCpus ) / countOfContrinutingProcs );
//                         // cpuSecMillis has the precision of millisec and real sec has precision of sec
//                         // So it is probable we can get percentage more that 100
//                         if ( mSumCPUUtillization > 100 ) {
//                                 mSumCPUUtillization = 100 ;
//                         }
//                         if ( mSumCPUUtillization < 0 )
//                                 mSumCPUUtillization = 0 ;
//                 }
//                 while ( oldPids.entries()) {
//                         RWCString * opid = oldPids.get();
//                         SingleProcInstance * killedProc = mProcIdDict.findValue(opid);
//                         mProcIdDict.remove(opid);
// //                         fprintf(stderr, "-tmp: %p; %s\n", opid, opid->data());
//                         delete opid ;
//                         delete killedProc;
//                 }

//                 mNumberOfInstances =  mProcIdDict.entries();
//                 if ( mNumberOfInstances )
//                         mAvailableFlag = 100 ;
//         }
//         uid = NULL;
//         (void) oldPids.clearAndDestroy();
        return pidCount;
}

#else  // _TRY_OLD_EXECUTION_
// Regular polling at User level
#ifndef  _STANDALONE_
int ProcInfo::executeRegularAtUserLevel(JNIEnv *env, jobject jobj,
                                        int proc_id_size, RWCString procIds[])
#else
int ProcInfo::executeRegularAtUserLevel(int proc_id_size, RWCString procIds[])
#endif
{
        int pidCount=0;
        int result  = 0;
        mNumberOfInstances = 0 ;
        mNumberOfNewInstances = 0 ;
        mSumCPUUtillization = -1 ;
        mAvailableFlag = 0 ;
        mAvailableFlagAtUserLvl = 0;
        double cpuSecMillis = 0 ;
        double mRealSec  = 0;
        float tmpSize = 0 ;
        mMaxSize = 0 ;
        mMaxRsSize = 0 ;
        mSumThreads = 0 ;
        mSumFds = 0 ;
        mTotalSize = 0;
        mTotalRsSize = 0;
        mErrorState = 0 ;
        int countOfContrinutingProcs = 0 ;
        char mLogString[1280];
        long uid = -857320316;
        int usrProcessAvail = 0;
        uid = atol(mUserName);
        snprintf(mLogString,sizeof(mLogString),"UID(Execute Reg AtUserLevel):%d", uid);
#ifndef  _STANDALONE_
        logDbg1(env,jobj,mLogString);
#else
        logDbg1(mLogString);
#endif
        psinfo currproc ;
        struct dirent *direntp;
        static DIR *procdir;
        seteuid(0);
        char errStr[384];
        int procFound = 0;
        if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
                mErrorState = -1;
                uid = NULL;
                return 0;
        }

        int fd = 0;
        char name[32] = {0};
        char proc_name[128];
        RWTPtrSlist <RWCString>  oldPids;
        RWTPtrHashDictionaryIterator<RWCString,SingleProcInstance>
                itr(mProcIdDict);
        for(;itr();) {
                RWCString *tmp = new RWCString(*itr.key());
//                 fprintf(stderr, "+tmp= %p; %s\n", tmp, tmp->data());
                oldPids.append(tmp);
//                 oldPids.append(new RWCString(*itr.key()));
        }
        for (rewinddir (procdir); (direntp = readdir (procdir));) {
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
                fd = open (name, O_RDONLY);
                if (fd == -1 ) {
                        if ((!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
                                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
                                         name,strerror(errno));
                                mErrorState = 100 ;
                        }
                        continue ;
                }
                int byteRead = read(fd, &currproc, sizeof(currproc));
                if ( byteRead <= 0 ) {
                        snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s",
                                 name, strerror(errno));
                        mErrorState = -1 ;
                        (void) close(fd);
                        continue;
                }
                (void) close (fd);
                if (currproc.pr_uid != uid) {
                        continue;
                }
                if (pidCount >= proc_id_size) {
                        break;
                }
                snprintf(mLogString, sizeof(mLogString), "<Process Name>%s\n<< Process Added >> %d",
                        currproc.pr_fname, currproc.pr_pid);
#ifndef  _STANDALONE_
                        logDbg1(env,jobj,mLogString);
#else
                        logDbg1(mLogString);
#endif
                mAvailableFlagAtUserLvl = 1;
                RWCString  pid = (RWCString)direntp->d_name;
                procFound = 1;
                SingleProcInstance * inst = mProcIdDict.findValue(&pid) ;
                if ( inst ==  0 ) {
                        RWCString * npid = new RWCString(pid) ;
                        inst = new SingleProcInstance(npid);
                        result = inst->execute(currproc);
                        mProcIdDict.insertKeyAndValue(npid,inst);
                        mNumberOfNewInstances ++ ;
                        if(inst->getSize() == 0)
                        {
                                procIds[pidCount]= pid;
                                pidCount++;
                        }
                } else {
                        result = inst->execute(currproc);
                        if ( inst->deltaMilliCPUSec() > -1 ) {
                                cpuSecMillis += inst->deltaMilliCPUSec()  ;
                                mRealSec  += inst->deltaSecs() ;
                                tmpSize = inst->getSize();

                                if(tmpSize == 0)
                                {
                                        procIds[pidCount]= pid;
                                        pidCount++;
                                }
                                if (mMaxSize < tmpSize)
                                        mMaxSize = tmpSize ;
                                mTotalSize += inst->getTotalSize();
                                tmpSize = inst->getRsSize();
                  	    
                                if (mMaxRsSize < tmpSize)
                                        mMaxRsSize = tmpSize ;
                                mTotalRsSize += inst->getTotalRsSize();
                                mSumThreads += inst->getThreads () ;
                                mSumFds += inst->getFds () ;
                                countOfContrinutingProcs ++ ;
                        }
                        RWCString  * tmpPid = oldPids.remove(&pid); //Remove if from old
//                         fprintf(stderr, "-tmp: %p; %s\n", tmpPid, tmpPid->data());
                        delete tmpPid ;
                }
                usrProcessAvail = 0;
        }
        closedir(procdir);
        if (procFound != 0) {
                if ( countOfContrinutingProcs > 0  ) {//Means first time
                        // half second is substracted from cpuSecMillis to compensate for its
                        // being in nano second
                        mSumCPUUtillization =  cpuSecMillis  /
                                (( mRealSec * 10000  * mNumCpus ) / countOfContrinutingProcs );
                        // cpuSecMillis has the precision of millisec and real sec has precision of sec
                        // So it is probable we can get percentage more that 100
                        if ( mSumCPUUtillization > 100 ) {
                                mSumCPUUtillization = 100 ;
                        }
                        if ( mSumCPUUtillization < 0 )
                                mSumCPUUtillization = 0 ;
                }
                while ( oldPids.entries()) {
                        RWCString * opid = oldPids.get();
                        SingleProcInstance * killedProc = mProcIdDict.findValue(opid);
                        mProcIdDict.remove(opid);
//                         fprintf(stderr, "-tmp: %p; %s\n", opid, opid->data());
                        delete opid ;
                        delete killedProc;
                }

                mNumberOfInstances =  mProcIdDict.entries();
                if ( mNumberOfInstances )
                        mAvailableFlag = 100 ;
        }
        uid = NULL;
        (void) oldPids.clearAndDestroy();
        return pidCount;
}
#endif

// Fast polling at User level
#ifndef  _STANDALONE_
void ProcInfo::executeFastAtUserLevel(JNIEnv *env, jobject jobj) {
#else
void ProcInfo::executeFastAtUserLevel() {
#endif
	//Reset the parameters
        int result  = 0;
        mAvailableFlagAtUserLvl = 0;
        mErrorState = 0 ;
        psinfo currproc ;
        prpsinfo old_currproc ;
        struct dirent *direntp = NULL;
        static DIR *procdir = NULL;
        seteuid(0);
        int fd = 0 ;
        long uid = -857320316;
	uid = atol(mUserName);

	PidPtrHashItr itr(mNewProcIdDict);
	char name[32] = {0};
	char mLogString[512] = {0};

	snprintf(mLogString,sizeof(mLogString),"UID(Execute Fast):%d\n"
			"Existing instances: %d", uid, mNewProcIdDict.entries());
#ifndef  _STANDALONE_
	logDbg1(env,jobj,mLogString);
#else
	logDbg1(mLogString);
#endif

	//char proc_name[100];
        for ( ; itr() ; ) {
                RWCString pid = *itr.key();
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, pid.data());
		fd = open (name, O_RDONLY);
		if (fd < 0) {
                        if ( (!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
                                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
                                         name,strerror(errno));
                                mErrorState = 100 ;
			}
			continue ;
		}

                int byteRead = read(fd,&currproc, sizeof(currproc));
                if ( byteRead <= 0 ) {
                        snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s\n",
                                 name,strerror(errno));
                        mErrorState = -1 ;
                        (void) close(fd);
                }
                close(fd);
                if (currproc.pr_uid == uid) {
                        mAvailableFlagAtUserLvl = 1;
                        return ; // one Process found no More Need to iterate
                }
        }

	if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
                mErrorState = -1 ;
                return ;
	}
        for (rewinddir (procdir); (direntp = readdir (procdir));) {
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"%s/%s/psinfo", PROCFS, direntp->d_name);
                fd = open (name, O_RDONLY);
                if (fd == -1 ) {
                        if ( (!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
                                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
                                         name,strerror(errno));
                                mErrorState = 100 ;
                        }
                        continue ;
                }
                int byteRead = read(fd,&currproc, sizeof(currproc));
                if ( byteRead <= 0 ) {
                        snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s",
                                 name,strerror(errno));
                        mErrorState = -1 ;
                        (void) close(fd);
                        continue;
                }
                close(fd);
                if (currproc.pr_uid == uid) {
                        mAvailableFlagAtUserLvl = 1;
                        closedir(procdir);
                        return;
                }
//                 } else {
//                         int ioCtl;
//                         ioCtl = ioctl (fd, PIOCPSINFO, &old_currproc);
//                         if (ioCtl < 0) {
//                                 sprintf (mErrorMessage,
//                                          "2102154 IOCTL FAILURE for PID %s.\nError is %s",
//                                          name,strerror(errno));
//                                 mErrorState = -1 ;
//                                 (void) close(fd);
//                                 closedir(procdir);
//                                 uid = NULL;
//                                 return ;
//                         }
//                         if (old_currproc.pr_uid == uid) {
//                                 mAvailableFlagAtUserLvl = 1;
//                                 (void) close(fd);
//                                 break;
//                         }
//                 }
//                 (void) close (fd);
        }
        closedir(procdir);
}

//************************** ASHISH ************************************//

//************************** NANDA STARTS******************************//

int ProcInfo::getCmdMatchCountNOProcsNoUser (int osType)
{
	//Reset the parameters
        int result  = 0;
        // NAN const RWCString procName = process;
        mOSVersionFlag = osType;
        mAvailableFlag = 0 ; //if one instance of the process found, this flag                                 is set to 1
        mErrorState = 0 ;
        struct dirent *direntp;
        static DIR *procdir;
        int iCounter = 0;
	char name[32] = {0};

        seteuid(0);
        if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
                mErrorState = -1;
                return 0;
	}
        //If the process is not found in the list of the Pids stored
        //from the last poll, we have to search entire /proc dir
        //It is expensive
        for (rewinddir (procdir); (direntp = readdir (procdir)); ) {
                if (direntp->d_name[0] == '.')
                        continue;
		iCounter++;
        }
//                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
// 		if ( mOSVersionFlag == 1 ) {//5.6 and above
// 			strcpy(name,PROCFS);
//                         strcat(name,"/");
//                         strcat(name,direntp->d_name);
//                         strcat(name,"/psinfo");
// 		} else
// 			strcpy(name,direntp->d_name);
//		fd = open (name, O_RDONLY);
//                if (fd == -1 ) {
//                        continue;
//                         snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file %s.\nError is %s",
//                                  name,strerror(errno));
//                         mErrorState = -1 ;
//                         closedir(procdir);
//                         return 0;
// 		}
// 		if ( mOSVersionFlag == 1 ) {//5.6 and above
// 			int byteRead = read(fd,&currproc ,(sizeof(currproc) + 1));
// 			if ( byteRead == 0 ) {
//                                 close(fd);
//                                 continue;
// 				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s",
//                                          name,strerror(errno));
//                                 mErrorState = -1 ;
//                                 (void) close(fd);
//                                 closedir(procdir);
//                                 return 0;
// 			}
// 			strcpy(proc_cmd,currproc.pr_psargs);
// 		} else {
// 			int ioCtl;
//                         ioCtl = ioctl (fd, PIOCPSINFO, &old_currproc);
// 			if (ioCtl < 0) {
//                                 close(fd);
//                                 continue;
// 				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102154 IOCTL FAILURE for PID %s.\nError is %s",
//                                          name,strerror(errno));
// 				mErrorState = -1 ;
//                                 (void) close(fd);
//                                 closedir(procdir);
//                                 return 0;
// 			}
// 			strcpy(proc_cmd,old_currproc.pr_psargs);
// 		}
//
// 		(void) close (fd);
// 		proc_Cmd = (RWCString) proc_cmd;
        closedir(procdir);
        return iCounter;
}

int ProcInfo::getCmdMatchNoUserNoProcess(int size, int osType, RWCString procCmd[])
{
	//Reset the parameters
        // NAN const RWCString procName = process;
        int result  = 0;
        mOSVersionFlag = osType;
        mAvailableFlag = 0 ; //if one instance of the process found, this flag is set to 1
        mErrorState = 0 ;
        psinfo currproc ;
        struct dirent *direntp;
        static DIR *procdir;
        int iCounter = 0;
        seteuid(0);
        int fd = 0 ;
        RWCString full_cmd;

        char errStr[384];
        if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s\n", PROCFS, strerror(errno));
                mErrorState = -1;
                return -1;
	}

	char name[32] = {0};
        //If the process is not found in the list of the Pids stored
        //from the last poll, we have to search entire /proc dir
        //It is expensive
        for (rewinddir (procdir); (direntp = readdir (procdir)); ) {
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
		fd = open (name, O_RDONLY);
                if (fd < 0) {
                        continue;
		}

// 		if ( mOSVersionFlag == 1 ) {//5.6 and above
                int byteRead = read(fd,&currproc, sizeof(currproc));
                if ( byteRead <= 0 ) {
                        close(fd);
                        continue;
                }
// 			strcpy(proc_cmd,currproc.pr_psargs);
// 		} else {
// 			int ioCtl;
//                         ioCtl = ioctl (fd, PIOCPSINFO, &old_currproc);
// 			if (ioCtl < 0) {
//                                 close(fd);
//                                 continue;
// // 				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102154 IOCTL FAILURE for PID %s.\nError is %s",
// //                                          name,strerror(errno));
// //                                 mErrorState = -1 ;
// //                                 (void) close(fd);
// //                                 closedir(procdir);
// //                                 return -1;
// 			}
// 			strcpy(proc_cmd,old_currproc.pr_psargs);
// 		}
		(void) close (fd);

                if (iCounter >= size) {
                        break;
                }
                int psinfo_args_len = strlen(currproc.pr_psargs);
                full_cmd = currproc.pr_psargs;
                if (psinfo_args_len > 70) {
                        full_cmd = longargs(currproc);
                        if (full_cmd.length() < psinfo_args_len) {
                                full_cmd = (RWCString) currproc.pr_psargs;
                        }
                }
                procCmd[iCounter++] = full_cmd;

#if 0                
		/* Desaiproc_Cmd = (RWCString) proc_cmd;*/
                if( strlen(proc_cmd) < 70 ) {
                        proc_Cmd = (RWCString) proc_cmd;
		}

                else
		{
			proc_Cmd = longargs(currproc.pr_pid,errStr);

			if (proc_Cmd.index ("ioctl failed", 0, RWCString::ignoreCase) == RW_NPOS)
			{
                                // snprintf(mLogString,sizeof(mLogString),"Native: Got LONGARGS:");
#ifndef  _STANDALONE_
                                // 	logDbg1(env,obj,mLogString);
#else
                                // 	logDbg1(LogString);
#endif
			}
			else
			{
                                //snprintf(mLogString,sizeof(mLogString),"Native: didnt Get LONGARGS:");
                                //logDbg1(env,obj,mLogString);
                                proc_Cmd=(RWCString) proc_cmd;
			}

                }

		
		PArgs = proc_cmd;
		RPArgs = (RWCString) proc_cmd;
		
		if (iCounter < size) {
			if(proc_Cmd.isNull())
                                procCmd[iCounter] = RPArgs;
			else
                                procCmd[iCounter] = proc_Cmd;
			//procCmd[iCounter] = (RWCString)proc_Cmd;
		} else {
			break;
		}
#endif
	}
	closedir(procdir);
        return iCounter;
}

int ProcInfo::getUserCmdMatchCountNOProcs (int osType, long uid)
{
        int result  = 0;
        mOSVersionFlag = osType;
        mAvailableFlag = 0 ; //if one instance of the process found, this flag is set to 1
        mErrorState = 0 ;
        psinfo currproc ;
        struct dirent *direntp;
        static DIR *procdir;
        int iCounter = 0;

        seteuid(0);
        int fd = 0 ;
        if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
                mErrorState = -1;
                return 0;
        }
        char name[32] = {0};
        //If the process is not found in the list of the Pids stored
        //from the last poll, we have to search entire /proc dir
        //It is expensive
        for (rewinddir (procdir); (direntp = readdir (procdir)); ) {
                //char buf[30];
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
//                 if ( mOSVersionFlag == 1 ) {//5.6 and above
//                         strcpy(name,PROCFS);
//                         strcat(name,"/");
//                         strcat(name,direntp->d_name);
//                         strcat(name,"/psinfo");
//                 } else {
//                         snprintf(name,sizeof(name),"%s/%s", PROCFS, direntp->d_name);
//                        strcpy(name,direntp->d_name);
//                 }
                fd = open (name, O_RDONLY);
                if (fd == -1 ) {
                        continue;
                }
//                 if ( mOSVersionFlag == 1 ) {//5.6 and above
                        int byteRead = read(fd,&currproc, sizeof(currproc));
                        if ( byteRead <= 0) {
                                close(fd);
                                continue;
                        }
//                         strcpy(proc_cmd,currproc.pr_psargs);
//                 } else {
//                         int ioCtl;
//                         ioCtl = ioctl (fd, PIOCPSINFO, &old_currproc);
//                         if (ioCtl < 0) {
//                                 continue;
//                                 snprintf(mErrorMessage,sizeof(mErrorMessage),"2102154 IOCTL FAILURE for PID %s.\nError is %s",
//                                          name,strerror(errno));
//                                 mErrorState = -1 ;
//                                 (void) close(fd);
//                                 closedir(procdir);
//                                 return 0;
//                         }
//                         strcpy(proc_cmd,old_currproc.pr_psargs);
//                 }

                (void) close (fd);
                if (currproc.pr_uid == uid ) {
                        iCounter++;
                }

        }
        closedir(procdir);
        return iCounter;
}

int ProcInfo::getUsrCmdMatchNoProcess (int size, int osType, RWCString procCmd[], long uid)
{
	//Reset the parameters
        int result  = 0;
        mOSVersionFlag = osType;
        mAvailableFlag = 0 ; //if one instance of the process found, this flag is set to 1
        mErrorState = 0 ;
        psinfo currproc ;
        prpsinfo old_currproc ;
        struct dirent *direntp;
        static DIR *procdir;
        char proc_cmd[1024];
	RWCString proc_Cmd;
        int iCounter = 0;
        char errStr[384];
        seteuid(0);

        int fd = 0 ;
        const char* PArgs;
        RWCString RPArgs;
        RWCString full_cmd;
    
        if (!(procdir = opendir (PROCFS))) {
                snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s\n", PROCFS);
                mErrorState = -1;
                return -1;
	}

        char name[32] = {0};
        //char buf[30];
        //If the process is not found in the list of the Pids stored
        //from the last poll, we have to search entire /proc dir
        //It is expensive
        for (rewinddir (procdir); (direntp = readdir (procdir));) {
                if (direntp->d_name[0] == '.')
                        continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
                fd = open (name, O_RDONLY);
                if (fd == -1) {
                        continue;
                }

                int byteRead = read(fd,&currproc, sizeof(currproc));
                if (byteRead == 0) {
                        close(fd);
                        continue;
                }
//                 } else {
//                         int ioCtl;
//                         ioCtl = ioctl (fd, PIOCPSINFO, &old_currproc);
//                         if (ioCtl < 0) {
//                                 close(fd);
//                                 continue;
// 				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102154 IOCTL FAILURE for P ID %s.  errono is =  %d", name, errno);
//                 mErrorState = -1 ;
//                 (void) close(fd);
//                 closedir(procdir);
//                 return -1;
//                         }
//                    strcpy(proc_cmd, old_currproc.pr_psargs);
//         }
                (void) close (fd);
                if (currproc.pr_uid != uid) {
                        continue;
                }
                if (iCounter >= size) {
                        break;
                }
                int psinfo_args_len = strlen(currproc.pr_psargs);
                full_cmd = currproc.pr_psargs;
                if (psinfo_args_len > 70) {
                        full_cmd = longargs(currproc);
                        if (full_cmd.length() < psinfo_args_len) {
                                full_cmd = (RWCString) currproc.pr_psargs;
                        }
                }
                procCmd[iCounter++] = full_cmd;
#if 0
                if (strlen(proc_cmd) < 70) {
                        proc_Cmd = (RWCString) proc_cmd;
                } else {
                        proc_Cmd = longargs(currproc.pr_pid,errStr);

                        if (proc_Cmd.index ("ioctl failed", 0, RWCString::ignoreCase) == RW_NPOS) {
                        } else {
                                proc_Cmd=(RWCString) proc_cmd;
                        }
                }

                PArgs = proc_cmd;
                RPArgs = (RWCString) proc_cmd;
		
                if (iCounter < size) {
                        if (currproc.pr_uid == uid) {
                                if(proc_Cmd.isNull())
                                        procCmd[iCounter] = RPArgs;
                                else
                                        procCmd[iCounter] = proc_Cmd;
                                //procCmd[iCounter] = proc_Cmd;
                        } else {
                                continue;
                        }
                } else {
                        break;
                }
#endif
        }
        closedir(procdir);
        return iCounter;
}


int ProcInfo::procNameMatchedRegExp(RWCString procArg, RWCString InputProcess) {

    int fullCompare = 0 ;
    if ( InputProcess.contains("/") )
      {
	    fullCompare = 1 ;
	  }
    RWCString arg;
    RWCTokenizer next(procArg);
    arg =  next();
    RWCString nextArgu;
    nextArgu = next();

    if ( fullCompare )
      {
		if ( arg == InputProcess )
        	return 0 ;
      }
    else {
	    RWCString slashedProcArg ;
        RWCTokenizer nextArg(arg);
        RWCString temp ;
        while (!(temp = nextArg("/")).isNull())
         {
	        slashedProcArg = temp ;
	     }
	   if ( slashedProcArg == InputProcess )
           return 0 ;

	   if (!nextArgu.isNull())
	    {
	       RWCString nextslashedProcArg ;
            RWCTokenizer nextArg(nextArgu);
            RWCString nexttemp ;

            while (!(nexttemp = nextArg("/")).isNull())
              {
	            nextslashedProcArg = nexttemp ;
	          }//End of while
            if ( nextslashedProcArg == InputProcess )
              {
	             return 0 ;
	          }//End of if
	    }//End of if

	  }
    return 1 ;
}

int ProcInfo::commandLineMatchFoundRegExp(RWCString progNameWithArgument, RWCString Input_Command) {

	RWCTokenizer next(progNameWithArgument);
    RWCString token = next(); //Advance to first token
    int len = token.length();
    RWCString args = progNameWithArgument(len, (progNameWithArgument.length() - len) ); //String without the process name


    if(args.contains(Input_Command) )
      {
	    return 1;
      }
    return 0 ;
}

#ifndef  _STANDALONE_
int ProcInfo::executeForRegExp(JNIEnv *env, jobject jobj, int pollType, jobjectArray procs, RWCString procIds[]) {
#else
//int ProcInfo::executeForRegExp(int pollType, jobjectArray procs, RWCString procIds[]) {
int ProcInfo::executeForRegExp(int pollType, char **procs, int procslength, RWCString procIds[]) {
#endif
	int PCount=0;
	if (pollType == 0)
	  {
#ifndef  _STANDALONE_
	    PCount= executeRegularForRegExp(env, jobj, procs, procIds);
#else
	    PCount= executeRegularForRegExp(procs, procslength, procIds);
#endif
	    return PCount;
	  }
	return PCount;
}

#ifndef  _TRY_OLD_EXECUTION_
#ifndef  _STANDALONE_
int ProcInfo::executeRegularForRegExp(JNIEnv *env, jobject jobj, jobjectArray procs, RWCString procIds[]) {
#else
int ProcInfo::executeRegularForRegExp(char **procs, int procslength, RWCString procIds[]) {
#endif
	
	int pidCount=0;	
	char mLogMessage[1280];
	int result  = 0;
	mNumberOfInstances = 0 ;
	mNumberOfNewInstances = 0 ;
	mSumCPUUtillization = -1 ;
	mAvailableFlag = 0 ;
	double cpuSecMillis = 0 ;
	double mRealSec  = 0;
	float tmpSize = 0 ;
	mMaxSize = 0 ;
	mMaxRsSize = 0 ;
	mSumThreads = 0 ;
	mSumFds = 0 ;
	mErrorState = 0 ;
	int countOfContrinutingProcs = 0 ;
	mTotalSize = 0;
	mTotalRsSize = 0;
	char mLogString[1280];

	psinfo currproc ;
	prpsinfo old_currproc ;
	/*static DIR *procdir = NULL; // set to null
	static DIR pD;*/
	static DIR *procdir;
	seteuid(0);
	char errStr[384];
	int procFound = 0;
	int arrLength = 0;

	if (!(procdir = opendir (PROCFS))) {
		snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
		mErrorState = -1 ;
	    return 0;
	}

        /**
         * Store pid's from mNewProcIdDict.
         */
        PidHash pidHash(RWCString::hash);
        PidPtrHashItr itr(mNewProcIdDict);
        for(;itr();) {
                pidHash.insert(*itr.key());
        }

#ifndef  _STANDALONE_
	arrLength = env->GetArrayLength(procs);
#else
        arrLength = procslength;
#endif
	int fd = 0;
	char name[32] = {0};
	char dirName[32] = {0};
        RWCString pid;

	for (int iterDir = 0; iterDir < arrLength; iterDir++) {
		fd = 0;
#ifndef  _STANDALONE_
		jstring procObj = (jstring) env->GetObjectArrayElement(procs, iterDir);
		//procObj = (jstring) env->GetObjectArrayElement(procs, iterDir);
		//strcpy (dirName, env->GetStringUTFChars(procObj, NULL));
		//strcpy (dirName, env->GetStringUTFChars(procObj, 0));
                const char *dn = env->GetStringUTFChars(procObj, 0);
#else
                const char *dn = procs[iterDir];
#endif
                if (dn) {
                        pid = (RWCString) dn;
                } else {
                        continue;
                }
#ifndef  _STANDALONE_
		env->DeleteLocalRef(procObj);
#endif
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, pid.data());

		//env->ReleaseStringUTFChars(procObj, dirName);
		//env->DeleteLocalRef(procObj);

		fd = open (name, O_RDONLY);
		if (fd == -1 ) {
			//errorno == 2 means process not found.
			if ((!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
					name,strerror(errno));
	           	mErrorState = 100 ;
			}
			continue ;
		}
			int byteRead = read(fd,&currproc, sizeof(currproc));
			if (byteRead <= 0) {
				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s",
					name, strerror(errno));
				mErrorState = -1 ;
				(void) close(fd);
                                continue;
			}
		(void) close (fd);

		snprintf(mLogString,sizeof(mLogString),"Added <Process Name> %s and <PID> %s", currproc.pr_fname, pid.data());
#ifndef  _STANDALONE_
		logDbg1(env,jobj,mLogString);
#else
		logDbg1(mLogString);
#endif
		procFound = 1;
                updateProcStats(currproc);
                if (NEED_PID(currproc.pr_dmodel)) {
                        procIds[pidCount++]= pid;
                }
                pidHash.remove(pid);
        }
        closedir(procdir);
        /**
         * Remove pid's from mNewProcIdDict that do not exist.
         */
        RWTValHashTableIterator<RWCString> pidItr(pidHash);
        for (; pidItr();) {
                RWCString tmpPid = pidItr.key ();
                RWCString *oldPid = mNewProcIdDict.remove(&tmpPid);
                if (oldPid) {
                        delete oldPid;
                }
        }
        pidHash.clear();
        
//                 mAvailableFlagAtUserLvl = 1;
//                 if (mSumCPUUtillization < 0) {
//                         mSumCPUUtillization = 0.0;
//                 }
//                 RWCString *npid = new RWCString(pid);
//                 SingleProcInstance proc_inst(npid);
//                 proc_inst.execute(currproc);
//                 tmpSize = proc_inst.getSize();
//                 if (NEED_PID(currproc.pr_dmodel)) {
//                         procIds[pidCount++]= pid;
//                 }
//                 if (mMaxSize < tmpSize) {
//                         mMaxSize = tmpSize;
//                 }
//                 mTotalSize += proc_inst.getTotalSize();
//                 tmpSize = proc_inst.getRsSize();
//                 if (mMaxRsSize < tmpSize) {
//                         mMaxRsSize = tmpSize;
//                 }
//                 mTotalRsSize += proc_inst.getTotalRsSize();
//                 mSumThreads += proc_inst.getThreads () ;
//                 mSumFds += proc_inst.getFds() ;
//                 mSumCPUUtillization += proc_inst.getCpuPerc();
//                 mAvailableFlag = 100 ;
//                 mNumberOfInstances++;
//                 if (!mNewProcIdDict.find(npid)) {
//                         mNewProcIdDict.insertKeyAndValue(npid,NULL);
//                 } else {
//                         delete npid;
//                 }
//         }
// 		SingleProcInstance * inst = mProcIdDict.findValue(&pid) ;
// 		if (inst == 0) {
// 			RWCString * npid = new RWCString(pid) ;
// 			inst = new SingleProcInstance(npid);
//                         result = inst->execute(currproc);
// // 			mProcIdDict.insertKeyAndValue(npid,inst);
//                         RWCString *prev = mProcIdDict.find(npid);
//                         if (prev) {
//                                 delete prev;
//                         }
//                         mProcIdDict.insertKeyAndValue(npid, inst);
// //                 } else {
// //                         delete npid;
// //                 }
// 			mNumberOfNewInstances ++ ;
			
// 		        //Nanda To support 64 bit process Starts
// 			if(inst->getSize() == 0)
// 			  {
//                                   procIds[pidCount]= (RWCString)pid;
//                                   pidCount++;
// 			  }
// 			//Nanda To support 64 bit process Ends
// 		} else {
//                         result = inst->execute(currproc);
// 			if ( inst->deltaMilliCPUSec() > -1 ) {
// 			   cpuSecMillis += inst->deltaMilliCPUSec()  ;
// 			   mRealSec  += inst->deltaSecs() ;
// 			   tmpSize = inst->getSize();
// 			   //Nanda To support 64 bit process Starts
// 			   if(tmpSize == 0)
// 			     {
// 			      procIds[pidCount]= (RWCString)pid;
// 			      pidCount++;
// 			     }
			   		  		    
//                           //Nanda To support 64 bit process Ends
// 			   if (mMaxSize < tmpSize)
// 				mMaxSize = tmpSize ;
// 		         mTotalSize += inst->getTotalSize();
// 		         tmpSize = inst->getRsSize();
		         
// 		         if (mMaxRsSize < tmpSize)
// 		         	mMaxRsSize = tmpSize ;
// 		         mTotalRsSize += inst->getTotalRsSize();
// 				 mSumThreads += inst->getThreads () ;
// 		         mSumFds += inst->getFds () ;
// 		         countOfContrinutingProcs ++ ;
// 			}

// 			RWCString  * tmpPid = oldPids.remove(&pid); //Remove if from old
//                         delete tmpPid ;
// 		}
// 	}
// 	closedir(procdir);

// 	if (procFound != 0) {
// 		if (countOfContrinutingProcs > 0) {
// 			mSumCPUUtillization =  cpuSecMillis  /
// 	     		(( mRealSec * 10000  * mNumCpus ) / countOfContrinutingProcs );
// 			if ( mSumCPUUtillization > 100 ) {
// 				mSumCPUUtillization = 100 ;
// 			}
// 			if ( mSumCPUUtillization < 0 )
// 				mSumCPUUtillization = 0 ;
// 		}
// 		while (oldPids.entries()) {
// 			RWCString * opid = oldPids.get();
// 			SingleProcInstance * killedProc = mProcIdDict.findValue(opid);
// 			mProcIdDict.remove(opid);
// 			delete opid ;
// 			delete killedProc ;
// 			//opid = NULL;
// 			//killedProc = NULL;
// 		}

// 		mNumberOfInstances =  mProcIdDict.entries();
// 		if (mNumberOfInstances)
// 			mAvailableFlag = 100 ;
// 	}
// 	(void) oldPids.clearAndDestroy();
        return pidCount;
}
#else  // -D_TRY_OLD_EXECUTION_
#ifndef  _STANDALONE_
int ProcInfo::executeRegularForRegExp(JNIEnv *env, jobject jobj, jobjectArray procs, RWCString procIds[]) {
#else
int ProcInfo::executeRegularForRegExp(char **procs, int procslength, RWCString procIds[]) {
#endif
	
	int pidCount=0;	
	char mLogMessage[1280];
	int result  = 0;
	mNumberOfInstances = 0 ;
	mNumberOfNewInstances = 0 ;
	mSumCPUUtillization = -1 ;
	mAvailableFlag = 0 ;
	double cpuSecMillis = 0 ;
	double mRealSec  = 0;
	float tmpSize = 0 ;
	mMaxSize = 0 ;
	mMaxRsSize = 0 ;
	mSumThreads = 0 ;
	mSumFds = 0 ;
	mErrorState = 0 ;
	int countOfContrinutingProcs = 0 ;
	mTotalSize = 0;
	mTotalRsSize = 0;
	char mLogString[1280];

	psinfo currproc ;
	prpsinfo old_currproc ;
	/*static DIR *procdir = NULL; // set to null
	static DIR pD;*/
	static DIR *procdir;
	seteuid(0);
	char errStr[384];
	int procFound = 0;
	int arrLength = 0;

	if (!(procdir = opendir (PROCFS))) {
		snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
		mErrorState = -1 ;
	    return 0;
	}

	RWTPtrSlist <RWCString>  oldPids;
	RWTPtrHashDictionaryIterator<RWCString,SingleProcInstance>
                itr(mProcIdDict);
        //Fill the oldPids with the pids of the last poll
        for(;itr();)
                oldPids.append(new RWCString(*itr.key()));

#ifndef  _STANDALONE_
	arrLength = env->GetArrayLength(procs);
#else
        arrLength = procslength;
#endif
	//jstring procObj = NULL;

	int fd = 0;
	char name[32] = {0};
	char dirName[32] = {0};
	char pid_name[128];
        RWCString pid;

	for (int iterDir = 0; iterDir < arrLength; iterDir++) {
		fd = 0;
#ifndef  _STANDALONE_
		jstring procObj = (jstring) env->GetObjectArrayElement(procs, iterDir);
		//procObj = (jstring) env->GetObjectArrayElement(procs, iterDir);
		//strcpy (dirName, env->GetStringUTFChars(procObj, NULL));
		//strcpy (dirName, env->GetStringUTFChars(procObj, 0));
                const char *dn = env->GetStringUTFChars(procObj, 0);
#else
                const char *dn = procs[iterDir];
#endif
                if (dn) {
                        pid = (RWCString) dn;
                } else {
                        continue;
                }
#ifndef  _STANDALONE_
		env->DeleteLocalRef(procObj);
#endif
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, pid.data());

		//env->ReleaseStringUTFChars(procObj, dirName);
		//env->DeleteLocalRef(procObj);

		fd = open (name, O_RDONLY);
		if (fd == -1 ) {
			//errorno == 2 means process not found.
			if ((!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
					name,strerror(errno));
	           	mErrorState = 100 ;
			}
			continue ;
		}
			int byteRead = read(fd,&currproc, sizeof(currproc));
			if (byteRead <= 0) {
				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s",
					name, strerror(errno));
				mErrorState = -1 ;
				(void) close(fd);
                                continue;
			}
		(void) close (fd);

		snprintf(mLogString,sizeof(mLogString),"Added <Process Name> %s and <PID> %s", currproc.pr_fname, pid.data());
#ifndef  _STANDALONE_
		logDbg1(env,jobj,mLogString);
#else
		logDbg1(mLogString);
#endif
		procFound = 1;

		SingleProcInstance * inst = mProcIdDict.findValue(&pid) ;
		if (inst == 0) {
			RWCString * npid = new RWCString(pid) ;
			inst = new SingleProcInstance(npid);
                        result = inst->execute(currproc);
// 			mProcIdDict.insertKeyAndValue(npid,inst);
                        RWCString *prev = mProcIdDict.find(npid);
                        if (prev) {
                                delete prev;
                        }
                        mProcIdDict.insertKeyAndValue(npid, inst);
//                 } else {
//                         delete npid;
//                 }
			mNumberOfNewInstances ++ ;
			
		        //Nanda To support 64 bit process Starts
			if(inst->getSize() == 0)
			  {
                                  procIds[pidCount]= (RWCString)pid;
                                  pidCount++;
			  }
			//Nanda To support 64 bit process Ends
		} else {
                        result = inst->execute(currproc);
			if ( inst->deltaMilliCPUSec() > -1 ) {
			   cpuSecMillis += inst->deltaMilliCPUSec()  ;
			   mRealSec  += inst->deltaSecs() ;
			   tmpSize = inst->getSize();
			   //Nanda To support 64 bit process Starts
			   if(tmpSize == 0)
			     {
			      procIds[pidCount]= (RWCString)pid;
			      pidCount++;
			     }
			   		  		    
                          //Nanda To support 64 bit process Ends
			   if (mMaxSize < tmpSize)
				mMaxSize = tmpSize ;
		         mTotalSize += inst->getTotalSize();
		         tmpSize = inst->getRsSize();
		         
		         if (mMaxRsSize < tmpSize)
		         	mMaxRsSize = tmpSize ;
		         mTotalRsSize += inst->getTotalRsSize();
				 mSumThreads += inst->getThreads () ;
		         mSumFds += inst->getFds () ;
		         countOfContrinutingProcs ++ ;
			}

			RWCString  * tmpPid = oldPids.remove(&pid); //Remove if from old
                        delete tmpPid ;
		}
	}
	closedir(procdir);

	if (procFound != 0) {
		if (countOfContrinutingProcs > 0) {
			mSumCPUUtillization =  cpuSecMillis  /
	     		(( mRealSec * 10000  * mNumCpus ) / countOfContrinutingProcs );
			if ( mSumCPUUtillization > 100 ) {
				mSumCPUUtillization = 100 ;
			}
			if ( mSumCPUUtillization < 0 )
				mSumCPUUtillization = 0 ;
		}
		while (oldPids.entries()) {
			RWCString * opid = oldPids.get();
			SingleProcInstance * killedProc = mProcIdDict.findValue(opid);
			mProcIdDict.remove(opid);
			delete opid ;
			delete killedProc ;
			//opid = NULL;
			//killedProc = NULL;
		}

		mNumberOfInstances =  mProcIdDict.entries();
		if (mNumberOfInstances)
			mAvailableFlag = 100 ;
	}
	(void) oldPids.clearAndDestroy();
	return pidCount;
}
#endif  // _TRY_OLD_EXECUTION_
 
#ifndef  _STANDALONE_
int ProcInfo::getUserCount(JNIEnv *env, jobject obj) {
#else
int ProcInfo::getUserCount() {
#endif
	int iCount = 0;
	struct passwd *pw = NULL;

	while((pw = getpwent()) != NULL) {
		iCount ++;
	}
	endpwent();
	return iCount;
}

#ifndef  _STANDALONE_
int ProcInfo::getUID(JNIEnv *env, jobject obj, int iCount, RWCString uids[]) {
#else
int ProcInfo::getUID(int iCount, RWCString uids[]) {
#endif
	char mLogMessage[1024];
	//snprintf(mLogMessage,sizeof(mLogMessage),"Native getUID STARTS");
#ifndef  _STANDALONE_
        //logDbg1(env, obj,mLogMessage);
#else
        //logDbg1(mLogMessage);
#endif
	struct passwd *pw = NULL;
	char uid[128] = {0};
	int i = 0;

	while ((pw = getpwent()) != NULL) {
                snprintf(uid, sizeof(uid), "%s/%d", pw->pw_name, pw->pw_uid);
		if (i < iCount) {
			uids[i] = (RWCString) uid;
		} else {
			break;
		}
		i++;
  	}
	endpwent();
        return i;
}

#ifndef  _STANDALONE_
int ProcInfo::getFullProcsInfo(JNIEnv *env, jobject jobj, RWCString proc[], int iCount, int osType) {
#else
int ProcInfo::getFullProcsInfo(RWCString proc[], int iCount, int osType) {
#endif
	char mLogMessage[1280];
	//snprintf(mLogMessage,sizeof(mLogMessage),"Native :getFullProcInfo:Start");
	//logDbg1(env, jobj,mLogMessage);

	static DIR *procdir;
	seteuid(0);

	if (!(procdir = opendir (PROCFS))) {
		snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
		mErrorState = -1 ;
		return 0;
	}

	psinfo currproc ;
	struct dirent *direntp;

        RWCString full_cmd;
        char tmp[128] = {0};
        int psinfo_args_len = 0;
	int procCounter = 0;
	int fd = 0;
	char name[32] = {0};

	char errStr[384];

	for (rewinddir (procdir); (direntp = readdir (procdir));) {
		if (direntp->d_name[0] == '.')
			continue;
                snprintf(name,sizeof(name),"/%s/%s/psinfo", PROCFS, direntp->d_name);
// 		if ( osType == 1 ) {
// 			strcpy(name,PROCFS);
// 			strcat(name,"/");
// 			strcat(name,direntp->d_name);
// 			strcat(name,"/psinfo");
// 		} else
//                         snprintf(name,sizeof(name),"/%s/%s", PROCFS, direntp->d_name);
// 			strcpy(name,direntp->d_name);

		fd = open (name, O_RDONLY);
		if (fd == -1 ) {
			if ((!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
                                         name,strerror(errno));
                                mErrorState = 100 ;
			}
			continue ;
		}

                int byteRead = read(fd,&currproc , sizeof(currproc));
                if ( byteRead <= 0 ) {
                        close(fd);
                        continue;
                }
                close(fd);
                if (procCounter >= iCount) {
                        break;
                }
                full_cmd = currproc.pr_psargs;
                psinfo_args_len = strlen(currproc.pr_psargs);
                if (psinfo_args_len > 70) {
                        full_cmd = longargs(currproc);
                        if (full_cmd.length() < psinfo_args_len) {
                                full_cmd = currproc.pr_psargs;
                        }
                }
                snprintf(tmp, sizeof(tmp), "%d{}%d{}%s{}",
                        currproc.pr_pid, currproc.pr_uid, currproc.pr_fname);
                proc[procCounter++] = (RWCString)tmp + full_cmd;

#if 0
                snprintf(procids,sizeof(procids),"%ld", currproc.pr_pid);
                strcpy(tmpUsrName, procids);
                strcat(tmpUsrName, "{}");
                snprintf(usrids,sizeof(usrids),"%ld", currproc.pr_uid);
                strcat(tmpUsrName, usrids);
                strcat(tmpUsrName, "{}");
                strcat(tmpUsrName, currproc.pr_fname);
                strcat(tmpUsrName, "{}");
                strcpy(args, currproc.pr_psargs);

                if(strlen(args) > 70) {
                        if(procCounter < iCount) {
                                //Getting full args if args length is > 80...
                                longCmd = longargs(currproc.pr_pid,errStr);//is causing any problem..???
                                proc[procCounter] = (RWCString) tmpUsrName;
                                proc[procCounter].append(longCmd);
                        } else {
                                (void) close (fd);
                                break;
                        }
                } else {
                        strcat(tmpUsrName, args);
                        if(procCounter < iCount) {
                                proc[procCounter] = (RWCString) tmpUsrName;
                        } else {
                                (void) close (fd);
                                break;
                        }
                }

                //snprintf(mLogMessage,sizeof(mLogMessage),"Native :getFullProcInfo:MARKER 7 %s", tmpUsrName);
                //logDbg1(env, jobj,mLogMessage);
//        } else {
                int ioCtl;
                ioCtl = ioctl (fd, PIOCPSINFO, &old_currproc);
                if (ioCtl < 0) {
                        snprintf(mErrorMessage,sizeof(mErrorMessage),"2102154 IOCTL FAILURE for PID %s.\nError is %s\n", name, strerror(errno));
		        mErrorState = -1 ;
		        (void) close(fd);
		        closedir(procdir);
                        /*if (direntp != NULL) {
                          (void) free (direntp);
                          direntp = NULL;
                          }*/
                        /*if (procdir) {
                          (void) free (procdir);
                          procdir = NULL;
                          }*/
                        //if(usrids != NULL) {
                        (void) free(usrids);
                        usrids = NULL;
                        //}
                        //if(procids != NULL) {
                        (void) free(procids);
                        procids = NULL;
                        //}
		        return 0;
                }
                snprintf(procids,sizeof(procids),"%ld", old_currproc.pr_pid);
                strcpy(tmpUsrName, procids);
                strcat(tmpUsrName, "{}");
                snprintf(usrids,sizeof(usrids),"%ld", old_currproc.pr_uid);
                strcat(tmpUsrName, usrids);
                strcat(tmpUsrName, "{}");
                strcat(tmpUsrName, old_currproc.pr_fname);
                strcat(tmpUsrName, "{}");
                strcpy(args, old_currproc.pr_psargs);

                if (strlen(args) > 70) {
                        if(procCounter < iCount) {
                                longCmd = longargs(old_currproc.pr_pid,errStr);
                                proc[procCounter] = (RWCString) tmpUsrName;
                                proc[procCounter].append(longCmd);
                        } else {
                                (void) close (fd);
                                break;
                        }
                } else {
                        strcat(tmpUsrName, args);
                        if(procCounter < iCount) {
                                proc[procCounter] = (RWCString) tmpUsrName;
                        } else {
                                (void) close (fd);
                                break;
                        }
                }
//        }
#endif
        }
	closedir(procdir);
	return procCounter;
}

//****************************************************************************************************//
#ifndef  _STANDALONE_
int ProcInfo::getFullProcsInfoCount(JNIEnv *env, jobject jobj, int osType) {
#else
int ProcInfo::getFullProcsInfoCount(int osType) {
#endif
	static DIR *procdir;
	seteuid(0);

	if (!(procdir = opendir (PROCFS))) {
		snprintf(mErrorMessage,sizeof(mErrorMessage),"2102150 Unable to open %s.\nError is  %s", PROCFS, strerror(errno));
		mErrorState = -1 ;
		return 0;
	}

	psinfo currproc ;
	prpsinfo old_currproc ;
	struct dirent *direntp;
	int procCounter = 0;

	int fd = 0;
	char name[1024];

	for (rewinddir (procdir); (direntp = readdir (procdir));) {
		if (direntp->d_name[0] == '.')
			continue;
                procCounter++;

// 		if ( osType == 1 ) {
// 			strcpy(name,PROCFS);
// 			strcat(name,"/");
// 			strcat(name,direntp->d_name);
// 			strcat(name,"/psinfo");
// 		} else
// 			strcpy(name,direntp->d_name);

// 		fd = open (name, O_RDONLY);

// 		if (fd == -1 ) {
// 			if ((!(errno == 2 || errno == 0) ) && ( mErrorState != 100 ) ) {
// 				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102152 Unable to open file  %s.\nError is %s",
// 		                                        name,strerror(errno));
// 		        mErrorState = 100 ;
// 			}
// 			continue ;
// 		}

// 		if ( osType == 1 ) {
// 			int byteRead = read(fd,&currproc ,(sizeof(currproc) + 1));
// 			if ( byteRead == 0 ) {
// 				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102153 Unable to read from  %s.\nError is %s",
// 						name, strerror(errno));
// 				mErrorState = -1 ;
// 				(void) close(fd);
// 				closedir(procdir);
// 				/*if (direntp != NULL) {
// 					(void) free (direntp);
// 					direntp = NULL;
// 				}*/
// 				/*if (procdir) {
// 					(void) free (procdir);
// 					procdir = NULL;
// 				}*/
// 				return 0;
// 			}
// 			procCounter++;
// 		} else {
// 			int ioCtl;
// 			ioCtl = ioctl (fd, PIOCPSINFO, &old_currproc);
// 			if (ioCtl < 0) {
// 				snprintf(mErrorMessage,sizeof(mErrorMessage),"2102154 IOCTL FAILURE for PID %s.\nError is %s\n", name, strerror(errno));
// 		        mErrorState = -1 ;
// 		        (void) close(fd);
// 		        closedir(procdir);
// 		        /*if (direntp != NULL) {
// 					(void) free (direntp);
// 					direntp = NULL;
// 				}*/
// 				/*if (procdir) {
// 					(void) free (procdir);
// 					procdir = NULL;
// 				}*/
// 				return 0;
// 			}
// 			procCounter++;
// 		}
// 		(void) close (fd);
// 		//memset (direntp, '\0', sizeof(struct dirent) + 64);
	}
	closedir(procdir);

	return procCounter;
}
