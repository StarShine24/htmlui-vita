# Build helper / minimal Makefile

# Adjust VITASDK if your installation is in a different path
VITASDK ?= /opt/vitasdk
CXX = $(VITASDK)/bin/arm-vita-eabi-g++
CC  = $(VITASDK)/bin/arm-vita-eabi-gcc

CXXFLAGS = -std=c++17 -O2 -G0 -fno-exceptions -fno-rtti -Isrc/civetweb -I$(VITASDK)/arm-vita-eabi/include
LDFLAGS  = -Wl,-Ttext-segment=0x81000000 -L$(VITASDK)/arm-vita-eabi/lib -lsceNet -lsceNetCtl -lscePthread -lSceDisplay -lSceKernel

SRCS = src/main.cpp src/civetweb/civetweb.c
OBJS = $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)

all: htmlui.elf

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CXXFLAGS) -c $< -o $@

htmlui.elf: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) htmlui.elf

.PHONY: all clean
