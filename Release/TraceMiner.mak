#############################################################################
# Makefile for building: TraceMiner
# Generated by qmake (2.01a) (Qt 4.8.6) on: Fri Feb 26 15:49:50 2016
# Project:  ../TraceMiner/TraceMiner.pro
# Template: app
# Command: /usr/lib/x86_64-linux-gnu/qt4/bin/qmake -spec /usr/share/qt4/mkspecs/linux-g++-64 -o Makefile ../TraceMiner/TraceMiner.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = 
CFLAGS        = -m64 -pipe -O2 -Wall -W $(DEFINES)
CXXFLAGS      = -m64 -pipe -O2 -Wall -W $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++-64 -I../TraceMiner -I../TraceMiner -I.
LINK          = g++
LFLAGS        = -m64 -Wl,-O1
LIBS          = $(SUBLIBS)    
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/lib/x86_64-linux-gnu/qt4/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = ../TraceMiner/TraceMiner.c \
		../TraceMiner/utils.c \
		../TraceMiner/linkedList.c 
OBJECTS       = TraceMiner.o \
		utils.o \
		linkedList.o
DESTDIR       = 
TARGET        = TraceMiner

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


####### Compile

TraceMiner.o: ../TraceMiner/TraceMiner.c ../TraceMiner/utils.h \
		../TraceMiner/linkedList.h \
		../TraceMiner/config.h \
		../TraceMiner/bindvalues.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o TraceMiner.o ../TraceMiner/TraceMiner.c

utils.o: ../TraceMiner/utils.c ../TraceMiner/utils.h \
		../TraceMiner/linkedList.h \
		../TraceMiner/config.h \
		../TraceMiner/bindvalues.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o utils.o ../TraceMiner/utils.c

linkedList.o: ../TraceMiner/linkedList.c ../TraceMiner/linkedList.h \
		../TraceMiner/config.h \
		../TraceMiner/utils.h \
		../TraceMiner/bindvalues.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o linkedList.o ../TraceMiner/linkedList.c

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

