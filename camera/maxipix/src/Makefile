SRCS = MaxipixReconstruction.cpp

maxipix-objs := MaxipixReconstruction.o


INCLUDES = -I../include \
	   -I../../../third-party/Processlib/core/include

CXXFLAGS += $(INCLUDES) -fPIC -Wall -pthread -g


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
