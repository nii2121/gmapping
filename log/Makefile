OBJS=configuration.o carmenconfiguration.o sensorlog.o sensorstream.o
APPS=log_test scanstudio2carmen

LDFLAGS+=-lsensor_base -lsensor_odometry -lsensor_range
CPPFLAGS+=-I../sensor 

-include ../global.mk
-include ../build_tools/Makefile.generic-shared-object

