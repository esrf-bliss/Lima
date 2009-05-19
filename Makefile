.PHONY: common hardware camera

all: common hardware control camera

common:
	make -C common

hardware:
	make -C hardware

control:

camera:
	make -C camera
