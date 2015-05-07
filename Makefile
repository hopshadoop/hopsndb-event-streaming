# ============================================================================
# Name        : Makefile
# Author      : sri
# Version     : 1.0
# Copyright   : SICS
# Description : Makefile for building the shared library of libhopsndbevent.so
# ============================================================================
## local compilation home/${USER}/workspace/MTVersion/ndbheaders
TARGET		=bin/libhopsndbevent.so

USER            =sri
INC             =ndbheaders
JAVA_HOME=$(shell readlink -f /usr/bin/javac | sed "s:bin/javac::")
INCDIRS         =-I./${INC}/include -I./${INC}/include/storage/ndb  -I./${INC}/include/storage/ndb/ndbapi \
                -I${JAVA_HOME}/include/linux -I${JAVA_HOME}/include


CXX 		=g++

# compiling flags here
CXXFLAGS 	=-Wall -Wextra -Wformat-security  -Woverloaded-virtual -Wno-unused-parameter \
                 -Wno-unused-but-set-variable -Wno-strict-aliasing -fPIC -O3 -g -fabi-version=2 -fno-omit-frame-pointer \
                 -fno-strict-aliasing -DDBUG_OFF $(INCDIRS)
	              
              
LDLIBS		=-lm -lrt -lpthread -lndbclient
###-lndbclient -lmysqlclient -lpthread
LDFLAGS         =-L../lib/native
###LDFLAGS         =-L/home/sri/workspace/Hopsndbeventlistener-V1/mysql-cluster-gpl-7.4.4-linux-glibc2.5-x86_64/lib

         

# change these to set the proper directories where each files shoould be
SRCDIR   	=src
OBJDIR   	=obj
BINDIR   	=bin
INCLUDEDIR 	=include

SOURCES  	:=$(wildcard $(SRCDIR)/*.cpp)
INCLUDES 	:=$(wildcard $(SRCDIR)/*.h)
OBJECTS  	:=$(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
rm       	=rm -f


$(TARGET): $(OBJECTS)
	@$(CXX) -shared $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@
	@echo "\033[1;31m Linking is performing now ....done! \033[0m"
	@echo " "

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -c  $< -o $@
	@echo "\033[22;33m\a Compiling the source file  "$<" successfully! \033[0m"

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(TARGET)
	@echo "Executable removed!"
