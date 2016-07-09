IDIR=../../
CC=g++
CFLAGS=-stdlib=libstdc++ -lstdc++ -std=c++11 -I$(IDIR)

LIBRARY_PATH=../../../../nodejs/out/Debug

LDIR=$(LIBRARY_PATH)/libnode.a $(LIBRARY_PATH)/libuv.a $(LIBRARY_PATH)/libcares.a \
	$(LIBRARY_PATH)/libopenssl.a $(LIBRARY_PATH)/libzlib.a \
	$(LIBRARY_PATH)/libhttp_parser.a $(LIBRARY_PATH)/libv8_inspector_stl.a $(LIBRARY_PATH)/libicudata.a \
	$(LIBRARY_PATH)/libv8_libbase.a $(LIBRARY_PATH)/libicui18n.a $(LIBRARY_PATH)/libv8_libplatform.a \
	$(LIBRARY_PATH)/libicustubdata.a $(LIBRARY_PATH)/libv8_nosnapshot.a $(LIBRARY_PATH)/libicuucx.a \
	$(LIBRARY_PATH)/libv8_snapshot.a $(LIBRARY_PATH)/libv8_base.a

LIBS=-framework CoreServices -lm -ldl -Wno-c++11-compat-deprecated-writable-strings -Wno-deprecated-declarations -Wno-unknown-warning-option -o test.o

testmake:
	$(CC) test.cpp $(CFLAGS) -Wl,-force_load,$(LDIR) $(LIBS) 

.PHONY: clean

clean:
	rm test
