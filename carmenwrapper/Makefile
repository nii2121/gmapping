OBJS= carmenwrapper.o
APPS= 

LIBS+= -L$(CARMEN_HOME)/lib -lsimulator_interface -lrobot_interface -llaser_interface -lglobal -lipc -lm -lutils -lsensor_range -llog -lscanmatcher -lpthread
CPPFLAGS+=-I../sensor -I$(CARMEN_HOME)/include

-include ../global.mk

ifeq ($(CARMENSUPPORT), 0)
OBJS=
	-include ../build_tools/Makefile.app
else
	-include ../build_tools/Makefile.generic-shared-object
endif
