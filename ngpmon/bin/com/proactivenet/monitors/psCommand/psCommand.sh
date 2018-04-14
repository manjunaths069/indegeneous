#!/bin/sh

EXECUTABLE_NAME="$0"
UNAME=`/bin/uname`

###Based on the OS, set the PS command and its options
if [ $UNAME = "SunOS" ] ; then
    
    ###Solaris has been considered the base.
    PS="/bin/ps -efo user,pid,ppid,s=STATE -o time=CPUTIME -o etime,tty,pcpu,pmem,vsz=VSZ(Kb) -o rss=RSS(Kb) -o args"

elif [ $UNAME = "HP-UX" ] ; then

    ###This has many extra columns like F, S, ADDR, WCHAN etc
    #PS="/bin/ps -efl"

    ###For -o option UNIX95 has to be exported or ps has to be
    ###invoked as: UNIX95= ps -e -o "pcpu args" 
    ###Compared to Solaris: %MEM is not available here
    UNIX95=true
    export UNIX95
    PS="/bin/ps -efo user,pid,ppid,state=STATE -o time=CPUTIME -o etime,tty,pcpu,vsz=VSZ(Kb) -o sz,args"

elif [ $UNAME = "AIX" ] ; then

    ###Following doesn't have PPID, PPID is available with 'l' option
    ###but then User name %CPU and %MEM are not available
    #PS="/bin/ps guw"
    
    ###Compared to Solaris: RSS is not available with -o option
    PS="/bin/ps -eo user,pid,ppid,stat=STATE -o time=CPUTIME -o etime,tty,pcpu,pmem,vsz=VSZ(Kb) -o args"

elif [ $UNAME = "Linux" ] ; then
    
    ###Linux has all the attributes as Solaris.
    ###Command is also same as Solaris apart from '-f' option
    PS="/bin/ps -eo user,pid,ppid,s=STATE -o time=CPUTIME -o etime,tty,pcpu,pmem,vsz=VSZ(Kb) -o rss=RSS(Kb) -o args"

fi

###Execute the Command
$PS | grep -v "user,pid,ppid" | grep -v $EXECUTABLE_NAME

###If there is an error, exit with status 1
if [ $? -ne 0 ] ; then
    echo "Error in executing the $PS command"
    exit 1
fi
