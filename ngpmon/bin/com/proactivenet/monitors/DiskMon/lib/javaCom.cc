#include "DiskMon.h"
#include "df.h"
#include <thread.h>
static mutex_t  itrMutex;
//case 1; Total Disk Size in MB
//case 2; Total Disk INODED
//case 3; % Disk
//case 4; % INODE
//case 5; fileStatus 
JNIEXPORT jfloat JNICALL Java_com_proactivenet_monitors_DiskMon_DiskMon_get
  (JNIEnv * env, jobject obj, jint typ, jstring fileDirName)
{
	mutex_lock(&itrMutex);
	const char *str = env->GetStringUTFChars(fileDirName, 0); //str is path name
	char p_name[1024];
	strcpy(p_name, str);
	env->ReleaseStringUTFChars(fileDirName, str);
	float val = -1 ;
	switch(typ)
	{
		case 1:
			val =   statFileSystem(p_name,1,env,obj) ;
			break ;
		case 2:
			val =   statFileSystem(p_name,2,env,obj) ;
			break ;
		case 3:
			val =   statFileSystem(p_name,3,env,obj) ;
			break ;
		case 4:
			val =   statFileSystem(p_name,4,env,obj) ;
			break ;
		case 5:
			val =   fileStatus(p_name,env,obj) ;
			break ;
		case 6:		//Added by Ashish to get the Free Space value in MB, 13th Oct, 2K6
			val =   statFileSystem(p_name,6,env,obj) ;
			break ;		
		default :
			val = -1 ;
			break ;
	}
	mutex_unlock(&itrMutex);
	return val;
}
//resolve is only ostype call
JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_DiskMon_DiskMon_resolve
  (JNIEnv * env, jobject obj, jint osType, jstring fileDirName)
{
	mutex_lock(&itrMutex);
	const char *str = env->GetStringUTFChars(fileDirName, 0); //str is path name
	char p_name[1024];
	strcpy(p_name, str);
	env->ReleaseStringUTFChars(fileDirName, str);
	char reslved[1024] ;
	resolvePath(osType,p_name,reslved,env,obj);
	mutex_unlock(&itrMutex);
	return env->NewStringUTF(reslved);
}
