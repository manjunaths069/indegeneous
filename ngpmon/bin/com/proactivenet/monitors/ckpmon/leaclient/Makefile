TOP     = ../../../../../../..

include Makefile.source
include $(TOP)/build/Makefile.c++
include $(TOP)/build/Makefile.local

#APPLIB=/usr/pw/pronto/lib
APPLIB=../conf

LIBS = -lthread -lposix4 -lresolv -ldl -lintl -lm  -lc -lw -lkstat -lkvm -lelf 

COPT += -I/engs41/non_sccs/CKPNGsdk/include -I$(NON_SCCS_DIR)/incl4.0 -I. -xtarget=ultra -xs -g -DDEBUG -DUNIXOS -KPIC -mt -xildoff -library=rwtools7,iostream  
COPT += -L$(SC_DIR)/lib/rw7 -L$(SDK_LIB) -L$(APPLIB) 

LINKOPT += $(LIBS) 

leaclient : $(leaclient_OBJS) 
	$(CP) $(CKP_LEA_UTIL) ../conf
	$(CP) $(CKP_LEA_APPI) ../conf
	$(COMPILE_CC) ${leaclient_OBJS:%.o=$(OBJ_DIR)/%.o} -o ../conf/leaclient $(leaclient_LD_LIBS) $(LIBS) 

%.o: %.cc
	$(COMPILE_CC) -c $*.cc -o $(OBJ_DIR)/$@

clean:
	$(RM) $(OBJ_DIR)/*.o6 ../conf/leaclient

