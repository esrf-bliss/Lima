
all:	src test

src:	processlib.src sps.src

processlib.src:	Processlib/sip/Makefile Processlib/sip/processlib.so

Processlib/sip/Makefile:
	cd Processlib/sip/ && python configure.py

Processlib_build_libprocesslib.so:
	make -C Processlib/build -j2

Processlib/sip/processlib.so: Processlib_build_libprocesslib.so
	make -C Processlib/sip -j2

sps.src:	Sps/Src/sps.o

Sps/Src/sps.o:	Sps/Src/sps.c Sps/Include/sps.h
	make -C Sps/Src CPPFLAGS=-I../Include CFLAGS=-fPIC sps.o

test:
	@echo

clean:
	cd Processlib/sip/ && python clean.py
	make -C Processlib/build clean
