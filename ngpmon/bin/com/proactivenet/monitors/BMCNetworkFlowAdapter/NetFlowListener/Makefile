DEPTH:=..
include ${DEPTH}/commondefs

INCLUDES+=-I${DEPTH}/tools -I${DEPTH}/include/NetFlow -I${DEPTH}/include

ifeq (${OSNAME},WINDOWS)
	TARGET:=libNetFlowListener.lib
else
	TARGET:=libNetFlowListener.a
endif

include ${DEPTH}/commonrules
