# gpshub automake src/Makefile.am

AUTOMAKE_OPTIONS = subdir-objects

INCLUDES = -I$(top_srcdir)/include
#CPPFLAGS = -std=c++0x -Wall -fexceptions
AM_LDFLAGS = -lpthread -lstdc++

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
gpshub_SOURCES = \
ComponentRegistry.cpp \
hubthread/CoordsBroadcastThread.cpp \
hubthread/CoordsUpdateThread.cpp \
hubthread/ServerThread.cpp \
main.cpp \
server/CmdPkg.cpp \
server/CommandHandler.cpp \
server/CommandServer.cpp \
server/GpsDataServer.cpp \
socket/Epoll.cpp \
socket/EpollException.cpp \
socket/Socket.cpp \
socket/SocketException.cpp \
socket/netutil.cpp \
thread/ConditionVariable.cpp \
thread/Mutex.cpp \
thread/RwLock.cpp \
thread/ScopeLock.cpp \
thread/ScopeLockRd.cpp \
thread/ScopeLockWr.cpp \
thread/SyncHashMap.cpp \
thread/SyncHashSet.cpp \
thread/SyncMap.cpp \
thread/SyncSet.cpp \
thread/Thread.cpp \
thread/ThreadException.cpp \
thread/ThreadSyncException.cpp \
user/User.cpp \
user/UserIdGenerator.cpp \
util/CircularBuffer.cpp

