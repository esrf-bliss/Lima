prosilica-objs = ProsilicaBufferCtrlObj.o ProsilicaCamera.o ProsilicaInterface.o \
		 ProsilicaSyncCtrlObj.o  ProsilicaVideoCtrlObj.o ProsilicaDetInfoCtrlObj.o

SRCS = $(prosilica-objs:.o=.cpp) 

CXXFLAGS += -I../include -I../../../hardware/include -I../../../common/include -I../sdk/include \
	-pthread -fPIC -g

all:	Prosilica.o

Prosilica.o:	$(prosilica-objs)
	$(LD) -o $@ -r $+

clean:
	rm -f *.o *.P

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
