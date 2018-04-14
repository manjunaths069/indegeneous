#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <procfs.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <string.h>

typedef struct test {
    char sz1[8];
    pid_t pr_pid;
    pid_t pr_ppid;
    pid_t pr_pgid;
    pid_t pr_sid;
    uid_t pr_uid;
    uid_t pr_euid;
    gid_t pr_gid;
    gid_t pr_egid;
    uintptr_t pr_addr;
	//char sz[44];
    size_t pr_size;
    size_t pr_rssize;
} test;

typedef struct _proc_state {
        int  iState;
        char sState[12];
} proc_state;
const proc_state StateArr[] = {
        { 'S', "sleep" },
        { 'O', "cpu" },
        { 'R', "run" },
        { 'Z', "zombie" },
        { 'T', "stopped" },
        { '0', "<unknown>"}
};

#define  MAX_TIME_LEN  32

void convertState(const int state, char *sState)
{
        for (int i = 0; i < (sizeof(StateArr)/sizeof(proc_state)); ++i) {
                if (StateArr[i].iState == state) {
                        sprintf(sState, StateArr[i].sState);
                }
        }
}

void convertSecToHMS(const uint & secs, char *str)
{
        int hr, min, sec;
        sec = secs%60;
        min = secs/60;
        min = min%60;
        hr  = min/60;
        snprintf(str, MAX_TIME_LEN, "%0d:%0d:%0d", hr, min, sec);
}

#define FRC2PCT(num) ((double)num/0x8000*100)

int main(int argc, char *argv[])
{
	int i=1;
        int fd;
	char file[32];
	DIR *dir;
	struct dirent *dp;
	struct psinfo  psi;
        int nProc = 0;
        char timeHMS[MAX_TIME_LEN+1] = {0};
        char user[64] = {0};
        char state[12] = {0};
	struct passwd *pass;
        int nTotal, nSleep, nRunnable, nRunning, nZombie, nStopped;
        enum ProcType { TOTAL=0, SLP, CPU, RUN, ZOMB, STOP, UNKN, PROC_TYPES };
        int nProcs[PROC_TYPES] = {0};

        //total_process , sleep_state , runnable_state , zombie_state , running_state,stopped_state);

        if (argc < 2) {
                char *sortField;
                sortField = getenv("SORT_ORDER");

                dir = opendir("/proc");
                if (!dir) {
                        return 1;
                }
                while ((dp = readdir(dir))) {
                        if (dp->d_name[0] == '.') {
                                continue;
                        }
                        snprintf(file, sizeof(file), "/proc/%s/psinfo", dp->d_name);
                        fd = open(file, O_RDONLY);
                        if (fd < 0) {
                                continue;
                        }
                        if (read(fd, &psi, sizeof(psi)) <= 0) {
                                close(fd);
                                continue;
                        }
                        close(fd);
                        convertSecToHMS(psi.pr_time.tv_sec, timeHMS);
                        pass = getpwuid(psi.pr_uid);
                        if (pass && pass->pw_name) {
                                snprintf(user, sizeof(user), "%s", pass->pw_name);
                        } else {
                                snprintf(user, sizeof(user), "%d", psi.pr_uid);
                        }
                        switch (psi.pr_lwp.pr_sname) {
                            case 'S': snprintf(state, sizeof(state), "sleep"); nProcs[SLP]++; break;
                            case 'O': snprintf(state, sizeof(state), "cpu"); nProcs[CPU]++; break;
                            case 'R': snprintf(state, sizeof(state), "run"); nProcs[RUN]++; break;
                            case 'Z': snprintf(state, sizeof(state), "zombie"); nProcs[ZOMB]++; break;
                            case 'T': snprintf(state, sizeof(state), "stopped"); nProcs[STOP]++; break;
                            default : snprintf(state, sizeof(state), "<unknown>"); nProcs[UNKN]++; break;
                        }
                        nProcs[TOTAL]++;
                        //convertState(psi.pr_lwp.pr_sname, state);
                        if (sortField && !strcmp(sortField, "MEMORY")) {
                                fprintf(stdout, "%f %f %s %f %d %d %d %s %s %d %s\n",
                                        psi.pr_size/1024.0, psi.pr_rssize/1024.0, timeHMS, FRC2PCT(psi.pr_pctcpu),
                                        psi.pr_nlwp, psi.pr_lwp.pr_pri, psi.pr_lwp.pr_nice - 20,
                                        state, user, psi.pr_pid, psi.pr_fname);
                        } else {
                                fprintf(stdout, "%f %s %f %f %d %d %d %s %s %d %s\n",
                                        FRC2PCT(psi.pr_pctcpu), timeHMS, psi.pr_rssize/1024.0, psi.pr_size/1024.0,
                                        psi.pr_nlwp, psi.pr_lwp.pr_pri, psi.pr_lwp.pr_nice - 20,
                                        state, user, psi.pr_pid, psi.pr_fname);
                        }
                }
                closedir(dir);
                fprintf(stdout, "SUMMARY: %d %d %d %d %d %d %d\n",
                        nProcs[TOTAL], nProcs[SLP], nProcs[CPU], nProcs[RUN],
                        nProcs[ZOMB], nProcs[STOP], nProcs[UNKN]);
        } else {
                char name[100];
                FILE *fp1;
                for (;i<argc;i++)
                {
                        sprintf(name, "/proc/%s/psinfo", argv[i]);
                        fp1 = fopen(name, "rb");
                        if(fp1)
                        {
				int ret;
                                ret = fread((void*)&psi,sizeof(psi),1,fp1);
                                fclose(fp1);
				if (ret > 0) {
                        		printf("%d-%lu-%lu\n",psi.pr_pid, psi.pr_size, psi.pr_rssize);
				}
                        }
                }
        }
	return 0;
}
