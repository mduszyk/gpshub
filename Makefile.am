# gpshub automake Makefile.am

# foreign is to let automake know that it is not a standard GNU package
AUTOMAKE_OPTIONS = foreign subdir-objects

INCLUDES = -Iinclude
#CPPFLAGS = -std=c++0x -Wall -fexceptions
LDFLAGS = -lpthread -lstdc++

if DEBUG
#    AM_CFLAGS = -g3 -DDEBUG -O0 -std=c++0x -Wall -fexceptions
    AM_CXXFLAGS = -g3 -DDEBUG -O0 -std=c++0x -Wall -fexceptions
else
#    AM_CFLAGS = -O2 -std=c++0x -Wall -fexceptions
    AM_CXXFLAGS = -O2 -std=c++0x -Wall -fexceptions
endif

# Define an executable target, which will be installed into the
# directory named by the predefined variable $(bindir).
bin_PROGRAMS = gpshub

# Define the list of source files. The file extension is recognized by
# Automake, and causes it to produce rules which invoke
# the C++ compiler to produce an object file (.o) from each source file. The
# header files (.h) do not result in object files by themselves, but will be
# included in distribution archives of the project.
gpshub_SOURCES = src/hubthread/CoordsBroadcastThread.cpp \
				 src/hubthread/CoordsUpdateThread.cpp \
				 src/hubthread/ServerThread.cpp \
				 src/main.cpp \
				 src/server/CmdPkg.cpp \
				 src/server/CommandHandler.cpp \
				 src/server/CommandServer.cpp \
				 src/server/GpsDataServer.cpp \
				 src/socket/Epoll.cpp \
				 src/socket/EpollException.cpp \
				 src/socket/Socket.cpp \
				 src/socket/SocketException.cpp \
				 src/socket/netutil.cpp \
				 src/thread/ConditionVariable.cpp \
				 src/thread/Mutex.cpp \
				 src/thread/RwLock.cpp \
				 src/thread/ScopeLock.cpp \
				 src/thread/ScopeLockRd.cpp \
				 src/thread/ScopeLockWr.cpp \
				 src/thread/SyncHashMap.cpp \
				 src/thread/SyncHashSet.cpp \
				 src/thread/SyncMap.cpp \
				 src/thread/SyncSet.cpp \
				 src/thread/Thread.cpp \
				 src/thread/ThreadException.cpp \
				 src/thread/ThreadSyncException.cpp \
				 src/user/User.cpp \
				 src/user/UserIdGenerator.cpp \
				 src/util/CircularBuffer.cpp

# Define an independent executable script for inclusion in the distribution
# archive. It will not be installed on an end user's system, however.
dist_noinst_SCRIPTS = autogen.sh

