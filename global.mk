### You should not need to change anything below.
# Compilers
CC=gcc
CXX=g++

# Paths
MAPPING_ROOT=/home/2009/niitomo/project/gmapping-dist
LIBDIR=/home/2009/niitomo/project/gmapping-dist/lib
BINDIR=/home/2009/niitomo/project/gmapping-dist/bin
RDK_KERNELD=/home/2009/niitomo/project/gmapping-dist/kernel

# Build tools
PRETTY=/home/2009/niitomo/project/gmapping-dist/build_tools/pretty_compiler
MESSAGE=/home/2009/niitomo/project/gmapping-dist/build_tools/message
TESTLIB=/home/2009/niitomo/project/gmapping-dist/build_tools/testlib

# QT support
MOC=/usr/lib/qt-3.3/bin/moc
QT_LIB=-L/usr/lib/qt-3.3/lib -lqt-mt
QT_INCLUDE=-I/usr/lib/qt-3.3/include

# KDE support
KDE_LIB=
KDE_INCLUDE=
UIC=/usr/lib/qt-3.3/bin/uic

# Generic makefiles
MAKEFILE_GENERIC=/home/2009/niitomo/project/gmapping-dist/build_tools/Makefile.generic-shared-object
MAKEFILE_APP=/home/2009/niitomo/project/gmapping-dist/build_tools/Makefile.app
MAKEFILE_SUBDIRS=/home/2009/niitomo/project/gmapping-dist/build_tools/Makefile.subdirs


# Flags
CPPFLAGS+= -I/home/2009/niitomo/project/gmapping-dist -DCARMEN_SUPPORT
CXXFLAGS+=
LDFLAGS+=
CARMENSUPPORT=1



include /home/2009/niitomo/project/gmapping-dist/manual.mk

