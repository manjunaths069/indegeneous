TOP     = ../../../../../../..

include Makefile.source
include $(TOP)/build/Makefile.c++

JAVAINC = $(STATIC_DIR)/jdk1.2.2_13/include
INCLUDE_DIR += -I$(JAVAINC) -I$(JAVAINC)/solaris
COPT += -G -D_REENTRANT -lCrun

libnfsmon.so : $(libnfsmon_OBJS)



