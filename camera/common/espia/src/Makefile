
ESPIA_DRV_BASE = /segfs/bliss/source/driver/linux-2.6/espia
ESPIA_DRV_INC = $(ESPIA_DRV_BASE)/src

CXXFLAGS = -I../include -I../../../../hardware/include \
	   -I../../../../common/include -I$(ESPIA_DRV_INC)

all:	clean Espia.o
	

clean:
	rm -f *.o
