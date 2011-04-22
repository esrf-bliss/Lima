## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libyat.la

libyat_la_LDFLAGS = -version-info 1:0:0
libyat_la_CXXFLAGS = -I$(top_srcdir)/include
libyat_la_SOURCES = \
	Exception.cpp \
	bitsstream/BitsStream.cpp \
	bitsstream/Endianness.cpp \
	network/Address.cpp \
	network/Socket.cpp \
	network/ClientSocket.cpp \
	network/SocketException.cpp \
	plugin/PlugIn.cpp \
	plugin/PlugInUnix.cpp \
	plugin/PlugInManager.cpp \
	threading/PosixThreadingImpl.cpp \
	threading/SharedObject.cpp \
	threading/Barrier.cpp \
	threading/Task.cpp \
	threading/Message.cpp \
	threading/MessageQ.cpp
