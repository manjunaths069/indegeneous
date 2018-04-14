TOP     = ../../../../../../..

include Makefile.source
include $(TOP)/build/Makefile.c++
JAVAINC = $(STATIC_DIR)/jdk1.2.2_13/include
INCLUDE_DIR += -I$(JAVAINC) -I$(JAVAINC)/solaris 
COPT += -DSOLBUILD -D_REENTRANT -Bdynamic  -G -pic -lsocket -lnsl -lpthread -lCrun
#COPT += -Bdynamic -G -pic -dy 
libSolNodePingMon.so : $(libSolNodePingMon_OBJS)
