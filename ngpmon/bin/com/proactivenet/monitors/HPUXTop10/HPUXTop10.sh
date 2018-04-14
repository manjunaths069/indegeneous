#!/bin/sh
	################################################################################# 
	#                                                                               #
	#  The `top` command utility is used to show top 10 processes by cpu/memory.    #
	#  Top on HP-UX does not have a flag to sort by memory, hence we use the        #
	#  our own logic to sort by memory.                                             #
	#  Also, top output is different for single and multi cpu machine.              #
	#  If it is a single cpu machine, then, the first column is TTy - terminal      #
	#  interface used by the process. If it is a multi cpu machine, then the first  #
	#  column is CPU Number - Processor number on which the process is executing.   # 
	#  Hence, if the output if for top 10 processes by memory,                      #
	#  	- on a single cpu - sorting is on the 6th column (Size of the process)  #
	#	- on a multi  cpu - sorting is on the 7th column (Size of the process)  # 
	#                                                                               #
	################################################################################# 
CAT="/bin/cat"
IO_SCAN="/usr/sbin/ioscan"
WC="/bin/wc"
HEAD="/bin/head"
PS="/bin/ps"
RM="/bin/rm"
SORT="/bin/sort"
TAIL="/bin/tail"
TOP="$3"
GREP="/bin/grep"
LS="/bin/ls"
AWK="/bin/awk"
ECHO="/bin/echo"
EXPR="/bin/expr"
DATE="/bin/date"

PROCESSOR="processor"
PROCESSORC="'^processor'"
CPU="cpu"
MEMORY="memory"
FILE_NAME="Top_Output"
ERROR_FILE="Error_Output"
VALUE_ZERO="0"
VALUE_ONE="1"
VALUE_SIX="6"
VALUE_SEVEN="7"
VALUE_NINE="9"
VALUE_TEN="10"
VALUE_ELEVEN="11"
FIRST_ARG=""
SECOND_ARG=""
NUM=""
TIME_STAMP_APPEND=""
NO="no"
YES="yes"
TO_EXE=""
NUM_PROCESSORS=""
HEAD_MULTI=""
TAIL_MULTI=""
NUM_PROCESSES=""

	################################################################################# 
	#                                                                               #
	#  Function : check_Processor                                                   #
	#                                                                               #
	#  This function is used to check if the no of processors is valid.             #
	#  If the number of processors is less than 1, we come out exiting out.         #
	#                                                                               #
	################################################################################# 
function check_Processor 
{
###
# Get the number of processors using /ioscan
###
TO_EXE=`$IO_SCAN -fnkC  $PROCESSOR | $GREP -c $PROCESSOR`
NUM_PROCESSORS=$TO_EXE

	if [ $NUM_PROCESSORS  -le  $VALUE_ZERO ]
	then
		$ECHO "Invalid Number of Processors. "
		exit 1
	fi
HEAD_MULTI=`$EXPR $VALUE_ELEVEN + $NUM_PROCESSORS`
TAIL_MULTI=`$EXPR $VALUE_ONE + $HEAD_MULTI`

###
# Get the number of processes using ps.
###
NUM_PROCESSES=`$PS -ef | $WC -l`
}

	################################################################################# 
	#                                                                               #
	#  Function : check_Error                                                       #
	#                                                                               #
	#  This function is used to check if some error has occured when executing top. #
	#  First check if the error file exists.  If it exits, then check the size of   #
	#  the error file.  If the size of the file is not equal to zero, then          #
	#  echo the error message, invokes the remove_Files function.                   #
	#  It then exits.                                                               #
	#                                                                               #
	################################################################################# 
function check_Error
{
if [ -f $ERROR_FILE ]
then
	FILE_SIZE=`($LS -l  "$ERROR_FILE" | $AWK '{ print $5 }')`
	if [ $FILE_SIZE  -ne $VALUE_ZERO ]
	then
		$ECHO "Error while executing top command" 
		$RM $ERROR_FILE
		if [ -f $FILE_NAME ]
		then
			$RM $FILE_NAME	 
		fi
		exit 1
	fi
fi
}

	################################################################################# 
	#                                                                               #
	#  Function : remove_Files                                                      #
	#                                                                               #
	#  This function checks if the error file exits and if found, removes it.       #
	#  It checks if the top output files exits and if found, removes it.            #
	#                                                                               #
	################################################################################# 

function remove_Files
{
if [ -f $ERROR_FILE ]
then
	$RM $ERROR_FILE
fi
if [ -f $FILE_NAME ]
then
	$RM $FILE_NAME	 
fi
}

	################################################################################# 
	#                                                                               #
	#  Function : check_Inputs                                                      #
	#                                                                               #
	#  Check if the input attributes are valid.  The inputs are                     #
	#   	- flag - which says cpu or memory                                       #
	#       - no of processes to be displayed                                       #
	#                                                                               #
	#  If the first input is neither cpu or memory, it is defaulted to cpu.         #
	#  If the second input is not a +ve number, it is defaulted to 10.              # 
	#                                                                               #
	################################################################################# 

function check_Inputs
{
if [ $1 = "cpu" ]
then
	FIRST_ARG="cpu"
else 
	if [ $1 = "memory" ]
	then
		FIRST_ARG="memory"
	else 
		$ECHO "HPUX Top Processes: Invalid Sort type, defaulting to CPU" >> ../logs/debug/DetailDiagnostics.log
		FIRST_ARG="cpu"
	fi
fi
$ECHO $2 | $GREP "^[0-9][0-9]*$" > /dev/null && NUM="yes" || NUM="no"
if [ "$NUM" = "$NO" ]
then
	SECOND_ARG=10
	$ECHO "HPUX Top Processes: Invalid Numeric type, defaulting to 10" >> ../logs/debug/DetailDiagnostics.log
else
	if [ $2 -le 0 ]
	then
		$ECHO "HPUX Top Processes: Invalid Numeric type, defaulting to 10" >> ../logs/debug/DetailDiagnostics.log
		SECOND_ARG=10
	else
		SECOND_ARG=$2
	fi
fi

}

	################################################################################# 
	#                                                                               #
	#  Function : set_Log_Files                                                     #
	#                                                                               #
	#  This function is used to append the timestamp for the output log and error   #
	#  files.                                                                       #
	#                                                                               #
	################################################################################# 
function set_Log_Files
{
ARG="_$1"
TIME_STAMP_APPEND="_$2"
	FILE_NAME=$FILE_NAME$TIME_STAMP_APPEND$ARG
	ERROR_FILE=$ERROR_FILE$TIME_STAMP_APPEND$ARG
}

	################################################################################# 
	#                                                                               #
	#  Function : check_Utilities                                                   #
	#                                                                               #
	#  This function is used to check the utilities used by this shell script.      #
 	#  If any of the utility is not found, then it exits.                           #
	#                                                                               #
	################################################################################# 
function check_Utilities
{
array[0]=$CAT
array[1]=$IO_SCAN
array[2]=$WC
array[3]=$HEAD
array[4]=$PS
array[5]=$RM
array[6]=$SORT
array[7]=$TAIL
array[8]=$TOP
array[9]=$GREP
array[10]=$LS
array[11]=$AWK
array[12]=$ECHO
array[13]=$EXPR
array[14]=$DATE

I=0
while [ $I -le 14 ] ; do
a=${array[$I]}
	if [ ! -f $a ]
	then
		$ECHO "Error: $a not available. "
		remove_Files
		exit 1	
	fi

I=$(( $I+1 ))
done
}
## The Execution Begins Here
###
#For multi cpu machine, we have to find the number 
###

check_Inputs $1 $2
check_Utilities
set_Log_Files $1 $4
check_Processor

`$TOP -d 1 -n $NUM_PROCESSES -f $FILE_NAME 2> $ERROR_FILE`

check_Error
  
if [ $NUM_PROCESSORS -eq $VALUE_ONE ]
then

	$HEAD -$VALUE_TEN $FILE_NAME

	if [ $FIRST_ARG = $CPU ]
	then
		$TAIL +$VALUE_ELEVEN $FILE_NAME | $HEAD -$SECOND_ARG
	else
		$TAIL +$VALUE_ELEVEN $FILE_NAME | $SORT -r -k$VALUE_SIX -k$VALUE_NINE| $HEAD -$SECOND_ARG
	fi
else
	$HEAD -$HEAD_MULTI $FILE_NAME

	if [ $FIRST_ARG = $CPU ]
	then
		$TAIL +$TAIL_MULTI $FILE_NAME | $HEAD -$SECOND_ARG
	else
		$TAIL +$TAIL_MULTI $FILE_NAME | $SORT -r -k$VALUE_SEVEN -k$VALUE_TEN| $HEAD -$SECOND_ARG
	fi
fi
remove_Files
		
