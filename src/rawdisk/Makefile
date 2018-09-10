CXX = g++
CXXFLAGS = -Wall 
CXXFLAGS += -I "../probing"
CXXFLAGS += -I "../exception"

LIB_NAME = sofs16Rawdisk

TARGET_LIB = lib$(LIB_NAME).a

OBJS = rawdisk.o

all:			$(TARGET_LIB)

$(TARGET_LIB):		$(OBJS)
	ar -r $(TARGET_LIB) $^
	cp $(TARGET_LIB) ../../lib
	rm -f $^ $(TARGET_LIB)

clean:
	rm -f $(OBJS) $(TARGET_LIB)
	rm -f *~ 

cleanall:	clean
	rm -f ../../lib/$(TARGET_LIB)
