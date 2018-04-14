#ifndef _OUR_OLD_PROCFS_H_
#define _OUR_OLD_PROCFS_H_
#include <procfs.h>
#define	PIOC		('q'<<8)
#define	PIOCPSINFO	(PIOC|30)	/* get ps(1) information */
typedef struct prpsinfo {
	char	pr_state;	/* numeric process state (see pr_sname) */
	char	pr_sname;	/* printable character representing pr_state */
	char	pr_zomb;	/* !=0: process terminated but not waited for */
	char	pr_nice;	/* nice for cpu usage */
	u_long	pr_flag;	/* process flags */
	uid_t	pr_uid;		/* real user id */
	gid_t	pr_gid;		/* real group id */
	pid_t	pr_pid;		/* unique process id */
	pid_t	pr_ppid;	/* process id of parent */
	pid_t	pr_pgrp;	/* pid of process group leader */
	pid_t	pr_sid;		/* session id */
	caddr_t	pr_addr;	/* physical address of process */
	long	pr_size;	/* size of process image in pages */
	long	pr_rssize;	/* resident set size in pages */
	caddr_t	pr_wchan;	/* wait addr for sleeping process */
	timestruc_t pr_start;	/* process start time, sec+nsec since epoch */
	timestruc_t pr_time;	/* usr+sys cpu time for this process */
	long	pr_pri;		/* priority, high value is high priority */
	char	pr_oldpri;	/* pre-SVR4, low value is high priority */
	char	pr_cpu;		/* pre-SVR4, cpu usage for scheduling */
	o_dev_t	pr_ottydev;	/* short tty device number */
	dev_t	pr_lttydev;	/* controlling tty device (PRNODEV if none) */
	char	pr_clname[PRCLSZ];	/* scheduling class name */
	char	pr_fname[PRFNSZ];	/* last component of execed pathname */
	char	pr_psargs[PRARGSZ];	/* initial characters of arg list */
	short	pr_syscall;	/* system call number (if in syscall) */
	short	pr_fill;
	timestruc_t pr_ctime;	/* usr+sys cpu time for reaped children */
	u_long	pr_bysize;	/* size of process image in bytes */
	u_long	pr_byrssize;	/* resident set size in bytes */
	int	pr_argc;	/* initial argument count */
	char	**pr_argv;	/* initial argument vector */
	char	**pr_envp;	/* initial environment vector */
	int	pr_wstat;	/* if zombie, the wait() status */
			/* The following percent numbers are 16-bit binary */
			/* fractions [0 .. 1] with the binary point to the */
			/* right of the high-order bit (one == 0x8000) */
	u_short	pr_pctcpu;	/* % of recent cpu time, one or all lwps */
	u_short	pr_pctmem;	/* % of of system memory used by the process */
	uid_t	pr_euid;	/* effective user id */
	gid_t	pr_egid;	/* effective group id */
	id_t	pr_aslwpid;	/* lwp id of the aslwp; zero if no aslwp */
	long	pr_filler[7];	/* for future expansion */
} prpsinfo_t;
#endif
