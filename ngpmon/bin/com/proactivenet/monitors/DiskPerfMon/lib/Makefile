TOP     = ../../../../../../..

include Makefile.source
include $(TOP)/build/Makefile.c++
JAVAINC = $(STATIC_DIR)/jdk1.2.2_13/include
INCLUDE_DIR += -I$(JAVAINC) -I$(JAVAINC)/solaris 
COPT += -Bdynamic -G -pic -dy -lrwtool -lkstat -Dsparc -D_REENTRANT -lCrun
#COPT += -Bdynamic -G -pic -dy 
libdiskperfmon.so : $(libdiskperfmon_OBJS)
