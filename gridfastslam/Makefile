OBJS= motionmodel.o gridslamprocessor_tree.o gridslamprocessor.o gfsreader.o
APPS= gfs2log gfs2rec gfs2neff 

LDFLAGS+= -lutils -lsensor_range -llog -lscanmatcher
CPPFLAGS+=-I../sensor

-include ../global.mk
-include ../build_tools/Makefile.generic-shared-object
