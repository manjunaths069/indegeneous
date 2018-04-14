#!/bin/ksh
# #################################################################### #
# Copyright   : @1999 Proactive Networks Inc.
# File        : fwprocmon.sh
# Description : Returns information on the Diskinfo MO (Managed Object)
# Author      : Vivek Chopra
# Date        : $Date: 1999/03/18 21:29:56 $
# RcsId       : $Id: diskinfo.sh,v 1.1 1999/03/18 21:29:56 chopra Exp $
# #################################################################### #

# Initialization etc. ################################################ #
unset PATH
unalias -a

LD_LIBRARY_PATH_OLD=$LD_LIBRARY_PATH
unset LD_LIBRARY_PATH

CUT=/bin/cut
DF=/bin/df
ECHO=echo             # shell built-in
EXPR=/bin/expr
GREP=/bin/grep
HEAD=/bin/head
SED=/bin/sed
TAIL=/bin/tail
TR=/bin/tr

PATH=/bin:/usr/bin:/usr/sbin:/etc
export PATH
ldlibpath=`cat $PROACTIVEHOME/monitors/monitors/CkpProcessMon/librarypath.conf`
flag=0
for line in $ldlibpath
do
 result=`$ECHO $line | $GREP "^CP" `
 if [ $flag -eq 1 ]; then
   LD_LIBRARY_PATH=$line
   break
 fi
 if [ "XYZ$result" != "XYZ" ]; then
  flag=1
 fi
done
export LD_LIBRARY_PATH

progname=`basename $0`
if [ $# -lt 4 ]
then
  $ECHO "usage $progname -[config|stat] <ip> <timeout> <fwd install dir> <fwd_ip>" >&2
  exit 1
fi


# Firewall Daemon     ################################################ #
# Output:
#    AVAILIBILTY           
#    

AVAILABILITY=0
FWD_INSTALL_DIR=$4
FW=${FWD_INSTALL_DIR}/fw
FWD_IP=$5

FWD_SUCCESS=`$FW stat ${FWD_IP} 2>&1 |   $TAIL -1 | $GREP ${FWD_IP}`
#FWD_AVAIL=`$FW stat ${FWD_IP} 2>&1 |   $TAIL -1 | $GREP 'Failed'` 

# Modified by - Manjunath ###
# The fw stat output has changed in NG-AI (R55) HFA_04, Hotfix 106 - Build 000. The previous
# logic was to GREP for "FAILED" but now we are checking if the second column (Policy) is 
# invalid (presence of Hyphen).
# Modified by - Manjunath ###

FWD_AVAIL=`$FW stat ${FWD_IP} 2>&1 |   $TAIL -1 | $CUT -d ' ' -f 2`        

if [ "${FWD_SUCCESS}" = "" ]
then
  $ECHO "error: could not find fw" 
else
  HAS_DATA=1
  FWD_STATUS=`$FW stat ${FWD_IP} | $TAIL -1 | $TR -s ' '`

  FWD_POLICY=`$ECHO $FWD_STATUS      | $CUT -d ' ' -f 2`
  FWD_POLICY_DATE=`$ECHO $FWD_STATUS | $CUT -d ' ' -f 3`
  FWD_TIME=`$ECHO $FWD_STATUS        | $CUT -d ' ' -f 4`
fi

##########

if [ "$1" = "-stat" ]
then
  if [ "${HAS_DATA}x" = "1x" ]
  then
    if [ "${FWD_AVAIL}x" = "-x" ]
    then
      AVAILABILITY=0
    else
      AVAILABILITY=100
    fi
  else
    AVAILABILITY=0
  fi 
  $ECHO "AVAILABILITY=$AVAILABILITY"
fi

###########
if [ "$1" = "-config" ]
then
  if [ "${HAS_DATA}x" = "1x" ]
  then
    $ECHO "FWD_POLICY=$FWD_POLICY"
    $ECHO "FWD_POLICY_DATE=$FWD_POLICY_DATE $FWD_TIME"
  else
    $ECHO "FWD_POLICY=NO_DATA"
    $ECHO "FWD_POLICY_DATE=NO_DATA"
  fi
fi
unset LD_LIBRARY_PATH
LD_LIBRARY_PATH=$LD_LIBRARY_PATH_OLD
export LD_LIBRARY_PATH
exit 0

