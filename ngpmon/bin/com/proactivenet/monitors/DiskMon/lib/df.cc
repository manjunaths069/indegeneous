#include <rw/ctoken.h>
#include <df.h>
#include "DiskMon.h"
//algo
//if chdir returns error = 2 // if is  invalid file no need to further process
// if chdir return 20 remove file name and proceed
// if chdir 13 then proceed to statfs if it returns 13 then do not proceed
const int BLOCK_SIZE = 512 ;
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
              mid = env->GetMethodID(cls,"logError","(Ljava/lang/String;)V");
              if(mid != NULL)
              {
                      jstring tString = env->NewStringUTF(merrStr);
                      env->CallVoidMethod(obj,mid,tString);
                      env->DeleteLocalRef(tString);
              }
        }
}

/* --------------------- Changes for JNI logging END------------------------------*/



int fileStatus(RWCString pname,JNIEnv * env, jobject obj)
{
    char mLogString[1000];

    /* ---------------------- JNI Logging Code START-----------------------------*/

    sprintf(mLogString,"Native Logging: Function filestatus: Start");
    logDbg(env,obj,mLogString);

    /* ---------------------- JNI Logging Code END -----------------------------*/

        errno = 0 ;
        RWCString name(pname);
        //if the file name given in valid, the function does chdir so that the entry comes
        //into /etc/mntab file (if is not alraedy there
        int ch = chdir (name );
        if ( ch == -1 )
        {
                        /* -------------------------JNI Logging -----------------*/
                        sprintf(mLogString,"Native Logging : Function fileStatus : Not a Directory . Error code : %d ",errno);
                            logDbg(env,obj,mLogString);

                        /* ---------------------- JNI Logging Code END -----------------------------*/

                        if (errno == 2 ) //means file not found no need to proceed
                        {
                                return errno *-1 ;
                        }
                if (  errno == 20  )
                {
                        // 20 = not a directory
                        //Get direcotory name by slashing the file name
                        RWCTokenizer next(name);
                        RWCString dirnComponent[256] ;
                        RWCString token ;
                        int counter = 0 ;
                        while (!(token = next("/")).isNull())
                        {
                                dirnComponent[counter] = token ;
                                counter++ ;
                        }
                        RWCString dirname = "/" ;
                        for (int i = 0 ; i < ( counter - 1 ); i ++ )
                                dirname +=  dirnComponent[i] + "/" ;
                        ch = chdir ( dirname );

                        if ( ch == -1 )
                        {
                                /* -------------------------JNI Logging -----------------*/
                                sprintf(mLogString,"Native Logging : Function fileStatus : Directory Not Found . Error code : %d ",errno);
                                    logErr(env,obj,mLogString);

                                /* ---------------------- JNI Logging Code END -----------------------------*/
                                return errno * -1;
                        }
                }
        }
       /* -------------------------JNI Logging -----------------*/
        sprintf(mLogString,"Native Logging : Function fileStatus : END");
       logDbg(env,obj,mLogString);

       /* ---------------------- JNI Logging Code END -----------------------------*/

        return 0 ;
}

//Get the linked path and copped the absolute path into resolved
void resolvePath(int osType, RWCString pfilename, char * resolved,JNIEnv * env, jobject obj )
{

     char mLogString[1000];

     /* ---------------------- JNI Logging Code START-----------------------------*/

     sprintf(mLogString,"Native Logging: Function resolvePath: Start");
     logDbg(env,obj,mLogString);

     /* ---------------------- JNI Logging Code END -----------------------------*/


 RWCString filename = pfilename.strip() ;

        //System call resolvepath is not supported bu solaris 2.5.1
        if ( osType > 0 )
        {
                int rs = resolvepath(filename.data(),resolved, 1024) ;
                if (rs ==  -1 )
                        strcpy(resolved,"NOT_FOUND");
                else
                        resolved[rs] = 0 ;
        }
        else
        {
                char  buff [1024];
                int result = 0 ;
                RWCTokenizer next(filename);           // Tokenize the string a
                RWCString token;                // Will receive each token
                // Advance until the null string is returned:
                RWCString resolvedTillNow = "" ;
                RWCString rest = "" ;
                while (!(token=next("/")).isNull())
                {
                        resolvedTillNow +=  "/" + token ;
                        result = readlink ( resolvedTillNow.data() , buff, 1024 ) ;
                        if (result != -1 ) // link found
                        {
                                buff[result] = 0 ; //because readlink doesn't return null terminated string
                                resolvePath(osType, buff, resolved, env, obj ) ;
                                resolvedTillNow = resolved ;
                        }
                        else
                        {
                                if (errno == 22 ) //File name was not link . OK proceed ahead
                                        continue ;
                                else
                                {
                                        strcpy (resolved,"NOT_FOUND");
                                        /* -------------------------JNI Logging -----------------*/

                                        sprintf(mLogString,"Native Logging: Function : resolvePath :  Resolved path ERROR  : Error no. %d",errno);
                                        logErr(env,obj,mLogString);
                                        /* ---------------------- JNI Logging Code END -----------------------------*/
                                        return ;
                                }
                        }
                }

        strcpy (resolved,resolvedTillNow.data()) ;

     /* ---------------------- JNI Logging Code START-----------------------------*/

     sprintf(mLogString,"Native Logging: Function resolvePath: END");
     logDbg(env,obj,mLogString);

     /* ---------------------- JNI Logging Code END -----------------------------*/

        }
}
/*
case 1; Total Disk Size in MB
case 2; Total Disk INODED
case 3; % Disk
case 4; % INODE
case 6: Disk Free Space in MB //Added by Ashish on 13th of Oct 2K6
*/
//Makes statvfs call and retrun the desired statistics
float statFileSystem(const char * fileName, int typ, JNIEnv * env, jobject obj)
{
        float tSize = 0 ; //MB
        float tINodes = 0 ;
        float diskUsed = 0.0 ;
        float iNodeUsed = 0 ;
        float diskFreeSpace = 0; //Added by Ashish
        struct statvfs fsd;
        char mLogString[1000];

     /* ---------------------- JNI Logging Code START-----------------------------*/

     sprintf(mLogString,"Native Logging: Function statFileSystem: Start");
     logDbg(env,obj,mLogString);

     /* ---------------------- JNI Logging Code END -----------------------------*/

        if (statvfs ( fileName, &fsd) == -1)
        {

             /* ---------------------- JNI Logging Code START-----------------------------*/

             sprintf(mLogString,"Native logging: statFileSystem: Error in statvfs function Error code : %d ",errno);
             logErr(env,obj,mLogString);

             /* ---------------------- JNI Logging Code END -----------------------------*/

                if (errno == 13 )
                        return errno * -1 ;
                else
                        return -1 ;
        }
        u_long fBlockSize =  fsd.f_frsize;
        if ( fBlockSize == 0 ) //if f_frsize in not supported
                fBlockSize = BLOCK_SIZE ;
        tSize = ((double)fsd.f_blocks  / 1024 / 1024 )* fBlockSize  ;
        
        /**
		* Inserted by Ashish on 13th of Oct 2K6
		* to get the Free Space in MB
		*/
		u_long fFreeBlocks = fsd.f_bfree;//Free Blocks
		
		sprintf(mLogString,"Native logging: Block Size: %d ", fBlockSize);
        logDbg(env,obj,mLogString);
        sprintf(mLogString,"Native logging: Free Blocks: %d ", fFreeBlocks);
        logDbg(env,obj,mLogString);
		/**
		* Multiplying Free Blocks number with Block Size
		* dividin to get the value in MB
		*/
		diskFreeSpace = ((double)fFreeBlocks / 1024 / 1024) * fBlockSize;
		
        
        tINodes = fsd.f_files;

        fsblkcnt_t usedNonsuperUserBlocks  = fsd.f_blocks - fsd.f_bfree ;
        sprintf(mLogString,"Native logging: usedNonsuperUserBlocks: %d ",usedNonsuperUserBlocks);
        logDbg(env,obj,mLogString);
        
        fsblkcnt_t totalNonsuperUserBlocks = usedNonsuperUserBlocks + fsd.f_bavail ;
        sprintf(mLogString,"Native logging: totalNonsuperUserBlocks: %d ",totalNonsuperUserBlocks);
        logDbg(env,obj,mLogString);
        
        fsfilcnt_t usedNonsuperUserInodes = fsd.f_files -  fsd.f_ffree  ;
        sprintf(mLogString,"Native logging: usedNonsuperUserInodes: %d ",usedNonsuperUserInodes);
        logDbg(env,obj,mLogString);
        
        fsfilcnt_t totalNonsuperUserInodes = usedNonsuperUserInodes + fsd.f_favail ;
        sprintf(mLogString,"Native logging: totalNonsuperUserInodes: %d ",totalNonsuperUserInodes);
        logDbg(env,obj,mLogString);
        
        
        diskUsed = ((float)usedNonsuperUserBlocks/totalNonsuperUserBlocks  ) * 100 ;
        sprintf(mLogString,"Native logging: diskUsed : %f ",diskUsed);
        logDbg(env,obj,mLogString);
       
        
        iNodeUsed =((float) usedNonsuperUserInodes/totalNonsuperUserInodes ) * 100 ;
        sprintf(mLogString,"Native logging: iNodeUsed: %f ",iNodeUsed);
        logDbg(env,obj,mLogString);



             /* ---------------------- JNI Logging Code START-----------------------------*/

             sprintf(mLogString,"Native logging: statFileSystem: Output : diskUsed (percent): %f. Inode used (percent) : %f. END",diskUsed,iNodeUsed);
             logDbg(env,obj,mLogString);

             /* ---------------------- JNI Logging Code END -----------------------------*/




        switch(typ)
        {
                case 1:

                        return tSize ;
                case 2:
                        return tINodes ;
                case 3:
                        return diskUsed ;
                case 4:
                        return iNodeUsed ;
				case 6: //Added by Ashish to get Free Space in MB, 13th Oct 2K6
						return diskFreeSpace;		                        
                default :
                        return -1;
        }
        return 0 ;
}
/*
main(int arc, char ** argv)
{
        cout << "RET IS " << fileStatus(argv[1]) << endl;
        char  a [1024] ;
        resolvePath(0,argv[1],a);
        cout << "RESPVED PATH " << a << endl ;
        cout << "Total Disk Sizer " << statFileSystem(a,1)<< endl ;
        cout << "Total INODE Sizer " << statFileSystem(a,2)<< endl ;
        cout << "Per Disk Sizer " << statFileSystem(a,3)<< endl ;
        cout << "Per Disk Sizer " << statFileSystem(a,4) << endl ;
}
*/
