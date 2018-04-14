TOP     = ../../../../../../..

include Makefile.source
include $(TOP)/build/Makefile.c++

INCLUDE_DIR =

COPT = -xarch=v9

getMemoryData : $(getMemoryData_OBJS)
	$(COMPILE_CC) ${getMemoryData_OBJS:%.o=$(OBJ_DIR)/%.o} -o ../conf/getMemoryData