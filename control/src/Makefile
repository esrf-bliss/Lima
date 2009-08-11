ct-objs := CtSaving.o CtControl.o CtAcquisition.o CtBuffer.o \
		CtDebug.o CtImage.o

SRCS = CtSaving.cpp CtControl.cpp CtAcquisition.cpp CtBuffer.cpp \
		CtDebug.cpp CtImage.cpp

INCLUDES = -I../include -I../../common/include -I../../hardware/include \
		-I../../third-party/Processlib/core/include

CXXFLAGS += $(INCLUDES) -fPIC -Wall -pthread


all: Ct.o


Ct.o: $(ct-objs) ../software_operation/src/CtSoftwareOperation.o
	ld -o $@ -r $+

clean:
	rm -f Ct.o $(ct-objs) *.P

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
