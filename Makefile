# paths
CUR_PATH      ::= $(dir $(lastword $(MAKEFILE_LIST)))
SRC_PATH      ::= src
BUILD_PATH    ::= build
SDK_PATH      ::= vstsdk2.4
SDK_SRC_PATH  ::= $(SDK_PATH)/public.sdk/source/vst2.x
INSTALL_PATH  ::= ~/bin #/usr/bin

# compilers
COMP          ::= g++
XCOMP32       ::= i686-w64-mingw32-g++
XCOMP64       ::= x86_64-w64-mingw32-g++
DEBUGGER      ::= gdb

# compiler options
OPT           ::= -I$(SRC_PATH) -Wall -O3
XOPT          ::= -I$(SDK_PATH) -I$(SDK_SRC_PATH) -Wno-multichar -Wno-narrowing -Wno-write-strings -static

# compiler invocation
CC            ::= $(COMP)    $(OPT)
XC32          ::= $(XCOMP32) $(OPT) $(XOPT)
XC64          ::= $(XCOMP64) $(OPT) $(XOPT)

# targets
TARGET_MAIN   ::= $(BUILD_PATH)/nanceloid
TARGET_TEST   ::= $(BUILD_PATH)/test
TARGET_VST_32 ::= $(BUILD_PATH)/nanceloid32.dll
TARGET_VST_64 ::= $(BUILD_PATH)/nanceloid64.dll

all: $(TARGET_MAIN) $(TARGET_TEST) vst



### STANDALONE SYNTH ###

$(TARGET_MAIN): $(BUILD_PATH)/main.o $(BUILD_PATH)/nanceloid.o $(BUILD_PATH)/saw_source.o $(BUILD_PATH)/waveguide.o $(BUILD_PATH)/segment.o
	$(CC) -lm -lsoundio -lrtmidi \
		$(BUILD_PATH)/main.o $(BUILD_PATH)/nanceloid.o $(BUILD_PATH)/saw_source.o $(BUILD_PATH)/waveguide.o $(BUILD_PATH)/segment.o \
		-o $(TARGET_MAIN)

$(BUILD_PATH)/main.o: $(BUILD_PATH) $(SRC_PATH)/main.cpp
	$(CC) -c \
		$(SRC_PATH)/main.cpp \
		-o $(BUILD_PATH)/main.o

$(BUILD_PATH)/nanceloid.o: $(BUILD_PATH) $(SRC_PATH)/nanceloid.h $(SRC_PATH)/nanceloid.cpp
	$(CC) -D DEBUG -c \
		$(SRC_PATH)/nanceloid.cpp \
		-o $(BUILD_PATH)/nanceloid.o

$(BUILD_PATH)/saw_source.o: $(BUILD_PATH) $(SRC_PATH)/glottal_source.h $(SRC_PATH)/saw_source.h $(SRC_PATH)/saw_source.cpp
	$(CC) -c \
		$(SRC_PATH)/saw_source.cpp \
		-o $(BUILD_PATH)/saw_source.o



### TEST PROGRAM ###

$(TARGET_TEST): $(BUILD_PATH)/test.o $(BUILD_PATH)/waveguide.o $(BUILD_PATH)/segment.o
	$(CC) -lm \
		$(BUILD_PATH)/test.o $(BUILD_PATH)/waveguide.o $(BUILD_PATH)/segment.o \
		-o $(TARGET_TEST)

$(BUILD_PATH)/test.o: $(BUILD_PATH) $(SRC_PATH)/test.cpp
	$(CC) -c \
		$(SRC_PATH)/test.cpp \
		-o $(BUILD_PATH)/test.o



### COMMON ###

$(BUILD_PATH)/waveguide.o: $(BUILD_PATH) $(SRC_PATH)/waveguide.h $(SRC_PATH)/waveguide.cpp
	$(CC) -c \
		$(SRC_PATH)/waveguide.cpp \
		-o $(BUILD_PATH)/waveguide.o

$(BUILD_PATH)/segment.o: $(BUILD_PATH) $(SRC_PATH)/segment.h $(SRC_PATH)/segment.cpp
	$(CC) -c \
		$(SRC_PATH)/segment.cpp \
		-o $(BUILD_PATH)/segment.o



### 32-BIT VST ###

$(TARGET_VST_32): $(BUILD_PATH)/nanceloid_x32.o $(BUILD_PATH)/saw_source_x32.o $(BUILD_PATH)/waveguide_x32.o $(BUILD_PATH)/segment_x32.o $(BUILD_PATH)/vst_x32.o $(BUILD_PATH)/audioeffect_x32.o $(BUILD_PATH)/audioeffectx_x32.o $(BUILD_PATH)/vstplugmain_x32.o
	$(XC32) -shared \
		$(BUILD_PATH)/nanceloid_x32.o $(BUILD_PATH)/saw_source_x32.o $(BUILD_PATH)/waveguide_x32.o $(BUILD_PATH)/segment_x32.o $(BUILD_PATH)/vst_x32.o \
		$(BUILD_PATH)/audioeffect_x32.o $(BUILD_PATH)/audioeffectx_x32.o $(BUILD_PATH)/vstplugmain_x32.o \
		-o $(TARGET_VST_32)

$(BUILD_PATH)/nanceloid_x32.o: $(BUILD_PATH) $(SRC_PATH)/nanceloid.h $(SRC_PATH)/nanceloid.cpp
	$(XC32) -fPIC -c \
		$(SRC_PATH)/nanceloid.cpp \
		-o $(BUILD_PATH)/nanceloid_x32.o

$(BUILD_PATH)/saw_source_x32.o: $(BUILD_PATH) $(SRC_PATH)/glottal_source.h $(SRC_PATH)/saw_source.h $(SRC_PATH)/saw_source.cpp
	$(XC32) -fPIC -c \
		$(SRC_PATH)/saw_source.cpp \
		-o $(BUILD_PATH)/saw_source_x32.o

$(BUILD_PATH)/waveguide_x32.o: $(BUILD_PATH) $(SRC_PATH)/waveguide.h $(SRC_PATH)/waveguide.cpp
	$(XC32) -fPIC -c \
		$(SRC_PATH)/waveguide.cpp \
		-o $(BUILD_PATH)/waveguide_x32.o

$(BUILD_PATH)/segment_x32.o: $(BUILD_PATH) $(SRC_PATH)/segment.h $(SRC_PATH)/segment.cpp
	$(XC32) -fPIC -c \
		$(SRC_PATH)/segment.cpp \
		-o $(BUILD_PATH)/segment_x32.o

$(BUILD_PATH)/vst_x32.o: $(BUILD_PATH) $(SRC_PATH)/vst.h $(SRC_PATH)/vst.cpp
	$(XC32) -fPIC -c \
		$(SRC_PATH)/vst.cpp \
		-o $(BUILD_PATH)/vst_x32.o

$(BUILD_PATH)/audioeffect_x32.o: $(SDK_PATH) $(SDK_SRC_PATH)/audioeffect.h $(SDK_SRC_PATH)/audioeffect.cpp
	$(XC32) -fPIC -c \
		$(SDK_SRC_PATH)/audioeffect.cpp \
		-o $(BUILD_PATH)/audioeffect_x32.o

$(BUILD_PATH)/audioeffectx_x32.o: $(SDK_PATH) $(SDK_SRC_PATH)/audioeffectx.h $(SDK_SRC_PATH)/audioeffectx.cpp
	$(XC32) -fPIC -c \
		$(SDK_SRC_PATH)/audioeffectx.cpp \
		-o $(BUILD_PATH)/audioeffectx_x32.o

$(BUILD_PATH)/vstplugmain_x32.o: $(SDK_PATH) $(SDK_SRC_PATH)/vstplugmain.cpp
	$(XC32) -fPIC -c \
		$(SDK_SRC_PATH)/vstplugmain.cpp -o \
		$(BUILD_PATH)/vstplugmain_x32.o



### 64-BIT VST ###

$(TARGET_VST_64): $(BUILD_PATH)/nanceloid_x64.o $(BUILD_PATH)/saw_source_x64.o $(BUILD_PATH)/waveguide_x64.o $(BUILD_PATH)/segment_x64.o $(BUILD_PATH)/vst_x64.o $(BUILD_PATH)/audioeffect_x64.o $(BUILD_PATH)/audioeffectx_x64.o $(BUILD_PATH)/vstplugmain_x64.o
	$(XC64) -shared \
		$(BUILD_PATH)/nanceloid_x64.o $(BUILD_PATH)/saw_source_x64.o $(BUILD_PATH)/waveguide_x64.o $(BUILD_PATH)/segment_x64.o $(BUILD_PATH)/vst_x64.o \
		$(BUILD_PATH)/audioeffect_x64.o $(BUILD_PATH)/audioeffectx_x64.o $(BUILD_PATH)/vstplugmain_x64.o \
		-o $(TARGET_VST_64)

$(BUILD_PATH)/nanceloid_x64.o: $(BUILD_PATH) $(SRC_PATH)/nanceloid.h $(SRC_PATH)/nanceloid.cpp
	$(XC64) -fPIC -c \
		$(SRC_PATH)/nanceloid.cpp \
		-o $(BUILD_PATH)/nanceloid_x64.o

$(BUILD_PATH)/saw_source_x64.o: $(BUILD_PATH) $(SRC_PATH)/glottal_source.h $(SRC_PATH)/saw_source.h $(SRC_PATH)/saw_source.cpp
	$(XC64) -fPIC -c \
		$(SRC_PATH)/saw_source.cpp \
		-o $(BUILD_PATH)/saw_source_x64.o

$(BUILD_PATH)/waveguide_x64.o: $(BUILD_PATH) $(SRC_PATH)/waveguide.h $(SRC_PATH)/waveguide.cpp
	$(XC64) -fPIC -c \
		$(SRC_PATH)/waveguide.cpp \
		-o $(BUILD_PATH)/waveguide_x64.o

$(BUILD_PATH)/segment_x64.o: $(BUILD_PATH) $(SRC_PATH)/segment.h $(SRC_PATH)/segment.cpp
	$(XC64) -fPIC -c \
		$(SRC_PATH)/segment.cpp \
		-o $(BUILD_PATH)/segment_x64.o

$(BUILD_PATH)/vst_x64.o: $(BUILD_PATH) $(SRC_PATH)/vst.h $(SRC_PATH)/vst.cpp
	$(XC64) -fPIC -c \
		$(SRC_PATH)/vst.cpp \
		-o $(BUILD_PATH)/vst_x64.o

$(BUILD_PATH)/audioeffect_x64.o: $(SDK_PATH) $(SDK_SRC_PATH)/audioeffect.h $(SDK_SRC_PATH)/audioeffect.cpp
	$(XC64) -fPIC -c \
		$(SDK_SRC_PATH)/audioeffect.cpp \
		-o $(BUILD_PATH)/audioeffect_x64.o

$(BUILD_PATH)/audioeffectx_x64.o: $(SDK_PATH) $(SDK_SRC_PATH)/audioeffectx.h $(SDK_SRC_PATH)/audioeffectx.cpp
	$(XC64) -fPIC -c \
		$(SDK_SRC_PATH)/audioeffectx.cpp \
		-o $(BUILD_PATH)/audioeffectx_x64.o

$(BUILD_PATH)/vstplugmain_x64.o: $(SDK_PATH) $(SDK_SRC_PATH)/vstplugmain.cpp
	$(XC64) -fPIC -c \
		$(SDK_SRC_PATH)/vstplugmain.cpp -o \
		$(BUILD_PATH)/vstplugmain_x64.o



### COMMON ###

vst: $(TARGET_VST_32) $(TARGET_VST_64)

$(SDK_PATH):
	$(error Please illegitimately obtain the VST SDK 2.4 and place the contents in "$(CUR_PATH)$(SDK_PATH)")

$(BUILD_PATH):
	mkdir $(BUILD_PATH)

.PHONY:
clean:
	rm -rf $(BUILD_PATH)

.PHONY:
run: $(TARGET_MAIN)
	$(TARGET_MAIN)

.PHONY:
test: $(TARGET_TEST)
	$(TARGET_TEST)

.PHONY:
debug: $(TARGET_TEST)
	$(DEBUGGER) $(TARGET_TEST)

.PHONY:
install: $(TARGET_MAIN)
	cp $(TARGET_MAIN) $(INSTALL_PATH)
