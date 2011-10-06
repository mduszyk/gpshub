# -*- mode: Makefile; -*-
# -----------------------------------------
# project gpshub


#export PATH := /opt/wx/2.8/bin:$(PATH)
#export LD_LIBRARY_PATH := /opt/wx/2.8/lib:$(LD_LIBRARY_PATH)

#_WX = /home/gr/projects/gui/codeblocks/wx
#_WX.LIB = $(_WX)/lib
#_WX.INCLUDE = $(_WX)/include

#_CB = /home/gr/projects/gui/codeblocks/cb/src
#_CB.INCLUDE = $(_CB)/include
#_CB.LIB = $(_CB)/devel



#CFLAGS_C = $(filter-out -include "sdk.h",$(CFLAGS))

# -----------------------------------------

# MAKE_DEP = -MMD -MT $@ -MF $(@:.o=.d)

CFLAGS = -std=c++0x -Wall -fexceptions 
INCLUDES = -Iinclude -Isrc 
LDFLAGS =  -s
RCFLAGS = 
LDLIBS = $(T_LDLIBS) -lpthread  -lstdc++

LINK_exe = gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)
LINK_con = gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)
LINK_dll = gcc -o $@ $^ $(LDFLAGS) $(LDLIBS) -shared
LINK_lib = rm -f $@ && ar rcs $@ $^
COMPILE_c = gcc $(CFLAGS_C) -o $@ -c $< $(MAKEDEP) $(INCLUDES)
COMPILE_cpp = g++ $(CFLAGS) -o $@ -c $< $(MAKEDEP) $(INCLUDES)
COMPILE_rc = windres $(RCFLAGS) -J rc -O coff -i $< -o $@ -I$(dir $<)

%.o : %.c ; $(COMPILE_c)
%.o : %.cpp ; $(COMPILE_cpp)
%.o : %.cxx ; $(COMPILE_cpp)
%.o : %.rc ; $(COMPILE_rc)
.SUFFIXES: .o .d .c .cpp .cxx .rc

all: all.before all.targets all.after

all.before :
	-
all.after : $(FIRST_TARGET)
	
all.targets : Release_target Debug_target 

clean :
	rm -fv $(clean.OBJ)
	rm -fv $(DEP_FILES)

.PHONY: all clean distclean

# -----------------------------------------
# Release_target

Release_target.BIN = bin/Release/gpshub
Release_target.OBJ = src/hubthread/CoordsBroadcastThread.o src/hubthread/CoordsUpdateThread.o src/hubthread/ServerThread.o src/main.o src/server/CmdPkg.o src/server/CommandHandler.o src/server/CommandServer.o src/server/GpsDataServer.o src/socket/Epoll.o src/socket/EpollException.o src/socket/Socket.o src/socket/SocketException.o src/socket/netutil.o src/thread/ConditionVariable.o src/thread/Mutex.o src/thread/RwLock.o src/thread/ScopeLock.o src/thread/ScopeLockRd.o src/thread/ScopeLockWr.o src/thread/SyncHashMap.o src/thread/SyncHashSet.o src/thread/SyncMap.o src/thread/SyncSet.o src/thread/Thread.o src/thread/ThreadException.o src/user/User.o src/user/UserIdGenerator.o src/util/CircularBuffer.o 
DEP_FILES += src/hubthread/CoordsBroadcastThread.d src/hubthread/CoordsUpdateThread.d src/hubthread/ServerThread.d src/main.d src/server/CmdPkg.d src/server/CommandHandler.d src/server/CommandServer.d src/server/GpsDataServer.d src/socket/Epoll.d src/socket/EpollException.d src/socket/Socket.d src/socket/SocketException.d src/socket/netutil.d src/thread/ConditionVariable.d src/thread/Mutex.d src/thread/RwLock.d src/thread/ScopeLock.d src/thread/ScopeLockRd.d src/thread/ScopeLockWr.d src/thread/SyncHashMap.d src/thread/SyncHashSet.d src/thread/SyncMap.d src/thread/SyncSet.d src/thread/Thread.d src/thread/ThreadException.d src/user/User.d src/user/UserIdGenerator.d src/util/CircularBuffer.d 
clean.OBJ += $(Release_target.BIN) $(Release_target.OBJ)

Release_target : Release_target.before $(Release_target.BIN) Release_target.after_always
Release_target : CFLAGS += -O2 -std=c++0x  -Os
Release_target : INCLUDES += 
Release_target : RCFLAGS += 
Release_target : LDFLAGS += -s   
Release_target : T_LDLIBS = 
ifdef LMAKE
Release_target : CFLAGS -= -O1 -O2 -g -pipe
endif

Release_target.before :
	
	
Release_target.after_always : $(Release_target.BIN)
	
$(Release_target.BIN) : $(Release_target.OBJ)
	$(LINK_con)
	

# -----------------------------------------
# Debug_target

Debug_target.BIN = bin/Debug/gpshub
Debug_target.OBJ = src/hubthread/CoordsBroadcastThread.o src/hubthread/CoordsUpdateThread.o src/hubthread/ServerThread.o src/main.o src/server/CmdPkg.o src/server/CommandHandler.o src/server/CommandServer.o src/server/GpsDataServer.o src/socket/Epoll.o src/socket/EpollException.o src/socket/Socket.o src/socket/SocketException.o src/socket/netutil.o src/thread/ConditionVariable.o src/thread/Mutex.o src/thread/RwLock.o src/thread/ScopeLock.o src/thread/ScopeLockRd.o src/thread/ScopeLockWr.o src/thread/SyncHashMap.o src/thread/SyncHashSet.o src/thread/SyncMap.o src/thread/SyncSet.o src/thread/Thread.o src/thread/ThreadException.o src/user/User.o src/user/UserIdGenerator.o src/util/CircularBuffer.o 
DEP_FILES += src/hubthread/CoordsBroadcastThread.d src/hubthread/CoordsUpdateThread.d src/hubthread/ServerThread.d src/main.d src/server/CmdPkg.d src/server/CommandHandler.d src/server/CommandServer.d src/server/GpsDataServer.d src/socket/Epoll.d src/socket/EpollException.d src/socket/Socket.d src/socket/SocketException.d src/socket/netutil.d src/thread/ConditionVariable.d src/thread/Mutex.d src/thread/RwLock.d src/thread/ScopeLock.d src/thread/ScopeLockRd.d src/thread/ScopeLockWr.d src/thread/SyncHashMap.d src/thread/SyncHashSet.d src/thread/SyncMap.d src/thread/SyncSet.d src/thread/Thread.d src/thread/ThreadException.d src/user/User.d src/user/UserIdGenerator.d src/util/CircularBuffer.d 
clean.OBJ += $(Debug_target.BIN) $(Debug_target.OBJ)

Debug_target : Debug_target.before $(Debug_target.BIN) Debug_target.after_always
Debug_target : CFLAGS += -std=c++0x -g -DDEBUG  -Os
Debug_target : INCLUDES += 
Debug_target : RCFLAGS += 
Debug_target : LDFLAGS +=   
Debug_target : T_LDLIBS = -lpthread 
ifdef LMAKE
Debug_target : CFLAGS -= -O1 -O2 -g -pipe
endif

Debug_target.before :
	
	
Debug_target.after_always : $(Debug_target.BIN)
	
$(Debug_target.BIN) : $(Debug_target.OBJ)
	$(LINK_con)
	

# -----------------------------------------
ifdef MAKE_DEP
-include $(DEP_FILES)
endif
