#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>

#include "pemapi.h"
#include "PemApiWrapper.h"

#define ARRAY_SIZE 1024

#define ENCRYPT_PWD_ERR_MSG "Cannot connect to PATROL Agent."
#define PING_ERR_MSG "PATROL Agent is not reachable."
#define OPEN_CONN_ERR_MSG "Cannot connect to PATROL Agent. User Name or Password may be incorrect."
#define PATROL_DISCONN_ERR_MSG "PATROL Agent disconnected while trying to open connection. User Name or Password may be incorrect."

static PemnCommHandle s_hComm = NULL;
void PNEncrypt(char *pabDst,const char *pabSrc);
#ifndef LINUX
#ifdef __cplusplus
extern "C" static void DisconnectCallback(PemnCommHandle hComm, PemnClientData pClientData) ;
#else
static void DisconnectCallback(PemnCommHandle hComm, PemnClientData pClientData) ;
#endif
#endif //LINUX

/**
 * Function: DisconnectCallback
 *------------------------------------------------------------------
 * Purpose :
 * Called if agent disconnect happens in
 * in the middle of PemnBOpen.
 * Then, the connection handle (s_hComm) becomes invalid. Hence, 
 * the connection handle is set to null.
 *------------------------------------------------------------------
 */
static void DisconnectCallback(PemnCommHandle hComm, PemnClientData pClientData) {
	s_hComm = NULL;
	#ifdef XTRALOG
        //printf("PemApiWrapper.DisconnectCallback(): PATROL disconnected during PemnBOpen. Communication Handle is now invalid.\n");
	#endif
}

//JNI Wrapper functions

JNIEXPORT void JNICALL initLibrary(JNIEnv *env, jobject obj, jobject logger) {
	env->MonitorEnter(obj);
	int status = PemnCheckInit();
	if (status == 0) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), "PEM library is not initialized. status=0");
	} else if (status ==1) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), "lib init. status=1");
	}
	env->MonitorExit(obj);
}

JNIEXPORT jint JNICALL ping(JNIEnv *env, jobject obj, jstring hostName, jint port, jint timeout, jstring protocol) {
        //printf("\nInside pemapiwrapper.cc ping() method\n");
	env->MonitorEnter(obj);
	//Get hostName
	const char *hostStr = env->GetStringUTFChars(hostName, 0);
	jsize hostNameSize = env->GetStringUTFLength(hostName);
	char host[ARRAY_SIZE];
	strcpy(host, hostStr);
	env->ReleaseStringUTFChars(hostName, hostStr);
        //printf("\nHost Name is: %s\n", host);

        //Get Protocol Used
        //This is used to know whether Protocol UDP Ping is True or False - Added by Kishore.
        const char *protocolStr = env->GetStringUTFChars(protocol, 0);
        jsize protocolSize = env->GetStringUTFLength(protocol);
        char prtcl[ARRAY_SIZE];
        strcpy(prtcl, protocolStr);
        env->ReleaseStringUTFChars(protocol, protocolStr); 
        //printf("\nProtocol used is (in pemapiwrapper.cc): %s\n", prtcl);
       
        PemnSetCommAttributes(0,0,0,PEMN_TCP_TRANSPORT);
        
        if(strcmp("true", prtcl) == 0)
        {
        //printf("\nInside UDP if cond()\n");
	const char *result = PemnBPing(host, port, timeout);
        //printf("\nValue of result is : %s\n", result);
	if (strcmp("OK\n", result) == 0 || strcmp("WARN\n", result) == 0 || strcmp("ALARM\n", result) == 0 || 
		strcmp("OFFLINE\n", result) == 0 || strcmp("VOID\n", result) == 0) {
		env->MonitorExit(obj);
		return 0;
	} else {
		env->MonitorExit(obj);
		return -1;
	}
        }

}

JNIEXPORT jstring JNICALL getApplicationList(JNIEnv *env, jobject obj, jstring hostName, jint port,
							jstring userName, jstring password, jint timeout, jstring protocol, jint encType) {
	env->MonitorEnter(obj);
	//Get hostName
	const char *hostStr = env->GetStringUTFChars(hostName, 0);
	jsize hostNameSize = env->GetStringUTFLength(hostName);
	char host[ARRAY_SIZE];
	strcpy(host, hostStr);
	env->ReleaseStringUTFChars(hostName, hostStr);

	//Get userName
	const char *userStr = env->GetStringUTFChars(userName, 0);
	jsize userNameSize = env->GetStringUTFLength(userName);
	char user[ARRAY_SIZE];
	strcpy(user, userStr);
	env->ReleaseStringUTFChars(userName, userStr);

	//Get password
	const char *pwdStr = env->GetStringUTFChars(password, 0);
	jsize passwordSize = env->GetStringUTFLength(password);
	char pwd[ARRAY_SIZE];
	strcpy(pwd, pwdStr);
	env->ReleaseStringUTFChars(password, pwdStr);

        //Get Protocol Used
        const char *protocolStr = env->GetStringUTFChars(protocol, 0);
        jsize protocolSize = env->GetStringUTFLength(protocol);
        char prtcl[ARRAY_SIZE];
        strcpy(prtcl, protocolStr);
        env->ReleaseStringUTFChars(protocol, protocolStr);
        //printf("\nProtocol used is (in pemapiwrapper.cpp): %s\n", prtcl);  

	//Connnect to PATROL Agent
	//Step 1.Encrypt Password
	char encryptedPwd[126 + 1];
	if (encType == 1) {								//DES Encryted
		PNEncrypt(encryptedPwd, (char*) pwd);
	}
	else if (encType == 2) {						//PEM Encrypted
		strcpy(encryptedPwd, pwd);
	} else {										//Plain
		if (! PemnEncrypt(encryptedPwd, 126, pwd)){
			env->ThrowNew(env->FindClass("java/lang/Exception"), ENCRYPT_PWD_ERR_MSG);
			env->MonitorExit(obj);
			return NULL;
		}
	}

        PemnSetCommAttributes(0,0,0,PEMN_TCP_TRANSPORT);	
	//Step 2.1 Ping the PATROL Agent
        if(strcmp("true", prtcl) == 0)
        {
	const char *result = PemnBPing(host, port, timeout);
	if (strcmp("OK\n", result) == 0 || strcmp("WARN\n", result) == 0 || strcmp("ALARM\n", result) == 0 || 
		strcmp("OFFLINE\n", result) == 0 || strcmp("VOID\n", result) == 0) {
		//log("PATROL Agent is reachable.");
	} else {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PING_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
        }
	//Step 2.2.Open the connection with the PATROL Agent
	char *status = PemnBOpen(& s_hComm, host, port, user, encryptedPwd, 5, DisconnectCallback, NULL);
	if (!s_hComm) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PATROL_DISCONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
	if (strcmp("OK\n", status)!= 0) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), OPEN_CONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}

	//Step 3. Do Work. Get Application List
	char *applicationList = PemnBGetApplList(s_hComm, timeout);
	jstring applicationListStr = env->NewStringUTF(applicationList);

	// Step 4: Close the connection
	if (s_hComm) {
		PemnBClose(s_hComm);
		s_hComm = NULL;
	}

	env->MonitorExit(obj);
	return applicationListStr;
}

JNIEXPORT jstring JNICALL getInstanceList(JNIEnv *env, jobject obj, jstring hostName, jint port, 
						jstring userName, jstring password, jstring application, jint timeout, jstring protocol, jint encType) {
	env->MonitorEnter(obj);
	//Get hostName
	const char *hostStr = env->GetStringUTFChars(hostName, 0);
	jsize hostNameSize = env->GetStringUTFLength(hostName);
	char host[ARRAY_SIZE];
	strcpy(host, hostStr);
	env->ReleaseStringUTFChars(hostName, hostStr);

	//Get userName
	const char *userStr = env->GetStringUTFChars(userName, 0);
	jsize userNameSize = env->GetStringUTFLength(userName);
	char user[ARRAY_SIZE];
	strcpy(user, userStr);
	env->ReleaseStringUTFChars(userName, userStr);

	//Get password
	const char *pwdStr = env->GetStringUTFChars(password, 0);
	jsize passwordSize = env->GetStringUTFLength(password);
	char pwd[ARRAY_SIZE];
	strcpy(pwd, pwdStr);
	env->ReleaseStringUTFChars(password, pwdStr);

	//Get application
	const char *applStr = env->GetStringUTFChars(application, 0);
	jsize applicationSize = env->GetStringUTFLength(application);
	char appl[ARRAY_SIZE];
	strcpy(appl, applStr);
	env->ReleaseStringUTFChars(application, applStr);

        //Get Protocol Used
        const char *protocolStr = env->GetStringUTFChars(protocol, 0);
        jsize protocolSize = env->GetStringUTFLength(protocol);
        char prtcl[ARRAY_SIZE];
        strcpy(prtcl, protocolStr);
        env->ReleaseStringUTFChars(protocol, protocolStr);
        //printf("\nProtocol used is (in pemapiwrapper.cpp): %s\n", prtcl);

	//Connnect to PATROL Agent
	//Step 1.Encrypt Password
	char encryptedPwd[126 + 1];
	if (encType == 1) {								//DES Encryted
		PNEncrypt(encryptedPwd, (char*) pwd);
	}
	else if (encType == 2) {						//PEM Encrypted
		strcpy(encryptedPwd, pwd);
	} else {										//Plain
		if (! PemnEncrypt(encryptedPwd, 126, pwd)){
			env->ThrowNew(env->FindClass("java/lang/Exception"), ENCRYPT_PWD_ERR_MSG);
			env->MonitorExit(obj);
			return NULL;
		}
	}

        PemnSetCommAttributes(0,0,0,PEMN_TCP_TRANSPORT);
	//Step 2.1 Ping the PATROL Agent
        if(strcmp("true", prtcl) == 0)
        {
	const char *result = PemnBPing(host, port, timeout);
	if (strcmp("OK\n", result) == 0 || strcmp("WARN\n", result) == 0 || strcmp("ALARM\n", result) == 0 || 
		strcmp("OFFLINE\n", result) == 0 || strcmp("VOID\n", result) == 0) {
		//log("PATROL Agent is reachable.");
	} else {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PING_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
        }
	//Step 2.2 Open the connection with the PATROL Agent
	char *status = PemnBOpen(& s_hComm, host, port, user, encryptedPwd, 5, DisconnectCallback, NULL);
	if (!s_hComm) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PATROL_DISCONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
	if (strcmp("OK\n", status)!= 0) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), OPEN_CONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}

	//Step 3. Do Work. Get Instance List
	char *instanceList = PemnBGetInstList(s_hComm, appl, timeout);
	jstring instanceListStr = env->NewStringUTF(instanceList);

	// Step 4: Close the connection
	if (s_hComm) {
		PemnBClose(s_hComm);
		s_hComm = NULL;
	}

	env->MonitorExit(obj);
	return instanceListStr;
}

JNIEXPORT jstring JNICALL getParametersList(JNIEnv *env, jobject obj, jstring hostName, jint port, 
						jstring userName, jstring password, jstring application, jstring instance, jint timeout, jstring protocol, jint encType) {
	env->MonitorEnter(obj);
	//Get hostName
	const char *hostStr = env->GetStringUTFChars(hostName, 0);
	jsize hostNameSize = env->GetStringUTFLength(hostName);
	char host[ARRAY_SIZE];
	strcpy(host, hostStr);
	env->ReleaseStringUTFChars(hostName, hostStr);

	//Get userName
	const char *userStr = env->GetStringUTFChars(userName, 0);
	jsize userNameSize = env->GetStringUTFLength(userName);
	char user[ARRAY_SIZE];
	strcpy(user, userStr);
	env->ReleaseStringUTFChars(userName, userStr);

	//Get password
	const char *pwdStr = env->GetStringUTFChars(password, 0);
	jsize passwordSize = env->GetStringUTFLength(password);
	char pwd[ARRAY_SIZE];
	strcpy(pwd, pwdStr);
	env->ReleaseStringUTFChars(password, pwdStr);

	//Get application
	const char *applStr = env->GetStringUTFChars(application, 0);
	jsize applicationSize = env->GetStringUTFLength(application);
	char appl[ARRAY_SIZE];
	strcpy(appl, applStr);
	env->ReleaseStringUTFChars(application, applStr);

	//Get instance
	const char *instStr = env->GetStringUTFChars(instance, 0);
	jsize instanceSize = env->GetStringUTFLength(instance);
	char inst[ARRAY_SIZE];
	strcpy(inst, instStr);
	env->ReleaseStringUTFChars(instance, instStr);

        //Get Protocol Used
        const char *protocolStr = env->GetStringUTFChars(protocol, 0);
        jsize protocolSize = env->GetStringUTFLength(protocol);
        char prtcl[ARRAY_SIZE];
        strcpy(prtcl, protocolStr);
        env->ReleaseStringUTFChars(protocol, protocolStr);
        //printf("\nProtocol used is (in pemapiwrapper.cpp): %s\n", prtcl);

	//Connnect to PATROL Agent
	//Step 1.Encrypt Password
	char encryptedPwd[126 + 1];
	if (encType == 1) {								//DES Encryted
		PNEncrypt(encryptedPwd, (char*) pwd);
	}
	else if (encType == 2) {						//PEM Encrypted
		strcpy(encryptedPwd, pwd);
	} else {										//Plain
		if (! PemnEncrypt(encryptedPwd, 126, pwd)){
			env->ThrowNew(env->FindClass("java/lang/Exception"), ENCRYPT_PWD_ERR_MSG);
			env->MonitorExit(obj);
			return NULL;
		}
	}

        PemnSetCommAttributes(0,0,0,PEMN_TCP_TRANSPORT);
	//Step 2.1 Ping the PATROL Agent
        if(strcmp("true", prtcl) == 0)
        {
	const char *result = PemnBPing(host, port, timeout);
	if (strcmp("OK\n", result) == 0 || strcmp("WARN\n", result) == 0 || strcmp("ALARM\n", result) == 0 || 
		strcmp("OFFLINE\n", result) == 0 || strcmp("VOID\n", result) == 0) {
		//log("PATROL Agent is reachable.");
	} else {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PING_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
        }
	//Step 2.2 Open the connection with the PATROL Agent
	char *status = PemnBOpen(& s_hComm, host, port, user, encryptedPwd, 5, DisconnectCallback, NULL);
	if (!s_hComm) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PATROL_DISCONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
	if (strcmp("OK\n", status)!= 0) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), OPEN_CONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}

	//Step 3. Do Work. Get Parameters List
	char *parametersList = PemnBGetParamList(s_hComm, appl, inst, timeout);
	jstring parametersListStr = env->NewStringUTF(parametersList);

	// Step 4: Close the connection
	if (s_hComm) {
		PemnBClose(s_hComm);
		s_hComm = NULL;
	}

	env->MonitorExit(obj);
	return parametersListStr;
}


JNIEXPORT jobjectArray JNICALL getParametersValueArray(JNIEnv *env, jobject obj, jstring hostName, 
						jint port, jstring userName, jstring password, jobjectArray paramObjArr, jint timeout, jstring protocol, jint encType) {
	env->MonitorEnter(obj);
	#ifdef XTRALOG
	//printf("================================================================\n");
	#endif
	//Get hostName
	const char *hostStr = env->GetStringUTFChars(hostName, 0);
	jsize hostNameSize = env->GetStringUTFLength(hostName);
	char host[ARRAY_SIZE];
	strcpy(host, hostStr);
	env->ReleaseStringUTFChars(hostName, hostStr);
	#ifdef XTRALOG
	//printf("PemApiWrapper.getParametersValueArray(): HostName=%s\n", host);
	#endif

	//Get userName
	const char *userStr = env->GetStringUTFChars(userName, 0);
	jsize userNameSize = env->GetStringUTFLength(userName);
	char user[ARRAY_SIZE];
	strcpy(user, userStr);
	env->ReleaseStringUTFChars(userName, userStr);
	#ifdef XTRALOG
	//printf("PemApiWrapper.getParametersValueArray(): UserName=%s\n", user);
	#endif

	//Get password
	const char *pwdStr = env->GetStringUTFChars(password, 0);
	jsize passwordSize = env->GetStringUTFLength(password);
	char pwd[ARRAY_SIZE];
	strcpy(pwd, pwdStr);
	env->ReleaseStringUTFChars(password, pwdStr);
	#ifdef XTRALOG
	//printf("PemApiWrapper.getParametersValueArray(): Got Password.\n");
	#endif

        //Get Protocol Used
        const char *protocolStr = env->GetStringUTFChars(protocol, 0);
        jsize protocolSize = env->GetStringUTFLength(protocol);
        char prtcl[ARRAY_SIZE];
        strcpy(prtcl, protocolStr);
        env->ReleaseStringUTFChars(protocol, protocolStr);
        //printf("\nProtocol used is (in pemapiwrapper.cpp): %s\n", prtcl);

	//Connnect to PATROL Agent
	//Step 1.Encrypt Password
	char encryptedPwd[126 + 1];
	if (encType == 1) {								//DES Encryted
		PNEncrypt(encryptedPwd, (char*) pwd);
	}
	else if (encType == 2) {						//PEM Encrypted
		strcpy(encryptedPwd, pwd);
	} else {										//Plain
		if (! PemnEncrypt(encryptedPwd, 126, pwd)){
			env->ThrowNew(env->FindClass("java/lang/Exception"), ENCRYPT_PWD_ERR_MSG);
			env->MonitorExit(obj);
			return NULL;
		}
	}

        PemnSetCommAttributes(0,0,0,PEMN_TCP_TRANSPORT);
	//Step 2.1 Ping the PATROL Agent
        if(strcmp("true", prtcl) == 0)
        {
	const char *result = PemnBPing(host, port, timeout);
	if (strcmp("OK\n", result) == 0 || strcmp("WARN\n", result) == 0 || strcmp("ALARM\n", result) == 0 || 
		strcmp("OFFLINE\n", result) == 0 || strcmp("VOID\n", result) == 0) {
		//log("PATROL Agent is reachable.");
	} else {
		#ifdef XTRALOG
		//printf("PemApiWrapper.getParametersValueArray(): Could not Ping the PATROL Agent.\n");
		#endif
		env->ThrowNew(env->FindClass("java/lang/Exception"), PING_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
        }
	//Step 2.2 Open the connection with the PATROL Agent
	char *status = PemnBOpen(& s_hComm, host, port, user, encryptedPwd, 5, DisconnectCallback, NULL);
	if (!s_hComm) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PATROL_DISCONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
	if (strcmp("OK\n", status)!= 0) {
		#ifdef XTRALOG
		//printf("PemApiWrapper.getParametersValueArray(): PATROL Connection status is not OK.\n");
		#endif
		env->ThrowNew(env->FindClass("java/lang/Exception"), OPEN_CONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}

	//Get paramObjArr
	#ifdef XTRALOG
	//printf("PemApiWrapper.getParametersValueArray(): Getting Class and Method IDs.\n");
	#endif
	jsize arrSize = env->GetArrayLength(paramObjArr);
	
	jobject initialElement = NULL;
	jclass attrMapClass = env->FindClass("com/proactivenet/monitors/BMCMonBase/AttrMap");
	jmethodID attrMapConstructorMId = env->GetMethodID(attrMapClass, "<init>", "()V");
	
	jmethodID getApplicationMId = env->GetMethodID(attrMapClass, "getAppType", "()Ljava/lang/String;");
	jmethodID setApplicationMId = env->GetMethodID(attrMapClass, "setAppType", "(Ljava/lang/String;)V");
	
	jmethodID getInstanceMId = env->GetMethodID(attrMapClass, "getAppInstance", "()Ljava/lang/String;");
	jmethodID setInstanceMId = env->GetMethodID(attrMapClass, "setAppInstance", "(Ljava/lang/String;)V");

	jmethodID getParameterMId = env->GetMethodID(attrMapClass, "getAppParam", "()Ljava/lang/String;");
	jmethodID setParameterMId = env->GetMethodID(attrMapClass, "setAppParam", "(Ljava/lang/String;)V");

	jmethodID getScaleMId = env->GetMethodID(attrMapClass, "getScale", "()F");
	jmethodID setScaleMId = env->GetMethodID(attrMapClass, "setScale", "(F)V");

	jmethodID getMeasureAsMId = env->GetMethodID(attrMapClass, "getMeasureAs", "()Ljava/lang/String;");
	jmethodID setMeasureAsMId = env->GetMethodID(attrMapClass, "setMeasureAs", "(Ljava/lang/String;)V");
	
	jmethodID setValueMId = env->GetMethodID(attrMapClass, "setValue", "(Ljava/lang/String;)V");

	jobjectArray attrMapObjArr = env->NewObjectArray(arrSize, attrMapClass, initialElement);
	//check attrMapObjArr is constructed successfully.
	if (!attrMapObjArr) {
		#ifdef XTRALOG
		//printf("PemApiWrapper.getParametersValueArray(): Could not construct New Object Array.\n");
		#endif
		env->ThrowNew(env->FindClass("java/lang/Exception"), "Could not construct New Object Array.");
	
	// Step 4: Close the connection
	if (s_hComm) {
		PemnBClose(s_hComm);
		s_hComm = NULL;
	}
		env->MonitorExit(obj);
		return NULL;
	}
	
	#ifdef XTRALOG
	//printf("PemApiWrapper.getParametersValueArray(): Done getting Class and Method IDs.\n");
	

	//printf("\n\nPemApiWrapper.getParametersValueArray(): Array Size=%d\n", arrSize);
	#endif
	int i = 0;
	for (i = 0; i < arrSize; i++) {
		#ifdef XTRALOG
		//printf("\n\nPemApiWrapper.getParametersValueArray(): For loop index=%d\n", i);
		#endif
		jobject paramObj = env->GetObjectArrayElement(paramObjArr, i);
		//Check if the object is null
		if (!paramObj) {
			#ifdef XTRALOG
			//printf("PemApiWrapper.getParametersValueArray(): AttrMap object is null.\n");
			#endif
			env->ThrowNew(env->FindClass("java/lang/Exception"), "AttrMap object is null.");
	
		// Step 4: Close the connection
		if (s_hComm) {
			PemnBClose(s_hComm);
			s_hComm = NULL;
		}
			env->MonitorExit(obj);
			return NULL;
		}
		//check the type of paramObj
		if (!(env->IsInstanceOf(paramObj, attrMapClass)) ) {
			#ifdef XTRALOG
			//printf("PemApiWrapper.getParametersValueArray(): Type mismatch. Object is not of type AttrMap.\n");
			#endif
			env->ThrowNew(env->FindClass("java/lang/Exception"), "Type mismatch. Object is not of type AttrMap.");
	
	// Step 4: Close the connection
	if (s_hComm) {
		PemnBClose(s_hComm);
		s_hComm = NULL;
	}
			env->MonitorExit(obj);
			return NULL;
		}

		jstring applicationType = (jstring) env->CallObjectMethod(paramObj, getApplicationMId);
		jstring instanceType = (jstring) env->CallObjectMethod(paramObj, getInstanceMId);
		jstring parameterType = (jstring) env->CallObjectMethod(paramObj, getParameterMId);
		jfloat scale = env->CallFloatMethod(paramObj, getScaleMId);
		jstring measureAs = (jstring) env->CallObjectMethod(paramObj, getMeasureAsMId);
		
		//Get application
		const char *applicationStr = env->GetStringUTFChars(applicationType, 0);
		jsize applicationSize = env->GetStringUTFLength(applicationType);
		char application[ARRAY_SIZE];
		strcpy(application, applicationStr);
		env->ReleaseStringUTFChars(applicationType, applicationStr);
		#ifdef XTRALOG
		//printf("PemApiWrapper.getParametersValueArray(): Application=%s\n", application);
		#endif

		//Get instance
		const char *instanceStr = env->GetStringUTFChars(instanceType, 0);
		jsize instanceSize = env->GetStringUTFLength(instanceType);
		char instance[ARRAY_SIZE];
		strcpy(instance, instanceStr);
		env->ReleaseStringUTFChars(instanceType, instanceStr);
		#ifdef XTRALOG
		//printf("PemApiWrapper.getParametersValueArray(): Instance=%s\n", instance);
		#endif

		//Get parameter
		const char *parameterStr = env->GetStringUTFChars(parameterType, 0);
		jsize parameterSize = env->GetStringUTFLength(parameterType);
		char parameter[ARRAY_SIZE];
		strcpy(parameter, parameterStr);
		env->ReleaseStringUTFChars(parameterType, parameterStr);
		#ifdef XTRALOG
		//printf("PemApiWrapper.getParametersValueArray(): Parameter=%s\n", parameter);
		#endif

		//Get value
		//TBD. If there is error, it returns error message. Get the error msg, throw exception and return.
		#ifdef XTRALOG
		//printf("PemApiWrapper.getParametersValueArray(): Before calling PemnBGetParamObj..\n");
		#endif
		PemnParamObjHandle hParamObj = PemnBGetParamObj(s_hComm, application, instance, parameter, timeout);
		#ifdef XTRALOG
		//printf("PemApiWrapper.getParametersValueArray(): Before calling PemnGetParamObjAttributes..\n");
		#endif
		PemnParamAttrArray *paramAttrArr = PemnGetParamObjAttributes(hParamObj);
		#ifdef XTRALOG
		//printf("PemApiWrapper.getParametersValueArray(): After calling PemnGetParamObjAttributes..\n");
		#endif
		char *currParamValue = NULL;
		if (paramAttrArr) {
			currParamValue = (*paramAttrArr)[PEMN_PARAMCURRENTVALUE];
		} else {
			currParamValue = "<error>";
		}
		#ifdef XTRALOG
		//printf("PemApiWrapper.getParametersValueArray(): Value=%s\n", currParamValue);
		#endif
		jstring currValue = env->NewStringUTF(currParamValue);
		

		//Set values to returnObjArr
		#ifdef XTRALOG
		//printf("PemApiWrapper.getParametersValueArray(): Creating AttrMap object..\n");
		#endif
		jobject attrMapObj = env->NewObject(attrMapClass, attrMapConstructorMId);
		env->CallVoidMethod(attrMapObj, setApplicationMId, applicationType);
		env->CallVoidMethod(attrMapObj, setInstanceMId, instanceType);
		env->CallVoidMethod(attrMapObj, setParameterMId, parameterType);
		env->CallVoidMethod(attrMapObj, setValueMId, currValue);
		env->CallVoidMethod(attrMapObj, setScaleMId, scale);
		env->CallVoidMethod(attrMapObj, setMeasureAsMId, measureAs);
		
		#ifdef XTRALOG
		//printf("PemApiWrapper.getParametersValueArray(): Before setting AttrMap object to Array..\n");
		#endif
		env->SetObjectArrayElement(attrMapObjArr, i, attrMapObj); 
		
		//Delete local references
		env->DeleteLocalRef(paramObj);
		env->DeleteLocalRef(applicationType);
		env->DeleteLocalRef(instanceType);
		env->DeleteLocalRef(parameterType);
		env->DeleteLocalRef(currValue);
	}
	
	// Step 4: Close the connection
	if (s_hComm) {
		PemnBClose(s_hComm);
		s_hComm = NULL;
	}

	env->MonitorExit(obj);
	return attrMapObjArr;
}


JNIEXPORT jstring JNICALL getVariablesList(JNIEnv *env, jobject obj, jstring hostName, jint port, 
						jstring userName, jstring password, jstring varListObjName, jstring keyword, jint timeout, jstring protocol, jint encType) {
	env->MonitorEnter(obj);
	//Get hostName
	const char *hostStr = env->GetStringUTFChars(hostName, 0);
	jsize hostNameSize = env->GetStringUTFLength(hostName);
	char host[ARRAY_SIZE];
	strcpy(host, hostStr);
	env->ReleaseStringUTFChars(hostName, hostStr);

	//Get userName
	const char *userStr = env->GetStringUTFChars(userName, 0);
	jsize userNameSize = env->GetStringUTFLength(userName);
	char user[ARRAY_SIZE];
	strcpy(user, userStr);
	env->ReleaseStringUTFChars(userName, userStr);

	//Get password
	const char *pwdStr = env->GetStringUTFChars(password, 0);
	jsize passwordSize = env->GetStringUTFLength(password);
	char pwd[ARRAY_SIZE];
	strcpy(pwd, pwdStr);
	env->ReleaseStringUTFChars(password, pwdStr);

	//Get varListObjName
	const char *varLstObjNameStr = env->GetStringUTFChars(varListObjName, 0);
	jsize varListObjNameSize = env->GetStringUTFLength(varListObjName);
	char varLstObjName[ARRAY_SIZE];
	strcpy(varLstObjName, varLstObjNameStr);
	env->ReleaseStringUTFChars(varListObjName, varLstObjNameStr);

	//Get keyword
	const char *keyStr = env->GetStringUTFChars(keyword, 0);
	jsize keywordSize = env->GetStringUTFLength(keyword);
	char key[ARRAY_SIZE];
	strcpy(key, keyStr);
	env->ReleaseStringUTFChars(keyword, keyStr);

        //Get Protocol Used
        const char *protocolStr = env->GetStringUTFChars(protocol, 0);
        jsize protocolSize = env->GetStringUTFLength(protocol);
        char prtcl[ARRAY_SIZE];
        strcpy(prtcl, protocolStr);
        env->ReleaseStringUTFChars(protocol, protocolStr);
        //printf("\nProtocol used is (in pemapiwrapper.cpp): %s\n", prtcl);

	//Connnect to PATROL Agent
	//Step 1.Encrypt Password
	char encryptedPwd[126 + 1];
	if (encType == 1) {								//DES Encryted
		PNEncrypt(encryptedPwd, (char*) pwd);
	}
	else if (encType == 2) {						//PEM Encrypted
		strcpy(encryptedPwd, pwd);
	} else {										//Plain
		if (! PemnEncrypt(encryptedPwd, 126, pwd)){
			env->ThrowNew(env->FindClass("java/lang/Exception"), ENCRYPT_PWD_ERR_MSG);
			env->MonitorExit(obj);
			return NULL;
		}
	}

        PemnSetCommAttributes(0,0,0,PEMN_TCP_TRANSPORT);
	//Step 2.1 Ping the PATROL Agent
        if(strcmp("true", prtcl) == 0)
        {
	const char *result = PemnBPing(host, port, timeout);
	if (strcmp("OK\n", result) == 0 || strcmp("WARN\n", result) == 0 || strcmp("ALARM\n", result) == 0 || 
		strcmp("OFFLINE\n", result) == 0 || strcmp("VOID\n", result) == 0) {
		//log("PATROL Agent is reachable.");
	} else {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PING_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
        }
	//Step 2.2 Open the connection with the PATROL Agent
	char *status = PemnBOpen(& s_hComm, host, port, user, encryptedPwd, 5, DisconnectCallback, NULL);
	if (!s_hComm) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PATROL_DISCONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
	if (strcmp("OK\n", status)!= 0) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), OPEN_CONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}

	//Step 3. Do Work. Get Variables List
	char *variablesList = PemnBGetVarList(s_hComm, varLstObjName, key, timeout);
	jstring variablesListStr = env->NewStringUTF(variablesList);

	// Step 4: Close the connection
	if (s_hComm) {
		PemnBClose(s_hComm);
		s_hComm = NULL;
	}

	env->MonitorExit(obj);
	return variablesListStr;

}


JNIEXPORT jstring JNICALL getVariableValue(JNIEnv *env, jobject obj, jstring hostName, jint port, 
							jstring userName, jstring password, jstring variableName, jint timeout, jstring protocol, jint encType) {
	env->MonitorEnter(obj);
	//Get hostName
	const char *hostStr = env->GetStringUTFChars(hostName, 0);
	jsize hostNameSize = env->GetStringUTFLength(hostName);
	char host[ARRAY_SIZE];
	strcpy(host, hostStr);
	env->ReleaseStringUTFChars(hostName, hostStr);

	//Get userName
	const char *userStr = env->GetStringUTFChars(userName, 0);
	jsize userNameSize = env->GetStringUTFLength(userName);
	char user[ARRAY_SIZE];
	strcpy(user, userStr);
	env->ReleaseStringUTFChars(userName, userStr);

	//Get password
	const char *pwdStr = env->GetStringUTFChars(password, 0);
	jsize passwordSize = env->GetStringUTFLength(password);
	char pwd[ARRAY_SIZE];
	strcpy(pwd, pwdStr);
	env->ReleaseStringUTFChars(password, pwdStr);

	//Get variableName
	const char *varNameStr = env->GetStringUTFChars(variableName, 0);
	jsize variableNameSize = env->GetStringUTFLength(variableName);
	char varName[ARRAY_SIZE];
	strcpy(varName, varNameStr);
	env->ReleaseStringUTFChars(variableName, varNameStr);

        //Get Protocol Used
        const char *protocolStr = env->GetStringUTFChars(protocol, 0);
        jsize protocolSize = env->GetStringUTFLength(protocol);
        char prtcl[ARRAY_SIZE];
        strcpy(prtcl, protocolStr);
        env->ReleaseStringUTFChars(protocol, protocolStr);
        //printf("\nProtocol used is (in pemapiwrapper.cpp): %s\n", prtcl);

	//Connnect to PATROL Agent
	//Step 1.Encrypt Password
	char encryptedPwd[126 + 1];
	if (encType == 1) {								//DES Encryted
		PNEncrypt(encryptedPwd, (char*) pwd);
	}
	else if (encType == 2) {						//PEM Encrypted
		strcpy(encryptedPwd, pwd);
	} else {										//Plain
		if (! PemnEncrypt(encryptedPwd, 126, pwd)){
			env->ThrowNew(env->FindClass("java/lang/Exception"), ENCRYPT_PWD_ERR_MSG);
			env->MonitorExit(obj);
			return NULL;
		}
	}

        PemnSetCommAttributes(0,0,0,PEMN_TCP_TRANSPORT);
	//Step 2.1 Ping the PATROL Agent
        if(strcmp("true", prtcl) == 0)
        {
	const char *result = PemnBPing(host, port, timeout);
	if (strcmp("OK\n", result) == 0 || strcmp("WARN\n", result) == 0 || strcmp("ALARM\n", result) == 0 || 
		strcmp("OFFLINE\n", result) == 0 || strcmp("VOID\n", result) == 0) {
		//log("PATROL Agent is reachable.");
	} else {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PING_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
        }
	//Step 2.2 Open the connection with the PATROL Agent
	char *status = PemnBOpen(& s_hComm, host, port, user, encryptedPwd, 5, DisconnectCallback, NULL);
	if (!s_hComm) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PATROL_DISCONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
	if (strcmp("OK\n", status)!= 0) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), OPEN_CONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}

	//Step 3. Do Work. Get Variable's value
	char *variableValue = PemnBGetVarObj(s_hComm, varName, timeout);
	jstring variableValueStr = env->NewStringUTF(variableValue);

	// Step 4: Close the connection
	if (s_hComm) {
		PemnBClose(s_hComm);
		s_hComm = NULL;
	}

	env->MonitorExit(obj);
	return variableValueStr;
}


JNIEXPORT jstring JNICALL getComputerParamList(JNIEnv *env, jobject obj, jstring hostName, 
						jint port, jstring userName, jstring password, jint timeout, jstring protocol, jint encType) {
	env->MonitorEnter(obj);
	//Get hostName
	const char *hostStr = env->GetStringUTFChars(hostName, 0);
	jsize hostNameSize = env->GetStringUTFLength(hostName);
	char host[ARRAY_SIZE];
	strcpy(host, hostStr);
	env->ReleaseStringUTFChars(hostName, hostStr);

	//Get userName
	const char *userStr = env->GetStringUTFChars(userName, 0);
	jsize userNameSize = env->GetStringUTFLength(userName);
	char user[ARRAY_SIZE];
	strcpy(user, userStr);
	env->ReleaseStringUTFChars(userName, userStr);

	//Get password
	const char *pwdStr = env->GetStringUTFChars(password, 0);
	jsize passwordSize = env->GetStringUTFLength(password);
	char pwd[ARRAY_SIZE];
	strcpy(pwd, pwdStr);
	env->ReleaseStringUTFChars(password, pwdStr);

        //Get Protocol Used
        const char *protocolStr = env->GetStringUTFChars(protocol, 0);
        jsize protocolSize = env->GetStringUTFLength(protocol);
        char prtcl[ARRAY_SIZE];
        strcpy(prtcl, protocolStr);
        env->ReleaseStringUTFChars(protocol, protocolStr);
        //printf("\nProtocol used is (in pemapiwrapper.cpp): %s\n", prtcl);

	//Connnect to PATROL Agent
	//Step 1.Encrypt Password
	char encryptedPwd[126 + 1];
	if (encType == 1) {								//DES Encryted
		PNEncrypt(encryptedPwd, (char*) pwd);
	}
	else if (encType == 2) {						//PEM Encrypted
		strcpy(encryptedPwd, pwd);
	} else {										//Plain
		if (! PemnEncrypt(encryptedPwd, 126, pwd)){
			env->ThrowNew(env->FindClass("java/lang/Exception"), ENCRYPT_PWD_ERR_MSG);
			env->MonitorExit(obj);
			return NULL;
		}
	}

        PemnSetCommAttributes(0,0,0,PEMN_TCP_TRANSPORT);
	//Step 2.1 Ping the PATROL Agent
        if(strcmp("true", prtcl) == 0)
        { 
	const char *result = PemnBPing(host, port, timeout);
	if (strcmp("OK\n", result) == 0 || strcmp("WARN\n", result) == 0 || strcmp("ALARM\n", result) == 0 || 
		strcmp("OFFLINE\n", result) == 0 || strcmp("VOID\n", result) == 0) {
		//log("PATROL Agent is reachable.");
	} else {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PING_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
        }
	//Step 2.2 Open the connection with the PATROL Agent
	char *status = PemnBOpen(& s_hComm, host, port, user, encryptedPwd, 5, DisconnectCallback, NULL);
	if (!s_hComm) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PATROL_DISCONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
	if (strcmp("OK\n", status)!= 0) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), OPEN_CONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}

	//Step 3. Do Work. Get Variable's value
	char *computerParamList = PemnBGetComputerParamList(s_hComm, timeout);
	jstring computerParamListStr = env->NewStringUTF(computerParamList);

	// Step 4: Close the connection
	if (s_hComm) {
		PemnBClose(s_hComm);
		s_hComm = NULL;
	}

	env->MonitorExit(obj);
	return computerParamListStr;
}

JNIEXPORT jstring JNICALL getComputerParamValue(JNIEnv *env, jobject obj, jstring hostName, 
						jint port, jstring userName, jstring password, jstring computerParamName, jint timeout, jstring protocol, jint encType) {
	env->MonitorEnter(obj);
	//Get hostName
	const char *hostStr = env->GetStringUTFChars(hostName, 0);
	jsize hostNameSize = env->GetStringUTFLength(hostName);
	char host[ARRAY_SIZE];
	strcpy(host, hostStr);
	env->ReleaseStringUTFChars(hostName, hostStr);

	//Get userName
	const char *userStr = env->GetStringUTFChars(userName, 0);
	jsize userNameSize = env->GetStringUTFLength(userName);
	char user[ARRAY_SIZE];
	strcpy(user, userStr);
	env->ReleaseStringUTFChars(userName, userStr);

	//Get password
	const char *pwdStr = env->GetStringUTFChars(password, 0);
	jsize passwordSize = env->GetStringUTFLength(password);
	char pwd[ARRAY_SIZE];
	strcpy(pwd, pwdStr);
	env->ReleaseStringUTFChars(password, pwdStr);

	//Get computerParamName
	const char *compParamNameStr = env->GetStringUTFChars(computerParamName, 0);
	jsize computerParamNameSize = env->GetStringUTFLength(computerParamName);
	char compParamName[ARRAY_SIZE];
	strcpy(compParamName, compParamNameStr);
	env->ReleaseStringUTFChars(computerParamName, compParamNameStr);

        //Get Protocol Used
        const char *protocolStr = env->GetStringUTFChars(protocol, 0);
        jsize protocolSize = env->GetStringUTFLength(protocol);
        char prtcl[ARRAY_SIZE];
        strcpy(prtcl, protocolStr);
        env->ReleaseStringUTFChars(protocol, protocolStr);
        //printf("\nProtocol used is (in pemapiwrapper.cpp): %s\n", prtcl);

	//Connnect to PATROL Agent
	//Step 1.Encrypt Password
	char encryptedPwd[126 + 1];
	if (encType == 1) {								//DES Encryted
		PNEncrypt(encryptedPwd, (char*) pwd);
	}
	else if (encType == 2) {						//PEM Encrypted
		strcpy(encryptedPwd, pwd);
	} else {										//Plain
		if (! PemnEncrypt(encryptedPwd, 126, pwd)){
			env->ThrowNew(env->FindClass("java/lang/Exception"), ENCRYPT_PWD_ERR_MSG);
			env->MonitorExit(obj);
			return NULL;
		}
	}

        PemnSetCommAttributes(0,0,0,PEMN_TCP_TRANSPORT);
	//Step 2.1 Ping the PATROL Agent
        if(strcmp("true", prtcl) == 0)
        {
	const char *result = PemnBPing(host, port, timeout);
	if (strcmp("OK\n", result) == 0 || strcmp("WARN\n", result) == 0 || strcmp("ALARM\n", result) == 0 || 
		strcmp("OFFLINE\n", result) == 0 || strcmp("VOID\n", result) == 0) {
		//log("PATROL Agent is reachable.");
	} else {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PING_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
        }
	//Step 2.2 Open the connection with the PATROL Agent
	char *status = PemnBOpen(& s_hComm, host, port, user, encryptedPwd, 5, DisconnectCallback, NULL);
	if (!s_hComm) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PATROL_DISCONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
	if (strcmp("OK\n", status)!= 0) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), OPEN_CONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}

	//Step 3. Do Work. Get Variable's value
	PemnParamObjHandle hParamObj = PemnBGetComputerParamObj(s_hComm, compParamName, timeout);
	PemnParamAttrArray *paramAttrArr = PemnGetParamObjAttributes(hParamObj);
	char *computerParamVal = (*paramAttrArr)[PEMN_PARAMCURRENTVALUE];
	jstring computerParamValStr = env->NewStringUTF(computerParamVal);

	// Step 4: Close the connection
	if (s_hComm) {
		PemnBClose(s_hComm);
		s_hComm = NULL;
	}

	env->MonitorExit(obj);
	return computerParamValStr;
}


JNIEXPORT jstring JNICALL executePSL(JNIEnv *env, jobject obj, jstring hostName, jint port, 
							jstring userName, jstring password, jstring pslScript, jint waitForResults, jint timeout, jstring protocol, jint encType) {
	env->MonitorEnter(obj);
	//Get hostName
	const char *hostStr = env->GetStringUTFChars(hostName, 0);
	jsize hostNameSize = env->GetStringUTFLength(hostName);
	char host[ARRAY_SIZE];
	strcpy(host, hostStr);
	env->ReleaseStringUTFChars(hostName, hostStr);

	//Get userName
	const char *userStr = env->GetStringUTFChars(userName, 0);
	jsize userNameSize = env->GetStringUTFLength(userName);
	char user[ARRAY_SIZE];
	strcpy(user, userStr);
	env->ReleaseStringUTFChars(userName, userStr);

	//Get password
	const char *pwdStr = env->GetStringUTFChars(password, 0);
	jsize passwordSize = env->GetStringUTFLength(password);
	char pwd[ARRAY_SIZE];
	strcpy(pwd, pwdStr);
	env->ReleaseStringUTFChars(password, pwdStr);

	//Get pslScript
	const char *pslStr = env->GetStringUTFChars(pslScript, 0);
	char psl[ARRAY_SIZE];
	strcpy(psl, pslStr);
	env->ReleaseStringUTFChars(pslScript, pslStr);

        //Get Protocol Used
        const char *protocolStr = env->GetStringUTFChars(protocol, 0);
        jsize protocolSize = env->GetStringUTFLength(protocol);
        char prtcl[ARRAY_SIZE];
        strcpy(prtcl, protocolStr);
        env->ReleaseStringUTFChars(protocol, protocolStr);
        //printf("\nProtocol used is (in pemapiwrapper.cpp): %s\n", prtcl);

	//Connnect to PATROL Agent
	//Step 1.Encrypt Password
	char encryptedPwd[126 + 1];
	if (encType == 1) {								//DES Encryted
		PNEncrypt(encryptedPwd, (char*) pwd);
	}
	else if (encType == 2) {						//PEM Encrypted
		strcpy(encryptedPwd, pwd);
	} else {										//Plain
		if (! PemnEncrypt(encryptedPwd, 126, pwd)){
			env->ThrowNew(env->FindClass("java/lang/Exception"), ENCRYPT_PWD_ERR_MSG);
			env->MonitorExit(obj);
			return NULL;
		}
	}

    PemnSetCommAttributes(0,0,0,PEMN_TCP_TRANSPORT);	
	//Step 2.1 Ping the PATROL Agent
        if(strcmp("true", prtcl) == 0)
        {
	const char *result = PemnBPing(host, port, timeout);
	if (strcmp("OK\n", result) == 0 || strcmp("WARN\n", result) == 0 || strcmp("ALARM\n", result) == 0 || 
		strcmp("OFFLINE\n", result) == 0 || strcmp("VOID\n", result) == 0) {
		//log("PATROL Agent is reachable.");
	} else {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PING_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
        }
	//Step 2.2.Open the connection with the PATROL Agent
	char *status = PemnBOpen(& s_hComm, host, port, user, encryptedPwd, 5, DisconnectCallback, NULL);
	if (!s_hComm) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PATROL_DISCONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
	if (strcmp("OK\n", status)!= 0) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), OPEN_CONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}

	//Step 3. Do Work. Get Application List
	char *pslExecResult = PemnBPslExec(s_hComm, timeout, waitForResults, psl);
	jstring pslExecResultStr = env->NewStringUTF(pslExecResult);

	// Step 4: Close the connection
	if (s_hComm) {
		PemnBClose(s_hComm);
		s_hComm = NULL;
	}

	env->MonitorExit(obj);
	return pslExecResultStr;
}

JNIEXPORT jstring JNICALL isConnectionValid(JNIEnv *env, jobject obj, jstring hostName, jint port, 
										   jstring userName, jstring password, jint timeout, jstring protocol, jint encType) { 
										   
	
	
	env->MonitorEnter(obj);
	//Get hostName
	const char *hostStr = env->GetStringUTFChars(hostName, 0);
	jsize hostNameSize = env->GetStringUTFLength(hostName);
	char host[ARRAY_SIZE];
	strcpy(host, hostStr);
	env->ReleaseStringUTFChars(hostName, hostStr);

	//Get userName
	const char *userStr = env->GetStringUTFChars(userName, 0);
	jsize userNameSize = env->GetStringUTFLength(userName);
	char user[ARRAY_SIZE];
	strcpy(user, userStr);
	env->ReleaseStringUTFChars(userName, userStr);

	//Get password
	const char *pwdStr = env->GetStringUTFChars(password, 0);
	jsize passwordSize = env->GetStringUTFLength(password);
	char pwd[ARRAY_SIZE];
	strcpy(pwd, pwdStr);
	env->ReleaseStringUTFChars(password, pwdStr);

	
    //Get Protocol Used
    const char *protocolStr = env->GetStringUTFChars(protocol, 0);
    jsize protocolSize = env->GetStringUTFLength(protocol);
    char prtcl[ARRAY_SIZE];
    strcpy(prtcl, protocolStr);
    env->ReleaseStringUTFChars(protocol, protocolStr);
    //printf("\nProtocol used is (in pemapiwrapper.cpp): %s\n", prtcl);

	//Connnect to PATROL Agent
	//Step 1.Encrypt Password
	char encryptedPwd[126 + 1];
	if (encType == 1) {								//DES Encryted
		PNEncrypt(encryptedPwd, (char*) pwd);
	}
	else if (encType == 2) {						//PEM Encrypted
		strcpy(encryptedPwd, pwd);
	} else {										//Plain
		if (! PemnEncrypt(encryptedPwd, 126, pwd)){
			env->ThrowNew(env->FindClass("java/lang/Exception"), ENCRYPT_PWD_ERR_MSG);
			env->MonitorExit(obj);
			return NULL;
		}
	}

        PemnSetCommAttributes(0,0,0,PEMN_TCP_TRANSPORT);
	//Step 2.1 Ping the PATROL Agent
        if(strcmp("true", prtcl) == 0)
        {
	const char *result = PemnBPing(host, port, timeout);
	if (strcmp("OK\n", result) == 0 || strcmp("WARN\n", result) == 0 || strcmp("ALARM\n", result) == 0 || 
		strcmp("OFFLINE\n", result) == 0 || strcmp("VOID\n", result) == 0) {
		//log("PATROL Agent is reachable.");
	} else {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PING_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	} 
        }
	//Step 2.2 Open the connection with the PATROL Agent
	char *status = PemnBOpen(& s_hComm, host, port, user, encryptedPwd, 5, DisconnectCallback, NULL);
	if (strcmp("OK\n", status)!= 0) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), OPEN_CONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
	if (!s_hComm) {
		env->ThrowNew(env->FindClass("java/lang/Exception"), PATROL_DISCONN_ERR_MSG);
		env->MonitorExit(obj);
		return NULL;
	}
	
	
	// Step 4: Close the connection
	if (s_hComm) {
		PemnBClose(s_hComm);
		s_hComm = NULL;
	}

	env->MonitorExit(obj);
	return NULL;
	
	}
	
/*********************************/
void PNEncrypt(char *pabDst,const char *pabSrc) {
	int f =  ( (rand() % 9) + 1); 
	int i, l = strlen(pabSrc), d;
	if (l == 0){
		strcpy(pabDst,"");
        return;
	}
   
	if (f < 5) 
		f = 5;
   
	if (f > l) 
		f = l;

	for (i = 0; i < f; i++){
		#ifdef OS400 
			pabDst[2*i]   = (char) ( (rand() % 9) + (int)'A');
		#else
			pabDst[2*i]   = (char) ( (rand() % 26) + (int)'A');
		#endif
		pabDst[2*i + 1] = pabSrc[i];
	}

	d = l - f;

    for (i=0; i < d; i++)
		pabDst[2*f + i] = pabSrc[f + i];
	
	pabDst[f + l]   = (char) (f + (int) '0');
    pabDst[f + l + 1] = (int) 0;
}
/*********************************/


//******************************************************************************************************************	

JNIEXPORT void JNICALL Java_com_proactivenet_monitors_BMCMonBase_PemApiWrapper_initLibrary
						(JNIEnv *env, jobject obj, jobject logger) {
	initLibrary(env, obj, logger);
}


JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_BMCMonBase_PemApiWrapper_ping
						(JNIEnv *env, jobject obj, jstring hostName, jint port, jint timeout, jstring protocol) {
	return ping(env, obj, hostName, port, timeout, protocol);
}


JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_BMCMonBase_PemApiWrapper_getApplicationList
							(JNIEnv *env, jobject obj, jstring hostName, jint port, jstring userName, jstring password, jint timeout, jstring protocol, jint encType) {
	return getApplicationList(env, obj, hostName, port, userName, password, timeout, protocol, encType);
}


JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_BMCMonBase_PemApiWrapper_getInstanceList
							(JNIEnv *env, jobject obj, jstring hostName, jint port, jstring userName, jstring password, jstring application, jint timeout, jstring protocol, jint encType) {
	return getInstanceList(env, obj, hostName, port, userName, password, application, timeout, protocol, encType);
}


JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_BMCMonBase_PemApiWrapper_getParametersList
							(JNIEnv *env, jobject obj, jstring hostName, jint port, jstring userName, jstring password, jstring application, jstring instance, jint timeout, jstring protocol, jint encType) {
	return getParametersList(env, obj, hostName, port, userName, password, application, instance, timeout, protocol, encType);
}


JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_BMCMonBase_PemApiWrapper_getParametersValueArray
								(JNIEnv *env, jobject obj, jstring hostName, jint port, jstring userName, jstring password, jobjectArray paramObjArr, jint timeout, jstring protocol, jint encType) {
	return getParametersValueArray(env, obj, hostName, port, userName, password, paramObjArr, timeout, protocol, encType);
}

JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_BMCMonBase_PemApiWrapper_getVariablesList
								(JNIEnv *env, jobject obj, jstring hostName, jint port, jstring userName, jstring password, jstring varListObjName, jstring keyword, jint timeout, jstring protocol, jint encType) {
	return getVariablesList(env, obj, hostName, port, userName, password, varListObjName, keyword, timeout, protocol, encType);

}

JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_BMCMonBase_PemApiWrapper_getVariableValue
		(JNIEnv *env, jobject obj, jstring hostName, jint port, jstring userName, jstring password, jstring variableName, jint timeout, jstring protocol, jint encType) {
	return getVariableValue(env, obj, hostName, port, userName, password, variableName, timeout, protocol, encType);
}

JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_BMCMonBase_PemApiWrapper_getComputerParamList
		(JNIEnv *env, jobject obj, jstring hostName, jint port, jstring userName, jstring password, jint timeout, jstring protocol, jint encType) {
	return getComputerParamList(env, obj, hostName, port, userName, password, timeout, protocol, encType);
}

JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_BMCMonBase_PemApiWrapper_getComputerParamValue
		(JNIEnv *env, jobject obj, jstring hostName, jint port, jstring userName, jstring password, jstring computerParamName, jint timeout, jstring protocol, jint encType) {
	return getComputerParamValue(env, obj, hostName, port, userName, password, computerParamName, timeout, protocol, encType);
}

JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_BMCMonBase_PemApiWrapper_executePSL
		(JNIEnv *env, jobject obj, jstring hostName, jint port, jstring userName, jstring password, jstring pslScript, jint waitForResults, jint timeout, jstring protocol, jint encType) {
	return executePSL(env, obj, hostName, port, userName, password, pslScript, waitForResults, timeout, protocol, encType);
}
JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_BMCMonBase_PemApiWrapper_isConnectionValid
		(JNIEnv *env, jobject obj, jstring hostName, jint port, jstring userName, jstring password, jint timeout, jstring protocol, jint encType) {
	 return isConnectionValid(env, obj, hostName, port, userName, password, timeout, protocol, encType);
}


