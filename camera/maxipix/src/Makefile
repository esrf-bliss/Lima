
maxipix-objs := MaxipixReconstruction.o PriamSerial.o PriamAcq.o PriamArray.o

SRCS = $(maxipix-objs:.o=.cpp)

include ../../common/espia/include/espia.inc

INCLUDES = -I../include \
           -I../../../common/include \
           -I../../../hardware/include \
           -I../../common/espia/include \
           -I$(ESPIA_DRV_INCL) \
	   -I../../../third-party/Processlib/core/include

CXXFLAGS += $(INCLUDES) -fPIC -Wall -pthread


all: Maxipix.o


Maxipix.o: $(maxipix-objs)
	ld -o $@ -r $+

clean:
	rm -f Maxipix.o $(maxipix-objs) *.P

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
