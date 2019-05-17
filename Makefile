CXXFLAGS += -O3 -std=c++14 -Isrc -Ilibbsc -Ilibbsc/libbsc/libbsc -Ilibbsc/libbsc/platform -IMash/src  -Icapnproto/include -I/usr/local//include
CPPFLAGS += 

UNAME_S=$(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	CXXFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++
else
	CXXFLAGS += -include Mash/src/mash/memcpyLink.h -Wl,--wrap=memcpy
	CFLAGS += -include Mash/src/mash/memcpyLink.h
endif


src1=\
	src/processor.cpp \
        src/clustering.cpp \
	src/ECC.cpp	\
	src/hirgc-compressor.cpp \
	src/hirgc-p.cpp \
	src/hirgc-decompressor.cpp \
	libbsc/libbsc/filters/detectors.cpp \
	libbsc/libbsc/filters/preprocessing.cpp \
	libbsc/libbsc/platform/platform.cpp \
	libbsc/libbsc/libbsc/libbsc.cpp \
	libbsc/bsc.cpp \
	libbsc/bsc_str_array.cpp \
	libbsc/libbsc/adler32/adler32.cpp \
	libbsc/libbsc/bwt/divsufsort/divsufsort.cpp \
	libbsc/libbsc/bwt/bwt.cpp \
	libbsc/libbsc/coder/coder.cpp \
	libbsc/libbsc/coder/qlfc/qlfc.cpp \
	libbsc/libbsc/coder/qlfc/qlfc_model.cpp \
	libbsc/libbsc/lzp/lzp.cpp \

src2=\
	Mash/src/mash/Command.cpp \
	Mash/src/mash/CommandBounds.cpp \
	Mash/src/mash/CommandContain.cpp \
	Mash/src/mash/CommandDistance.cpp \
	Mash/src/mash/CommandScreen.cpp \
	Mash/src/mash/CommandTriangle.cpp \
	Mash/src/mash/CommandFind.cpp \
	Mash/src/mash/CommandInfo.cpp \
	Mash/src/mash/CommandPaste.cpp \
	Mash/src/mash/CommandSketch.cpp \
	Mash/src/mash/CommandList.cpp \
	Mash/src/mash/hash.cpp \
	Mash/src/mash/HashList.cpp \
	Mash/src/mash/HashPriorityQueue.cpp \
	Mash/src/mash/HashSet.cpp \
	Mash/src/mash/MinHashHeap.cpp \
	Mash/src/mash/MurmurHash3.cpp \
	Mash/src/mash/Sketch.cpp \
	Mash/src/mash/sketchParameterSetup.cpp \

OBJECTS=$(src1:.cpp=.o) $(src2:.cpp=.o) Mash/src/mash/capnp/MinHash.capnp.o

all : ECC libmash.a

ECC : libmash.a Mash/src/mash/memcpyWrap.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o ECC Mash/src/mash/memcpyWrap.o libmash.a capnproto/lib/libcapnp.a capnproto/lib/libkj.a -L/usr/local//lib -lgsl -lgslcblas -lstdc++ -lz -lm -lpthread

libmash.a : $(OBJECTS)
	ar -cr libmash.a $(OBJECTS)
	ranlib libmash.a

.SUFFIXES :

%.o : %.cpp Mash/src/mash/capnp/MinHash.capnp.h
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o : %.c++
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

Mash/src/mash/memcpyWrap.o : Mash/src/mash/memcpyWrap.c
	$(CC) $(CFLAGS) -c -o $@ $<

Mash/src/mash/capnp/MinHash.capnp.c++ Mash/src/mash/capnp/MinHash.capnp.h : Mash/src/mash/capnp/MinHash.capnp
	cd Mash/src/mash/capnp;export PATH=capnproto/bin/:${PATH};capnp compile -I capnproto/include -oc++ MinHash.capnp

install : mash
	mkdir -p /usr/local/bin/
	mkdir -p /usr/local/lib/
	mkdir -p /usr/local/include/
	mkdir -p /usr/local/include/mash
	mkdir -p /usr/local/include/mash/capnp
	cp `pwd`/mash /usr/local/bin/
	cp `pwd`/libmash.a /usr/local/lib/
	cp `pwd`/src/mash/*.h /usr/local/include/mash/

clean :
	-rm -f ECC
	-rm -f libmash.a
	-rm -f src/*.o
	-rm -f Mash/src/mash/capnp/*.o
	-rm -f Mash/src/mash/*.o
#	-rm -f src/mash/capnp/*.c++
#	-rm -f src/mash/capnp/*.h
	-rm -f libbsc/*.o
	-rm -f libbsc/adler32/*.o
	-rm -f libbsc/bwt/divsufsort/*.o
	-rm -f libbsc/bwt/*.o
	-rm -f libbsc/libbsc/coder/*.o
	-rm -f libbsc/libbsc/coder/qlfc/*.o
	-rm -f libbsc/libbsc/filters/*.o
	-rm -f libbsc/libbsc/lzp/*.o
	-rm -f libbsc/libbsc/platform/*.o
