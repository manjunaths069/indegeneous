// NetFlowListenerJNI.cpp : Defines the exported functions for the DLL application.
//

//uncomment to compile on windows from Visual Studio
//#include "stdafx.h"

#include <NetFlowListenerJNI.h>
/*
 * Class:     com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI
 * Method:    getAllDeviceCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI_getAllDeviceCount
(JNIEnv * env, jobject obj){


   jint result = 0;
   jint deviceCount = 100;

   printf("getAllDeviceCount allDeviceCount -->%d\n", deviceCount);	

   return result;

}

/*
 * Class:     com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI
 * Method:    enableDevice
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI_enableDevice__Ljava_lang_String_2
(JNIEnv * env, jobject obj, jstring name){

	jint result = 0;

	char buf[128];
	int len = env->GetStringLength(name);
	env->GetStringUTFRegion(name, 0, len, buf);
	printf("enableDevice string -->%s\n", buf);

	return result;

}



/*
 * Class:     com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI
 * Method:    getAllDevices
 * Signature: (Lcom/proactivenet/monitors/BMCNetworkFlowAdapter/NetFlowListenerJNI;)I
 */

/*
types defined in:
   
   jni.h

	typedef unsigned char	jboolean;
	typedef unsigned short	jchar;
	typedef short			jshort;
	typedef float			jfloat;
	typedef double			jdouble;


	jni_md.h

	typedef long jint;
	typedef __int64 jlong;
	typedef signed char jbyte;

*/
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI_getAllDevices
(JNIEnv * env, jobject obj, jobject arg){

		jint srcaddr	=	1;			
		jint dstaddr	=	2;			
		jint nexthop	=	3;			
		jshort input	=	4;			
		jshort output	=	5;		
		jint dPkts		=	6;			
		jint dOctets	=	7;		
		jint First		=	8;			
		jint Last		=	9;			
		jshort srcport	=	10;		
		jshort dstport  =   11;		
		jbyte pad       =   12;			
		jbyte tcp_flags	=	13;		
		jbyte prot		=	14;          
		jbyte tos		=	15;		    
		jshort src_as	=	16;   	    
		jshort dst_as	=	17;   		
		jbyte src_mask	=	18;       
		jbyte dst_mask	=	19;       
		jshort drops	=	20;         
	

	    

	    jint return_value = 0;
		
		jclass	netFlow5Record_clazz;
		jobject netFlow5Record_object;
	


		

		printf("Getting jclass...");
		jclass clazz = env->GetObjectClass(arg);
		printf("done\n");


		printf("Getting array field id...");
		jfieldID arrayFieldId = env->GetFieldID(clazz, "netFlow5Records", "[Lcom/proactivenet/monitors/BMCNetworkFlowAdapter/NetFlow5Record;");
		printf("done\n");



		printf("Getting reference to the array field...");
		jobjectArray arrayField = (jobjectArray)env->GetObjectField(obj, arrayFieldId);
		printf("done\n");


		printf("Getting the first array element...");
		jobject first = env->GetObjectArrayElement(arrayField, 0);
		printf("done\n");



		netFlow5Record_clazz  = env->FindClass("Lcom/proactivenet/monitors/BMCNetworkFlowAdapter/NetFlow5Record;");

		netFlow5Record_object =  env->NewObject(netFlow5Record_clazz, env->GetMethodID(netFlow5Record_clazz, "<init>", "()V"));


		if (netFlow5Record_clazz == NULL) {
			printf("netFlow5Record == NULL");
		}else{
			printf("construct succesfull netFlow5Record");
		}


	/*
		public int srcaddr;			Signature: I
		public int dstaddr;			Signature: I
		public int nexthop;			Signature: I
		public short input;			Signature: S
		public short output;		Signature: S
		public int dPkts;			Signature: I
		public int dOctets;			Signature: I
		public int First;			Signature: I
		public int Last;			Signature: I
		public short srcport;		Signature: S
		public short dstport;		Signature: S
		public byte pad;			Signature: B
		public byte tcp_flags;		Signature: B
		public byte prot;           Signature: B
		public byte tos;		    Signature: B
		public short src_as;   	    Signature: S
		public short dst_as;   		Signature: S
		public byte src_mask;       Signature: B
		public byte dst_mask;       Signature: B
		public short drops;         Signature: S
		*/




		//TODO need to add all variables

		jfieldID arrayFieldId_srcaddr		= env->GetFieldID(netFlow5Record_clazz, "srcaddr", "I");
		jfieldID arrayFieldId_dstaddr		= env->GetFieldID(netFlow5Record_clazz, "dstaddr", "I");
		jfieldID arrayFieldId_nexthop		= env->GetFieldID(netFlow5Record_clazz, "nexthop", "I");
		jfieldID arrayFieldId_input			= env->GetFieldID(netFlow5Record_clazz, "input", "S");
		jfieldID arrayFieldId_output		= env->GetFieldID(netFlow5Record_clazz, "output", "S");
		jfieldID arrayFieldId_dPkts			= env->GetFieldID(netFlow5Record_clazz, "dPkts", "I");
		jfieldID arrayFieldId_dOctets		= env->GetFieldID(netFlow5Record_clazz, "dOctets", "I");
		jfieldID arrayFieldId_First			= env->GetFieldID(netFlow5Record_clazz, "First", "I");
		jfieldID arrayFieldId_Last			= env->GetFieldID(netFlow5Record_clazz, "Last", "I");
		jfieldID arrayFieldId_srcport		= env->GetFieldID(netFlow5Record_clazz, "srcport", "S");
		jfieldID arrayFieldId_dstport		= env->GetFieldID(netFlow5Record_clazz, "dstport", "S");
		jfieldID arrayFieldId_pad			= env->GetFieldID(netFlow5Record_clazz, "pad", "B");
		jfieldID arrayFieldId_tcp_flags		= env->GetFieldID(netFlow5Record_clazz, "tcp_flags", "B");
		jfieldID arrayFieldId_prot			= env->GetFieldID(netFlow5Record_clazz, "prot", "B");
		jfieldID arrayFieldId_tos			= env->GetFieldID(netFlow5Record_clazz, "tos", "B");
		jfieldID arrayFieldId_src_as		= env->GetFieldID(netFlow5Record_clazz, "src_as", "S");
		jfieldID arrayFieldId_dst_as		= env->GetFieldID(netFlow5Record_clazz, "dst_as", "S");
		jfieldID arrayFieldId_src_mask		= env->GetFieldID(netFlow5Record_clazz, "src_mask", "B");
		jfieldID arrayFieldId_dst_mask		= env->GetFieldID(netFlow5Record_clazz, "dst_mask", "B");	
		jfieldID arrayFieldId_drops			= env->GetFieldID(netFlow5Record_clazz, "drops", "S");



		//TODO need to add all variables and initials values
		env->SetIntField(netFlow5Record_object, arrayFieldId_srcaddr, 1);
		env->SetIntField(netFlow5Record_object, arrayFieldId_dstaddr, 2);
		env->SetIntField(netFlow5Record_object, arrayFieldId_nexthop, 3);
		env->SetIntField(netFlow5Record_object, arrayFieldId_input, 4);
		env->SetIntField(netFlow5Record_object, arrayFieldId_output, 5);
		env->SetIntField(netFlow5Record_object, arrayFieldId_dPkts, 6);
		env->SetIntField(netFlow5Record_object, arrayFieldId_dOctets, 7);
		env->SetIntField(netFlow5Record_object, arrayFieldId_First, 8);
		env->SetIntField(netFlow5Record_object, arrayFieldId_Last, 9);
		env->SetIntField(netFlow5Record_object, arrayFieldId_srcport, 10);
		env->SetIntField(netFlow5Record_object, arrayFieldId_dstport, 11);
		env->SetIntField(netFlow5Record_object, arrayFieldId_pad, 'a');
		env->SetIntField(netFlow5Record_object, arrayFieldId_tcp_flags, 'b');
		env->SetIntField(netFlow5Record_object, arrayFieldId_prot, 'c');
		env->SetIntField(netFlow5Record_object, arrayFieldId_tos, 'd');
		env->SetIntField(netFlow5Record_object, arrayFieldId_src_as, 12);
		env->SetIntField(netFlow5Record_object, arrayFieldId_dst_as, 13);
		env->SetIntField(netFlow5Record_object, arrayFieldId_src_mask, 'e');
		env->SetIntField(netFlow5Record_object, arrayFieldId_dst_mask, 'f');
		env->SetIntField(netFlow5Record_object, arrayFieldId_drops, 14);



		env->SetObjectArrayElement(arrayField, 0, netFlow5Record_object);


		//env->GetObjectField(first ,
/*
		printf("Getting the srcaddr field of  netflow5Record...");
		jfieldID srcaddr = env->GetIntField(first, "srcaddr", "I");
*/

        
/*
		printf("Setting first element int field to 23...");
		env->SetIntField(first, srcaddr, 23);
		printf("done\n");
*/
		return 0;

}



/*
 * Class:     com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI
 * Method:    enableDevice
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI_enableDevice__J
(JNIEnv * env, jobject obj, jlong number){

	   jint result = 0;


	printf("enableDevice long -->%d\n", number);


	   return result;

}

/*
 * Class:     com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI
 * Method:    disableDevice
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI_disableDevice__Ljava_lang_String_2
(JNIEnv * env, jobject obj, jstring name){

		  jint result = 0;


		char buf[128];
		int len = env->GetStringLength(name);
		env->GetStringUTFRegion(name, 0, len, buf);
		printf("disableDevice string -->%s\n", buf);

		return result;


}

/*
 * Class:     com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI
 * Method:    disableDevice
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI_disableDevice__J
(JNIEnv * env, jobject obj, jlong number){

		   jint result = 0;

		   printf("disableDevice long -->%d\n", number);

		   return result;

}

/*
 * Class:     com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI
 * Method:    getMaxQueueSize
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI_getMaxQueueSize__Ljava_lang_String_2
(JNIEnv * env , jobject obj, jstring name){

		jint result = 0;
		jint maxQueueSize=0;
			
		char buf[128];
		int len = env->GetStringLength(name);
		env->GetStringUTFRegion(name, 0, len, buf);
		printf("getMaxQueueSize queueName -->%s\n", buf);	

		maxQueueSize = 15;

		return maxQueueSize;

}

/*
 * Class:     com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI
 * Method:    getMaxQueueSize
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI_getMaxQueueSize__J
(JNIEnv * env, jobject obj, jlong number){
			
			jint result = 0;
			jint queueNumber = 0;

			// for test purpose

			queueNumber = 10;

			return queueNumber;

}

/*
 * Class:     com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI
 * Method:    setMaxQueueSize
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI_setMaxQueueSize__Ljava_lang_String_2
(JNIEnv * env, jobject obj, jstring name){

		jint result = 0;
		jint maxQueueSize = 0;
		/* assume the prompt string and user input has less than 128 characters */

		char buf[128];
		int len = env->GetStringLength(name);
		env->GetStringUTFRegion(name, 0, len, buf);
		printf("setMaxQueueSize queueName -->%s\n", buf);

		//place where we set 
		maxQueueSize = 5;
		printf("setMaxQueueSize queueName -->%d\n", maxQueueSize);
		
		return result;

}

/*
 * Class:     com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI
 * Method:    setMaxQueueSize
 * Signature: (J)I
 */


JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_BMCNetworkFlowAdapter_NetFlowListenerJNI_setMaxQueueSize__J
(JNIEnv * env , jobject obj , jlong number){


		

			jint result = 0;

	printf("setMaxQueueSize queueName -->%d\n", number);

			return result;

}

