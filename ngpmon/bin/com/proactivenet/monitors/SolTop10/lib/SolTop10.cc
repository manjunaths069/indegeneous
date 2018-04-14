/*
 * SolTop10.cc
 */

#include <stdio.h>
#include <iostream.h>
#include <kstat.h>
#include<sys/sysinfo.h>
#include <pwd.h>
#include<iomanip.h>
/*#include <rw/cstring.h>*/
#include <dirent.h>
#include <procfs.h>
#include <fcntl.h>
#include <unistd.h>
#include "TopStruct.h"
#include <stdlib.h>
#ifndef	 _STANDALONE_
#  include <jni.h>
#endif
#include "SolTop10.h"
#include <time.h>
#include <stdlib.h>
#include <thread.h>
#include <string.h>
#include <stddef.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#define	  Free(x)  if (x) { free(x); x = NULL; }
#define	  ZERO(x)  bzero(&x, sizeof(x))

#ifndef _STANDALONE_
void logDbg(JNIEnv *env, jobject obj, char *logStr)
#else
void logDbg(const char *logStr)
#endif
{
	if(logStr == NULL)
		return;
#ifndef	 _STANDALONE_
	jclass cls = env->GetObjectClass(obj);
	jmethodID mid;
	if(cls != NULL) {
		mid = env->GetMethodID(cls,"logDbg","(Ljava/lang/String;)V");
		if(mid != NULL) {
			jstring tString = env->NewStringUTF(logStr);
			env->CallVoidMethod(obj,mid,tString);
			env->DeleteLocalRef(tString);
		}
	}
#else
	fprintf(stdout, "LOG:: %s\n", logStr);
#endif
}

void display( struct top *top_struct,int act_num_proc);
void sort_by_cpu(top top_struct[],int display_num, int act_num_proc_ );
void sort_by_memory(top top_struct[],int display_num, int act_nm_proc );
void sort_by_time(top top_struct[],int display_num, int act_nm_proc );
void sort_by_time1(top top_struct[],int display_num, int act_nm_proc );
void sort(top top_struct[],int display_num, int act_num_proc);
void print(const char *temp);
int  internal_mem(kstat_ctl_t *kc, double * int_func_mem);


#ifdef	_STANDALONE_
#define	 PRINT_FUNC_NAME  fprintf(stdout, "calling: %s\n", __func__)

static void setDate(const char *date)
{
	PRINT_FUNC_NAME;
	fprintf(stdout, "system_date: %s\n", date);
}

static	void setCPUState(double midle,double muser,double mkernel, double miowait,double mswap)
{
	PRINT_FUNC_NAME;
	fprintf(stdout, "Idle= %f\n", midle);
	fprintf(stdout, "User= %f\n", muser);
	fprintf(stdout, "Kernel= %f\n", mkernel);
	fprintf(stdout, "IOWait= %f\n", miowait);
	fprintf(stdout, "Swap= %f\n", mswap);
}

static void setMem(double mreal,double mfree,double mswapuse,double mswapfree)
{
	PRINT_FUNC_NAME;
	fprintf(stdout, "memory in MB: %.2f %.2f %.2f %.2f\n", mreal, mfree, mswapuse, mswapfree);
}

static	void setLoadAvg(float loadavg1,float loadavg5,float loadavg15)
{
	PRINT_FUNC_NAME;
	fprintf(stdout, "setLoadAvg begins loadavg1 : %d loadavg5: %d loadavg15: %d\n",
		loadavg1, loadavg5, loadavg15);
}

void setProc(int pid, char *musername, int thr, int pri, int nice,
	     double size, double res, char *state, int hr, int min, int sec,
	     double cpu, char *command)
{
	PRINT_FUNC_NAME;
	fprintf(stdout, "setProc begins : pid = %d username = %s", pid, musername);
	fprintf(stdout, "Thread = %d Priority %d nice %d size %.0f\n",
		thr, pri, nice, size);
	fprintf(stdout, "RSS %f state %s time %d:%d:%d cpu %f command %s\n",
		res, state, hr, min, sec, cpu, command);
}

void setProcState(int total ,int sleeping, int running,int zombie, int cpu, int stopped)
{
	PRINT_FUNC_NAME;
	fprintf(stdout, "setProcState begins "
		"total %d sleeping %d running %d zombie %d cpu %d stopped %d\n",
		total, sleeping, running, zombie, cpu, stopped);
}
#endif


const char *PROCFS = "/proc";
static mutex_t	itrMutex;

char mErrorMessage[255] = {0};


/*
  Password(passwd) is a structure defined in "pwd.h" .
  A system call (getpwuid) accepts a uid and then populates this structure.
  One of the attribute of the passwd structure is the pw_name ( username ) .
  FILES :
  pwd.h -> passwd.
  SYSTEM CALL
  getpwuid.
*/


void uid_to_name(uid_t uid, char *uid_c, const int len)
{
	struct passwd *pass = getpwuid(uid);
	if (pass)
		snprintf(uid_c, len, "%s", pass->pw_name);
	else
		snprintf(uid_c, len, "%d", uid);
}


/* Calculation of system time.
   Validation has not yet been done if asctime does not return the right value.
*/


char * internal_date()
{
	time_t timer = NULL;
	timer = time(NULL);
	if ( timer == NULL)
	{
		print("internal_date : Unable to allocate memeory for date calculation");
		return NULL;
	}
	return asctime(localtime(&timer));
}

int split(char *data, const char *dataSep,
	  const int maxArgs, char ***argv)
{
	int curArg = 0;
	int offset;
	int DQ = '\"';
	int doTrim =0;

	if( !data || !*data )
		return 0;

	while (*data) {
		if (curArg >= maxArgs)
			return curArg;

		/* remove the white-spaces from the begining of the field */
		if (doTrim) {
			while (isspace((int)*data)) {
				data++;
			}
		}
		offset = strcspn(data, dataSep);
		if (offset != 0) {
			/* see if the field starts with double-quote (")
			 * everything after the ending " is ignored
			 */
			if (*data == DQ) {
				int nDqPos, i=-1;

				data++;
				(*argv)[curArg++] = data;
				nDqPos = strcspn( data, "\"" );
				data += nDqPos;

				/* see for escaped double-quotes ("abc""d"), if there are then keep only one */
				while (1) {
					/* this will be the end-of-field (marked by single ") */
					if( *data == DQ && *(data+1) != DQ )
					{ data -= i; break; }

					if( *data == DQ && *(data+1) == DQ )
					{ i++; data++; }
					*(data-i) = *(data+1);
					data++;
				}
				*(data-1) = '\0';
				offset = strcspn( data, dataSep );
				data += offset;
				continue;
			}
			else
				(*argv)[curArg++] = data;
		}
		data += offset;

		if (*data) {
			/* remove the white-spaces from the end of the field */
			if (doTrim) {
				while (isspace((int)*(data-1))) {
					data--;
				}
			}
			*data = '\0';
		} else {
			break;
		}
		data++;
	}
	return curArg;
}

/*
  Solaris mainitains the list of current processes in the directory structure proc.
  The information needed can be picked from going into each directory of proc and reading the file psinfo.
  The path would be /proc/(processdir)/psinfo.

  "sys/dirent.h" contains the struct of dirent.

  psinfo file is read into a structure variable currproc . This structure is defined in file sys/procfs.h.
  NOTE :
  psinfo has a pointer reffering another structure called lwpsinfo.

  Values retrived from psinfo :
  PID.
  UID ( passed to uid_to_name function to get username )
  THR (number of threads - lwps)
  SIZE and RSVALUE ( converted to MB)
  CPU STATUS ( O - Process on cpu , Z zombie process , S process on sleep , R Process ready to run. )
  CPU % ( The value is a 16 bit binary	where 1.0 = 0x8000 . This number has to be divided by 0x8000.)
  COMMAND NAME char[] by name pr_fname.

  Values retrived from lwpsinfo :
  PRI (priority of the process)
  NICE ( Nice value of the process .This value ranges from 0 to 40.
  Nice value has to be subtracted by 20 to bring it in sync with the nice range of (-20 to 20)

  COUNT OF PROCESS :
  Count is the number of times the loop executes. Since this number is always higher by the "Top" count by 4,
  the processes having PID 0,1,2,3 are neglected and the count is not taken.


  EQUIV COMMAND TO TEST :

  ps -flaL


  NOTE : PID 0 1 2 3 ARE NEGLECTED.
*/

#ifndef	 _STANDALONE_
JNIEXPORT void JNICALL Java_com_proactivenet_monitors_SolTop10_SolTop10_getProcs
(JNIEnv *env, jobject obj,jint t_num_to_display,jstring colName, jstring pathNm)
#else
void getProcs(int t_num_to_display, const char *colName, const char *pathNm)
#endif
{
#define	 MAX_PATH_LEN	512
	int result  = 0;
	int total_process=0;
	int running_state=0;
	int sleep_state=0;
	int zombie_state=0;
	int runnable_state=0;
	int stopped_state=0;
	int unkn_state=0;
	char mcpu_status[10] = {0};
	int mem_allocate = 0;
	char *username = NULL;
	struct psinfo currproc ;
	struct dirent *direntp;

	top *top_struct = NULL;
	DIR *procdir = NULL;
	char mLogMessage[255] = {0};
	char fname[MAX_PATH_LEN] = {0};
	char buf[BUFSIZ] = {0};
	char sort_field[8] = {0};
	FILE *ptr = NULL;
	int i = 0;
	struct stat stat_info;
	enum { NLWP=4, PRIO, NICE, STATE, USER, PID, FNAME, MAX_PS_OUT };
	enum ProcType { TOTAL=1, SLP, CPU, RUN, ZOMB, STOP, UNKN, PROC_TYPES };
	int VMSIZE=0, RSS=1, TIME=2, PCPU=3;
	char **nFields = NULL;
	int nargs = 0;

	mutex_lock(&itrMutex);
	snprintf(mLogMessage, sizeof(mLogMessage), "NativeCode");
#ifndef	 _STANDALONE_
	logDbg(env,obj,mLogMessage);
#else
	logDbg(mLogMessage);
#endif
	if (t_num_to_display <= 0 || !pathNm) {
		return;
	}

#ifndef	 _STANDALONE_
	jboolean isCopy;
	const char *colSort;
	colSort = env ->GetStringUTFChars(colName,&isCopy);
	const char* pathName =	env ->GetStringUTFChars(pathNm,&isCopy);
#else
	const char *colSort = colName;
	const char *pathName= pathNm;
#endif

	if (colSort && !strcasecmp(colSort, "cpu")) {
		snprintf(sort_field, sizeof(sort_field), "CPU");
		PCPU=0; TIME=1; RSS=2; VMSIZE=3;
	} else if (colSort && !strcasecmp(colSort, "memory")) {
		snprintf(sort_field, sizeof(sort_field), "MEMORY");
	} else {
		snprintf(mErrorMessage, sizeof(mErrorMessage), "Unknown sorting method.");
#ifndef	 _STANDALONE_
		logDbg(env, obj, mErrorMessage);
#else
		logDbg(mErrorMessage);
#endif
		goto out;
	}

	top_struct = (top *)calloc(sizeof(top), t_num_to_display);
	if (!top_struct) {
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010601:Unable to allocate memory");
#ifndef	 _STANDALONE_
		logDbg(env, obj, mErrorMessage);
#else
		logDbg(mErrorMessage);
#endif
		goto out;
	}
	snprintf(fname, sizeof(fname), "SORT_ORDER=%s %s|sort -nr", sort_field, pathName);
	snprintf(mLogMessage, sizeof(mLogMessage), "Executing command: %s\n", fname);
#ifndef	 _STANDALONE_
	logDbg(env, obj, mLogMessage);
#else
	logDbg(mLogMessage);
#endif

	nFields = (char **) calloc(MAX_PS_OUT, sizeof(char *));
	if (!nFields) {
		goto out;
	}
	ptr = popen(fname, "r");
	if (!ptr) {
		snprintf(mLogMessage, sizeof(mLogMessage), "Could not execute command: %s", fname);
#ifndef	 _STANDALONE_
		logDbg(env, obj, mLogMessage);
#else
		logDbg(mLogMessage);
#endif
		goto out;
	}

	while (fgets(buf, BUFSIZ, ptr)) {
		nargs = split(buf, " ", MAX_PS_OUT, &nFields);
		if (nargs == PROC_TYPES && !strcmp(nFields[0], "SUMMARY:")) {
			       total_process = atoi(nFields[TOTAL]);
			       sleep_state   = atoi(nFields[SLP]);
			       running_state = atoi(nFields[CPU]);
			       runnable_state= atoi(nFields[RUN]);
			       zombie_state  = atoi(nFields[ZOMB]);
			       stopped_state = atoi(nFields[STOP]);
			       unkn_state    = atoi(nFields[UNKN]);
		} else if (nargs == MAX_PS_OUT) {
			if (i < t_num_to_display) {
				top_struct[i].top_size	  = atof(nFields[VMSIZE]);
				top_struct[i].top_ressize = atof(nFields[RSS]);
				top_struct[i].top_cpu	  = atof(nFields[PCPU]);
				top_struct[i].top_thread  = atoi(nFields[NLWP]);
				top_struct[i].top_priority= atoi(nFields[PRIO]);
				top_struct[i].top_nice	  = atoi(nFields[NICE]);
				top_struct[i].top_pid	  = atoi(nFields[PID]);
				snprintf(top_struct[i].top_state,
					 sizeof(top_struct[i].top_state), "%s", nFields[STATE]);
				snprintf(top_struct[i].top_username,
					 sizeof(top_struct[i].top_username), "%s", nFields[USER]);
				snprintf(top_struct[i].top_command,
					 sizeof(top_struct[i].top_command), "%s", nFields[FNAME]);
				sscanf(nFields[TIME], "%d:%d:%d", &top_struct[i].top_time.hr,
				       &top_struct[i].top_time.min,&top_struct[i].top_time.sec);
			}
                        i++;
		} else {
			snprintf(mLogMessage, sizeof(mLogMessage), "Invalid line: %s", buf);
#ifndef	 _STANDALONE_
			logDbg(env, obj, mLogMessage);
#else
			logDbg(mLogMessage);
#endif
		}
	}
	pclose(ptr);
	Free(nFields);

	if (i < t_num_to_display) {
		snprintf(mLogMessage, sizeof(mLogMessage), "Less number number of processes. Check if command runs fine.\n");
#ifndef	 _STANDALONE_
		logDbg(env, obj, mLogMessage);
#else
		logDbg(mLogMessage);
#endif
		t_num_to_display = i;
	}

#ifndef	 _STANDALONE_
	jfieldID fid;
	jmethodID mid;
	jclass cls=env->GetObjectClass(obj);

	if(cls == NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010617:Memory allocation failed");
		print ("getproc : Unable to reference jclass variable");
		Free(top_struct);
		mutex_unlock(&itrMutex);
		return;
	}

	mid = env->GetMethodID( cls, "setProc" , "(ILjava/lang/String;IIIDDLjava/lang/String;IIIDLjava/lang/String;)V" );
	if(mid == NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010617:Memory allocation failed");
		print ("getproc : Unable to reference jmethod variable");
		Free(top_struct);
		mutex_unlock(&itrMutex);
		return;
	}

	jstring temp_username_t = NULL,temp_command_t = NULL ,temp_state_t = NULL;
	for(i=0; i<t_num_to_display; i++)
	{
		temp_username_t = env->NewStringUTF(top_struct[i].top_username);
		temp_command_t = env->NewStringUTF(top_struct[i].top_command);
		temp_state_t = env->NewStringUTF(top_struct[i].top_state);

		env->CallVoidMethod(obj, mid, top_struct[i].top_pid, temp_username_t,
				    top_struct[i].top_thread, top_struct[i].top_priority,
				    top_struct[i].top_nice, top_struct[i].top_size,
				    top_struct[i].top_ressize, temp_state_t ,top_struct[i].top_time.hr,
				    top_struct[i].top_time.min, top_struct[i].top_time.sec,
				    top_struct[i].top_cpu, temp_command_t);
		env->DeleteLocalRef(temp_username_t);
		env->DeleteLocalRef(temp_command_t);
		env->DeleteLocalRef(temp_state_t);

	}

	mid = env->GetMethodID( cls, "setProcState" , "(IIIIII)V" );
	if(mid == NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010617:Memory allocation failed");
		print ("getproc : Unable to reference jmethod variable (setprocstate)");
		Free(top_struct);
		mutex_unlock(&itrMutex);
		return;
	}
	env->CallVoidMethod(obj, mid, total_process, sleep_state, runnable_state,
			    zombie_state, running_state, stopped_state);

#else
	/*
	 * Need to call functions in _STANDALONE_ mode:
	 *  - setProc
	 *  - setProcState
	 */
	setProcState(total_process , sleep_state , runnable_state , zombie_state , running_state,stopped_state);
	for(i=0;i<t_num_to_display;i++)
	{
		char *temp_username_t = top_struct[i].top_username;
		char *temp_command_t = top_struct[i].top_command;
		char *temp_state_t = top_struct[i].top_state;
		setProc(top_struct[i].top_pid, temp_username_t, top_struct[i].top_thread,
			top_struct[i].top_priority, top_struct[i].top_nice, top_struct[i].top_size,
			top_struct[i].top_ressize, temp_state_t, top_struct[i].top_time.hr,
			top_struct[i].top_time.min, top_struct[i].top_time.sec,
			top_struct[i].top_cpu, temp_command_t);
	}
#endif
out:
	Free(top_struct);
	mutex_unlock(&itrMutex);
	snprintf(mLogMessage, sizeof(mLogMessage), "Leaving getProcs.\n");
#ifndef	 _STANDALONE_
	logDbg(env,obj,mLogMessage);
#else
	logDbg(mLogMessage);
#endif
}

/*
  internal_cpustate calculates the number of idle,user kernel,nice swap cpu process.

  Move the file pointer to the first line of cpu_stat.
  Read the information into "k_space"
  Copy the kspace data into the variable of struct cpu_stat.
  cpu_stat is a structure  defined in "sysinfo.h".
  The first 2 elements of this structure stores the information required.

  EQUIV COMMAND TO TEST :

  vmstat (last 3 columns)


*/

int interal_cpustate(kstat_ctl_t *kc , double *int_fun_cpustate)
{
	print("Internal_cpustate : function begins ")  ;

	kstat_t *k_space;
	void *val_ptr;
	CpuState presentpoll,diffpoll;
	kstat_ctl_t *kc_temp = kc;

	// Move the file pointer to the first line containing "cpu_stat".

	int t_num_cpu = 0;

	for(kstat *ksp_temp=kc->kc_chain; ksp_temp != NULL ; ksp_temp = ksp_temp->ks_next)
	{
		if( ksp_temp->ks_type == KSTAT_TYPE_RAW && !strcmp(ksp_temp->ks_module,"cpu_stat"))
		{
			t_num_cpu++;
		}
	}
	if(t_num_cpu == 0)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010614:Could not find cpu_stat for CPU state module");
		print(mErrorMessage);
		return 1;
	}
	int num_cpu = 0;
	kc = kc_temp;
	double states = 0;

	print("Internal_cpustate : Read the data for each CPU")	 ;

	for(kstat *ksp=kc->kc_chain;((ksp != NULL) && (num_cpu <= t_num_cpu)); ksp = ksp->ks_next)
	{
		if( ksp->ks_type == KSTAT_TYPE_RAW && !strcmp(ksp->ks_module,"cpu_stat"))
		{
			k_space = kstat_lookup(kc,"cpu_stat",ksp->ks_instance,ksp->ks_name);
			if(k_space ==  NULL)
			{
				snprintf(mErrorMessage, sizeof(mErrorMessage), "26010607:kstat_lookup failed for module name cpu_stat") ;
				print(mErrorMessage);
				return 1;
			}

			// Read the data into kspace.

			if(kstat_read(kc,k_space,NULL) == -1)
			{
				snprintf(mErrorMessage, sizeof(mErrorMessage), "26010608:kstat_read failed for module name cpu_stat") ;
				print(mErrorMessage);
				return 1;
			}

			if(k_space->ks_data == NULL)
			{
				snprintf(mErrorMessage, sizeof(mErrorMessage), "26010609:After read kstat_data is NULL for module name cpu_stat") ;
				print(mErrorMessage);
				return 1;
			}

			// Copying from kspace to a cpu_stat struct variable
			cpu_stat mystat ;
			memcpy(&mystat,k_space->ks_data,sizeof(cpu_stat));

			fprintf(stdout, "user= %f\n", (mystat.cpu_sysinfo).cpu[CPU_KERNEL]);
			int_fun_cpustate[SOL_CPU_IDLE] +=  (mystat.cpu_sysinfo).cpu[CPU_IDLE] ;
			int_fun_cpustate[SOL_CPU_USER] +=  (mystat.cpu_sysinfo).cpu[CPU_USER];
			int_fun_cpustate[SOL_CPU_KERNEL] += (mystat.cpu_sysinfo).cpu[CPU_KERNEL] ;
			int_fun_cpustate[SOL_CPU_STATES] +=  (mystat.cpu_sysinfo).cpu[CPU_STATES] ;
			int_fun_cpustate[SOL_CPU_IO] += (mystat.cpu_sysinfo).wait[W_IO];
			int_fun_cpustate[SOL_CPU_SWAP] += (mystat.cpu_sysinfo).wait[W_SWAP];
			num_cpu++;
		}

	}

	if(num_cpu <= 0)
		num_cpu = 1;
	int_fun_cpustate[SOL_CPU_NUM] =num_cpu;
	print("Internal_cpustate :  function ends")  ;
	return 0;
}


/*
  Gets the virtual memory statistics by reading the line in the kernel structure contaning the line vminfo. The read format is a structure "vminfo" defined in sys/sysinfo.h.
  The real memory is pagesize * num of pages ..both of which can be obtained by evoking the system call sysconf.

  EQUIV COMMAND TO TEST :

  vmstat ( but not in the right format ).


*/


int  internal_mem(kstat_ctl_t *kc, double * int_func_mem)
{
	kstat_t *k_space;
	void *val_ptr;
	k_space = kstat_lookup(kc,"unix",0,"vminfo");

	print("internal_mem : function begins");
	if(k_space ==  NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010607:kstat_lookup failed for module vminfo") ;
		print(mErrorMessage);
		return 1;
	}
	if(kstat_read(kc,k_space,NULL) == -1)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010608:kstat_read failed in module vminfo") ;
		print(mErrorMessage);
		return 1;
	}

	if(k_space->ks_data == NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010609:After reading the data is NULL in module vminfo");
		print(mErrorMessage);
		return 1;
	}

	vminfo mystat;
	memcpy(&mystat,k_space->ks_data,sizeof(vminfo));

	print("internal_mem : Finished collecting data ");

	int_func_mem[SOL_FREEMEM]=  mystat.freemem ;
	int_func_mem[SOL_SWAP_RESV] = mystat.swap_resv;
	int_func_mem[SOL_SWAP_AVAIL] = mystat.swap_avail;
	int_func_mem[SOL_TIME] = time(0);

	return 0;
}




/*
  sets the value returned by "internal_cpustate" into the java code by calling
  setCPUState() defined in java.

  Also calls internal_mem function to gather virtual memory information
  and then passes it to the java program by calling setMem() funtion

  It also takes care of kstat_open and kstat_close.

*/
#ifndef	 _STANDALONE_
JNIEXPORT void JNICALL Java_com_proactivenet_monitors_SolTop10_SolTop10_getCPUMem
(JNIEnv *env, jobject obj)
#else
void getCPUMem(void)
#endif
{
	kstat_ctl_t *kstat_t;
	double int_cpustate_diff[6] = { 0,0,0,0,0,0};
	double int_cpustate_first[7] = { 0,0,0,0,0,0,0};
	double int_cpustate_second[7] = { 0,0,0,0,0,0,0};
	double	int_mem_first[4]={0,0,0,0};
	double	int_mem_second[4]={0,0,0,0};
	int i = 0 ;
	ulong temptime = 1;
	double total = 0;
	int result = 0;

	ZERO(mErrorMessage);

	mutex_lock(&itrMutex);
	longlong_t mpagesize =	sysconf(_SC_PAGESIZE) ;
	longlong_t pages = sysconf(_SC_PHYS_PAGES);
	double totalmem = mpagesize * pages;

	for(i=0;i<2;i++)
	{
		kstat_t=kstat_open();
		if (!kstat_t) {
			return;
		}
		if( i == 0)
		{
			print("getCPUMem : Calling internal_cpustate function for the first time");
			result = interal_cpustate(kstat_t,int_cpustate_first);
			if ( result == 1)
			{
				print("getCPUMem : internal_cpustate function failed");
				kstat_close(kstat_t);
				mutex_unlock(&itrMutex);
				return;
			}

			print("getCPUMem : Calling internal_mem function for the first time");
			result = internal_mem(kstat_t,int_mem_first);
			if ( result == 1)
			{
				print("getCPUMem : internal_mem function failed");
				kstat_close(kstat_t);
				mutex_unlock(&itrMutex);
				return;
			}
		}
		else
		{
			print("getCPUMem : Calling internal_cpustate function for the second time");
			result = interal_cpustate(kstat_t,int_cpustate_second);
			if ( result == 1)
			{
				print("getCPUMem : internal_cpustate function failed");
				kstat_close(kstat_t);
				mutex_unlock(&itrMutex);
				return;
			}

			print("getCPUMem : Calling internal_mem function for the second time");
			result = internal_mem(kstat_t,int_mem_second);
			if ( result == 1)
			{
				print("getCPUMem : internal_mem function failed");
				kstat_close(kstat_t);
				mutex_unlock(&itrMutex);
				return;
			}

		}
		kstat_close(kstat_t);
		sleep(POLL_DELAY);
	}
	mutex_unlock(&itrMutex);

	for (i = 0;i <= 5 ; i++) {
		int_cpustate_diff[i] = int_cpustate_second[i] - int_cpustate_first[i];
	}
	total = int_cpustate_diff[SOL_CPU_IDLE] + int_cpustate_diff[SOL_CPU_USER]
		+ int_cpustate_diff[SOL_CPU_KERNEL] + int_cpustate_diff[SOL_CPU_STATES];
	if(total <= 0 )
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010605:Divide by zero error in cpustate calculation") ;
#ifndef	 _STANDALONE_
		logDbg(env, obj, mErrorMessage);
#else
		logDbg(mErrorMessage);
#endif
		return;
	}
	for(i = 0;i <= 5 ; i++)
		int_cpustate_diff[i]  = ((int_cpustate_diff[i]/total)) * 100;

	temptime =int_mem_second[SOL_TIME] - int_mem_first[SOL_TIME];
	if( temptime <= 0)
		temptime = int_mem_second[SOL_TIME];

	if (temptime <= 0 || totalmem <= 0)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010605:Divide by zero error in memory calculation") ;
#ifndef	 _STANDALONE_
		logDbg(env, obj, mErrorMessage);
#else
		logDbg(mErrorMessage);
#endif
		return;
	}

	double mPercentMemUsed =(1 - ((double)(((int_mem_second[SOL_FREEMEM] - int_mem_first[SOL_FREEMEM])
						/ (temptime))*mpagesize)) / totalmem) * 100;

	ulong tempswap_resv = int_mem_second[SOL_SWAP_RESV] - int_mem_first[SOL_SWAP_RESV];
	if (tempswap_resv <= 0)
		tempswap_resv = int_mem_second[SOL_SWAP_RESV];

	ulong tempswap_avail = int_mem_second[SOL_SWAP_AVAIL] - int_mem_first[SOL_SWAP_AVAIL];
	if (tempswap_avail <= 0)
		tempswap_avail = int_mem_second[SOL_SWAP_AVAIL];

	if ((tempswap_avail+tempswap_resv) <= 0)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010605:Divide by zero error in swap calculation") ;
#ifndef	 _STANDALONE_
		logDbg(env, obj, mErrorMessage);
#else
		logDbg(mErrorMessage);
#endif
		return;
	}
	double mSwapUsed =((double)((tempswap_resv) /1024)  /((tempswap_resv) + (tempswap_avail)) ) * 100 * 1024;
	double mtotalswap = (double)((((tempswap_avail) + (tempswap_resv)  )/ temptime ) * mpagesize);

	mtotalswap = (mtotalswap/1024)/1024;
	totalmem = (totalmem/1024)/1024;

	double mfree =totalmem - (( totalmem * mPercentMemUsed)/100) ;
	double mswap_in_use = (mtotalswap *  mSwapUsed) / 100 ;
	double mswap_free =mtotalswap - mswap_in_use ;


#ifndef	 _STANDALONE_
	jfieldID fid;

	jclass cls=env->GetObjectClass(obj);
	if(cls == NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010617:Memory allocation failed");
		print ("getCPUMem : Unable to reference jclass variable");
		return;
	}
	jmethodID mid = env->GetMethodID( cls, "setCPUState" , "(DDDDD)V" );
	if(mid == NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010617:Memory allocation failed");
		print ("getCPUMem : Unable to reference jmethod variable");
		return;
	}

	env->CallVoidMethod(obj , mid ,int_cpustate_diff[SOL_CPU_IDLE] ,int_cpustate_diff[SOL_CPU_USER],int_cpustate_diff[SOL_CPU_KERNEL] , int_cpustate_diff[SOL_CPU_IO],int_cpustate_diff[SOL_CPU_SWAP]);

	cls=env->GetObjectClass(obj);
	if(cls == NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010617:Memory allocation failed");
		print ("getCPUMem : Unable to reference jclass variable");
		return;
	}

	mid = env->GetMethodID( cls, "setMem" , "(DDDD)V" );
	if(mid == NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010617:Memory allocation failed");
		print ("getCPUMem : Unable to reference jmethod variable");
		return;
	}
	env->CallVoidMethod(obj , mid ,totalmem , mfree , mswap_in_use ,mswap_free );

#else
	/*
	 * Call functions:
	 *  - setCPUState
	 *  - setMem
	 */
	setCPUState(int_cpustate_diff[SOL_CPU_IDLE], int_cpustate_diff[SOL_CPU_USER],
		    int_cpustate_diff[SOL_CPU_KERNEL], int_cpustate_diff[SOL_CPU_IO],
		    int_cpustate_diff[SOL_CPU_SWAP]);
	setMem(totalmem , mfree , mswap_in_use ,mswap_free);
#endif
}

/*
  To get load average the kernel is read and the file pointer is moved to the line
  matching "avenrun_Xmin" where X may be 15 1 or 5.
  but to be sure,  first search for line "system_misc" and read the line.
  In that  line	 move to "avenrun_Xmin" and store the reference in a void pointer.
  Type case the pointer as a kstat_named struct and access the element "value.i32"

  EQUIV COMMAND TO TEST :

  uptime.


*/
int internal_loadavg(kstat_ctl_t *kc , float *int_fun_loadavg)
{
	print("internal_loadavg :  function begins ") ;

	kstat_t *k_space =NULL;
	void *val_ptr =NULL;
	float mSysload15 = 0,mSysload5 = 0,mSysload1 = 0;

	k_space = kstat_lookup(kc,"unix",0,"system_misc");
	if (k_space == NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010607:kstat_lookup failed for module loadavg") ;
		print(mErrorMessage);
		return 1;

	}
	if(kstat_read(kc,k_space, NULL ) == -1 )
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010608:kstat_read failed in module loadavg") ;
		print(mErrorMessage);
		return 1;

	}
	if ((val_ptr = (void*)kstat_data_lookup(k_space,"avenrun_15min")) == NULL )
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010609:kstat_data_lookup failed for avgrun15 in module loadavg") ;
		print(mErrorMessage);
		return 1;

	}
	mSysload15  = ((float)((kstat_named_t *) val_ptr)->value.i32)/256 ;
	if ((val_ptr = (void*)kstat_data_lookup(k_space,"avenrun_1min")) == NULL )
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010609:kstat_data_lookup failed for avgrun1 in module loadavg") ;
		print(mErrorMessage);
		return 1;
	}
	mSysload1  = ((float)((kstat_named_t *) val_ptr)->value.i32)/256 ;

	if ((val_ptr = (void*)kstat_data_lookup(k_space,"avenrun_5min")) == NULL )
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010609:kstat_data_lookup failed for avgrun5 in module loadavg") ;
		print(mErrorMessage);
		return 1;
	}
	mSysload5  = ((float)((kstat_named_t *) val_ptr)->value.i32)/256 ;

	int_fun_loadavg[SOL_LOAD_1] = mSysload1;
	int_fun_loadavg[SOL_LOAD_5] = mSysload5;
	int_fun_loadavg[SOL_LOAD_15] = mSysload15;

	return 0;
}

/*
  Java_SolTop10_getLoadAvg calls internal_loadavg function to gather loadaverages and then passes it to the java program by calling setLoadAvg() funtion

*/
#ifndef	 _STANDALONE_
JNIEXPORT void JNICALL Java_com_proactivenet_monitors_SolTop10_SolTop10_getLoadAvg
(JNIEnv *env, jobject obj)
#else
void getLoadAvg(void)
#endif
{
	float int_loadavg[3] = {0,0,0};
	kstat_ctl_t *kstat_t = NULL;

	ZERO(mErrorMessage);
	mutex_lock(&itrMutex);
	kstat_t=kstat_open();
	if (!kstat_t) {
		return;
	}

	if( internal_loadavg(kstat_t , int_loadavg) == 1)
	{
		kstat_close(kstat_t);
		print("getLoadAvg : Error in function internal_loadavg");
		mutex_unlock(&itrMutex);
		return;
	}
	kstat_close(kstat_t);
	mutex_unlock(&itrMutex);

#ifndef	 _STANDALONE_
	jfieldID fid;
	jclass cls=env->GetObjectClass(obj);
	if(cls == NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010617:Memory allocation failed");
		print("getLoadAvg : Unable to reference jclass variable");
		return;
	}
	jmethodID mid = env->GetMethodID(cls, "setLoadAvg" , "(FFF)V" );
	if(mid == NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010617:Memory allocation failed");
		print("getLoadAvg : Unable to reference jmethod variable");
		return;
	}
	env->CallVoidMethod(obj , mid ,int_loadavg[SOL_LOAD_1] ,int_loadavg[SOL_LOAD_5] ,int_loadavg[SOL_LOAD_15] );
#else
	/*
	 * Call to functions:
	 *  - setLoadAvg
	 */
	setLoadAvg(int_loadavg[SOL_LOAD_1] ,int_loadavg[SOL_LOAD_5] ,int_loadavg[SOL_LOAD_15] );
#endif
}

#ifndef	 _STANDALONE_
JNIEXPORT void JNICALL Java_com_proactivenet_monitors_SolTop10_SolTop10_getDate (JNIEnv *env, jobject obj)
#else
void getDate(void)
#endif
{
	char *t_time = internal_date();

#ifndef	 _STANDALONE_
	jfieldID fid;

	print("getDate : function starts");
	ZERO(mErrorMessage);

	jclass cls=env->GetObjectClass(obj);

	if(cls == NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010617:Memory allocation failed");
		print("getDate : Unable to reference jclass variable");
		return;
	}

	jmethodID mid = env->GetMethodID(cls, "setDate" , "(Ljava/lang/String;)V" );

	if(mid == NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010617:Memory allocation failed");
		print("getDate : Unable to reference jmethod variable");
		return;
	}

	jstring temp_time_t = env->NewStringUTF(t_time ? t_time : "<error>");
	if(temp_time_t == NULL)
	{
		snprintf(mErrorMessage, sizeof(mErrorMessage), "26010617:Memory allocation failed");
		print("getDate : Unable to allocate memory for temp_time_t");
		return;
	}

	env->CallVoidMethod(obj,mid,temp_time_t);
	env->DeleteLocalRef(temp_time_t);

	//print("getDate : function ends");
#else
	/*
	 * Call functions:
	 *   - setDate
	 */
	setDate(t_time ? t_time : "<error>");
#endif
}


void display(top top_struct[],int act_num_proc)
{

	for(int i=0;i<act_num_proc;i++)
	{
		cout << top_struct[i].top_pid  <<  "   "
		     <<	 top_struct[i].top_username  <<	 "   "
		     <<	 top_struct[i].top_thread  <<  "   "
		     <<	 top_struct[i].top_priority   <<  "   "
		     <<	 top_struct[i].top_nice	  <<  "	  "
		     <<	 top_struct[i].top_size	  <<  "	  "
		     <<	 top_struct[i].top_ressize   <<	 "   "
		     <<	 top_struct[i].top_state   <<  "   "
		     <<	 top_struct[i].top_time.min << " : " <<	 top_struct[i].top_time.sec  <<	 "   "
		     <<	 top_struct[i].top_cpu	  <<  "	  "
		     <<	 top_struct[i].top_command   <<	 "   "
		     << endl;
	}

}

/**
 * No more using sorting functions.. using command: sort -nr
 */
#if  0
/*
  partly implemented .
  A quasi bubble sort algorithm.
  sorts the processes on cpu
*/


void sort_by_cpu(top top_struct[],int display_num, int act_num_proc)
{
	top temp;
	// print("sort_by_cpu : function starts");
	for(int i=0;i<(act_num_proc-1);i++)
	{
		for(int j=i ; j < (act_num_proc-1) ; j++ )
		{
			if(top_struct[j].top_cpu >  top_struct[j+1].top_cpu )
			{
				temp = top_struct[j];
				top_struct[j] = top_struct[j+1];
				top_struct[j+1] = temp;
			}

		}

	}
	//print("sort_by_cpu : function ends");
}
/*
  void sort_by_cpu(top top_struct[],int display_num, int act_num_proc)
  {
  top temp;
  for(int i=0;i<display_num;i++)
  {
  for(int j=i ; j < (act_num_proc-1) ; j++ )
  {
  if(top_struct[j].top_cpu >=  top_struct[j+1].top_cpu )
  {
  temp = top_struct[j];
  top_struct[j] = top_struct[j+1];
  top_struct[j+1] = temp;
  }

  }

  }

  }
*/
void sort_by_memory(top top_struct[],int display_num, int act_num_proc)
{
	top temp;

	//print("sort_by_mem : function starts");

	for(int i=0;i<(act_num_proc - 1);i++)
	{
		for(int j=i ; j < (act_num_proc-1) ; j++ )
		{
			if(top_struct[j].top_size > top_struct[j+1].top_size )
			{
				temp = top_struct[j];
				top_struct[j] = top_struct[j+1];
				top_struct[j+1] = temp;
			}

		}

	}

	// print("sort_by_mem : function ends");

}

void sort_by_time(top top_struct[],int display_num, int act_num_proc)
{
	top temp;

	//print("sort_by_time : function starts");

	for(int i=0;i<(act_num_proc-1);i++)
	{
		for(int j=i ; j < (act_num_proc-1) ; j++ )
		{
			if(top_struct[j].top_cpu == top_struct[j+1].top_cpu )
			{
				if(top_struct[j].toptime > top_struct[j+1].toptime )
				{
					temp = top_struct[j];
					top_struct[j] = top_struct[j+1];
					top_struct[j+1] = temp;
				}
			}

		}
	}

	//print("sort_by_time : function ends");
}


void sort_by_time1(top top_struct[],int display_num, int act_num_proc)
{
	top temp;

	//print("sort_by_time : function starts");

	for(int i=0;i<(act_num_proc-1);i++)
	{
		for(int j=i ; j < (act_num_proc-1) ; j++ )
		{
			if(top_struct[j].top_size == top_struct[j+1].top_size)
			{
				if(top_struct[j].toptime > top_struct[j+1].toptime )
				{
					temp = top_struct[j];
					top_struct[j] = top_struct[j+1];
					top_struct[j+1] = temp;
				}
			}

		}
	}

	//print("sort_by_time : function ends");
}
void sort(top top_struct[],int display_num, int act_num_proc)
{
//	if(strcasecmp(column_to_sort,"memory") == 0)
//		sort_by_memory(top_struct , display_num,act_num_proc);
//	else

	//print("sort : function starts");

	sort_by_cpu(top_struct , display_num, act_num_proc);
	sort_by_time(top_struct , display_num, act_num_proc);

	//print("sort : function ends");
}
#endif

#ifndef	 _STANDALONE_
JNIEXPORT jstring JNICALL Java_com_proactivenet_monitors_SolTop10_SolTop10_getError (JNIEnv *env, jobject obj)
{
	if(env != NULL)
		return env->NewStringUTF(mErrorMessage);
	return NULL;
}
#endif

/*
  Check code.*/
int main()
{

}

void print(const char *temp)
{
	if (!temp)
		return;
	//fprintf(stdout, "%s\n", temp);
}
