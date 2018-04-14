#!/bin/sh
	
        #################################################################################
	#                                                                               #
	#  The traceroute utility is used to trace the path taken by the ip packet.     #
	#  Traceroute on Unix platforms, writes the output to a file and displays the   #
	#  same.  The file is deleted.                                                  #
	#                                                                               #
	#  Author Padmini S.T.                                     Date: 21 Nov 2003    #
        #################################################################################

TRACEROUTE=""
TRACE_OUT="$3Trace_out_$2"
RM="/bin/rm"
CAT="/bin/cat"
UNAME="/bin/uname"
ECHO="/bin/echo"

        #################################################################################
	#                                                                               #
	#  Check if /bin/rm, /bin/cat and /bin/uname exits.  If not come out.           #
	#                                                                               #
        #################################################################################

if [ ! -f $RM ] ; 
then
     $ECHO "ERROR !!  ERROR !!   $RM not available ---- Exiting"
	exit 1
fi
if [ ! -f $CAT ] ; 
then
     $ECHO "ERROR !!  ERROR !!   $CAT not available ---- Exiting"
	exit 1
fi
if [ ! -f $UNAME ] ; 
then
     $ECHO "ERROR !!  ERROR !!   $UNAME not available ---- Exiting"
	exit 1
fi

        #################################################################################
	#                                                                               #
	#  Get the os and depending on the os, set the traceroute path                  #
	#                                                                               #
        #################################################################################

UNAME=`/bin/uname`
if [ $UNAME = "SunOS" ] ; then

	TRACEROUTE="/usr/sbin/traceroute"

elif [ $UNAME = "HP-UX" ] ; then

	TRACEROUTE="/usr/contrib/bin/traceroute"

elif [ $UNAME = "AIX" ] ; then

	TRACEROUTE="/usr/bin/traceroute"

elif [ $UNAME = "Linux" ] ; then

	TRACEROUTE="/usr/sbin/traceroute"

fi
        #################################################################################
	#                                                                               #
	#  Check if the tracerout utility exists.                                       #
	#                                                                               #
        #################################################################################
if [ ! -f $TRACEROUTE ]
then
     $ECHO "ERROR !!  ERROR !!   $TRACEROUTE not available ---- Exiting"
     exit 1
fi

TRACEROUTE="$TRACEROUTE $1"

        #################################################################################
	#                                                                               #
	#  Execute the traceroute command and write it to a file                        #
	#                                                                               #
        #################################################################################

$TRACEROUTE 1>$TRACE_OUT 2>&1
        #################################################################################
	#                                                                               #
	#  Check if the output file exists. If so, cat the output and remove the file   # 
	#                                                                               #
        #################################################################################
if [ -f $TRACE_OUT ]
then
	$CAT $TRACE_OUT
	$RM  $TRACE_OUT
fi
