CXX = g++
CXXFLAGS = -Wall
CXXFLAGS += -I "../exception"

LIB_NAME = sofs16Probing

TARGET_LIB = lib$(LIB_NAME).a

OBJS = probing.o 

all:		$(TARGET_LIB) clean

$(TARGET_LIB):		$(OBJS)
	ar -r $@ $^
	cp $@ ../../lib
	rm -f $@

clean:
	rm -f *.o $(TARGET_LIB)
	rm -f *~ 

cleanall:	clean
	rm -f ../../lib/$(TARGET_LIB)

