
include ../config.inc

############################
# LIMA - CORE
############################

CORE_LDFLAGS	:= -L../third-party/Processlib/build
CORE_LDLIBS	:= -lprocesslib

ifneq ($(COMPILE_CBF_SAVING),0)
CORE_LDFLAGS += -L../third-party/CBFLib/lib
CORE_LDLIBS += -lcbf
endif

ifneq ($(COMPILE_CORE),0)

name		:= core
core-objs	:= ../common/src/Common.o \
		   ../hardware/src/Hw.o \
		   ../control/src/Ct.o
version-file	:= ../common/VERSION
core-flags	:= $(CORE_LDFLAGS)
core-deps	:= $(CORE_LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)

core-base	:= liblima$(name).so
core-full	:= $(core-base).$(full-version)
core-maj	:= $(core-base).$(maj-version)
core-flags	+= -fPIC -Wl,-soname,$(core-maj)

$(core-full):		$(core-objs)
	$(CXX) -shared -o $@ $(core-flags) $+ $(core-deps)
	rm -f $(core-maj);  ln -s $(core-full) $(core-maj)
	rm -f $(core-base); ln -s $(core-maj)  $(core-base)

build_targets := $(core-full)

endif

LDFLAGS		:= $(CORE_LDFLAGS) -L.
LDLIBS		:= $(CORE_LDLIBS) -llimacore


############################
# LIMA - SIMULATOR
############################

ifneq ($(COMPILE_SIMULATOR),0)

name		:= simulator
simu-objs	:= ../camera/simulator/src/Simu.o
version-file	:= ../camera/simulator/VERSION
simu-flags	:= $(LDFLAGS)
simu-deps	:= $(LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)

simu-base	:= liblima$(name).so
simu-full	:= $(simu-base).$(full-version)
simu-maj	:= $(simu-base).$(maj-version)
simu-flags	+= -fPIC -Wl,-soname,$(simu-maj)

$(simu-full):		$(simu-objs)
	$(CXX) -shared -o $@ $(simu-flags) $+ $(simu-deps)
	rm -f $(simu-maj);  ln -s $(simu-full) $(simu-maj)
	rm -f $(simu-base); ln -s $(simu-maj)  $(simu-base)

build_targets += $(simu-full)

endif


############################
# LIMA - ESPIA
############################


ifneq ($(COMPILE_ESPIA),0)

include ../camera/common/espia/include/espia.inc
ESPIA_LDFLAGS	:= $(LDFLAGS) -L$(ESPIA_DRV_LIB)
ESPIA_LDLIBS	:= $(LDLIBS) -lespia

name		:= espia
espia-objs	:= ../camera/common/espia/src/Espia.o
version-file	:= ../camera/common/espia/VERSION
espia-flags	:= $(ESPIA_LDFLAGS)
espia-deps	:= $(ESPIA_LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)

espia-base	:= liblima$(name).so
espia-full	:= $(espia-base).$(full-version)
espia-maj	:= $(espia-base).$(maj-version)
espia-flags	+= -fPIC -Wl,-soname,$(espia-maj)

$(espia-full):		$(espia-objs)
	$(CXX) -shared -o $@ $(espia-flags) $+ $(espia-deps)
	rm -f $(espia-maj);  ln -s $(espia-full) $(espia-maj)
	rm -f $(espia-base); ln -s $(espia-maj)  $(espia-base)

build_targets += $(espia-full)

endif


############################
# LIMA - FRELON
############################

ifneq ($(COMPILE_FRELON),0)

FRELON_LDFLAGS	:= $(ESPIA_LDFLAGS) -L.
FRELON_LDLIBS	:= $(ESPIA_LDLIBS) -llimaespia

name		:= frelon
frelon-objs	:= ../camera/frelon/src/Frelon.o
version-file	:= ../camera/frelon/VERSION
frelon-flags	:= $(FRELON_LDFLAGS)
frelon-deps	:= $(FRELON_LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)

frelon-base	:= liblima$(name).so
frelon-full	:= $(frelon-base).$(full-version)
frelon-maj	:= $(frelon-base).$(maj-version)
frelon-flags	+= -fPIC -Wl,-soname,$(frelon-maj)

$(frelon-full):		$(frelon-objs)
	$(CXX) -shared -o $@ $(frelon-flags) $+ $(frelon-deps)
	rm -f $(frelon-maj);  ln -s $(frelon-full) $(frelon-maj)
	rm -f $(frelon-base); ln -s $(frelon-maj)  $(frelon-base)

build_targets += $(frelon-full)

endif


############################
# LIMA - MAXIPIX
############################

ifneq ($(COMPILE_MAXIPIX),0)

MAXIPIX_LDFLAGS	:= $(ESPIA_LDFLAGS) -L.
MAXIPIX_LDLIBS	:= $(ESPIA_LDLIBS) -llimaespia

name		:= maxipix
maxipix-objs	:= ../camera/maxipix/src/Maxipix.o
version-file	:= ../camera/maxipix/VERSION
maxipix-flags	:= $(MAXIPIX_LDFLAGS)
maxipix-deps	:= $(MAXIPIX_LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)

maxipix-base	:= liblima$(name).so
maxipix-full	:= $(maxipix-base).$(full-version)
maxipix-maj	:= $(maxipix-base).$(maj-version)
maxipix-flags	+= -fPIC -Wl,-soname,$(maxipix-maj)

$(maxipix-full):		$(maxipix-objs)
	$(CXX) -shared -o $@ $(maxipix-flags) $+ $(maxipix-deps)
	rm -f $(maxipix-maj);  ln -s $(maxipix-full) $(maxipix-maj)
	rm -f $(maxipix-base); ln -s $(maxipix-maj)  $(maxipix-base)

build_targets += $(maxipix-full)

endif

############################
# LIMA - PROSILICA
############################

ifneq ($(COMPILE_PROSILICA),0)

PROSILICA_LDFLAGS	:= -L. -L../camera/prosilica/sdk/bin/x64 -L../camera/prosilica/sdk/bin/x86
PROSILICA_LDLIBS	:= -lPvAPI

name		:= prosilica
prosilica-objs	:= ../camera/prosilica/src/Prosilica.o
version-file	:= ../camera/prosilica/VERSION
prosilica-flags	:= $(PROSILICA_LDFLAGS)
prosilica-deps	:= $(PROSILICA_LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)

prosilica-base	:= liblima$(name).so
prosilica-full	:= $(prosilica-base).$(full-version)
prosilica-maj	:= $(prosilica-base).$(maj-version)
prosilica-flags	+= -fPIC -Wl,-soname,$(prosilica-maj)

$(prosilica-full):		$(prosilica-objs)
	$(CXX) -shared -o $@ $(prosilica-flags) $+ $(prosilica-deps)
	rm -f $(prosilica-maj);  ln -s $(prosilica-full) $(prosilica-maj)
	rm -f $(prosilica-base); ln -s $(prosilica-maj)  $(prosilica-base)

build_targets += $(prosilica-full)

endif

src:		$(build_targets)

test:
	@true

clean:
	rm -f *.o liblima*.so*
