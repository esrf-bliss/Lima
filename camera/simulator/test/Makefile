
objs = BufferSave.o
	
CXX = g++
CXXFLAGS += -I../include -I../../../common/include -Wall

all: 	clean $(objs)
	$(CXX) $(CXXFLAGS) test.cpp -o test $(objs) ../src/FrameBuilder.o

clean:
	rm -f $(objs) test *.edf
