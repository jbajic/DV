CROSS_COMPILE=$(TOOLCHAIN_CROSS_COMPILE)

CC_PREFIX=$(CROSS_COMPILE)-
CC=$(CC_PREFIX)gcc
CXX=$(CC_PREFIX)g++
LD=$(CC_PREFIX)ld
OBJFILES=init_configuration.o main.o
TARGET=JureProjekt

SYSROOT=$(SDK_ROOTFS)
GALOIS_INCLUDE=$(SDK_ROOTFS)/rootfs/home/galois/include

INCS =	-I./../tdp_api
INCS += -I./include/ 							\
		-I$(SYSROOT)/usr/include/         \
		-I$(GALOIS_INCLUDE)/Common/include/     \
		-I$(GALOIS_INCLUDE)/OSAL/include/		\
		-I$(GALOIS_INCLUDE)/OSAL/include/CPU1/	\
		-I$(GALOIS_INCLUDE)/PE/Common/include/	\
		-I$(SYSROOT)/usr/include/directfb/

LIBS_PATH = -L./../../tdp_api

LIBS_PATH += -L$(SYSROOT)/home/galois/lib/

LIBS_PATH += -L$(SYSROOT)/home/galois/lib/directfb-1.4-6-libs

LIBS := $(LIBS_PATH) -ltdp -ldirectfb -ldirect -lfusion -lrt

LIBS += $(LIBS_PATH) -lOSAL	-lshm -lPEAgent       

CFLAGS += -D__LINUX__ -O0 -Wno-psabi --sysroot=$(SYSROOT)

CXXFLAGS = $(CFLAGS)

all: $(TARGET)

SRCS = ./init_configuration.c
SRCS = ./main.c

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(INCS) $(LIBS)
    
clean:
	rm -f $(OBJFILES)
