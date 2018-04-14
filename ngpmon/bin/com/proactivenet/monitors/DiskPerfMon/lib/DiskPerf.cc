#include <unistd.h>
#include <sys/sysinfo.h>
#include <kstat.h>
#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/dkio.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include "DiskPerfInfo.h"
#include <strings.h>
#include <errno.h>


# define  SUCCESS          0
# define  FAILURE         -1
# define  DI_INIT_FAILURE -2
# define  NULL_ERROR      -3


/* This method is used to match the device path */
int read_dir(char *disk_path, char *value,JNIEnv *,jobject);
int getDiskPath(char *disk_name, char *path,JNIEnv *,jobject);

static jmethodID updateGetPath = NULL;

/* --------------------- Changes for JNI logging START------------------------------*/

void logDbg(JNIEnv *env, jobject obj, char *logStr)
{
        if(logStr == NULL)
                return;
        char mlogStr[1280];
        strcpy(mlogStr,logStr);
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL)
        {
              mid = env->GetMethodID(cls,"logDbg","(Ljava/lang/String;)V");
              if(mid != NULL)
              {
                      jstring tString = env->NewStringUTF(mlogStr);
                      env->CallVoidMethod(obj,mid,tString);
                      env->DeleteLocalRef(tString);
              }
        }
}

void logErr(JNIEnv *env, jobject obj, char *errStr)
{
        if(errStr == NULL)
                return;
        char merrStr[1280];
        strcpy(merrStr,errStr);
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL)
        {
              mid = env->GetMethodID(cls,"logErr","(Ljava/lang/String;)V");
              if(mid != NULL)
              {
                      jstring tString = env->NewStringUTF(merrStr);
                      env->CallVoidMethod(obj,mid,tString);
                      env->DeleteLocalRef(tString);
              }
        }
}

/* --------------------- Changes for JNI logging END------------------------------*/


JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_DiskPerfMon_DiskPerfInfo_validateDiskName (JNIEnv *env, jobject obj, jstring jdiskname)
{
    kstat_ctl_t    *kc;
    kstat_t        *ksp;
    char mLogString[1280];

    /* ---------------------- JNI Logging Code START-----------------------------*/

    sprintf(mLogString,"Native Logging: Function validateDiskName: Start");
    logDbg(env,obj,mLogString);

    /* ---------------------- JNI Logging Code END -----------------------------*/



    kc = kstat_open();
    if (kc == NULL )
        return -1;
    /* Get the disk name */
     char *diskname = (char*)env->GetStringUTFChars(jdiskname, 0);
    //printf("%s",diskname);
    for (ksp = kc->kc_chain; ksp != NULL; ksp = ksp->ks_next) {
        if (ksp->ks_type == KSTAT_TYPE_IO && !(strcmp(ksp->ks_class ,"disk")) &&!(strcmp(ksp->ks_name,diskname)))
                {
                        kstat_close(kc);
                        kc = NULL;
                        env->ReleaseStringUTFChars(jdiskname, diskname);
                        /* ---------------------- JNI Logging Code START-----------------------------*/

                       sprintf(mLogString,"Native Logging: Function validateDiskName: Disk Name found: %s. Function validateDiskName: END",diskname);
                       logDbg(env,obj,mLogString);

                        /* ---------------------- JNI Logging Code END -----------------------------*/



                        return 1;
                }
        }
        env->ReleaseStringUTFChars(jdiskname, diskname);
        /* ---------------------- JNI Logging Code START-----------------------------*/

        sprintf(mLogString,"Native Logging: Function validateDiskName: Disk Name NOT found: %s. Function validateDiskName: END",diskname);
        logErr(env,obj,mLogString);

        /* ---------------------- JNI Logging Code END -----------------------------*/



        return -1; // If the Disk Name is not found in the structure
}


JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_DiskPerfMon_DiskPerfInfo_getPerfData (JNIEnv *env, jobject obj, jstring jdiskname)
{
    kstat_ctl_t    *kc;
    kstat_t        *ksp;
    kstat_io_t     kio;

     jfieldID idBytesRead,
              idBytesWritten,
              idReads,
              idWrites,
              idwTime,
              idwlenTime,
              idwlastUpdate,
              idrTime,
              idrlenTime,
              idrlastUpdate,
              idwCnt,
              idrCnt,
              idrecordTime;

    /* ---------------------- JNI Logging Code START-----------------------------*/

    char mLogString[1280];
    sprintf(mLogString,"Native Logging: Function getPerfData: Start");
    logDbg(env,obj,mLogString);

    /* ---------------------- JNI Logging Code END-----------------------------*/


        /* Get the Class of the object passed*/

              jclass cls = env->GetObjectClass(obj);

              //printf("Got the Class Object\n");

        // Get the Field ids of the member Variables of the Java Object
              idBytesRead = env->GetFieldID(cls, "numBytesRead","J");
              idBytesWritten = env->GetFieldID(cls, "numBytesWritten","J");
              idReads = env->GetFieldID(cls, "numReads","J");
              idWrites = env->GetFieldID(cls, "numWrites","J");
              idwTime = env->GetFieldID(cls, "wTime","J");
              idwlenTime = env->GetFieldID(cls, "wlenTime","J");
              idwlastUpdate = env->GetFieldID(cls, "wlastUpdate","J");
              idrTime = env->GetFieldID(cls, "rTime","J");
              idrlenTime = env->GetFieldID(cls, "rlenTime","J");
              idrlastUpdate = env->GetFieldID(cls, "rlastUpdate","J");
              idwCnt = env->GetFieldID(cls, "wCnt","J");
              idrCnt = env->GetFieldID(cls, "rCnt","J");
              idrecordTime = env->GetFieldID(cls, "recordTime","J");



            kc = kstat_open();
            if (kc == NULL )
            {
                /* ---------------------- JNI Logging Code START-----------------------------*/

                sprintf(mLogString,"Native Logging: Function getPerfData: kstat_open fail Error code %d. Function getPerfData: END",errno);
                logErr(env,obj,mLogString);

                /* ---------------------- JNI Logging Code END-----------------------------*/

                return -1;
            }

        /* Get the disk name */
         char *diskname = (char*)env->GetStringUTFChars(jdiskname, 0);
         //printf("DiskName =  %s\n" , diskname);

            for (ksp = kc->kc_chain; ksp != NULL; ksp = ksp->ks_next) {
                      if ((ksp->ks_type == KSTAT_TYPE_IO)  && !(strcmp(ksp->ks_class,"disk")) && !(strcmp(ksp->ks_name,diskname)))
                        {
                            //printf("Found the disk\n");
                            kstat_read(kc, ksp, &kio);
                            //printf("Read ths stat structure\n");

                             //Set the Java object member variables with the values from this structure
                             env->SetLongField(obj,idBytesRead,kio.nread);
                             env->SetLongField(obj,idBytesWritten,kio.nwritten);
                             env->SetLongField(obj,idReads,kio.reads);
                             env->SetLongField(obj,idWrites,kio.writes);
                             env->SetLongField(obj,idwTime,kio.wtime);
                             env->SetLongField(obj,idwlenTime,kio.wlentime);
                             env->SetLongField(obj,idwlastUpdate,kio.wlastupdate);
                             env->SetLongField(obj,idrTime,kio.rtime);
                             env->SetLongField(obj,idrlenTime,kio.rlentime);
                             env->SetLongField(obj,idrlastUpdate,kio.rlastupdate);
                             env->SetLongField(obj,idwCnt,kio.wcnt);
                             env->SetLongField(obj,idrCnt,kio.rcnt);
                             env->SetLongField(obj,idrecordTime,ksp->ks_snaptime);
                             kstat_close(kc);
                             kc=NULL;
                             env->ReleaseStringUTFChars(jdiskname, diskname);
                             /* ---------------------- JNI Logging Code START-----------------------------*/

                             sprintf(mLogString,"Native Logging: Function getPerfData: Stats data collected.Function getPerfData: END",diskname);
                             logDbg(env,obj,mLogString);

                             /* ---------------------- JNI Logging Code END-----------------------------*/

                             return 1;
                        }
                }
             //printf("Could Not Locate DiskName = %s \n" , diskname);
            // Close the kstat structure and return -1 to indicate that the Disk Name could not be found in the structure
             kstat_close(kc);
             kc=NULL;
             env->ReleaseStringUTFChars(jdiskname, diskname);
             /* ---------------------- JNI Logging Code START-----------------------------*/

             sprintf(mLogString,"Native Logging: Function getPerfData: Could not locate disk %s.Function getPerfData: END",diskname);
             logErr(env,obj,mLogString);

             /* ---------------------- JNI Logging Code END-----------------------------*/

             return -1;

}
/* List disk names from kstat with ks_type == KSTAT_TYPE_IO and ks_class ,"disk" */

JNIEXPORT jobjectArray JNICALL Java_DiskPerfInfo_getDiskNames (JNIEnv *jenv, jobject jobj )
{

    kstat_ctl_t    *kc;
    kstat_t        *ksp;
    char disknames[50];
    int i=0;

    /* ---------------------- JNI Logging Code START-----------------------------*/

    char mLogString[1280];
    sprintf(mLogString,"Native Logging: Function getDiskNames: Start");
    logDbg(jenv,jobj,mLogString);

    /* ---------------------- JNI Logging Code END-----------------------------*/

    kc = kstat_open();
   // if (kc == NULL )
//      return null;
    for (ksp = kc->kc_chain; ksp != NULL; ksp = ksp->ks_next) {
        if (ksp->ks_type == KSTAT_TYPE_IO && !(strcmp(ksp->ks_class ,"disk") ))
                {
                        i++;
                }
        }
        jobjectArray ret = jenv->NewObjectArray(i,jenv->FindClass("java/lang/String"),jenv->NewStringUTF(""));
        i=0;

       for (ksp = kc->kc_chain; ksp != NULL; ksp = ksp->ks_next) {
                if (ksp->ks_type == KSTAT_TYPE_IO && !(strcmp(ksp->ks_class ,"disk") ))
                {
                        //disknames=ksp->ks_name;
                        strcpy(disknames,ksp->ks_name);
                        jenv->SetObjectArrayElement(ret, i, jenv->NewStringUTF(disknames));
                        i++;
                            /* ---------------------- JNI Logging Code START-----------------------------*/

                            sprintf(mLogString,"Native Logging: Function getDiskNames: Found Disk Name  %s. Function getDiskNames: END",disknames);
                            logDbg(jenv,jobj,mLogString);

                            /* ---------------------- JNI Logging Code END-----------------------------*/

                        if (jenv->ExceptionOccurred()){
                                kstat_close(kc);
                            /* ---------------------- JNI Logging Code START-----------------------------*/

                            sprintf(mLogString,"Native Logging: Function getDiskNames: Exception occured in finding Disk Names.Error code %d. Returning NULL. Function getDiskNames: END",errno);
                            logErr(jenv,jobj,mLogString);

                            /* ---------------------- JNI Logging Code END-----------------------------*/

                                return NULL;
                        }
                }
        }
    /* ---------------------- JNI Logging Code START-----------------------------*/

    sprintf(mLogString,"Native Logging:Found %d Disks. Function getDiskNames: END",i);
    logDbg(jenv,jobj,mLogString);

    /* ---------------------- JNI Logging Code END-----------------------------*/

        kstat_close(kc);
        return ret;
}
JNIEXPORT jobjectArray JNICALL Java_com_proactivenet_monitors_DiskPerfMon_DiskPerfInfo_getDiskNames (JNIEnv *jenv, jobject jobj )
{
        return Java_DiskPerfInfo_getDiskNames(jenv,jobj);
}

        //This method is used to check the availability of the disk.
        //This method tries to open the block device file under /dev/dsk,
        //if the disk is available, then open is successful.
        //If the open is a failure, then we check to see if the error is an IO error
        //and return -1 else we return 0.

JNIEXPORT jint JNICALL Java_com_proactivenet_monitors_DiskPerfMon_DiskPerfInfo_checkAvailability(JNIEnv *env, jobject obj, jstring path)
{
        int file_fd = -100;
        char *disk_path = (char *)env->GetStringUTFChars(path, NULL);
        char tmpnode[MAXPATHLEN];
        char tmpdsk[MAXPATHLEN];

        char mLogString[MAXPATHLEN + 255];
        bzero(tmpdsk,MAXPATHLEN);
        bzero(tmpnode,MAXPATHLEN);
        /* ---------------------- JNI Logging Code START-----------------------------*/

        sprintf(mLogString,"Native Logging: Function checkAvailability : Start");
        logDbg(env,obj,mLogString);

        /* ---------------------- JNI Logging Code END-----------------------------*/

        if(disk_path == NULL)
        {

           /* ---------------------- JNI Logging Code START-----------------------------*/
            sprintf(mLogString,"Native Logging: Function checkAvailability :I/P disk Name is NULL. Function checkAvailability: END");
            logErr(env,obj,mLogString);
            /* ---------------------- JNI Logging Code END-----------------------------*/

                return NULL_ERROR;
        }

        strcpy(tmpdsk, disk_path);
        getDiskPath(tmpdsk, tmpnode,env,obj);
        if(tmpnode == NULL)
        {
            /* ---------------------- JNI Logging Code START-----------------------------*/

            sprintf(mLogString,"Native Logging: Function checkAvailability :Derived disk Name is NULL. Function checkAvailability: END");
            logErr(env,obj,mLogString);

            /* ---------------------- JNI Logging Code END-----------------------------*/

                return NULL_ERROR;
        }//End of if


        file_fd = open(tmpnode, O_RDONLY);

        if(file_fd < 0)
        {
            /* ---------------------- JNI Logging Code START-----------------------------*/

            sprintf(mLogString,"Native Logging: Function checkAvailability :Open function fails for disk %s. Error number %d. Function checkAvailability: END",tmpnode,errno);
            logDbg(env,obj,mLogString);

            /* ---------------------- JNI Logging Code END-----------------------------*/

                if(errno == EIO)
                {
                        close(file_fd);
                        env->ReleaseStringUTFChars(path,disk_path);
                        return -5;
                }//End of if
                if(errno == ENXIO)
                {
                        close(file_fd);
                        env->ReleaseStringUTFChars(path,disk_path);
                        return -6;
                }//End of if
                if(errno == EACCES)
                {
                        close(file_fd);
                        env->ReleaseStringUTFChars(path,disk_path);
                        return -13;
                }//End of if
        }//End of file_fd
        close(file_fd);
        env->ReleaseStringUTFChars(path,disk_path);
        /* ---------------------- JNI Logging Code START-----------------------------*/

            sprintf(mLogString,"Native Logging: Returning 0.Function checkAvailability: END");
            logDbg(env,obj,mLogString);

        /* ---------------------- JNI Logging Code END-----------------------------*/

        return 0;
}//End of checkAvailability

        //This method is used to traverse the devinfo tree looking
        //for devices using the driver specified in the nodeName
        //list and obtaining the physical path information about them.
        //These values are updated in the static Hashtable in java.

int getDiskPath(char  *disk_name, char *path,JNIEnv *env, jobject obj)
{
        /* To store the device link info */
        char value[MAXPATHLEN];
        char file[MAXPATHLEN];
        bzero(value,MAXPATHLEN);
        bzero(file,MAXPATHLEN);
        int retVal = -100;
        char mLogString[MAXPATHLEN+255];

        /* ---------------------- JNI Logging Code START-----------------------------*/

        sprintf(mLogString,"Native Logging: Function getDiskPath : Start");
        logDbg(env,obj,mLogString);

        /* ---------------------- JNI Logging Code END-----------------------------*/


        strcpy(file, disk_name);

        retVal = read_dir(file, value, env,obj);
        if(retVal == 0)
        {
                strcpy(path,value);
                /* ---------------------- JNI Logging Code START-----------------------------*/

                sprintf(mLogString,"Native Logging: Function getDiskPath : Disk Name %s . Function getDiskPath: END",path);
                logDbg(env,obj,mLogString);

                /* ---------------------- JNI Logging Code END-----------------------------*/

                return SUCCESS;
        }//End of if

        /* ---------------------- JNI Logging Code START-----------------------------*/
        sprintf(mLogString,"Native Logging: Function getDiskPath : Could not find Disk Name. Function getDiskPath: END");
        logDbg(env,obj,mLogString);

        /* ---------------------- JNI Logging Code END-----------------------------*/


        return FAILURE;


}//End of getDiskPath

        //This method just reads the link of each entry in
        ///the dev/dsk directory and compares it with the physical
        //path information recieved from the caller method. If
        //they match and the file is a block device, the routine
        //updates the value character array and returns 0. Else
        //it returns appropriate return codes.

int read_dir(char *disk_path, char *value, JNIEnv *env, jobject obj)
{
        DIR     *dirp;
        struct  dirent *dp;
        struct  stat stbuf;
        char    current_dir[MAXPATHLEN];
        char    file_name[MAXPATHLEN];
        char    temp_name[MAXPATHLEN];
        char    dir_dname[MAXPATHLEN];
        char    tmp_dir[MAXPATHLEN];

        int retVal = -100;
        char mLogString[MAXPATHLEN + 255];

        /* ---------------------- JNI Logging Code START-----------------------------*/

        sprintf(mLogString,"Native Logging: Function read_dir : Start");
        logDbg(env,obj,mLogString);

        /* ---------------------- JNI Logging Code END-----------------------------*/



        strcpy(current_dir, "/dev/dsk");

        if ((dirp = opendir(current_dir)) != NULL)
        {
                while ((dp = readdir(dirp)) != NULL)
                {
                        sprintf(temp_name,"../..%s",disk_path);
                        sprintf(dir_dname,"/dev/dsk/%s",dp->d_name);

                        retVal = stat(dir_dname,&stbuf);

                        if(retVal == -1)
                        {
				/* Fix for ARS 14671 - if stat() fails we
				 * need to continue on and search others in
				 * /dev/dsk, not return immediately
				 */
                                //closedir(dirp);
                                /* ---------------------- JNI Logging Code START-----------------------------*/

                                //sprintf(mLogString,"Native Logging: Function read_dir: Error in stat function. Returning with -1. Function read_dir : END");
                                //logErr(env,obj,mLogString);

                                /* ---------------------- JNI Logging Code END--------- ------- ----------- -- */

				bzero(file_name,MAXPATHLEN);
				bzero(temp_name,MAXPATHLEN);
				bzero(dir_dname,MAXPATHLEN);
				bzero(tmp_dir,MAXPATHLEN);

				continue;

                                //return -1;
                        }//End of if

                        if (S_ISBLK(stbuf.st_mode))
                        {
                                retVal = readlink(dir_dname, file_name, sizeof(file_name));

                                if(retVal == -1)
                                {
					/* Fix for ARS 14671 - if readlink()
					 * fails we need to continue on and
					 * search others in /dev/dsk, not
					 * return immediately
					 */
                                        //closedir(dirp);
                                /* ---------------------- JNI Logging Code START------------------ ----------- */

                                //sprintf(mLogString,"Native Logging: Function read_dir: Error in readLink function. Returning with -1. Function read_dir : END");
                                //logErr(env,obj,mLogString);

                                /* ---------------------- JNI Logging Code END ---------------------- ------- */

					bzero(file_name,MAXPATHLEN);
                                	bzero(temp_name,MAXPATHLEN);
					bzero(dir_dname,MAXPATHLEN);
                                	bzero(tmp_dir,MAXPATHLEN);

                                continue;

                                        //return -1;
                                }//End of if

                                int length = strlen(temp_name);

                                if (strncmp(file_name, temp_name, length) == 0)
                                {
                                        int length_dir = strlen(dir_dname);
                                        length_dir = length_dir - 2;
                                        strncpy(tmp_dir,dir_dname, length_dir);
                                        strcat(tmp_dir,"s2");
                                        strcpy(value,tmp_dir);
                                        closedir(dirp);
                                        /* ---------------------- JNI Logging Code START------------------ ----------- */

                                        sprintf(mLogString,"Native Logging: Function read_dir: Directory is   %s. function read_dir : END",value);
                                        logDbg(env,obj,mLogString);

                                        /* ---------------------- JNI Logging Code END ---------------------- ------- */

                                        return 0;
                                }//End of if
                        }//End of if

                        bzero(file_name,MAXPATHLEN);
                        bzero(temp_name,MAXPATHLEN);
                        bzero(dir_dname,MAXPATHLEN);
                        bzero(tmp_dir,MAXPATHLEN);

                }//End of while

                closedir(dirp);
        }//End of if
        /*---------------------- JNI Logging Code START-----------------------------*/

        sprintf(mLogString,"Native Logging: Function read_dir: Could not find path %s. Function read_dir : END",value);
        logDbg(env,obj,mLogString);

        /*---------------------- JNI Logging Code END -----------------------------*/

        return -1;
}//End of read_dir
