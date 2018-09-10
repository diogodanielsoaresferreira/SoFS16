CXX = g++
CXXFLAGS = -Wall
CXXFLAGS += -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26
CXXFLAGS += -I ../probing
CXXFLAGS += -I ../exception
CXXFLAGS += -I ../rawdisk
CXXFLAGS += -I ../core
CXXFLAGS += -I ../dealers
CXXFLAGS += -I ../freelists
CXXFLAGS += -I ../filecluster
CXXFLAGS += -I ../direntries
CXXFLAGS += -I ../syscalls

SUFFIX = $(shell getconf LONG_BIT)

TARGET_APPS = sofsmount

OBJS = sofsmount.o

LDFLAGS = -L../../lib
LDFLAGS += -lsofs16Syscalls
LDFLAGS += -lsofs16Syscalls_bin_$(SUFFIX)
LDFLAGS += -lsofs16Direntries
LDFLAGS += -lsofs16Direntries_bin_$(SUFFIX)
LDFLAGS += -lsofs16Filecluster
LDFLAGS += -lsofs16Filecluster_bin_$(SUFFIX)
LDFLAGS += -lsofs16Freelists
LDFLAGS += -lsofs16Freelists_bin_$(SUFFIX)
LDFLAGS += -lsofs16Dealers
LDFLAGS += -lsofs16Dealers_bin_$(SUFFIX)
LDFLAGS += -lsofs16Rawdisk
LDFLAGS += -lsofs16Probing
LDFLAGS += -lpthread -lfuse -lrt -ldl

all:		$(OBJS) $(TARGET_APPS)

$(TARGET_APPS):	$(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)
	cp $@ ../../bin/
	rm -f $^ $@

$(OBJS):

clean:
	rm -f $(TARGET_APPS) $(TARGET_APPS).o $(OBJS)
	rm -f *~ 

cleanall:	clean
	rm -f ../../bin/$(TARGET_APPS)

