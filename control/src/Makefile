include ../../config.inc

ct-objs := CtSaving.o CtControl.o CtAcquisition.o CtBuffer.o \
	   CtImage.o CtSaving_Edf.o CtShutter.o CtAccumulation.o

INCLUDES = -I. -I../include -I../../common/include -I../../hardware/include \
		-I../software_operation/include \
		-I../../third-party/Processlib/core/include \
		-I../../third-party/Processlib/tasks/include \
		-I../../third-party/Sps/Include

ifndef COMPILE_CBF_SAVING
COMPILE_CBF_SAVING = 0
endif

ifneq ($(COMPILE_CBF_SAVING),0)
ct-objs += CtSaving_Cbf.o 
INCLUDES += -I../../third-party/CBFLib/include
CXXFLAGS += -DWITH_CBF_SAVING
endif

ifndef COMPILE_SPS_IMAGE
COMPILE_SPS_IMAGE = 0
endif

ifneq ($(COMPILE_SPS_IMAGE),0)
ct-objs += CtSpsImage.o  ../../third-party/Sps/Src/sps.o
CXXFLAGS += -DWITH_SPS_IMAGE
endif

SRCS = $(ct-objs:.o=.cpp)

CXXFLAGS += $(INCLUDES) -fPIC -Wall -pthread -g


all: Ct.o


Ct.o: $(ct-objs) ../software_operation/src/CtSoftwareOperation.o
	ld $(LIBS) -o $@ -r $+

clean:
	rm -f Ct.o $(ct-objs) *.P

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
