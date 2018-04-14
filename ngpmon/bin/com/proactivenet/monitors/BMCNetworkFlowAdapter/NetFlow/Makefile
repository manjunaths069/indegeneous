DEPTH:=..
include ${DEPTH}/commondefs

INCLUDES+=-I${DEPTH}/tools -I${DEPTH}/include/NetFlow -I${DEPTH}/include

ifeq (${OSNAME},WINDOWS)
	TARGET:=libNetFlow.lib
else
	TARGET:=libNetFlow.a
endif

include ${DEPTH}/commonrules
