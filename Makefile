sub-dirs = third-party common hardware control camera build

all:	src test

include config.inc

include global.inc

config.inc:
	@echo "*** Creating default \"config.inc\" with LImA core"
	@echo "*** Please edit it to activate compilation of hardware modules"
	cp config.inc_default config.inc
	@false

config:
	cd third-party/Processlib/sip; python configure.py
	$(MAKE) -C sip config

include install.inc

