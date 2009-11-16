-include ../global.mk

CXXFLAGS += -O0 -ggdb

OBJS= gsp_thread.o qparticleviewer.o qgraphpainter.o gfs2img.o
APPS= gfs_simplegui
#APPS= gfs_nogui gfs_simplegui gfs2img

LDFLAGS+=  $(QT_LIB) $(KDE_LIB) -lutils -lsensor_range -llog -lgridfastslam -lpthread -lcarmenwrapper -lconfigfile
#CPPFLAGS+= -I../sensor $(QT_INCLUDE) $(KDE_INCLUDE) -I$(CARMEN_HOME)/include
CPPFLAGS+= -I../sensor $(QT_INCLUDE) $(KDE_INCLUDE) -I$(CARMEN_HOME)/include


-include ../build_tools/Makefile.generic-shared-object



