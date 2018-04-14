#include <stdio.h>
#include <iostream.h>
#include <kstat.h>
#include<sys/sysinfo.h>
#include <pwd.h>
#include<iomanip.h>
#include <dirent.h>
#include <sys/procfs.h>
#include "our_old_procfs.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/types.h>
#include <rw/ctoken.h>
#define MAXIMUMSIZE	5000


void print(char *outStr)
{
	//cout << " Print " << outStr << endl;
}


//RWCString longargs (long pid,char *errStr)
RWCString longargs (const psinfo & psi)
{
#ifdef  _OLD_LONG_ARGS_
        prpsinfo_t prp;
        int count = 0, fd, ret, i;
        long offset = 0;
        long argoff = 0;
        char proc_id[150];
        strcpy (proc_id, "/proc/");
        char pidStr[50];
        snprintf(pidStr,sizeof(pidStr),"%ld",pid);
        strcat (proc_id, pidStr);
	// strcat(proc_id,"/psinfo");
        RWCString resultStr;
        seteuid(0);
	long argcount = 0;

        fprintf(stderr, "pid= %d\n", pid);
        resultStr = "";
        /* Open the given processes procfs entry */

        if ((fd = open (proc_id, O_RDONLY)) <= 0)
        {
	        snprintf(errStr,sizeof(errStr),"Cannot open file : %s ",proc_id);
	        //print(errStr);
                // return "";
	        return resultStr;
        }

        /* Do the ioctl() call */

        if ((ret = ioctl (fd, PIOCPSINFO, &prp)) < 0)
        {

		snprintf(errStr,sizeof(errStr)," ioctl failed for path : %s ",proc_id);
		//print(errStr);
                close (fd);
                return resultStr;
        }
        resultStr = prp.pr_psargs;

        /* We use a pread to read all the argument vector into the 'args'
           array. We don't check to see how many arguments there are but
           merely read 250 char*'s from the start of the argv pointer  */

        offset = (long) prp.pr_argv;
        argcount = prp.pr_argc;
	if(argcount == 0)
	{
		close(fd);
		return resultStr;
	}
        // long *args = new long[argcount];
	//long *args = (long *)malloc(sizeof(long) * argcount);
        
	long *args = (long *)calloc(sizeof(long), argcount);
	if(args == NULL)
	{
		close(fd);
		return resultStr;
	}

        // long *argsdiff = new long[argcount];
	//long *argsdiff = (long *)malloc(sizeof(long) * argcount);
	long *argsdiff = (long *)calloc(sizeof(long), argcount);

	if(argsdiff == NULL)
	{
		close(fd);
		if(args !=NULL)
		{
			free(args); 
			args = NULL;
		}
		return resultStr;
	}
        //fprintf(stderr, "what is this?? %d\n", pid);

        //(void) memset (args, 0, sizeof(args));
        //(void) memset (argsdiff, 0, sizeof(argsdiff));
	for(i=0;i<argcount;i++)
	{
		args[i]=0;
		argsdiff[i] = 0;
	}	

        //long *tmp =new long(0);
	long *tmp = (long *)malloc(sizeof(long));
	if(tmp == NULL)
	{
		close(fd);
		if(args !=NULL)
		{
			free(args); 
			args = NULL;
		}
		if(argsdiff != NULL)
		{
			free(argsdiff);
			argsdiff = NULL;
		}
		return resultStr;
	}

        for(i=0;i<argcount;i++)
        {
                if(pread(fd,tmp,sizeof(long),offset) > 0)
                {
                        args[i] = *tmp;
                        offset += sizeof(long);
                }
        }
        offset = (long) prp.pr_argv;
	if(tmp != NULL)
    		free(tmp);

        for(i=0;i<(argcount - 1);i++)
        {
                if(args[i+1] != 0)
                        argsdiff[i] = args[i+1] - args[i];
        }
        argsdiff[i] = MAXIMUMSIZE;

        int maxsize = MAXIMUMSIZE;
        for(i = 0;i< argcount; i++)
        {
                if(argsdiff[i] > maxsize)
                        maxsize = argsdiff[i];
        }

        if(maxsize > MAXIMUMSIZE)
        {
                for(i=0;i<argcount;i++)
                {
                        if(argsdiff[i] == MAXIMUMSIZE)
                                argsdiff[i] = maxsize;
                }
        }
        char *buf = (char *) malloc (maxsize + 10);
	if(buf == NULL)
	{
		close(fd);
		if(args !=NULL)
		{
			free(args); 
			args = NULL;
		}
		if(argsdiff != NULL)
		{
			free(argsdiff);
			argsdiff = NULL;
		}
		return resultStr;
	}
        if ((argoff = args[count++]) == 0)
        {
                snprintf(errStr,sizeof(errStr),"No additional arguments given for %s",proc_id);
                //print(errStr);
                close(fd);
                if(args !=NULL)
                {
                        free(args); 
                        args = NULL;
                }
                if(argsdiff != NULL)
                {
                        free(argsdiff);
                        argsdiff = NULL;
                }
                if(buf != NULL)
                {
                        free(buf);
                        buf = NULL;
                }
                return resultStr;
        }
        resultStr = "";

        int size = argsdiff[0];
        for(i = 0; i< argcount;i++)
        {
                if((pread (fd, buf,size, argoff)) <= 0)
 		{
 			//cout << " pread is less than 0 " << endl;

 			break;
 		}
 		resultStr += buf;
 		resultStr += " ";
 		size = argsdiff[count];
 		argoff =args[count++];
 		if (argoff == 0)
        	{
            		break;
        	}
        }
	close(fd);
	if(args !=NULL)
	{
		free(args); 
		args = NULL;
	}
	if(argsdiff != NULL)
	{
		free(argsdiff);
		argsdiff = NULL;
	}
	if(buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
//    fprintf(stderr, "%d: len= %d##\n", pid, resultStr.length());
//    resultStr.strip(RWCString::both);
#else
    int fd, pfd;
    char fname[32] = {0};
    int nArgs;
#define  ARG_MAX_LEN  10240
// #define  ARG_MAX_LEN  NCARGS
    int bsize = ARG_MAX_LEN;
    char tempArgs[ARG_MAX_LEN] = {0};
    int i;
    size_t len;
    uintptr_t *argv;
    RWCString resultStr;

    resultStr = (RWCString)psi.pr_psargs;
    snprintf(fname,sizeof(fname),"/proc/%d/as", psi.pr_pid);
    pfd = open(fname, O_RDONLY);
    if (pfd < 0) {
            goto out;
    }
    nArgs = psi.pr_argc;
    if (psi.pr_nlwp == 0 || strcmp(psi.pr_lwp.pr_clname, "SYS") == 0)
    {
            goto out;
    }

    argv = (uintptr_t *)calloc( nArgs, sizeof(uintptr_t) );
    if (argv == NULL) {
            goto out;
    }

    if (psi.pr_dmodel == PR_MODEL_NATIVE)
    {
            if (pread(pfd, argv, nArgs*sizeof(uintptr_t), psi.pr_argv) <= 0)
            {
                    free(argv);
                    goto out;
            }
    }
    /* reading 32-bit address space from 64-bit process will be fine,
     * (only when the executable is 64-bit binary), else pread() will fail.
     */
    else
    {
            uint32_t *argv32;
            argv32 = (uint32_t *)calloc( nArgs, sizeof(uint32_t) );
            if( argv32 == NULL )
            {
                    free( argv );
                    goto out;
            }
            if( pread(pfd, argv32, nArgs*sizeof(uint32_t), psi.pr_argv) <= 0 )
            {
                    free( argv32 );
                    free( argv );
                    goto out;
            }
            for (i = 0; i < nArgs; i++)
                    argv[i] = argv32[i];
            free( argv32 );
    }
    resultStr = "";
    for( i=0; i<nArgs; ++i )
    {
            int ret;
            if ( (ret=pread(pfd, tempArgs, bsize, argv[i])) <= 0 )
            {
                    if (ret == 0 || errno == EIO)
                            break;
                    free( argv );
                    goto out;
            }
            resultStr += tempArgs;
            resultStr += " ";
//             len = strlen(tempArgs);
//             bsize -= len + 1;
    }
    free( argv );
  out:
    close(pfd);
#endif
    resultStr.strip(RWCString::both);
    return resultStr;
}


