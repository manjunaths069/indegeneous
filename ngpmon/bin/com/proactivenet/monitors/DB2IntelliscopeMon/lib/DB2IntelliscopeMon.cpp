//DB2 Intelliscope Monitor
#include <string.h>
#include <stdlib.h>

#include <time.h>
#include <sqlutil.h>
#include <sqlenv.h>
#include "sqlca.h"
#include <sqlmon.h>
#ifdef _WINDOWS
	#include <windows.h>
#endif
#include "DB2IntelliscopeMon.h"

#define NO_OF_ITEMS 8 //Used for the ItemList index, # of items to be got from sqlfupd
		      //Added for UDB Catalog Cache support on DB2 8.1, ARS 9334


#define FAILURE          -1   // flag to indicate ERROR
#define SUCCESS           0  // flag to indicate SUCCESS

/*****************************************************************************/
/* Internal Function Prototypes                                              */
/*****************************************************************************/
int dump_snapshot_output(FILE* fp, sqlm_collected collected, char* snap);

/*****************************************************************************/
/*  Structure to Hold the Context					     */
/*****************************************************************************/
struct CtxtHolder
{
    int index;
    void * ptrCtxt;
};

/*****************************************************************************/
/*  Global Variables                                                         */
/*****************************************************************************/
static int contextTypeSet = 0;
static int noOfAttachs = 0;
struct CtxtHolder holder [2000];

static jmethodID setHitRatioValues = NULL;
static jmethodID setErrorInfo = NULL;
static jmethodID setIOValues = NULL;
static jmethodID setIndex = NULL;


int writetolog(const char *s)
{
	printf("%s\n", s);

        FILE *fp = fopen("test","a+");
		fflush(fp);
		fputs(s,fp);
	fputs("\n",fp);
        fclose(fp);

        return 0;
}//writetolog


void sendErrorToJava (JNIEnv *env, jobject obj, struct sqlca * error)
{
    jlong code;
    jstring msg;
    char buffer[200];
    int retval = -1;
	
    // If the Method Id for the Java Function is not Obtained get it
    if(setErrorInfo == NULL){
        jclass myclass = env->GetObjectClass(obj);
        setErrorInfo = env->GetMethodID(myclass,"setError","(JLjava/lang/String;)V");
    }
    
    code = error->sqlcode;
    // Get the Error Message with line breaks every 60 chars
    retval = sqlaintp(buffer, sizeof(buffer), 60, error);

    if ( retval < 0 )
    {
        char errmsg[200];
	if ( code == -1445 )
	    strcpy ( errmsg, "A valid attachment is not available. Check if DB2INSTANCE environment variable is set.");
	else if ( code == -1390 )
	    strcpy ( errmsg, "DB2INSTANCE environment variable should be set.");
	else
	    sprintf( errmsg, "Getting Error message from sqlaintp failed with Return Value = %d", retval);

        msg = env->NewStringUTF(errmsg);
    }//end of if
    else
    {
        buffer[200] = '\0';
	   #ifdef _WINDOWS
		wchar_t* wideCharBuffer = 0;
		int slen = strlen(buffer);
		int length = MultiByteToWideChar( CP_ACP, 0, (LPCSTR)buffer, slen, NULL, 0 );
		wideCharBuffer = (unsigned short *)malloc( length*sizeof(WCHAR) + 1 );
		MultiByteToWideChar( CP_ACP, 0, (LPCSTR)buffer, slen, (LPWSTR)wideCharBuffer, length );
		msg = env->NewString((jchar*)wideCharBuffer, length );
	   #endif
	   #ifdef _UNIX
		msg = env->NewStringUTF(buffer);
	   #endif
	
    }//end of if
    
    // Make a Java String object from the char buffer
    //msg = env->NewStringUTF(buffer);
    env->CallVoidMethod(obj, setErrorInfo, code, msg);
	
    // Free up the String variable
    env->DeleteLocalRef(msg);

    return;
}//end of sendErrorToJava

/*
 * Class:     com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon
 * Method:    setContextType
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon_setContextType  (JNIEnv *, jobject)
{
    jint retVal;

    if ( contextTypeSet == 0 )
    {
        retVal = sqleSetTypeCtx (SQL_CTX_MULTI_MANUAL);
        if ( retVal != SUCCESS )
	    retVal = FAILURE;
	contextTypeSet = 1;
    }//end of if
    else
	retVal = SUCCESS;

    return retVal;
}//end of setContextType

/*
 * Class:     com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon
 * Method:    attachToInstance
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon_attachToInstance
(JNIEnv * env, jobject obj, jstring nodeName, jstring userId, jstring password) {
    char * nodename, * userid, * pass_wd;
    int retvalue = FAILURE;
    struct sqlca sqlerr;

    // Variables for Beginning the Context for this thread
    struct sqlca sqlcaCt;
    void * pvCTXT;
    jint retVal = FAILURE;

    // Make a call to API function sqleBeginCtx with flag SQL_CTX_BEGIN_ALL
    // to begin the Context for this thread
    //retVal = sqleBeginCtx (&(holder[noOfAttachs].ptrCtxt), SQL_CTX_BEGIN_ALL, NULL, &sqlcaCt);
    retVal = sqleBeginCtx (&pvCTXT, SQL_CTX_BEGIN_ALL, NULL, &sqlcaCt);
    if ( (retVal != SUCCESS) || ((sqlcaCt.sqlcode != 0L) && (sqlcaCt.sqlcode != 100L)) )
    {
	sendErrorToJava (env, obj, &sqlcaCt);

#ifdef _UNIX
	if ( pvCTXT != NULL )
	{
	    free (pvCTXT);
	    pvCTXT = NULL;
	}//end of if
#endif

	retVal = FAILURE;
	return retVal;
    }//end of if
	
    // Get the String values passed from Java
    nodename = (char*) env->GetStringUTFChars(nodeName, 0);
    userid = (char*) env->GetStringUTFChars(userId, 0);
    pass_wd = (char*) env->GetStringUTFChars(password, 0);

    retvalue = sqleatin(nodename, userid, pass_wd, &sqlerr);	
    if( (retvalue != SUCCESS) || ((sqlerr.sqlcode != 0L) && (sqlerr.sqlcode != 100L)) ){
		sendErrorToJava(env, obj, &sqlerr);
    		env->ReleaseStringUTFChars(nodeName, nodename);
    		env->ReleaseStringUTFChars(userId, userid);
    		env->ReleaseStringUTFChars(password, pass_wd);

#ifdef _UNIX
		if ( pvCTXT != NULL )
		{
		    free (pvCTXT);
		    pvCTXT = NULL;
		}//end of if
#endif

		retvalue = FAILURE;
		return retvalue;
    }else retvalue = SUCCESS;

    if ( setIndex == NULL )
    {
        jclass myclass = env->GetObjectClass(obj);
        setIndex = env->GetMethodID( myclass,"setIndexId","(I)V");
    }//end of if
    
    // Now we have a static holder, so we do not need to malloc and realloc
    //if ( holder == NULL )
	//holder = (struct CtxtHolder *) malloc (sizeof (struct CtxtHolder));
    //else
	//realloc (holder, (noOfAttachs + 1) * (sizeof (struct CtxtHolder)) );

    holder[noOfAttachs].index = noOfAttachs;
    holder[noOfAttachs].ptrCtxt = pvCTXT;

    env->CallVoidMethod( obj, setIndex, noOfAttachs);
 
    noOfAttachs++;

    env->ReleaseStringUTFChars(nodeName, nodename);
    env->ReleaseStringUTFChars(userId, userid);
    env->ReleaseStringUTFChars(password, pass_wd);

    return retvalue;
}

/*
 * Class:     com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon
 * Method:    detachFromInstance
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon_detachFromInstance
(JNIEnv * env, jobject obj, jint index, jint noOfActiveThreads){
    int retvalue = FAILURE;
    struct sqlca sqlerr;

    // Variables for Ending the Context for this thread
    struct sqlca sqlcaCt;
    //void * pvCTXT;

    retvalue = sqledtin (&sqlerr);
    if( (retvalue != SUCCESS) || ((sqlerr.sqlcode != 0L) && (sqlerr.sqlcode != 100L)) ) {
	sendErrorToJava (env, obj, &sqlerr);
	if ( (index >= 0) && (holder[index].ptrCtxt != NULL) )
	{
#ifdef _UNIX
	    free ( holder[index].ptrCtxt);
#endif
	    holder[index].ptrCtxt = NULL;
	}//end of if
	retvalue = FAILURE;
	return retvalue;
    }else retvalue = SUCCESS;
		
    // Go furhter if and only if the index is not < 0
    // This could happen in case of instanceCreation
    // where in Attaching fails and the index is set
    // to -1, then when the code tries to detach, it
    // will dump if we access not intialized area
    // so return with SUCCESS
    if ( index < 0 )
    {
	retvalue = SUCCESS;
	return retvalue;
    }//end of if

    // Make a call to API function sqleEndCtx with flag SQL_CTX_END_ALL
    // to end the Context for this thread
    retvalue = FAILURE;
    //retvalue = sqleEndCtx (&pvCTXT, SQL_CTX_END_ALL, NULL, &sqlcaCt);
    if ( holder[index].ptrCtxt != NULL )
      retvalue = sqleEndCtx (&(holder[index].ptrCtxt), SQL_CTX_END_ALL, NULL, &sqlcaCt);
    if ((retvalue != SUCCESS) || ((sqlcaCt.sqlcode != 0L) && (sqlcaCt.sqlcode != 100L)))
    {
	sendErrorToJava (env, obj, &sqlcaCt);
	retvalue = FAILURE;
    }//end of if

    if ( holder[index].ptrCtxt != NULL )
    {
	free ( holder[index].ptrCtxt);
	holder[index].ptrCtxt = NULL;
    }//end of if

    // Check if the no of Active Threads is Zero, free the 
    // Context Holder structure
    if ( noOfActiveThreads == 0 )
    {
	if ( holder != NULL )
	{
	    // "holder" is a static structure now, so no need to free
	    //free (holder);
	    //holder = NULL;

	    noOfAttachs = 0;
        }//end of if
    }//end of if

    return retvalue;
}


/*
 * Class:     com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon
 * Method:    getBufferSize
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon_getBufferSize
(JNIEnv * env, jobject obj, jstring dbName) {
	char * databasename;
    int retvalue = FAILURE;
    struct sqlca sqlca;
    sqluint32 buffer_sz;
    jint buffersize;

    // Get the String values passed from Java
    databasename = (char*) env->GetStringUTFChars(dbName, 0);	

    struct sqlma* sqlma = (struct sqlma *) malloc(SQLMASIZE(1));
    sqlma->obj_num = 1;
    sqlma->obj_var[0].obj_type = SQLMA_DBASE;
    strcpy(sqlma->obj_var[0].object, databasename);

    retvalue = sqlmonsz(SQLM_DBMON_VERSION2, NULL, sqlma, &buffer_sz, &sqlca);

    if( (retvalue != SUCCESS) || ((sqlca.sqlcode != 0L) && (sqlca.sqlcode != 100L)) ){
		sendErrorToJava (env, obj, &sqlca);
		buffersize = FAILURE;
    }else {
		if (buffer_sz == 0) buffersize = FAILURE;
		else buffersize = buffer_sz;
    }

    // Free up the memory
    env->ReleaseStringUTFChars(dbName, databasename);	
	
    if(sqlma != NULL){
        free(sqlma);
		sqlma = NULL;
    }

    return buffersize;
}

/*
 * Class:     com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon
 * Method:    turnOnSwitches
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon_turnOnSwitches
(JNIEnv * env, jobject obj) {

   unsigned int          i;                   /* loop counter               */
   int                   rc = 0;              /* return code                */
   struct sqlca          sqlca;
   struct sqlm_recording_group states[SQLM_NUM_GROUPS];

	//printf("Entering to the turnOnSwitches function\n");

   /********************************************************/
   /* Update the state of the switches to on               */
   /********************************************************/
   for (i = 0; i < SQLM_NUM_GROUPS; i++)
   {
      states[i].input_state = SQLM_ON;
   }

   rc = sqlmon(SQLM_DBMON_VERSION5_2, NULL, states, &sqlca);
   if (rc != 0)
   {
      sendErrorToJava(env, obj, &sqlca);
      return(rc);
   }

   if (sqlca.sqlcode != 0L)
   {
	sendErrorToJava(env, obj, &sqlca);
	return(sqlca.sqlcode);
   }

   /***************************** Displaying the switch states for testing *******************/
    /********************************************************/
   /* Query the current state of the switches              */
   /********************************************************/
   for (i = 0; i < SQLM_NUM_GROUPS; i++)
   {
      states[i].input_state = SQLM_HOLD;
   }

   rc = sqlmon(SQLM_DBMON_VERSION5_2, NULL, states, &sqlca);
   if (rc != 0)
   {
      //printf("\nReturn code from SQLMON is %d \n", rc);
      return(rc);
   }

   if (sqlca.sqlcode != 0L)
   {
      //printf("\nSqlcode from SQLMON is not 0. \n");
   }

   /********************************************************/
   /* Display the current state of the switches            */
   /********************************************************/
   //printf("\nCurrent Switch settings:\n");

   //printf("1 - UOW Switch state       : %s %24.24s\n", (states[0].output_state) ? "ON , start time :" : "OFF",
                         //(states[0].start_time.seconds) ? ctime((time_t *)&states[0].start_time.seconds) : "");
   //printf("2 - STMT Switch state      : %s %24.24s\n", (states[1].output_state) ? "ON , start time :" : "OFF",
                         //(states[1].start_time.seconds) ? ctime((time_t *)&states[1].start_time.seconds) : "");
   //printf("3 - TABLE Switch state     : %s %24.24s\n", (states[2].output_state) ? "ON , start time :" : "OFF",
                         //(states[2].start_time.seconds) ? ctime((time_t *)&states[2].start_time.seconds) : "");
   //printf("4 - BUFF POOL Switch state : %s %24.24s\n", (states[3].output_state) ? "ON , start time :" : "OFF",
                         //(states[3].start_time.seconds) ? ctime((time_t *)&states[3].start_time.seconds) : "");
   //printf("5 - LOCK Switch state      : %s %24.24s\n", (states[4].output_state) ? "ON , start time :" : "OFF",
                         //(states[4].start_time.seconds) ? ctime((time_t *)&states[4].start_time.seconds) : "");
   //printf("6 - SORT Switch state      : %s %24.24s\n", (states[5].output_state) ? "ON , start time :" : "OFF",
                         //(states[5].start_time.seconds) ? ctime((time_t *)&states[5].start_time.seconds) : "");

   /***************************** Displaying the switch states for testing (Done) *******************/

   return rc;
}

/*
 * Class:     com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon
 * Method:    getHitRatioData
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon_getHitRatioData
(JNIEnv *env, jobject obj, jstring dbName, jint bufferSize) {
    int rtVal, buffer_sz;

    //jfloat poolHitRatio, poolIndexHitRatio, pkgCheHitRatio, ctgCheHitRatio;
	jlong pkgCheLookups, pkgCheInserts, ctgCheLookups, ctgCheInserts;
	jlong pool_data_l_reads, pool_data_p_reads, pool_index_l_reads, pool_index_p_reads;

    struct sqlca sqlca;
    struct sqlma* sqlma;
    char * buffer_ptr, * databasename;
    sqlm_collected collected;       /* info. structure for DB SYS. MON. APIs */
    
    rtVal = FAILURE;
    buffer_sz = bufferSize;
    
    databasename = (char*) env->GetStringUTFChars(dbName,0);

    // If the Method Id for the Java Function is not Obtained
    // get it
    if(setHitRatioValues == NULL){
        jclass myclass = env->GetObjectClass(obj);
        setHitRatioValues = env->GetMethodID( myclass,"setStatsValues","(JJJJJJJJ)V");
    }
    
    sqlma = (struct sqlma *) malloc(SQLMASIZE(1));
    sqlma->obj_num = 1;
    sqlma->obj_var[0].obj_type = SQLMA_DBASE;
    strcpy(sqlma->obj_var[0].object, databasename);
    
    buffer_ptr = (char *) malloc(buffer_sz);    /* allocate buffer */

    rtVal = sqlmonss(SQLM_DBMON_VERSION2, NULL, sqlma, buffer_sz, (void *) buffer_ptr, &collected, &sqlca);

    if( (rtVal != SUCCESS) || ((sqlca.sqlcode != 0L) && (sqlca.sqlcode != 100L)) ){
		sendErrorToJava (env, obj, &sqlca);
        rtVal = FAILURE;
    }else {
	
		/* Check the block type which is available in the 
		* 5th byte of any top-level block
		* It should hold only one block as we had requested
		* for only SQLM_DBASE
		*/
     
     	switch ((unsigned char) *(buffer_ptr + 4)) 
     	{
     	    // Though we are expecting SQLM_DBASE_SS only
     	    // so if we get anything else we record error
			case SQLM_DBASE_SS: {
				sqlm_dbase* db_buffer_ptr = (sqlm_dbase*) buffer_ptr;

				pool_data_l_reads = db_buffer_ptr->pool_data_l_reads;
				pool_data_p_reads = db_buffer_ptr->pool_data_p_reads;
				pool_index_l_reads = db_buffer_ptr->pool_index_l_reads;
				pool_index_p_reads = db_buffer_ptr->pool_index_p_reads;

				pkgCheLookups = db_buffer_ptr->pkg_cache_lookups;
				pkgCheInserts = db_buffer_ptr->pkg_cache_inserts;

				ctgCheLookups = db_buffer_ptr->cat_cache_lookups;
				ctgCheInserts = db_buffer_ptr->cat_cache_inserts;

    			// Call the Java Function to set the values
    			env->CallVoidMethod(obj, setHitRatioValues, pool_data_l_reads, pool_data_p_reads,
							pool_index_l_reads, pool_index_p_reads,
							pkgCheLookups, pkgCheInserts, 
							ctgCheLookups, ctgCheInserts);

				rtVal = SUCCESS;
				}
				break;
			default:
				rtVal = FAILURE;
		}//end of switch		
    }//end of else

    // Free the memory
    
    // Free up the String Memory
    env->ReleaseStringUTFChars(dbName, databasename);
    
    if(buffer_ptr != NULL){
        free(buffer_ptr);
		buffer_ptr = NULL;
    }

    if(sqlma != NULL){
        free(sqlma);
		sqlma = NULL;
    }

    return rtVal;
}

/*
 * Class:     com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon
 * Method:    getIOData
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon_getIOData
  (JNIEnv *env, jobject obj, jstring db_name, jint bufsz) {
    int rtVal, buffer_sz;
    jlong log_Data_Reads, phy_Data_Reads, direct_Reads;
    jlong log_Idx_Reads, phy_Idx_Reads, async_Data_Reads;
    jlong data_Writes, idx_Writes, direct_Writes;
    jlong async_Data_Writes, async_Idx_Writes;
    struct sqlca sqlca;
    struct sqlma* sqlma;
    char * buffer_ptr, * databasename;
    sqlm_collected collected;       /* info. structure for DB SYS. MON. APIs */
    
    rtVal = FAILURE;
    buffer_sz = bufsz;
    
    databasename = (char*) env->GetStringUTFChars(db_name,0);

    // If the Method Id for the Java Function is not Obtained
    // get it
    if ( setIOValues == NULL )
    {
        jclass myclass = env->GetObjectClass(obj);
        setIOValues = env->GetMethodID( myclass,"setStatsValues","(JJJJJJJJJJJ)V");
    }//end of if
    
    sqlma = (struct sqlma *) malloc(SQLMASIZE(1));
    sqlma->obj_num = 1;
    sqlma->obj_var[0].obj_type = SQLMA_DBASE;
    strcpy(sqlma->obj_var[0].object, databasename);
    
    buffer_ptr = (char *) malloc(buffer_sz);    /* allocate buffer */

    rtVal = sqlmonss(SQLM_DBMON_VERSION2, NULL, sqlma, buffer_sz, (void *) buffer_ptr, &collected, &sqlca);

    if ( (rtVal != SUCCESS) || ((sqlca.sqlcode != 0L) && (sqlca.sqlcode != 100L)) )
    {
	sendErrorToJava (env, obj, &sqlca);
        rtVal = FAILURE;
    }//end of if
    else
    {
	
    /* Check the block type which is available in the 
     * 5th byte of any top-level block
     * It should hold only one block as we had requested
     * for only SQLM_DBASE
     */
     
     	switch ((unsigned char) *(buffer_ptr + 4)) 
     	{
     	    // Though we are expecting SQLM_DBASE_SS only
     	    // so if we get anything else we record error
     	    case SQLM_DBASE_SS:
	    {
	        sqlm_dbase* db_buffer_ptr = (sqlm_dbase*) buffer_ptr;
	        log_Data_Reads = db_buffer_ptr->pool_data_l_reads;
	        phy_Data_Reads = db_buffer_ptr->pool_data_p_reads;
	        data_Writes = db_buffer_ptr->pool_data_writes;
	        log_Idx_Reads = db_buffer_ptr->pool_index_l_reads;
	        phy_Idx_Reads = db_buffer_ptr->pool_index_p_reads;
	        idx_Writes = db_buffer_ptr->pool_index_writes;
	        async_Data_Reads = db_buffer_ptr->pool_async_data_reads;
    	        async_Data_Writes = db_buffer_ptr->pool_async_data_writes;
    	        async_Idx_Writes = db_buffer_ptr->pool_async_index_writes;
    	        direct_Writes = db_buffer_ptr->direct_writes;
    	        direct_Reads = db_buffer_ptr->direct_reads;
    			
    		// Call the Java Function to set the values
    		env->CallVoidMethod( obj, setIOValues, log_Data_Reads, phy_Data_Reads, data_Writes, log_Idx_Reads, phy_Idx_Reads, idx_Writes, async_Data_Reads, async_Data_Writes, async_Idx_Writes, direct_Writes, direct_Reads);
		rtVal = SUCCESS;
	    }//end of case
	    break;
	    default:
	    {
	        rtVal = FAILURE;
	    }//end of default
        }//end of switch		
    }//end of else

    // Free the memory
    
    // Free up the String Memory
    env->ReleaseStringUTFChars(db_name, databasename);
    
    if ( buffer_ptr != NULL )
    {
        free(buffer_ptr);
	buffer_ptr = NULL;
    }//end of if
	
    if ( sqlma != NULL )
    {
        free(sqlma);
	sqlma = NULL;
    }//end of if	
    return rtVal;
}

/*
 * Class:     com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon
 * Method:    getInternalsData
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon_getInternalsData
  (JNIEnv *env, jobject obj, jstring dbName, jint bufferSize) {

	sqluint32 lvLockList, lvLockListinUse, lvLogUsed, lvCatSize, lvLogBuf, 
			lvBuffPage, lvLogFileSize, lvPrimLog, lvSecLog;
	
	char* lvDBName = (char*) env->GetStringUTFChars(dbName,0);
	int   rc = 0;

	struct    sqlca sqlca;

	int lvBufferSize = bufferSize;
	sqlm_collected collected;
	struct sqlma* sqlma;
	char     *buffer_ptr;
	buffer_ptr = (char *) malloc(lvBufferSize); 

	jfieldID msg, jlvLockList, jlvLockListinUse, jlvLogUsed, jlvCatSize, 
			jlvLogBuf, jlvBuffPage , jlvLogFileSize, jlvPrimLog, jlvSecLog;
	jstring errval;
	jclass cs = env->GetObjectClass(obj);
	msg = env->GetFieldID(cs,"error_msg", "Ljava/lang/String;");
	
	char  lvNativeErrMsg[1024];
	struct sqlfupd itemList[NO_OF_ITEMS];
	int val = -2;
	int idx = 0;

   itemList[0].token = SQLF_DBTN_LOCKLIST ;
   itemList[0].ptrvalue = (char *)malloc(sizeof(short));

   itemList[1].token = SQLF_DBTN_CATALOGCACHE_SZ;
   itemList[1].ptrvalue = (char *)malloc(sizeof(long));

   itemList[2].token = SQLF_DBTN_LOGBUFSZ;
   itemList[2].ptrvalue = (char *)malloc(sizeof(short));

   itemList[3].token = SQLF_DBTN_BUFF_PAGE;
   itemList[3].ptrvalue = (char *)malloc(sizeof(long));

   itemList[4].token = SQLF_DBTN_LOGFIL_SIZ;
   itemList[4].ptrvalue = (char *)malloc(sizeof(long));

   itemList[5].token = SQLF_DBTN_LOGPRIMARY ;
   itemList[5].ptrvalue = (char *)malloc(sizeof(short));

   itemList[6].token = SQLF_DBTN_LOGSECOND;
   itemList[6].ptrvalue = (char *)malloc(sizeof(short));

   // Added for getting UDB Catalog Cache value for DB2 8.1, ARS 9334
   itemList[7].token = SQLF_DBTN_MAXAPPLS;
   itemList[7].ptrvalue = (char *)malloc(sizeof(short));

   sqlfxdb (lvDBName, NO_OF_ITEMS, itemList, &sqlca);
   if(sqlca.sqlcode != 0 && sqlca.sqlcode != 100)
	{
		sendErrorToJava (env, obj, &sqlca);
		env->ReleaseStringUTFChars(dbName, lvDBName);
		for(idx = 0; idx < NO_OF_ITEMS; idx++)
		{
			free(itemList[idx].ptrvalue);
		}
		//sqledtin (&sqlca);
		return -1;
	}//End of if


	lvLockList    = *(short *)itemList[0].ptrvalue;


	lvLogBuf      = *(short *)itemList[2].ptrvalue;

	lvBuffPage    = *(long *)itemList[3].ptrvalue;

	lvLogFileSize = *(long *)itemList[4].ptrvalue;
	lvPrimLog     = *(short *)itemList[5].ptrvalue;
	lvSecLog      = *(short *)itemList[6].ptrvalue;


	
	sqlma = (struct sqlma *) malloc(SQLMASIZE(1));
	sqlma->obj_num = 1;
    sqlma->obj_var[0].obj_type = SQLMA_DBASE;
    strcpy(sqlma->obj_var[0].object, lvDBName);

	rc = sqlmonss(SQLM_DBMON_VERSION2, NULL, sqlma, lvBufferSize, buffer_ptr,&collected, &sqlca);
	if(sqlca.sqlcode != 0 && sqlca.sqlcode != 100)
	{
		sendErrorToJava (env, obj, &sqlca);
		for(idx = 0; idx < NO_OF_ITEMS; idx++)
		{
			free(itemList[idx].ptrvalue);
		}
		free(sqlma);
		free (buffer_ptr); 
		env->ReleaseStringUTFChars(dbName, lvDBName);
		//sqledtin(&sqlca);
		return -1;
	}//End of if

	// For DB2 8.1, Catalog Cache Size = (MAXAPPLS*4) whereas for 
	// DB2 7.2 its directly the value. Hence we have introduced the
	// following check for DB2 Version detection to be used to decide
	// over the computation of UDB Catalog Cache, ARS 9334
	if ( !strncmp (collected.server_prdid, "SQL08", 5) )
	{
	    sqluint32 max_applns = *(short *)itemList[7].ptrvalue;
	    lvCatSize = max_applns * 4;
	}//end of if
	else
	    lvCatSize     = *(long *)itemList[1].ptrvalue;

	val = dump_snapshot_output(stdout, collected, buffer_ptr);
	if(val != 0)
	{

		strcpy(lvNativeErrMsg, "Error in fecthing the value from the Native Code ");
		errval = env->NewStringUTF(lvNativeErrMsg);
		env->SetObjectField(obj, msg, errval);
		env->DeleteLocalRef(errval);
		for(idx = 0; idx < NO_OF_ITEMS; idx++)
		{
			free(itemList[idx].ptrvalue);
		}
		free(sqlma);
		free (buffer_ptr); 
		env->ReleaseStringUTFChars(dbName, lvDBName);
		//sqledtin(&sqlca);
		return -1;
	}//End of if

	//When the data is returned then we retrive the data, into the respective variables.

	sqlm_dbase* db_snap = (sqlm_dbase*) buffer_ptr;

	lvLockListinUse = db_snap->lock_list_in_use;
	lvLogUsed       = db_snap->tot_log_used_top;

	jlvCatSize             = env->GetFieldID(cs, "ncatalogCacheSize", "J");
	jlvLogBuf              = env->GetFieldID(cs, "nlogBufSize", "J");
	jlvBuffPage            = env->GetFieldID(cs, "nbufferPollSize", "J");

	jlvLockList            = env->GetFieldID(cs, "totLockList", "J");
	jlvLockListinUse       = env->GetFieldID(cs, "lockList_UMem", "J");
	jlvLogUsed             = env->GetFieldID(cs, "logUsed", "J");
	jlvLogFileSize         = env->GetFieldID(cs, "logFileSize", "J");
	jlvPrimLog             = env->GetFieldID(cs, "primLog", "J");
	jlvSecLog              = env->GetFieldID(cs, "secLog", "J");


	env->SetLongField(obj, jlvCatSize, lvCatSize);
	env->SetLongField(obj, jlvLogBuf, lvLogBuf);
	env->SetLongField(obj, jlvBuffPage, lvBuffPage);
	env->SetLongField(obj, jlvLockListinUse, lvLockListinUse);
	env->SetLongField(obj, jlvLockList, lvLockList);
	env->SetLongField(obj, jlvLogUsed, lvLogUsed);
	env->SetLongField(obj, jlvLogFileSize, lvLogFileSize);
	env->SetLongField(obj, jlvPrimLog, lvPrimLog);
	env->SetLongField(obj, jlvSecLog, lvSecLog);
	

	for(idx = 0; idx < NO_OF_ITEMS; idx++)
	{
		free(itemList[idx].ptrvalue);
	}

	free(sqlma);
	free (buffer_ptr); 
	env->ReleaseStringUTFChars(dbName, lvDBName);
	//sqledtin(&sqlca);
return 0;
}

/*
 * Class:     com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon
 * Method:    getSQLActivityData
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon_getSQLActivityData
  (JNIEnv *env, jobject obj, jstring dbName, jint bufferSize) {
	//**********************************************************************/
	//*  These are the variables into which the values will be fetched.    */
	//*  The size of sqluint32 is 4 bytes on a 32-bit architecture         */
	//*  and 8-bytes on a 64-bit architecture.                             */
	//*  These values being unsinged, can be accomodated in jlong on       */	
	//*  java side for a 32-bit architecture.                              */
	//**********************************************************************/

	sqluint32 lvLockWaits, lvAvgLockWaitTime, lvTotalSorts, lvAvgSortTime, lvActiveSorts,
		      lvSortOverFlow, lvCommits, lvRollBacks, lvDynamic, lvStatic, lvSelect, lvUID,
			  lvBinds, lvCntInsert, lvCntUpdates, lvCntDeletes, lvCntSelects, lvAutoRebind,
			  lvIntCommits, lvIntRollBacks, lvDeadLockRollBacks;

	//long lvTimeTaken;

	jclass cs;
	
	jfieldID  jlvLockWaits, jlvAvgLockWaitTime, jlvTotalSorts, jlvAvgSortTime, jlvActiveSorts,
		      jlvSortOverFlow, jlvCommits, jlvRollBacks, jlvDynamic, jlvStatic, jlvSelect, jlvUID,
			  jlvBinds, jlvCntInsert, jlvCntUpdates, jlvCntDeletes, jlvCntSelects, jlvAutoRebind,
			  jlvIntCommits, jlvIntRollBacks, jlvDeadLockRollBacks;
	//jfieldID jlvTimeTaken;

	jfieldID msg;
	jstring errval;

	cs = env->GetObjectClass(obj);

	char* lvDBName = (char*) env->GetStringUTFChars(dbName,0);

	//writetolog("From SQL Activity, Database Name: ");
	//writetolog(lvDBName);

	char  lvNativeErrMsg[1024];
	char     *buffer_ptr; 
	int       rc;
	struct    sqlca sqlca;
	sqlm_collected collected;
	int lvBufferSize = bufferSize;

	struct sqlma* sqlma = (struct sqlma *) malloc(SQLMASIZE(1));
	buffer_ptr = (char *) malloc(lvBufferSize); 

	//time_t t1= time(&t1);

	msg = env->GetFieldID(cs,"error_msg", "Ljava/lang/String;");
    int val = -2;


	sqlma->obj_num = 1;
    sqlma->obj_var[0].obj_type = SQLMA_DBASE;
    strcpy(sqlma->obj_var[0].object, lvDBName);

	rc = sqlmonss(SQLM_DBMON_VERSION2, NULL, sqlma, lvBufferSize, buffer_ptr,&collected, &sqlca);
	if(sqlca.sqlcode != 0 && sqlca.sqlcode != 100)
	{
		sendErrorToJava (env, obj, &sqlca);
		free(sqlma);
		free (buffer_ptr); 
		env->ReleaseStringUTFChars(dbName, lvDBName);
		//sqledtin(&sqlca);
		return -1;
	}//End of if

	val = dump_snapshot_output(stdout, collected, buffer_ptr);
	if(val != 0)
	{

		strcpy(lvNativeErrMsg, "Error in fecthing the value from the Native Code ");
		errval = env->NewStringUTF(lvNativeErrMsg);
		env->SetObjectField(obj, msg, errval);
		env->DeleteLocalRef(errval);
		free(sqlma);
		free (buffer_ptr); 
		env->ReleaseStringUTFChars(dbName, lvDBName);
		//sqledtin(&sqlca);
		return -1;
	}//End of if

	//When the data is returned then we retrive the data, into the respective variables.

	sqlm_dbase* db_snap = (sqlm_dbase*) buffer_ptr;

	lvLockWaits           =  db_snap->lock_waits;
    	lvAvgLockWaitTime     =  db_snap->lock_wait_time;
	lvTotalSorts          =  db_snap->total_sorts;
	lvAvgSortTime         =  db_snap->total_sort_time;
	lvActiveSorts         =  db_snap->active_sorts;
	lvSortOverFlow        =  db_snap->sort_overflows;
	lvCommits             =  db_snap->commit_sql_stmts;
	lvRollBacks           =  db_snap->rollback_sql_stmts;
	lvDynamic             =  db_snap->dynamic_sql_stmts;
	lvStatic              =  db_snap->static_sql_stmts;
	lvSelect              =  db_snap->select_sql_stmts;
	lvUID                 =  db_snap->uid_sql_stmts;
	lvBinds               =  db_snap->binds_precompiles;
	lvCntInsert           =  db_snap->rows_inserted;
	lvCntUpdates          =  db_snap->rows_updated;
	lvCntDeletes          =  db_snap->rows_deleted;
	lvCntSelects          =  db_snap->rows_selected;
	lvAutoRebind          =  db_snap->int_auto_rebinds;
	lvIntCommits          =  db_snap->int_commits;
	lvIntRollBacks        =  db_snap->int_rollbacks;
	lvDeadLockRollBacks   =  db_snap->int_deadlock_rollbacks;
	//lvTimeTaken           =  t1;
	
	//printf("value of lvIntCommits %ld \n", lvIntCommits);


	jlvLockWaits            = env->GetFieldID(cs, "mLockWaits", "J");
	jlvAvgLockWaitTime      = env->GetFieldID(cs, "mAvgLockWaitTime", "J");
	jlvTotalSorts           = env->GetFieldID(cs, "mTotalSorts", "J");
	jlvAvgSortTime          = env->GetFieldID(cs, "mAvgSortTime", "J");
	jlvActiveSorts          = env->GetFieldID(cs, "mActiveSorts", "J");
	jlvSortOverFlow         = env->GetFieldID(cs, "mSortOverFlow", "J");
	jlvCommits              = env->GetFieldID(cs, "mCommits", "J");
	jlvRollBacks            = env->GetFieldID(cs, "mRollBacks", "J");
	jlvDynamic              = env->GetFieldID(cs, "mDynamic", "J");
	jlvStatic               = env->GetFieldID(cs, "mStatic", "J");
	jlvSelect               = env->GetFieldID(cs, "mSelect", "J");
	jlvUID                  = env->GetFieldID(cs, "mUID", "J");
	jlvBinds                = env->GetFieldID(cs, "mBinds", "J");
	jlvCntInsert            = env->GetFieldID(cs, "mCntInsert", "J");
	jlvCntUpdates           = env->GetFieldID(cs, "mCntUpdates", "J");
	jlvCntDeletes           = env->GetFieldID(cs, "mCntDeletes", "J");
	jlvCntSelects           = env->GetFieldID(cs, "mCntSelects", "J");
	jlvAutoRebind           = env->GetFieldID(cs, "mAutoRebind", "J");
	jlvIntCommits           = env->GetFieldID(cs, "mIntCommits", "J");
	jlvIntRollBacks         = env->GetFieldID(cs, "mIntRollBacks", "J");
	jlvDeadLockRollBacks    = env->GetFieldID(cs, "mDeadLockRollBacks", "J");
	//jlvTimeTaken            = env->GetFieldID(cs, "TimeTaken", "J");


	//Setting the values for the above fields
	env->SetLongField(obj, jlvLockWaits, lvLockWaits);
	env->SetLongField(obj, jlvAvgLockWaitTime, lvAvgLockWaitTime);
	env->SetLongField(obj, jlvTotalSorts, lvTotalSorts);
	env->SetLongField(obj, jlvAvgSortTime, lvAvgSortTime);
	env->SetLongField(obj, jlvActiveSorts, lvActiveSorts);
	env->SetLongField(obj, jlvSortOverFlow, lvSortOverFlow);
	env->SetLongField(obj, jlvCommits, lvCommits);
	env->SetLongField(obj, jlvRollBacks, lvRollBacks);
	env->SetLongField(obj, jlvDynamic, lvDynamic);
	env->SetLongField(obj, jlvStatic, lvStatic);
	env->SetLongField(obj, jlvSelect, lvSelect);
	env->SetLongField(obj, jlvUID, lvUID);
	env->SetLongField(obj, jlvBinds, lvBinds);
	env->SetLongField(obj, jlvCntInsert, lvCntInsert);
	env->SetLongField(obj, jlvCntUpdates, lvCntUpdates);
	env->SetLongField(obj, jlvCntDeletes, lvCntDeletes);
	env->SetLongField(obj, jlvCntSelects, lvCntSelects);
	env->SetLongField(obj, jlvAutoRebind, lvAutoRebind);
	env->SetLongField(obj, jlvIntCommits, lvIntCommits);
	env->SetLongField(obj, jlvIntRollBacks, lvIntRollBacks);
	env->SetLongField(obj, jlvDeadLockRollBacks, lvDeadLockRollBacks);
	//env->SetLongField(obj, jlvTimeTaken, lvTimeTaken);

	//sqledtin(&sqlca);
	free(sqlma);
	free (buffer_ptr); 


	env->ReleaseStringUTFChars(dbName, lvDBName);
	return 0;
}	

int dump_snapshot_output(FILE* fp, sqlm_collected collected, char* snap) {

    /* Compute the number of top-level data blocks returned */
    int num_top_lvl_structs = collected.db2 + collected.databases +
                       collected.table_databases + collected.lock_databases +
                       collected.applications + collected.applinfos +
                       collected.dcs_applinfos + collected.tablespace_databases;

    /* Process each block */
    while (num_top_lvl_structs--) {
	
        /* Check the block type, (5th byte of any top-level block) */
        switch ((unsigned char) *(snap+4)) {
       
        case SQLM_DBASE_SS:
        {
			return 0;
        }
        break;
       
        default:
        {
			return -1;
        }
       } /* end switch on structure type */
    
	} /* end while there are top-level structs in the snapshot output buffer */
	 return -1;
} /* end of dump_snapshot_output */

/*
 * Class:     com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon
 * Method:    activateDatabase
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_DB2IntelliscopeMon_DB2IntelliscopeMon_activateDatabase
  (JNIEnv * env, jobject obj, jstring dbName, jstring userId, jstring password) {
    char * databasename, * userid, * pass_wd;
    int retvalue = FAILURE;
    struct sqlca sqlerr;
    void * pvCTXT;

    // Get the String values passed from Java
    databasename = (char*) env->GetStringUTFChars(dbName, 0);
    userid = (char*) env->GetStringUTFChars(userId, 0);
    pass_wd = (char*) env->GetStringUTFChars(password, 0);

    retvalue = sqle_activate_db (databasename, userid, pass_wd, &pvCTXT, &sqlerr);
    if( (retvalue != SUCCESS) || ((sqlerr.sqlcode != 0L) && (sqlerr.sqlcode != 100L)) )
    {
	sendErrorToJava(env, obj, &sqlerr);
	retvalue = FAILURE;
    }//end of if
    else 
	retvalue = SUCCESS;

    env->ReleaseStringUTFChars(dbName, databasename);
    env->ReleaseStringUTFChars(userId, userid);
    env->ReleaseStringUTFChars(password, pass_wd);
    return retvalue;
}//end of activateDatabase
