#include "ProcInfoManager.h"

int
#ifndef  _STANDALONE_
ProcInfoManager::add(int version, const char *procName, const char *commandLineMatch,
                     const char * userName, JNIEnv * env, jobject obj)
#else
ProcInfoManager::add(int version, const char *procName, const char *commandLineMatch,
                     const char * userName)
#endif
{
	/* --------------------- Changes for JNI logging START------------------------------*/
    //char mLogMessage[1024];
   // snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: ProcInfoManager: function add: Start");
    //logDbg(env,obj,mLogMessage);
    /* --------------------- Changes for JNI logging END------------------------------*/
	// Modified by Ashish to insert the user name also
	//ProcInfo  * inst = new ProcInfo(version,procName,commandLineMatch);
    ProcInfo  * inst = new ProcInfo(version,procName,commandLineMatch, userName);
    //Get max id and increament it by one
    int newId = 0 ;
    RWTPtrHashDictionaryIterator<int,ProcInfo> itr(mProcIdDict );
    itr.reset() ;
    for (;itr() ;)
        {
                int * id = itr.key() ;
                if ( newId < *id )
                        newId = *id ;
        }
    newId ++ ;
    mProcIdDict.insertKeyAndValue(new int (newId), inst ) ;
    //snprintf(mLogMessage,sizeof(mLogMessage),"ProcInfoManager:<add> %s %s %s %d", procName, commandLineMatch, userName, newId);
    //logDbg(env,obj,mLogMessage);
    /* --------------------- Changes for JNI logging START------------------------------*/
    //snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: ProcInfoManager: function add: END newId== %d", newId);
    //logDbg(env,obj,mLogMessage);
    /* --------------------- Changes for JNI logging END------------------------------*/

    return newId ;
}
void
ProcInfoManager::remove (int id )
{
        int * sotoredID =  mProcIdDict.find(&id) ;
        ProcInfo  * info = mProcIdDict.findValue(&id) ;
        mProcIdDict.remove(&id);
        if (info) {
                delete info;
        }
        if (sotoredID) {
                delete sotoredID;
        }
}
char *
#ifndef  _STANDALONE_
ProcInfoManager::execute ( JNIEnv *env, jobject obj, int id , int polltype)
#else
ProcInfoManager::execute (int id , int polltype)
#endif
{
        /* --------------------- Changes for JNI logging START------------------------------*/
        //char mLogMessage[255];
        //snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: ProcInfoManager: function execute. ID %d : Start",id);
        //logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
        
        char * TemPid;

        ProcInfo  * info = mProcIdDict.findValue(&id) ;
        if (  info == 0 )
                return NULL;
        
        /* --------------------- Changes for JNI logging START------------------------------*/
        //snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: ProcInfoManager: function execute: END");
        //logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/
        
#ifndef  _STANDALONE_
        TemPid = info->execute(env, obj, polltype) ;
#else
        TemPid = info->execute(polltype) ;
#endif
        return TemPid;

}
float
#ifndef  _STANDALONE_
ProcInfoManager::get (int id, int infoId, JNIEnv * env, jobject obj)
#else
ProcInfoManager::get (int id, int infoId)
#endif
{
        /* --------------------- Changes for JNI logging START------------------------------*/
        //char mLogMessage[255];
        //snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: ProcInfoManager: function get. ID %d. InfoId %d: Entered",id,infoId);
        //logDbg(env,obj,mLogMessage);
        /* --------------------- Changes for JNI logging END------------------------------*/

        ProcInfo  * info = mProcIdDict.findValue(&id) ;
        if ( info == 0 )
                return -300 ; //Special number to represent id not found
        switch  ( infoId )
        {
                case 1:
                        return info->getSumCPUTUtilization();
                case 2:
                        return info->getMaxSize();
                case 3:
                        return info->getSumThreads () ;
                case 4:
                        return info->getNumberOfInstances() ;
                case 5:
                        return info->getNumberOfNewInstances() ;
                case 6 :
                        return info->getErrorState() ;
                case 7 :
                        return info->getSumFds() ;
                case 8 :
                        return info->getAvailavility() ;
                case 9:
                        return info->getMaxRsSize();
				case 10:
                        return info->getAvailavilityAtUserLevel(); //Ashish: getting the Availability at user level
                case 11:
                        return info->getTotalSize(); // Ashish: getting Total Virtual Size
                case 12:
                        return info->getTotalRsSize(); // Ashish: getting Total Resident Size
				default :
                        return -301 ; //Special number to inidicate bad info type
        }
}
char *
ProcInfoManager::getErrorMsg (int id)
{
        ProcInfo  * info = mProcIdDict.findValue(&id) ;
        //Padmini's code change for Agent crashing begin
        if (  info == 0 )
                return("2102160 Error getting ProcInfoManager Object");
        //Padmini's code change for Agent crashing end
        return info->getErrorMsg() ;
}
/* --------------------- Changes for JNI logging START------------------------------*/

#ifndef  _STANDALONE_
void ProcInfoManager::logDbg(JNIEnv *env, jobject obj, char *logStr)
#else
void ProcInfoManager::logDbg(char *logStr)
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
        fprintf(stdout, "DBG: %s\n", logStr);
#endif
}

#ifndef  _STANDALONE_
void ProcInfoManager::logErr(JNIEnv *env, jobject obj, char *errStr)
#else
void ProcInfoManager::logErr(char *errStr)
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
        fprintf(stdout, "ERR: %s\n", errStr);
#endif
}

/* --------------------- Changes for JNI logging END------------------------------*/

//************************** ASHISH ************************************//
// Following functios have been written to support the enhancement.
// Now processes are getting listed based on user also.
//************************** ASHISH ************************************//
#ifndef  _STANDALONE_
int ProcInfoManager::executeAtUserLevel(JNIEnv *env, jobject obj,
                                        int id, int polltype,
                                        int proc_id_size, RWCString procIds[]) {
#else
int ProcInfoManager::executeAtUserLevel(int id, int polltype,
                                        int proc_id_size, RWCString procIds[]) {
#endif
    int iCount;
    ProcInfo  * info = mProcIdDict.findValue(&id) ;
    if (  info == 0 )
    	return 0;

#ifndef  _STANDALONE_
    iCount = info->executeAtUserLevel(env, obj, polltype, proc_id_size, procIds) ;
#else
    iCount = info->executeAtUserLevel(polltype, proc_id_size, procIds);
#endif

    return iCount;
}
//************************** ASHISH ************************************//




int
#ifndef  _STANDALONE_
ProcInfoManager::addForRegExp(int version, JNIEnv *env, jobject obj) {
#else
ProcInfoManager::addForRegExp(int version) {
#endif
	ProcInfo  * inst = new ProcInfo(version);
    //Get max id and increament it by one
    int newId = 0 ;
    RWTPtrHashDictionaryIterator<int,ProcInfo> itr(mProcIdDict );
    itr.reset() ;
    for (;itr() ;) {
		int * id = itr.key() ;
        if ( newId < *id )
        	newId = *id ;
	}
    newId ++ ;
    mProcIdDict.insertKeyAndValue(new int (newId), inst ) ;
    return newId ;
}

#ifndef  _STANDALONE_
int ProcInfoManager::executeForRegExp(JNIEnv *env, jobject obj, int id, int polltype,
                                      jobjectArray procs, RWCString procIds[]) {
#else
//int ProcInfoManager::executeForRegExp(int id, int polltype, jobjectArray procs, RWCString procIds[]) {
int ProcInfoManager::executeForRegExp(int id, int polltype, char **procs,
                                      int procslength, RWCString procIds[]) {
#endif
    
    int iCount;
    /* --------------------- Changes for JNI logging START------------------------------*/
    //char mLogMessage[255];
    //snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: ProcInfoManager: function executeForRegExp. ID %d : Start",id);
    //logDbg(env,obj,mLogMessage);
    /* --------------------- Changes for JNI logging END------------------------------*/
    ProcInfo  * info = mProcIdDict.findValue(&id) ;
    if (info == 0)
    	return 0;
#ifndef  _STANDALONE_
    iCount = info->executeForRegExp(env, obj, polltype, procs, procIds) ;
#else
    iCount = info->executeForRegExp(polltype, procs, procslength, procIds) ;
#endif
    /* --------------------- Changes for JNI logging START------------------------------*/
    //snprintf(mLogMessage,sizeof(mLogMessage),"Native Logging: ProcInfoManager: function executeForRegExp: END");
    //logDbg(env,obj,mLogMessage);
    /* --------------------- Changes for JNI logging END------------------------------*/
        
    return iCount;
}



