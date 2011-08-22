TARGET = limaadsc.so
OFILES = lima_init_numpy.o siplimaadsccmodule.o siplimaadscAdscCamera.o siplimaadscAdscHwInterface.o siplimaadscAdscBinCtrlObj.o siplimaadscAdscSyncCtrlObj.o siplimaadscAdscDetInfoCtrlObj.o
HFILES = sipAPIlimaadsc.h 

CC = gcc
CXX = g++
LINK = g++
CPPFLAGS = -DNDEBUG -I. -I../core -I../../third-party/Processlib/sip -I/users/blissadm/python/bliss_python/redhate5/lib/python2.6/site-packages/numpy/core/include -I/segfs/bliss/depot/pythonbliss_5.0/installdir/redhate5/python/include/python2.6 -I../../third-party/Processlib/core/include -I../../third-party/Processlib/tasks/include -I../../common/include -I../../hardware/include -I../../control/include -I../../control/software_operation/include -I../../camera/adsc/include
CFLAGS = -pipe -fPIC -O2 -Wall -W
CXXFLAGS = -pthread -g -DWITH_SPS_IMAGE -I. -I../core -I../../third-party/Processlib/sip -I/users/blissadm/python/bliss_python/redhate5/lib/python2.6/site-packages/numpy/core/include -I/segfs/bliss/depot/pythonbliss_5.0/installdir/redhate5/python/include/python2.6 -I../../third-party/Processlib/core/include -I../../third-party/Processlib/tasks/include -I../../common/include -I../../hardware/include -I../../control/include -I../../control/software_operation/include -I../../camera/adsc/include -pipe -fPIC -O2 -Wall -W
LFLAGS = -shared -Wl,-rpath,../../build
LIBS = -L../../build -lpthread -llimaadsc
.SUFFIXES: .c .o .cpp .cc .cxx .C


.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

all: $(TARGET)

$(OFILES): $(HFILES)

$(TARGET): $(OFILES)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OFILES) $(LIBS)

install: $(TARGET)
	@test -d $(DESTDIR)/segfs/bliss/depot/pythonbliss_5.0/installdir/redhate5/python/lib/python2.6/site-packages || mkdir -p $(DESTDIR)/segfs/bliss/depot/pythonbliss_5.0/installdir/redhate5/python/lib/python2.6/site-packages
	cp -f $(TARGET) $(DESTDIR)/segfs/bliss/depot/pythonbliss_5.0/installdir/redhate5/python/lib/python2.6/site-packages/$(TARGET)
	strip $(DESTDIR)/segfs/bliss/depot/pythonbliss_5.0/installdir/redhate5/python/lib/python2.6/site-packages/$(TARGET)
	@test -d $(DESTDIR)/segfs/bliss/depot/pythonbliss_5.0/installdir/redhate5/python/share/sip/lima || mkdir -p $(DESTDIR)/segfs/bliss/depot/pythonbliss_5.0/installdir/redhate5/python/share/sip/lima
	cp -f limaadsc.sip $(DESTDIR)/segfs/bliss/depot/pythonbliss_5.0/installdir/redhate5/python/share/sip/lima/limaadsc.sip
	@test -d $(DESTDIR)/segfs/bliss/depot/pythonbliss_5.0/installdir/redhate5/python/lib/python2.6/site-packages || mkdir -p $(DESTDIR)/segfs/bliss/depot/pythonbliss_5.0/installdir/redhate5/python/lib/python2.6/site-packages
	cp -f limaconfig.py $(DESTDIR)/segfs/bliss/depot/pythonbliss_5.0/installdir/redhate5/python/lib/python2.6/site-packages/limaconfig.py

clean:
	-rm -f $(TARGET)
	-rm -f lima_init_numpy.o
	-rm -f siplimaadsccmodule.o
	-rm -f siplimaadscAdscCamera.o
	-rm -f siplimaadscAdscHwInterface.o
	-rm -f siplimaadscAdscBinCtrlObj.o
	-rm -f siplimaadscAdscSyncCtrlObj.o
	-rm -f siplimaadscAdscDetInfoCtrlObj.o
