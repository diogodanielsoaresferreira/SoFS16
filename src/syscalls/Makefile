CXX = g++
CXXFLAGS = -Wall 
CXXFLAGS += -I "../rawdisk"
CXXFLAGS += -I "../probing"
CXXFLAGS += -I "../exception"
CXXFLAGS += -I "../core"
CXXFLAGS += -I "../dealers"
CXXFLAGS += -I "../freelists"
CXXFLAGS += -I "../filecluster"
CXXFLAGS += -I "../direntries"

LIB_NAME = sofs16Syscalls

TARGET_LIB = lib$(LIB_NAME).a

# uncomment those that we want to include
OBJS =
#OBJS += syscalls.o
OBJS += read.o
OBJS += write.o
OBJS += mkdir.o
OBJS += rmdir.o
OBJS += readdir.o
OBJS += rename.o
OBJS += mknod.o
OBJS += symlink.o
OBJS += readlink.o
OBJS += truncate.o
OBJS += unlink.o
OBJS += link.o

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
