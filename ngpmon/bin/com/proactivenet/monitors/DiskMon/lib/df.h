#ifndef _DF_CC_DF_CC_
#define _DF_CC_DF_CC_
#include <errno.h>
#include <iostream.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
# include <sys/statfs.h>
# include <sys/statvfs.h>
#include <rw/ctoken.h>
#include <rw/cstring.h>
#include "DiskMon.h"
//algo 
//if chdir returns error = 2 // if is  invalid file no need to further process
// if chdir return 20 remove file name and proceed
// if chdir 13 then proceed to statfs if it returns 13 then do not proceed
int fileStatus(RWCString name,JNIEnv * env, jobject obj) ;
void resolvePath(int osType, RWCString filename, char * resolved,JNIEnv * env, jobject obj );
/*
case 1; Total Disk Size in MB
case 2; Total Disk INODED
case 3; % Disk 
case 4; % INODE
*/
float statFileSystem(const char * fileName, int typ,JNIEnv * env,jobject obj) ; 

#endif 
