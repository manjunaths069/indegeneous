# This makefile is specially created in order to satisfy the
# requirments for building a shared library which refers a bunch
# of static archive libraries.
#
#
TOP     = ../../../../../../..

include $(TOP)/build/Makefile.local
include $(TOP)/build/Makefile.c++

OBJ_DIR  = $(TMP_DIR)/obj/$(subst $(TOP_FULLNAME),,$(PWD))

JAVAINC = $(STATIC_DIR)/jdk1.2.2_13/include
INCLUDE_DIR += -I$(JAVAINC) -I$(JAVAINC)/solaris -I.
COPT = +p -g -w -ptr$(TMP_DIR) -Ddebug -Dsparc -features=no%localfor -features=no%bool -features=no%export -features=no%mutable -features=no%altspell -library=iostream -DSOLBUILD -Bdynamic -pic -I$(APPS3RDPARTY_DIR)/bmc/Solaris/include
LIBPATH = -L$(APPS3RDPARTY_DIR)/bmc/Solaris/lib
#SHAREDLIBS = -lpemapi -lhandlers -lsocket -lnsl -ldl /usr/lib/libC.so.5
SHAREDLIBS = -lpemapi -lhandlers -lmls -lsocket -lnsl -ldl -lCrun
LINKOPTS = -G -pic $(LIBPATH)
LINKCC = $(STATIC_DIR)/ws6.2/SUNWspro/WS6U2/bin/CC
LINK = /usr/ccs/bin/ld



SRC_FILE = PemApiWrapper.cc
SHARED_LIB_FILE = libpemapiwrapper.so
OBJ_FILE = PemApiWrapper.o
LIBOBJS = PemApiWrapper.o

install: $(SHARED_LIB_FILE)

$(SHARED_LIB_FILE):$(LIBOBJS)
	$(LINKCC) $(COPT) $(LINKOPTS) -o $(TOP)/lib/$(SHARED_LIB_FILE) \
		$(OBJ_DIR)/$(OBJ_FILE) $(SHAREDLIBS)

$(OBJ_FILE) : $(SRC_FILE)
	mkdir -p $(OBJ_DIR)
	$(LINKCC) $(INCLUDE_DIR) $(COPT) -o $(OBJ_DIR)/$(OBJ_FILE) \
		 -c $(SRC_FILE)
