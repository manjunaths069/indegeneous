TOP= ../../../..

all: main

include $(TOP)/build/Makefile.local
include $(TOP)/build/Makefile.java
include $(TOP)/build/Makefile.MonList
include Makefile.source

copy_util: 
	@echo copying util files
	@for monName in $(MONITORS) ; \
		do \
			$(RUN_JAVA) -classpath $(TOP)/web/java/com/proactivenet CopyUtil $${monName} ./monitors/;\
		done ;

	@for ddName in $(DETAIL_DIAGNOSTICS) ; \
		do \
			$(RUN_JAVA) -classpath $(TOP)/web/java/com/proactivenet CopyUtil $${ddName} ./monitors/;\
		done ;

main: copy_util install
