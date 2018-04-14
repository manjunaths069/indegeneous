TOP     = ../../../../../../..

include Makefile.source
include $(TOP)/build/Makefile.c++
JAVAINC = $(STATIC_DIR)/jdk1.2.2_13/include
DB2APIINC = $(APPS3RDPARTY_DIR)/db2/include/solaris
INCLUDE_DIR += -I$(JAVAINC) -I$(JAVAINC)/solaris -I$(DB2APIINC) -I.
LIBPATH = -L$(APPS3RDPARTY_DIR)/db2/lib/solaris
COPT = -D_UNIX -Bdynamic -G $(LIBPATH) -pic -ldb2 -lCrun
libSolDB2IntelliscopeMon.so : $(libSolDB2IntelliscopeMon_OBJS)

