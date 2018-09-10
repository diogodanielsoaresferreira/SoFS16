CXX = g++
CXXFLAGS = -Wall
CXXFLAGS += -I "."
CXXFLAGS += -I "../probing"
CXXFLAGS += -I "../exception"
CXXFLAGS += -I "../rawdisk"
CXXFLAGS += -I "../core"

TARGET_APPS = mksofs

OBJS = mksofs_main.o
OBJS += mksofs_SB.o 
OBJS += mksofs_IT.o
OBJS += mksofs_RD.o
OBJS += mksofs_FCT.o
OBJS += mksofs_RC.o

LIBS += -lsofs16Rawdisk
LIBS += -lsofs16Probing

LFLAGS = -L "../../lib" $(LIBS)

all:		$(OBJS) $(TARGET_APPS)

$(TARGET_APPS):	$(OBJS)
	$(CXX) -o $@ $^ $(LFLAGS)
	cp $@ ../../bin/
	rm -f $^ $@

$(OBJS):	mksofs.h

clean:
	rm -f $(TARGET_APPS) $(TARGET_APPS).o $(OBJS)
	rm -f *~ 

cleanall:	clean
	rm -f ../../bin/$(TARGET_APPS)

