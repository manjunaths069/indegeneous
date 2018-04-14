TOP     = ../../../../../../..

include Makefile.source
include $(TOP)/build/Makefile.c++
JAVAINC = $(STATIC_DIR)/jdk1.2.2_13/include
INCLUDE_DIR += -I$(JAVAINC) -I$(JAVAINC)/solaris 
COPT += -Bdynamic -G -pic -dy -lrwtool -lkstat -Dsparc -lCrun
#COPT += -Bdynamic -G -pic -dy 
libprocmondet.so : $(libprocmondet_OBJS)
