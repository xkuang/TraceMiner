#############################################################################
# Makefile for building: TraceMiner
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = 
CFLAGS        = -m32 -pipe -O2 -Wall -W $(DEFINES)
CXXFLAGS      = -m32 -pipe -O2 -Wall -W $(DEFINES)
INCPATH       = -I../TraceMiner -I.
LINK          = g++
LFLAGS        = -m32 -Wl,-O1
LIBS          = $(SUBLIBS)    
DEL_FILE      = rm -f
DEL_DIR       = rmdir

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

