/*
 * ProcInfo.h
 */

#ifndef _PROC_NFO_H__
#define _PROC_NFO_H__
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
//#include <sys/procfs.h>
#include <dirent.h>
#include "SingleProcInstance.h"

#ifndef  _STANDALONE_
#  include <jni.h>
#endif
#include <rw/cstring.h>
#include <rw/tphdict.h>
#include <rw/tpslist.h>
#include <rw/tvhasht.h>

const RWCString procfs = "/proc";
#define  PROCFS  procfs.data()
//const char *PROCFS = "/proc";

typedef RWTPtrHashDictionary<RWCString,void> PidPtrHash;
typedef RWTPtrHashDictionaryIterator<RWCString,void> PidPtrHashItr;

typedef RWTValHashTable<RWCString> PidHash;
typedef RWTValHashTableIterator<RWCString> PidHashItr;

class ProcInfo
{

	private  :
		RWCString mProcName ;
		RWCString mCommandLineMatch ;
		RWCString mUserName;
		RWTPtrHashDictionary<RWCString,SingleProcInstance> mProcIdDict ;
// 		RWTPtrHashDictionary<RWCString,void> mNewProcIdDict ;
		PidPtrHash mNewProcIdDict;
		float mNumberOfInstances ;
		float mNumberOfNewInstances ;
		float mSumCPUUtillization ;
		float mMaxSize ;
		float mMaxRsSize ;
		float mSumThreads ;
		float mSumFds ;
		float mAvailableFlag ;
		//Code change has been done by Ashish to support the enhancement
		int mAvailableFlagAtUserLvl;
		float mTotalRsSize;
		float mTotalSize;
		//**********
		int mNumCpus ;
		int mErrorState ;
		int mOSVersionFlag ; //1 = 5.6 0 = 5.5
		char mErrorMessage[512] ;
#ifndef  _STANDALONE_
        char * executeRegular(JNIEnv *env, jobject jobj);
        void executeFast(JNIEnv *env, jobject jobj);
        void executeProcFast(int flag, long id, JNIEnv *env, jobject obj);
        int executeRegularAtUserLevel(JNIEnv *env, jobject jobj, int, RWCString procNames[]);
        void executeFastAtUserLevel(JNIEnv *env, jobject jobj);
        void executeStatsRegExp(JNIEnv *env, jobject jobj, int UserFlag);
        void executeFastRegExp(JNIEnv *env, jobject jobj, int UserFlag);
#else
        char * executeRegular(void);
        void executeFast(void);
        void executeProcFast(int flag, long id);
        int executeRegularAtUserLevel(int, RWCString[]);
        void executeFastAtUserLevel();
        void executeStatsRegExp(int UserFlag);
        void executeFastRegExp(int UserFlag);
#endif
        void executeProcRegular(int flag, long id);
        void updateProcStats(const psinfo & psi);

	public :
		// Inserting the User Name also: Ashish
		//ProcInfo(int osVersion , RWCString processName, RWCString commandLineMatch) ;
		ProcInfo(int osVersion , RWCString processName, RWCString commandLineMatch, RWCString userName) ;

		//For regular Expression ;; Ashish
		ProcInfo(int osVersion) ;
        ~ProcInfo();
        //void clean_up();

#ifndef  _STANDALONE_        
		int executeForRegExp(JNIEnv *env, jobject jobj, int pollType, jobjectArray procs, RWCString procNames[]);
		int executeRegularForRegExp(JNIEnv *env, jobject jobj, jobjectArray procs, RWCString procNames[]);
		int getUID(JNIEnv *env, jobject obj, int count, RWCString ids[]);
		int getFullProcsInfo(JNIEnv *env, jobject thisObj, RWCString proc[], int iCount, int osType);
		int getUserCount(JNIEnv *env, jobject obj);
		int getFullProcsInfoCount(JNIEnv *env, jobject jobj, int osType);
#else
        //int executeForRegExp(int pollType, jobjectArray procs, RWCString procNames[]);
        int executeForRegExp(int pollType, char **procs, int procslength, RWCString procNames[]);
        //int executeRegularForRegExp(jobjectArray procs, RWCString procNames[]);
        int executeRegularForRegExp(char **procs, int procslength, RWCString procNames[]);
		int getUID(int count, RWCString ids[]);
		int getFullProcsInfo(RWCString proc[], int iCount, int osType);
        int getUserCount();
		int getFullProcsInfoCount(int osType);
#endif
        // For Attribute Choice enhancements.
                ProcInfo ();
                int getProcessCount (int osType);
                int getProcessNames (int size, int osType, RWCString listOfProcs[]);
                int getCmdMatchCount (int osType, RWCString procName);
                int getCmdMatch (int iTotalCmdMatch, int osType, RWCString procName, RWCString listOfCmdMatcedProcs[]);
				//********** ASHISH ******************//
				// Following functions has been introduced to support the enhancement
#ifndef  _STANDALONE_
                int getUsrIDs (JNIEnv *env, jobject jobj, int iCount, RWCString usrIDArr[], jint osType);
                int getUserProcessCount (JNIEnv *env, jobject jobj, int osType, long name);
        int getUserProcessNames (JNIEnv *env, jobject jobj, int iCount, int osType, long name, RWCString strProcsName[]);
        int getProcName (JNIEnv *env, jobject obj, int iSize, RWCString procNames[],long uid, int osType);
        int getUsrCmdMatchCount (JNIEnv *env, jobject jobj, int osType, RWCString strProcName, long user);
        int getUsrCmdMatch (JNIEnv *env, jobject thisObj, int size, int osType, RWCString strProcName, RWCString strCmdMatch[], long user);
#else
        int getUsrIDs ( int iCount, RWCString usrIDArr[], int osType);
        int getUserProcessCount (int osType, long name);
        int getUserProcessNames (int iCount, int osType, long name, RWCString strProcsName[]);
        int getProcName (int iSize, RWCString procNames[],long uid, int osType);
        int getUsrCmdMatchCount (int osType, RWCString strProcName, long user);
        int getUsrCmdMatch (int size, int osType, RWCString strProcName, RWCString strCmdMatch[], long user);
#endif        
				//********** ASHISH ******************//
				//********** NANDA *******************//
				int getCmdMatchCountNOProcsNoUser (int osType);
				int getUserCmdMatchCountNOProcs (int osType, long uid);
				int getCmdMatchNoUserNoProcess (int iTotalCmdMatch, int osType, RWCString listOfCmdMatcedProcs[]);
				int getUsrCmdMatchNoProcess (int size, int osType, RWCString procCmd[], long uid);
				//********** NANDA *******************//

		float getNumberOfInstances() {
			return mNumberOfInstances ;
		}

		float getNumberOfNewInstances() {
			return mNumberOfNewInstances ;
		}

		float getSumCPUTUtilization() {
			return mSumCPUUtillization;
		}

		float getSumThreads () {
			return mSumThreads ;
		}

		float getSumFds () {
			return mSumFds ;
		}

		float getMaxSize() {
			return mMaxSize/1024;
		}

		float getMaxRsSize() {
			return mMaxRsSize/1024;
		}

		char *  getErrorMsg() {
			return mErrorMessage ;
		}

		float getAvailavility() {
			return mAvailableFlag ;
		}

		int getErrorState() {
			return mErrorState;
		}

#ifndef  _STANDALONE_        
        char * execute(JNIEnv *env, jobject obj, int pollType);
        int executeAtUserLevel(JNIEnv *env, jobject jobj, int pollType, int, RWCString procNames[]);
#else
        char * execute(int pollType);
        int executeAtUserLevel(int pollType, int, RWCString procNames[]);
#endif
        int procNameMatched(RWCString proc_name) ;
		int commandLineMatchFound(RWCString progNameWithArgument);
		/*
		* Inserted by NandaKumar for Regular Expression
		*/
		int procNameMatchedRegExp(RWCString proc_name, RWCString Input_Proc) ;
		int commandLineMatchFoundRegExp(RWCString progNameWithArgument, RWCString Input_Comd);

		int getNuberOfCPUs() ;
		char * oracleProc(char t_proc[]);

		/*
		 * Inserted by Ashish to get the Availability value at user level
		*/
		int getAvailavilityAtUserLevel() {
			return mAvailableFlagAtUserLvl;
		}

		/*
		 * Inserted by Ashish to get the Total Resident Size
		*/
		float getTotalRsSize() {
			return mTotalRsSize/1024;
		}

		/*
		 * Inserted by Ashish to get the Total Virtual Size
		*/
		float getTotalSize() {
			return mTotalSize/1024;
		}



		void print() {
			cout << "Proc Name ->" << mProcName  << "\n"
			<< "Commnad Line Match->" <<  mCommandLineMatch  << "\n"
			<< "Number of instance ->" <<  mNumberOfInstances << "\n"
			<< "New Instanses ->" <<  mNumberOfNewInstances << "\n"
			<< "CPU utillization ->" <<  mSumCPUUtillization << "\n"
			<< "Max Size-> " <<  mMaxSize << "\n"
			<< "Max Resident Size-> " <<  mMaxRsSize << "\n"
			<< "Sum Threads-> " <<  mSumThreads << endl ;
		}
};
#endif// End ProcInfo class


