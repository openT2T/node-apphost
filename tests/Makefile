IDIR=../patch/node/
CC=g++
CFLAGS=-stdlib=libstdc++ -lstdc++ -std=c++11 -I$(IDIR)

ifeq (Release, ${BUILD_TYPE})
$(info TESTING RELEASE BUILD)
else
ifeq (Debug, ${BUILD_TYPE})
$(info TESTING DEBUG BUILD)
else
$(info missing BUILD_TYPE=Debug/Release argument. Forcing DEBUG BUILD)
BUILD_TYPE=Debug
endif
endif

ifeq (nodejs, ${TEST_BINARY})
LIBRARY_PATH=../nodejs/out/$(BUILD_TYPE)
DEF=-DNODEJS
LDIR=$(LIBRARY_PATH)/libnode.a $(LIBRARY_PATH)/libuv.a $(LIBRARY_PATH)/libcares.a \
	$(LIBRARY_PATH)/libopenssl.a $(LIBRARY_PATH)/libzlib.a \
	$(LIBRARY_PATH)/libhttp_parser.a $(LIBRARY_PATH)/libv8_inspector_stl.a $(LIBRARY_PATH)/libicudata.a \
	$(LIBRARY_PATH)/libv8_libbase.a $(LIBRARY_PATH)/libicui18n.a $(LIBRARY_PATH)/libv8_libplatform.a \
	$(LIBRARY_PATH)/libicustubdata.a $(LIBRARY_PATH)/libv8_nosnapshot.a $(LIBRARY_PATH)/libicuucx.a \
	$(LIBRARY_PATH)/libv8_snapshot.a $(LIBRARY_PATH)/libv8_base.a
else
LIBRARY_PATH=../jxcore/out/$(BUILD_TYPE)
DEF=-DJXCORE -DJS_ENGINE_MOZJS -DJS_PUNBOX64
LDIR=$(LIBRARY_PATH)/libcares.a  $(LIBRARY_PATH)/libjx.a $(LIBRARY_PATH)/libsqlite3.a \
    $(LIBRARY_PATH)/libchrome_zlib.a $(LIBRARY_PATH)/libmozjs.a  $(LIBRARY_PATH)/libuv.a \
    $(LIBRARY_PATH)/libhttp_parser.a  $(LIBRARY_PATH)/libopenssl.a
endif

LIBS=-framework CoreServices -lm -ldl -Wno-c++11-compat-deprecated-writable-strings -Wno-deprecated-declarations -Wno-unknown-warning-option -o test.o

testmake:
	$(CC) $(DEF) $(TARGET_TEST)/test.cpp $(CFLAGS) -Wl,-force_load,$(LDIR) $(LIBS)

.PHONY: clean

clean:
	rm test
