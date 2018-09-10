CXX = g++
CXXFLAGS = -Wall
CXXFLAGS += -I ../probing
CXXFLAGS += -I ../exception
CXXFLAGS += -I ../rawdisk
CXXFLAGS += -I ../core
CXXFLAGS += -I ../dealers
CXXFLAGS += -I ../freelists
CXXFLAGS += -I ../filecluster
CXXFLAGS += -I ../direntries

SUFFIX = $(shell getconf LONG_BIT)

TARGET_APPS = showblock testtool testdealers

OBJS = blockviews.o

LDFLAGS = -L../../lib
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

all:		$(TARGET_APPS) clean

$(TARGET_APPS):	%:	%.o $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)
	cp $@ ../../bin
	rm -f $@

clean:
	rm -f *.o $(TARGET_LIB)
	rm -f *~ 

cleanall:	clean
	for file in $(TARGET_APPS); do rm -f ../../bin/$$file; done

