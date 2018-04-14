DEPTH:=..
include ${DEPTH}/commondefs

INCLUDES+=-I${DEPTH}/tools -I${DEPTH}/include/NetFlow -I${DEPTH}/include

ifeq (${OSNAME},WINDOWS)
	TARGET:=libtools.lib
else
	TARGET:=libtools.a
endif

include ${DEPTH}/commonrules
