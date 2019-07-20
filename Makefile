# paths
CUR_PATH      ::= $(dir $(lastword $(MAKEFILE_LIST)))
SRC_PATH      ::= src
BUILD_PATH    ::= build
SDK_PATH      ::= vstsdk2.4
SDK_SRC_PATH  ::= $(SDK_PATH)/public.sdk/source/vst2.x

# compilers
COMP          ::= gcc
XCOMP32       ::= i686-w64-mingw32-g++
XCOMP64       ::= x86_64-w64-mingw32-g++

# compiler options
OPT           ::= -I$(SRC_PATH) -Wall -g
XOPT          ::= -I$(SDK_PATH) -I$(SDK_SRC_PATH) -static

# compiler invocation
CC            ::= $(COMP)    $(OPT)
XC32          ::= $(XCOMP32) $(OPT) $(XOPT)
XC64          ::= $(XCOMP64) $(OPT) $(XOPT)

# targets
TARGET_TEST   ::= $(BUILD_PATH)/test
TARGET_VST_32 ::= $(BUILD_PATH)/nnwgnp32.dll
TARGET_VST_64 ::= $(BUILD_PATH)/nnwgnp64.dll



all: $(TARGET_TEST) vst

### TEST PROGRAM ###

$(TARGET_TEST): $(BUILD_PATH)/test.o $(BUILD_PATH)/waveguide.o $(BUILD_PATH)/list.o
	$(CC) -lm \
		$(BUILD_PATH)/test.o $(BUILD_PATH)/waveguide.o $(BUILD_PATH)/list.o \
		-o $(TARGET_TEST)

$(BUILD_PATH)/test.o: $(BUILD_PATH) $(SRC_PATH)/test.c
	$(CC) -c \
		$(SRC_PATH)/test.c \
		-o $(BUILD_PATH)/test.o

$(BUILD_PATH)/waveguide.o: $(BUILD_PATH) $(SRC_PATH)/waveguide.h $(SRC_PATH)/waveguide.c
	$(CC) -c \
		$(SRC_PATH)/waveguide.c \
		-o $(BUILD_PATH)/waveguide.o

$(BUILD_PATH)/list.o: $(BUILD_PATH) $(SRC_PATH)/list.h $(SRC_PATH)/list.c
	$(CC) -c \
		$(SRC_PATH)/list.c \
		-o $(BUILD_PATH)/list.o



### 32-BIT VST ###

$(TARGET_VST_32): $(BUILD_PATH)/waveguide_x32.o $(BUILD_PATH)/list_x32.o $(BUILD_PATH)/vst_x32.o $(BUILD_PATH)/audioeffect_x32.o $(BUILD_PATH)/audioeffectx_x32.o $(BUILD_PATH)/vstplugmain_x32.o
	$(XC32) -shared \
		$(BUILD_PATH)/waveguide_x32.o $(BUILD_PATH)/list_x32.o $(BUILD_PATH)/vst_x32.o \
		$(BUILD_PATH)/audioeffect_x32.o $(BUILD_PATH)/audioeffectx_x32.o $(BUILD_PATH)/vstplugmain_x32.o \
		-o $(TARGET_VST_32)

$(BUILD_PATH)/waveguide_x32.o: $(BUILD_PATH) $(SRC_PATH)/waveguide.h $(SRC_PATH)/waveguide.c
	$(XC32) -fPIC -c -x c \
		$(SRC_PATH)/waveguide.c \
		-o $(BUILD_PATH)/waveguide_x32.o

$(BUILD_PATH)/list_x32.o: $(BUILD_PATH) $(SRC_PATH)/list.h $(SRC_PATH)/list.c
	$(XC32) -fPIC -c -x c \
		$(SRC_PATH)/list.c \
		-o $(BUILD_PATH)/list_x32.o

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

$(TARGET_VST_64): $(BUILD_PATH)/waveguide_x64.o $(BUILD_PATH)/list_x64.o $(BUILD_PATH)/vst_x64.o $(BUILD_PATH)/audioeffect_x64.o $(BUILD_PATH)/audioeffectx_x64.o $(BUILD_PATH)/vstplugmain_x64.o
	$(XC64) -shared \
		$(BUILD_PATH)/waveguide_x64.o $(BUILD_PATH)/list_x64.o $(BUILD_PATH)/vst_x64.o \
		$(BUILD_PATH)/audioeffect_x64.o $(BUILD_PATH)/audioeffectx_x64.o $(BUILD_PATH)/vstplugmain_x64.o \
		-o $(TARGET_VST_64)

$(BUILD_PATH)/waveguide_x64.o: $(BUILD_PATH) $(SRC_PATH)/waveguide.h $(SRC_PATH)/waveguide.c
	$(XC64) -fPIC -c -x c \
		$(SRC_PATH)/waveguide.c \
		-o $(BUILD_PATH)/waveguide_x64.o

$(BUILD_PATH)/list_x64.o: $(BUILD_PATH) $(SRC_PATH)/list.h $(SRC_PATH)/list.c
	$(XC64) -fPIC -c -x c \
		$(SRC_PATH)/list.c \
		-o $(BUILD_PATH)/list_x64.o

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
test: $(TARGET_TEST)
	$(TARGET_TEST)

.PHONY:
debug: $(TARGET_TEST)
	gdb $(TARGET_TEST)
