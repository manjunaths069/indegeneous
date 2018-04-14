#!/bin/sh

if [ -x /usr/bin/nawk ] ; then
    AWK=/usr/bin/nawk
else
    AWK=/usr/bin/awk
fi

DF="/bin/df -k"
ECHO=/usr/bin/echo
EXECUTABLE_NAME="$0"
FIND=/bin/find
GREP=/usr/bin/grep
HEAD=/usr/bin/head
HOSTNAME=`/bin/uname -n`
IFCONFIG="/usr/sbin/ifconfig"
ISAINFO=/usr/bin/isainfo
KERNELARCH=`/bin/uname -m`
KERNELID=`/bin/uname -v`
LASTREBOOT="/bin/who -b"
LS=/usr/bin/ls
NATIVEINSTR=`/bin/isainfo -n`
NETSTAT="/bin/netstat"
OS=`/bin/uname`
PROCESSOR=`/bin/uname -p`
PRTCONF=/usr/sbin/prtconf
PSRINFO=/usr/sbin/psrinfo
RELEASE=`/bin/uname -r`
RUNLEVEL="/bin/who -r"
SED=/bin/sed
SHOWREV=/bin/showrev
SWAPL="/usr/sbin/swap -l"
SWAPS="/usr/sbin/swap -s"
TAIL=/usr/bin/tail
TR=/bin/tr
UPTIME=/bin/uptime
WC=/usr/bin/wc

if [ $OS = "SunOS" -a $PROCESSOR = "sparc" -o $PROCESSOR = "i386" ] ; then

	$ECHO "System Configuration for $HOSTNAME (`$SHOWREV | $GREP Hostid`) running $OS $RELEASE"
	$ECHO
	$ECHO "Kernel"
	$ECHO "======"
	$ECHO "Kernel ID: $KERNELID"
	$ECHO "Kernel architecture: $KERNELARCH"
	$ECHO "Kernel type: `$ISAINFO -kv | $AWK '{print $1}'`"
	$ECHO
	$ECHO "System uptime"
	$ECHO "============="
	$ECHO "Running since: `$UPTIME | $AWK '{print $3,$4,$5}' | $SED -e 's/\,$//` hour(s)"
	$ECHO "Last reboot: `$LASTREBOOT | $AWK '{print $4,$5,$6}`"
	$ECHO
	$ECHO "Run level"
	$ECHO "========="
	$ECHO "Current run level: `$RUNLEVEL | $AWK '{print $3}'`"
	$ECHO
	$LS /var/sadm/patch >/dev/null 2>&1
	RETCODE=$?
	if [ $RETCODE -eq 0 ] ; then
		$ECHO "Last ten patches applied"
		$ECHO "========================="
		#ppathak-Fix for (22419) junk characters appearing in config DD,if num of patches < 10 -start
		filecount=`$LS -l /var/sadm/patch|wc -l`
		if [ $filecount -le 11 ] ; then
			tailcount=`expr $filecount - 1`
			if [ $tailcount -eq 0 ] ; then
				$ECHO "No patches applied !"	
				$ECHO
			else
				$ECHO "Only $tailcount patch(es) applied."	
				$ECHO
			fi	
			$LS -lrt /var/sadm/patch | $TAIL -$tailcount | $AWK '{print $NF}'
			$ECHO
		else
			#$LS -lrat /var/sadm/patch | $TAIL -12 | $HEAD -10 | $AWK '{print $NF}'
			$LS -lrt /var/sadm/patch | $TAIL -10 | $AWK '{print $NF}'
			$ECHO
		fi
		#ppathak-Fix for (22419) junk characters appearing in config DD,if num of patches < 10 -end
	fi
	$ECHO "Files in the /etc directory changed in the last one day"
	$ECHO "======================================================="
	$FIND /etc -mtime -1 -ls 2> /dev/null | sort +8 
	$ECHO
	$ECHO "Processor information"
	$ECHO "====================="
	if [ "xx$RELEASE" = "xx5.10" ] ; then
		$ECHO "Physical processor count: `$PSRINFO -p`"
		TOTALPROC=`$PSRINFO | $WC -l | $TR -d " "`
		$ECHO "Virtual processor count: $TOTALPROC"
	else
		TOTALPROC=`$PSRINFO | $WC -l | $TR -d " "`
		$ECHO "Processor count: $TOTALPROC"
	fi
	$ECHO "Processor(s) online: `$PSRINFO | $GREP "on-line" | $WC -l | $TR -d \" \"` (of $TOTALPROC)"
	$ECHO "Processor address space: `$ISAINFO -b` bits"
	FIRSTPROC=`$PSRINFO | $HEAD -1 | $AWK '{print $1}'`
	PROCFREQ=`$PSRINFO -v $FIRSTPROC | $GREP operates | $AWK '{print $6,$7}'`
	PROCFREQ=`$ECHO $PROCFREQ | $SED -e 's/\,$//'`
	$ECHO "Processor(s) operating at: $PROCFREQ"
	$ECHO "Processor Instruction Set Architecture: $PROCESSOR"
	$ECHO "Native instruction set: $NATIVEINSTR"
	$ECHO
	$ECHO "Physical memory"
	$ECHO "==============="
	$ECHO "`$PRTCONF | $GREP Memory`"
	$ECHO
	SWAPSUMM=`$SWAPS | $AWK -F: '{print $2}'`
	SWAPALLOC=`$ECHO $SWAPSUMM | $AWK -F+ '{print $1}'`
	SWAPRES=`$ECHO $SWAPSUMM | $AWK -F= '{print $1}' | $AWK -F+ '{print $2}' | $TR -d \" \"`
	SWAPUSED=`$ECHO $SWAPSUMM | $AWK -F= '{print $2}' | $AWK -F, '{print $1}' | $TR -d \" \"`
	$ECHO "Swap space usage summary:"
	$ECHO "========================="
	$ECHO "$SWAPALLOC"
	$ECHO "$SWAPRES"
	$ECHO "$SWAPUSED"
	$ECHO
	$ECHO "Status of swap areas:"
	$ECHO "====================="
	$SWAPL
	$ECHO
	$ECHO "Disk files and blocks information:"
	$ECHO "=================================="
	$ECHO
	$DF
	$ECHO
	$ECHO "Network status:"
	$ECHO "==============="
	$ECHO "Network interface parameters"
	$ECHO "----------------------------"
	$IFCONFIG -a
	$ECHO
	$ECHO "State of physical and logical interfaces used for IP traffic:"
	$ECHO "-------------------------------------------------------------"
	$ECHO "`$NETSTAT -ia`"
	$ECHO
	$ECHO "Routing information:"
	$ECHO "--------------------"
	$ECHO "`$NETSTAT -r`"
	$ECHO
	$ECHO "TCP sockets (IPv4) listening for incoming connections"
	$ECHO "-----------------------------------------------------"
	$NETSTAT -an -f inet | $GREP LISTEN

else

	echo "$OS not supported"
	exit 1    

fi

