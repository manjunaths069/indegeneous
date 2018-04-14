DEPTH:=.
include ${DEPTH}/commondefs

INCLUDES+=-I${DEPTH}/tools -I${DEPTH}/include/NetFlow -I${DEPTH}/include
SUBDIRS:=tools NetFlow NetFlowListener

TARGET:=BMCNetworkFlowAdapter

LDLIBS:=-lNetFlowListener -lNetFlow -ltools 

ifeq (${KERNEL}, SunOS)
	LDLIBS+=-lpthread -lsocket
endif

include ${DEPTH}/commonrules
