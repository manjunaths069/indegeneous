TOP     = ../../../../../../..

include Makefile.source
include $(TOP)/build/Makefile.c++
JAVAINC = $(STATIC_DIR)/jdk1.2.2_13/include
INCLUDE_DIR += -I$(JAVAINC) -I$(JAVAINC)/solaris -I/usr/lib
COPT += -G -pic -dy -lrwtool -lkstat -Dsparc -lCrun
#COPT += -Bdynamic -G -pic -dy -lkstat
libcpumon.so : $(libcpumon_OBJS)
