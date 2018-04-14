#define SOL_LOAD_1 0
#define SOL_LOAD_5 1
#define SOL_LOAD_15 2

#define SOL_FREEMEM 0
#define SOL_SWAP_RESV 1
#define SOL_SWAP_AVAIL 2
#define SOL_TIME 3

#define SOL_CPU_IDLE 0
#define SOL_CPU_USER 1
#define SOL_CPU_KERNEL 2
#define SOL_CPU_STATES 3
#define SOL_CPU_IO 4
#define SOL_CPU_SWAP 5
#define SOL_CPU_NUM 6

#define POLL_DELAY 3


struct display_time
{
	int hr;
	int min;
	int sec;
};

#define  MAX_USER_LEN   16
#define  MAX_STATE_LEN  10
#define  MAX_FNAME_LEN  20

#define  QUOTE_(x)   #x
#define  QUOTE(x)    QUOTE_(x)
#define  FMT_LEN(x)  QUOTE(x)

#define  SUMMARY_FMT    "%*s %d %d %d %d %d %d %d"
#define  MEM_SORT_FMT   "%f %f %d:%d:%d %f %d %d %d %" FMT_LEN(MAX_STATE_LEN) \
                        "s %" FMT_LEN(MAX_USER_LEN) "s %d %" FMT_LEN(MAX_FNAME_LEN) "s"
#define  CPU_SORT_FMT   "%f %d:%d:%d %f %f %d %d %d %" FMT_LEN(MAX_STATE_LEN) \
                        "s %" FMT_LEN(MAX_USER_LEN) "s %d %" FMT_LEN(MAX_FNAME_LEN) "s"

struct top
{
	int top_pid;
	char top_username[MAX_USER_LEN+1];
	int top_thread;
	int top_priority;
	int top_nice;
	float top_size;
	float top_ressize;
	char top_state[MAX_STATE_LEN+1];
	display_time top_time;
	long toptime;
	float top_cpu;
	char top_command[MAX_FNAME_LEN+1];
	int swap(char *msort , int i1, int i2);
};

struct CpuState
{
	ulong idle;
	ulong user;
	ulong kernel;
	ulong stats;
	ulong wait_io;
	ulong wait_swap;
};
