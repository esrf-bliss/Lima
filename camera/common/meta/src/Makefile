meta-objs = MetaInterface.o MetaDetInfoCtrlObj.o MetaSyncCtrlObj.o

SRCS = $(meta-objs:.o=.cpp)

CXXFLAGS += -I../include -I../../../../hardware/include -I../../../../common/include \
	-I../../../../control/include -I../../../../control/software_operation/include \
	-I../../../../third-party/Processlib/core/include -Wall -pthread -fPIC -g

all:	Meta.o

Meta.o:	$(meta-objs)
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



.PHONY: check-syntax
check-syntax:
	$(CXX) -Wall -Wextra -fsyntax-only $(CXXFLAGS) $(CHK_SOURCES)
