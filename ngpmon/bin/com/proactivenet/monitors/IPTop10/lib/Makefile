TOP     = ../../../../../../..

include Makefile.source
include $(TOP)/build/Makefile.c++
JAVAINC = $(STATIC_DIR)/jdk1.3/include
INCLUDE_DIR += -I$(JAVAINC) -I$(JAVAINC)/solaris -I$(APPS3RDPARTY_DIR)/pcap/Solaris/include
LINKOPT += -L$(APPS3RDPARTY_DIR)/pcap/Solaris/lib -R$(APPS3RDPARTY_DIR)/pcap/Solaris/lib -lpcap -lsocket -lnsl -lresolv -lCrun -lCstd
ADDCOPT = -Dsparc -features=no%localfor -features=no%export -features=no%altspell -library=rwtools7,iostream
COPTS = -Bdynamic -G -pic -dy
#COPT += -Bdynamic -G -pic -dy 
libiptop10.so : $(libiptop10_OBJS)
		$(CC) ${libiptop10_OBJS:%.o=$(OBJ_DIR)/%.o} $(COPTS) -o $(TMP_LIB_DIR)/$(TARGET_LIB_SHARED) $(LINKOPT)
