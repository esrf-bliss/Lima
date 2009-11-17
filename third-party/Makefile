
all:	src test

src:	Processlib/sip/Makefile Processlib/sip/processlib.so

test:
	@echo

Processlib/sip/Makefile:
	cd Processlib/sip/ && python configure.py

Processlib_build_libprocesslib.so:
	make -C Processlib/build -j2


Processlib/sip/processlib.so: Processlib_build_libprocesslib.so
	make -C Processlib/sip -j2
clean:
	cd Processlib/sip/ && python clean.py
	make -C Processlib/build clean
