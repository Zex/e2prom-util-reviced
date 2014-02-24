#
# Makefile for E2PROM binary-ASCII converting examples
#
# Author: Zex <zex@hytera.com>
#
PROJECT		= examples#$(shell basename $(shell pwd))
INCLUDES	= ../include
CXXFLAGS	= -std=c++11 -pg -g3 -O0 $(INCLUDES:%=-I%) #-DDEBUG
LDPATH      = $(subst /$(PROJECT_SUB),,$(BUILD_DIR))	\
			   ../build/$(PLATFORM)/debug
SOURCES		= .
LIBS		= e2prom

#ifndef TOOLCHAIN-$(PLATFORM)
#	TOOLCHAIN-$(PLATFORM) = i686-pc-linux-gnu-
#endif

TOOLCHAIN-i586-unknown-linux-gnu = /opt/OSELAS.Toolchain-2012.12.1/i586-unknown-linux-gnu/gcc-4.7.2-glibc-2.16.0-binutils-2.22-kernel-3.6-sanitized/bin/i586-unknown-linux-gnu-
TOOLCHAIN-arm-cortexa9-linux-gnueabihf = /opt/OSELAS.Toolchain-2012.12.1/arm-cortexa9-linux-gnueabihf/gcc-4.7.3-glibc-2.16.0-binutils-2.22-kernel-3.6-sanitized/bin/arm-cortexa9-linux-gnueabihf-

TOOLCHAIN-AEOS_Intel = $(TOOLCHAIN-i586-unknown-linux-gnu)
TOOLCHAIN-AEOS_ARM = $(TOOLCHAIN-arm-cortexa9-linux-gnueabihf)

export TOOLCHAIN-i586-unknown-linux-gnu
export TOOLCHAIN-arm-cortexa9-linux-gnueabihf

export TOOLCHAIN-AEOS_Intel
#include ../extern/makefiles/appl.mk
#include ../extern/makefiles/presets.mk

.cpp:
	@$(MKDIR) -p ../$(BUILD_DIR)/examples
	$(LINK.cpp) $< $(wildcard ../lib/typlate/*.cpp) $(LOADLIBES) $(LDLIBS) $(LDPATH:%=-L%) $(LIBS:%=-l%) -o ../$(BUILD_DIR)/$(PROJECT_SUB)/$@

#all : $(TARGET)

