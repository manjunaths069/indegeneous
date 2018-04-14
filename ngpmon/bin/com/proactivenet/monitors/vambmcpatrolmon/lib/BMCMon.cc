// BMCNTMon.cpp : Defines the entry point for the DLL application.
//


#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <thread.h>
#define DEBUG 0
#define STATIC_PEMAPI
#include <pemapi.h>
#include <iostream.h>

#include "BMCMon.h"
static mutex_t  itrMutex;

/*static 	int  			s_iPort=0;
static  int 			s_iIteration = 2;
static	char 			s_cHostName[512];
static	char 			s_cUserName[512];
static	char 			s_cPassword[512];
static	char 			s_cAppType[512];
static	char 			s_cAppInst[512];
static	char 			s_cAppParam[512];
static	char 			s_cPortNb[56];
static	PemnCommHandle  	s_hComm =NULL;*/
static  char			s_cErrorMsg[512];

void Debug(char* dbgInfo)
{
	if(DEBUG)
	{
		cout<< dbgInfo ;
		cout.flush();
	}
}

void Debug(int dbgResult)
{
	if(DEBUG)
	{
		cout<< dbgResult;
		cout.flush();
	}
}


static void _MyDisconnectCallback(
	PemnCommHandle          hComm,
	PemnClientData  	pClientData
	)
{
	//printf("_MyDisconnectCallback called: s_hComm is now invalid\n");
}

JNIEXPORT jstring JNICALL Java_BMCPatrolMonInfo_getErrorMsg
(JNIEnv * jenv, jobject jobj)
{
	mutex_lock(&itrMutex);
	if(s_cErrorMsg == NULL)
		strcpy(s_cErrorMsg,"No Error");
	jstring temp = jenv->NewStringUTF(s_cErrorMsg);
	mutex_unlock(&itrMutex);
	return temp;
}

JNIEXPORT jint JNICALL Java_BMCPatrolMonInfo_nativeInit
(JNIEnv * jenv, jobject jobj, jstring hostName, jint portNo, jstring userName, jstring password)
{
	mutex_lock(&itrMutex);

	Debug( "\nStarting of nativeInit() " );
	int  			s_iPort=0;
	char 			s_cHostName[512];
	char 			s_cUserName[512];
	char 			s_cPassword[512];
	PemnCommHandle  	s_hComm =NULL;
	char	cEncryptedPassword[126+1];
	char *pcOpenResult;
	char *pcPingResult;
	char *user_name;
	char *p_word ;
	int chkInit=0;
	const char *str = jenv->GetStringUTFChars(hostName, 0);
	char *machine_name = strdup(str);
	strcpy(s_cHostName,machine_name);
	jenv->ReleaseStringUTFChars(hostName, str);

	s_iPort = portNo;

	str = jenv->GetStringUTFChars(userName, 0);
	user_name = strdup(str);
	strcpy(s_cUserName,user_name);
	jenv->ReleaseStringUTFChars(userName, str);

	str = jenv->GetStringUTFChars(password, 0);
	p_word = strdup(str);
	strcpy(s_cPassword,p_word);
	jenv->ReleaseStringUTFChars(password, str);

	if ( ! PemnEncrypt(cEncryptedPassword, 126, s_cPassword)){
	//printf("Could not encrypt password\n");
	mutex_unlock(&itrMutex);
	return -1;
	}
	printf("\nThe hostname is %s",s_cHostName);
	printf("\nThe portname is %d",s_iPort);
	printf("\nThe username is %s",s_cUserName);

	Debug( "\nBefore PemnCheckInit() ");
	PemnCheckInit();
	Debug( "\nAfter PemnCheckInit() ");
	Debug( "\nBefore PemnSetCommAttributes ()");
	PemnSetCommAttributes(10000,2000,5,PEMN_TCP_TRANSPORT);
	Debug( "\nAfter PemnSetCommAttributes ()");

	Debug( "\nBefore PemnBPing()");
	pcPingResult=PemnBPing(s_cHostName,s_iPort,5);
	Debug( "\nAfter PemnBPing() ");
	Debug("\nThe pcPing Result is ");
	Debug(pcPingResult );
	/*if (strcmp(PEMN_INTERRUPT, pcPingResult)==0){
		strcpy(s_cErrorMsg,"Unable to connect to the Host");
		//printf("\nThe Error is %s",PemnGetLastMessage());
		mutex_unlock(&itrMutex);
		return -1;
	}*/

	Debug( "\nThe Return from PemnBPing is" );
	Debug( pcPingResult );
	if (strcmp("OK\n", pcPingResult)!=0 ){
                        if(strcmp("WARN\n", pcPingResult)!=0)
                        {
                                if(strcmp("ALARM\n", pcPingResult)!=0)
                                {
                                        if( strcmp("OFFLINE\n", pcPingResult)!=0 )
                                        {
                                                if(strcmp("VOID\n", pcPingResult) !=0)
                                                {
                                                        strcpy(s_cErrorMsg,"Unable to connect to the Host");
                                                        printf("\nThe Error is %s",PemnGetLastMessage());
                                                        mutex_unlock(&itrMutex);
                                                        return -1;
                                                }
                                        }
                                }
                        }
                }
	Debug( "\nBefore PemOpen() ");

		pcOpenResult = PemnBOpen(& s_hComm,
		s_cHostName,s_iPort,	/* agent details 		*/
		s_cUserName,		/* account to log in the agent:
					* user name and encrypted password*/
		cEncryptedPassword,
		5, 			/* Loggin retry Counter 	*/
		_MyDisconnectCallback,  /* Called if agent disconnect happens in
					* in the middle of PemnBOpen   */
		NULL
		);
	Debug( "\nAfter PemOpen() ");
	Debug( "\nThe Return from PemnOpen is" );
	Debug( pcOpenResult );

		if (strcmp("OK\n", pcOpenResult)==0){
			Debug("\nPEMA_OK");

			if (s_hComm) {
				PemnBClose(s_hComm);
				s_hComm = NULL;
				Debug("PemnBClose done\n\n");
			}
		}
		else
		{
			s_hComm = NULL;
			strcpy(s_cErrorMsg,"Authentication Failed");
			printf("\nThe Error is %s",PemnGetLastMessage());
			mutex_unlock(&itrMutex);
			return -1;
		}
	printf("\nEnd of nativeInit()");
	mutex_unlock(&itrMutex);
	return 0;

}

JNIEXPORT jstring JNICALL Java_BMCPatrolMonInfo_getAppTypes  (JNIEnv * jenv, jobject jobj, jstring hostName, jint portNo, jstring userName, jstring password)
{
	mutex_lock(&itrMutex);

	Debug( "\nStarting of getAppTypes() " );
	int  			s_iPort=0;
	char 			s_cHostName[512];
	char 			s_cUserName[512];
	char 			s_cPassword[512];
	PemnCommHandle  	s_hComm =NULL;
	char	cEncryptedPassword[126+1];
	char *pcOpenResult;
	char *pcPingResult;
	char *user_name;
	char *p_word ;
	char * pc;
	const char *str = jenv->GetStringUTFChars(hostName, 0);
	char *machine_name = strdup(str);
	strcpy(s_cHostName,machine_name);
	jenv->ReleaseStringUTFChars(hostName, str);

	s_iPort = portNo;

	str = jenv->GetStringUTFChars(userName, 0);
	user_name = strdup(str);
	strcpy(s_cUserName,user_name);
	jenv->ReleaseStringUTFChars(userName, str);

	str = jenv->GetStringUTFChars(password, 0);
	p_word = strdup(str);
	strcpy(s_cPassword,p_word);
	jenv->ReleaseStringUTFChars(password, str);

	if ( ! PemnEncrypt(cEncryptedPassword, 126, s_cPassword)){
	printf("Could not encrypt password\n");
	mutex_unlock(&itrMutex);
	return NULL;
	}
	Debug("\nThe hostname is ");
	Debug(s_cHostName);
	Debug("\nThe portname is ");
	Debug(s_iPort);
	Debug("\nThe username is ");
	Debug(s_cUserName);

	int chkinit=0;
	Debug( " \nBefore PemnCheckInit() ");
	chkinit=PemnCheckInit();
	Debug( " \nAfter PemnCheckInit() ");
	//cout<<"\n\nResult of PemnCheckInit is "<<chkinit;
	Debug( "\nBefore PemnSetCommAttributes()");

	PemnSetCommAttributes(10000,2000,5,PEMN_TCP_TRANSPORT);
	Debug( "\nAfter PemnSetCommAttributes() ");

	Debug( "\nBefore PemnBPing() ");
	pcPingResult=PemnBPing(s_cHostName,s_iPort,5);
	Debug( "\nAfter PemnBPing() ");

	//printf("\nThe pcPing Result is %s\n",pcPingResult );
	Debug( "The result of Ping is " );
	Debug( pcPingResult );

	if (strcmp("OK\n", pcPingResult)!=0 ){
			if(strcmp("WARN\n", pcPingResult)!=0)
			{
				if(strcmp("ALARM\n", pcPingResult)!=0)
				{
					if( strcmp("OFFLINE\n", pcPingResult)!=0 )
					{
						if(strcmp("VOID\n", pcPingResult) !=0)
						{
							strcpy(s_cErrorMsg,"Unable to connect to the Host");
							printf("\nThe Error is %s",PemnGetLastMessage());
							mutex_unlock(&itrMutex);
							return NULL;
						}
					}
				}
			}
		}

	//printf("\n The Encripted Password is %s", cEncryptedPassword);
	Debug( "\nBefore PemnBOpen()");

	pcOpenResult = PemnBOpen(& s_hComm,
	s_cHostName,s_iPort,	/* agent details 		*/
	s_cUserName,		/* account to log in the agent:
				* user name and encrypted password*/
	cEncryptedPassword,
	5, 			/* Loggin retry Counter 	*/
	_MyDisconnectCallback,  /* Called if agent disconnect happens in
			 * in the middle of PemnBOpen   */
	NULL
	);
	Debug( "\nAfter PemnBOpen()");
	Debug( "\n The result of PemnBOpen is ");
	Debug( pcOpenResult);
	if (strcmp("OK\n", pcOpenResult)!=0){
		pcOpenResult=NULL;
		strcpy(s_cErrorMsg,pcPingResult);
		printf("\nThe Error is %s",PemnGetLastMessage());
		mutex_unlock(&itrMutex);
		return NULL;
	}
		pc = PemnBGetApplList(s_hComm, USE_DEFAULT_TIMEOUT);
	//printf(">Get Appl. List done %s\n", pc);
	if (strcmp("OK\n", pcOpenResult)==0){
		Debug("\nPEMA_OK");

		if (s_hComm) {
			PemnBClose(s_hComm);
			s_hComm = NULL;
			Debug("\nPemnBClose done\n\n");
		}
	}
	else
	{
		s_hComm = NULL;
		strcpy(s_cErrorMsg,PemnGetLastMessage());
		printf("\nThe Error is %s",PemnGetLastMessage());
		mutex_unlock(&itrMutex);
		return NULL;
	}


	Debug("\nEnd of getAppTypes");
	jstring temp = jenv->NewStringUTF(pc);
	mutex_unlock(&itrMutex);
	return temp;

}
JNIEXPORT jstring JNICALL Java_BMCPatrolMonInfo_getAppInsts
(JNIEnv * jenv, jobject jobj, jstring hostName, jint portNo, jstring userName, jstring password,jstring appType)
{
	mutex_lock(&itrMutex);
	Debug( " Starting of getAppInsts()");
	int  			s_iPort=0;
	char 			s_cHostName[512];
	char 			s_cUserName[512];
	char 			s_cPassword[512];
	char 			s_cAppType[512];
	PemnCommHandle  	s_hComm =NULL;
	char	cEncryptedPassword[126+1];
	char *pcOpenResult;
	char *pcPingResult;
	char *user_name;
	char *p_word ;
	char * pc;
	const char *str = jenv->GetStringUTFChars(hostName, 0);
	char *machine_name = strdup(str);
	strcpy(s_cHostName,machine_name);
	jenv->ReleaseStringUTFChars(hostName, str);

	s_iPort = portNo;

	str = jenv->GetStringUTFChars(userName, 0);
	user_name = strdup(str);
	strcpy(s_cUserName,user_name);
	jenv->ReleaseStringUTFChars(userName, str);

	str = jenv->GetStringUTFChars(password, 0);
	p_word = strdup(str);
	strcpy(s_cPassword,p_word);
	jenv->ReleaseStringUTFChars(password, str);

	str = jenv->GetStringUTFChars(appType, 0);
	char *applType = strdup(str);
	strcpy(s_cAppType,applType);
	jenv->ReleaseStringUTFChars(appType, str);

	if ( ! PemnEncrypt(cEncryptedPassword, 126, s_cPassword)){
	printf("Could not encrypt password\n");
	mutex_unlock(&itrMutex);
	return NULL;
	}
	Debug("\nThe hostname is ");
	Debug(s_cHostName);
	Debug("\nThe portname is ");
	Debug(s_iPort);
	Debug("\nThe username is ");
	Debug(s_cUserName);
	Debug( "\nBefore PemnCheckInit()");
	int checkResult =0;

	checkResult = PemnCheckInit();
	Debug( "\nAfter PemnCheckInit()");
	Debug( "The Result of PemnCheckInit() is");
	Debug( checkResult);
	Debug( "\nBefore PemnSetCommAttributes()");

	PemnSetCommAttributes(10000,2000,5,PEMN_TCP_TRANSPORT);
	Debug( "\nAfter PemnSetCommAttributes()");
	Debug( "\nBefore PemnBPing()");

	pcPingResult=PemnBPing(s_cHostName,s_iPort,5);
	Debug( "\nAfter PemnBPing()");
	Debug( "The Result of pcPingResult is ");
	Debug( pcPingResult );

	if (strcmp("OK\n", pcPingResult)!=0 ){
			if(strcmp("WARN\n", pcPingResult)!=0)
			{
				if(strcmp("ALARM\n", pcPingResult)!=0)
				{
					if( strcmp("OFFLINE\n", pcPingResult)!=0 )
					{
						if(strcmp("VOID\n", pcPingResult) !=0)
						{
							strcpy(s_cErrorMsg,"Unable to connect to the Host");
							printf("\nThe Error is %s",PemnGetLastMessage());
							mutex_unlock(&itrMutex);
							return NULL;
						}
					}
				}
			}
		}

	Debug( "\n Before PemnBOpen() ");
	pcOpenResult = PemnBOpen(& s_hComm,
	s_cHostName,s_iPort,	/* agent details 		*/
	s_cUserName,		/* account to log in the agent:
				 * user name and encrypted password*/
	cEncryptedPassword,
	5, 			/* Loggin retry Counter 	*/
	_MyDisconnectCallback,  /* Called if agent disconnect happens in
				 * in the middle of PemnBOpen   */
	NULL
	);
	Debug( "\n After PemnBOpen()");
	Debug( "\n The result of PemnBOpen is ");
	Debug(pcOpenResult);
	if (strcmp("OK\n", pcOpenResult)!=0){
		pcOpenResult=NULL;
		strcpy(s_cErrorMsg,PemnGetLastMessage());
		printf("\nThe Error is %s",PemnGetLastMessage());
		mutex_unlock(&itrMutex);
		return NULL;
	}
	Debug( "\n Before PemnBGetInstList Function call");
	pc = PemnBGetInstList(s_hComm,
		s_cAppType, //"FILESYSTEM",
		USE_DEFAULT_TIMEOUT
		);
	Debug( "\n After PemnBGetInstList Function call");
	if (strcmp("OK\n", pcOpenResult)==0){
		Debug("\nPEMA_OK");

		if (s_hComm) {
			PemnBClose(s_hComm);
			s_hComm = NULL;
			Debug("PemnBClose done\n\n");
		}
	}
	else
	{
		s_hComm = NULL;
		strcpy(s_cErrorMsg,PemnGetLastMessage());
		printf("\nThe Error is %s",PemnGetLastMessage());
		mutex_unlock(&itrMutex);
		return NULL;
	}
	Debug("\nEnd of getApplList \n" );

	jstring temp = jenv->NewStringUTF(pc);
	mutex_unlock(&itrMutex);
	return temp;
}

JNIEXPORT jstring JNICALL Java_BMCPatrolMonInfo_getAppParams
(JNIEnv * jenv, jobject jobj, jstring hostName, jint portNo, jstring userName, jstring password,jstring appType, jstring appInst)
{
	mutex_lock(&itrMutex);
	Debug( "\n In getAppParams()");
	int  			s_iPort=0;
	char 			s_cHostName[512];
	char 			s_cUserName[512];
	char 			s_cPassword[512];
	char 			s_cAppType[512];
	char 			s_cAppInst[512];
	PemnCommHandle  	s_hComm =NULL;
	char	cEncryptedPassword[126+1];
	char *pcOpenResult;
	char *pcPingResult;
	char *user_name;
	char *p_word ;
	char * pc;
	const char *str = jenv->GetStringUTFChars(hostName, 0);
	char *machine_name = strdup(str);
	strcpy(s_cHostName,machine_name);
	jenv->ReleaseStringUTFChars(hostName, str);

	s_iPort = portNo;

	str = jenv->GetStringUTFChars(userName, 0);
	user_name = strdup(str);
	strcpy(s_cUserName,user_name);
	jenv->ReleaseStringUTFChars(userName, str);

	str = jenv->GetStringUTFChars(password, 0);
	p_word = strdup(str);
	strcpy(s_cPassword,p_word);
	jenv->ReleaseStringUTFChars(password, str);

	str = jenv->GetStringUTFChars(appType, 0);
	char *applType = strdup(str);
	strcpy(s_cAppType,applType);
	jenv->ReleaseStringUTFChars(appType, str);

	str = jenv->GetStringUTFChars(appInst, 0);
	char *instName = strdup(str);
	strcpy(s_cAppInst,instName);
	jenv->ReleaseStringUTFChars(appInst, str);

	if ( ! PemnEncrypt(cEncryptedPassword, 126, s_cPassword)){
	strcpy(s_cErrorMsg,PemnGetLastMessage());
	printf("\nCould not encrypt password\n");
	return NULL;
	}
	Debug("\nThe hostname is ");
	Debug(s_cHostName);
	Debug("\nThe portname is ");
	Debug(s_iPort);
	Debug("\nThe username is ");
	Debug(s_cUserName);
	Debug( "\nBefore PemnCheckInit() ");
	int checkResult =0;

	PemnCheckInit();
	Debug( "\nAfter PemnCheckInit() ");
	Debug( "\n checkResult is ");
	Debug( checkResult );
	Debug( "\nBefore PemnSetCommAttributes() ");

	PemnSetCommAttributes(10000,2000,5,PEMN_TCP_TRANSPORT);
	Debug( "\nAfter PemnSetCommAttributes() ");
	Debug( "\nBefore PemnBPing() ");

	pcPingResult=PemnBPing(s_cHostName,s_iPort,5);
	Debug( "\nAfter PemnBPing() ");
	Debug( "\n The Result of pcPingResult is ");
	Debug( pcPingResult );

	if (strcmp("OK\n", pcPingResult)!=0 ){
			if(strcmp("WARN\n", pcPingResult)!=0)
			{
				if(strcmp("ALARM\n", pcPingResult)!=0)
				{
					if( strcmp("OFFLINE\n", pcPingResult)!=0 )
					{
						if(strcmp("VOID\n", pcPingResult) !=0)
						{
							strcpy(s_cErrorMsg,"Unable to connect to the Host");
							printf("\nThe Error is %s",PemnGetLastMessage());
							mutex_unlock(&itrMutex);
							return NULL;
						}
					}
				}
			}
		}
	Debug( "\n Before PemnBOpen() ");

	pcOpenResult = PemnBOpen(& s_hComm,
	s_cHostName,s_iPort,	/* agent details 	*/
	s_cUserName,		/* account to log in the agent:
				 * user name and encrypted password*/
	cEncryptedPassword,
	5, 			/* Loggin retry Counter 	*/
	_MyDisconnectCallback,  /* Called if agent disconnect happens in
				 /* in the middle of PemnBOpen   */
	NULL
	);
	Debug( "\n After PemnBOpen() ");
	Debug( "\nThe result of PemnBOpen is ");
	Debug( pcOpenResult );

	if (strcmp("OK\n", pcOpenResult)!=0){
		strcpy(s_cErrorMsg,PemnGetLastMessage());
		printf("\nThe Error is %s",PemnGetLastMessage());
		mutex_unlock(&itrMutex);
		return NULL;
	}

	Debug( "\n Before the PemnBGetParamList() ");
	pc = PemnBGetParamList(s_hComm,
		s_cAppType, //"FILESYSTEM",
		s_cAppInst,
		USE_DEFAULT_TIMEOUT
		);
	//printf(">Get Appl. List done %s\n", pc);
	Debug( "\n After the PemnBGetParamList() ");

	if (strcmp("OK\n", pcOpenResult)==0){
		Debug("\nPEMA_OK");

		if (s_hComm) {
			PemnBClose(s_hComm);
			s_hComm = NULL;
			Debug("\nPemnBClose done\n\n");
		}
	}
	else
	{
		s_hComm = NULL;
		pcOpenResult=NULL;
		strcpy(s_cErrorMsg,PemnGetLastMessage());
		printf("\nThe Error is %s",PemnGetLastMessage());
		mutex_unlock(&itrMutex);
		return NULL;
	}
	Debug( "\n End of getAppParams Method");
	jstring temp = jenv->NewStringUTF(pc);
	mutex_unlock(&itrMutex);
	return temp ;
}



JNIEXPORT jobjectArray JNICALL Java_BMCPatrolMonInfo_poll
  (JNIEnv * jenv, jobject jobj, jstring hostName, jint portNo, jstring userName, jstring password, jstring appType, jstring appInst, jstring appParam)
{
	mutex_lock(&itrMutex);
	Debug( "\nStarting of poll()");
	int  			s_iPort=0;
	char 			s_cHostName[512];
	char 			s_cUserName[512];
	char 			s_cPassword[512];
	char 			s_cAppType[512];
	char 			s_cAppInst[512];
	char 			s_cAppParam[512];
	PemnCommHandle  	s_hComm =NULL;
//		PemnApplObjHandle hApplObj;
 		PemnParamObjHandle hParamObj;
//		PemnVarObjHandle hVarObj;
//		PemnApplAttrArray *pApplAttr;
//		PemnInstAttrArray *pInstAttr;
		PemnParamAttrArray *pParamAttr;
		jobjectArray objArr;
		char	cEncryptedPassword[126+1];
		char *app_type;
		char *app_inst;
		char *app_param;
		char *pcOpenResult;
		char *pcPingResult;
		int i;
		char *user_name;
		char *p_word ;
		const char *str = jenv->GetStringUTFChars(hostName, 0);
		char *machine_name = strdup(str);
		strcpy(s_cHostName,machine_name);
		jenv->ReleaseStringUTFChars(hostName, str);
		//printf("Starting of poll");
		s_iPort = portNo;

		str = jenv->GetStringUTFChars(userName, 0);
		user_name = strdup(str);
		strcpy(s_cUserName,user_name);
		jenv->ReleaseStringUTFChars(userName, str);

		str = jenv->GetStringUTFChars(password, 0);
		p_word = strdup(str);
		strcpy(s_cPassword,p_word);
		jenv->ReleaseStringUTFChars(password, str);

		str = jenv->GetStringUTFChars(appType, 0);
		app_type = strdup(str);
		strcpy(s_cAppType,app_type);
		jenv->ReleaseStringUTFChars(appType, str);

		str = jenv->GetStringUTFChars(appInst, 0);
		app_inst = strdup(str);
		strcpy(s_cAppInst,app_inst);
		jenv->ReleaseStringUTFChars(appInst, str);

		str = jenv->GetStringUTFChars(appParam, 0);
		app_param = strdup(str);
		strcpy(s_cAppParam,app_param);
		jenv->ReleaseStringUTFChars(appParam, str);

		Debug("The hostname is ");
		Debug(s_cHostName);
		Debug("The portname is ");
		Debug(s_iPort);
		Debug("The username is ");
		Debug(s_cUserName);
		Debug("The password is ");
		Debug(s_cPassword);
		Debug("The App Type is ");
		Debug(s_cAppType);
		Debug("The App Inst is ");
		Debug(s_cAppInst);
		Debug("The App Param is ");
		Debug(s_cAppParam);


		if ( ! PemnEncrypt(cEncryptedPassword, 126, s_cPassword)){
		strcpy(s_cErrorMsg,PemnGetLastMessage());
		printf("Could not encrypt password\n");
		mutex_unlock(&itrMutex);
		return NULL;
		}

		int checkInit =0;
		Debug( "\nBefore PemnCheckInit()");
		checkInit=PemnCheckInit();
		Debug( "\nAfter PemnCheckInit()");
		Debug("\n The result of PemnCheckInit is ");
		Debug(checkInit);
		Debug( "\nBefore PemnSetCommAttributes()");

		PemnSetCommAttributes(10000,2000,5,PEMN_TCP_TRANSPORT);
		Debug( "\nAfter PemnSetCommAttributes()");
		Debug( "\nBefore PemnBPing()");

		pcPingResult=PemnBPing(s_cHostName,s_iPort,5);
		Debug( "\nAfter PemnBPing()");

		//printf("The Return from pcPingResult is %s",pcPingResult);
		Debug( "The Return from pcPingResult is");
		Debug(pcPingResult );
		//if (strcmp("OK\n", pcPingResult)!=0 || strcmp("WARN\n", pcPingResult)!=0 || strcmp("ALARM\n", pcPingResult)!=0 || strcmp("OFFLINE\n", pcPingResult)!=0 || strcmp("VOID\n", pcPingResult)!=0){
		if (strcmp("OK\n", pcPingResult)!=0 ){
			if(strcmp("WARN\n", pcPingResult)!=0)
			{
				if(strcmp("ALARM\n", pcPingResult)!=0)
				{
					if( strcmp("OFFLINE\n", pcPingResult)!=0 )
					{
						if(strcmp("VOID\n", pcPingResult) !=0)
						{
							strcpy(s_cErrorMsg,"Unable to connect to the Host");
							printf("\nThe Error is %s",PemnGetLastMessage());
							mutex_unlock(&itrMutex);
							return NULL;
						}
					}
				}
			}
		}
		Debug( "\nBefore PemnBOpen()");
		pcOpenResult = PemnBOpen(& s_hComm,
		s_cHostName,s_iPort,	/* agent details 		*/
		s_cUserName,		/* account to log in the agent:
					 /* user name and encrypted password*/
		cEncryptedPassword,
		5, 			/* Loggin retry Counter 	*/
		_MyDisconnectCallback,  /* Called if agent disconnect happens in
					 /* in the middle of PemnBOpen   */
		NULL
		);
		Debug( "\nAfter PemnBOpen()");
		Debug( "\n Result of PemnBOpen() is ");
		Debug( pcOpenResult );
		if (strcmp("OK\n", pcOpenResult)==0)
		{
			if (s_hComm)
			{
				//printf(">Get Param. Obj started ---------------------------------------\n");
				hParamObj = PemnBGetParamObj(s_hComm,
				s_cAppType, //"FILESYSTEM",
				s_cAppInst,
				s_cAppParam,
				USE_DEFAULT_TIMEOUT
				);

				pParamAttr = PemnGetParamObjAttributes(hParamObj);
				if (pParamAttr)
				{
					objArr = jenv->NewObjectArray((int) PEMN_PARAM_MAX_ATTR,jenv->FindClass("java/lang/String"),
								jenv->NewStringUTF(""));
					for (i=0; i < (int) PEMN_PARAM_MAX_ATTR; i++)
					{
						jenv->SetObjectArrayElement(objArr, i, jenv->NewStringUTF(((*pParamAttr)[i])?(*pParamAttr)[i]:"NIL"));
						//printf("attribute[%d]=<%s>\n", i,
			 		//			((*pParamAttr)[i])?(*pParamAttr)[i]:"NIL");
					}
				}
				PemnBClose(s_hComm);
				s_hComm = NULL;
				Debug("\nPemnBClose done\n\n");
			}
			else
			{
					PemnBClose(s_hComm);
					s_hComm = NULL;
					pcOpenResult=NULL;
					strcpy(s_cErrorMsg,PemnGetLastMessage());
					printf("Could not read object's attributes\n");
					mutex_unlock(&itrMutex);
					return NULL;
			}
		}
		Debug( "\n End of poll()");
		mutex_unlock(&itrMutex);
		return objArr;
}



JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_vambmcpatrolmon_BMCPatrolMonInfo_poll
  (JNIEnv * jenv, jobject jobj, jstring hostName, jint portNo, jstring userName, jstring password, jstring appType, jstring appInst, jstring appParam)
{
	 return Java_BMCPatrolMonInfo_poll(jenv,jobj,hostName,portNo,userName,password,appType,appInst,appParam);
}

JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_vambmcpatrolmon_BMCPatrolMonInfo_nativeInit
(JNIEnv * jenv, jobject jobj, jstring hostName, jint portNo, jstring userName, jstring password)
{
	return Java_BMCPatrolMonInfo_nativeInit(jenv,jobj,hostName,portNo,userName,password);
}

JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_vambmcpatrolmon_BMCPatrolMonInfo_getAppTypes
(JNIEnv * jenv , jobject jobj, jstring hostName, jint portNo, jstring userName, jstring password)
{
	return Java_BMCPatrolMonInfo_getAppTypes(jenv , jobj, hostName, portNo, userName, password);
}

JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_vambmcpatrolmon_BMCPatrolMonInfo_getAppInsts
(JNIEnv * jenv , jobject jobj, jstring hostName, jint portNo, jstring userName, jstring password ,jstring appType)
{
	return Java_BMCPatrolMonInfo_getAppInsts(jenv , jobj, hostName, portNo, userName, password, appType);
}

JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_vambmcpatrolmon_BMCPatrolMonInfo_getAppParams
(JNIEnv * jenv , jobject jobj, jstring hostName, jint portNo, jstring userName, jstring password ,jstring appType, jstring appInst)
{
	return Java_BMCPatrolMonInfo_getAppParams(jenv , jobj, hostName, portNo, userName, password, appType, appInst);
}

JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_vambmcpatrolmon_BMCPatrolMonInfo_getErrorMsg (JNIEnv * jenv, jobject jobj)
{
	return Java_BMCPatrolMonInfo_getErrorMsg (jenv,jobj);
}


