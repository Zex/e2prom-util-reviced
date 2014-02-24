#
# Makefile for E2PROM binary-ASCII converting tools
#
# Author: Zex <zex@hytera.com>
#
PROJECT		= tools#$(shell basename $(shell pwd))
INCLUDES	= ../include \
	../build/$(PLATFORM)/$(if $(DEBUG:off=),debug,release)/src
CXXFLAGS	= -std=c++11 -pg -g3 -O0 $(INCLUDES:%=-I%) #-DDEBUG
LDPATH      = ../build/$(PLATFORM)/$(if $(DEBUG:off=),debug,release)
# $(subst /$(PROJECT_SUB),,$(BUILD_DIR))	# \
			   
SOURCES		= . 
LIBS		= e2prom

ifndef TOOLCHAIN-$(PLATFORM)
	TOOLCHAIN-$(PLATFORM) = i686-pc-linux-gnu-
endif

#include ../extern/makefiles/appl.mk
#include ../extern/makefiles/presets.mk

.cpp:
	@$(MKDIR) ../$(BUILD_DIR)
	$(LINK.cpp) $< $(LOADLIBES) $(LDLIBS) $(LDPATH:%=-L%) $(LIBS:%=-l%) -o ../$(BUILD_DIR)/$@

#all : $(TARGET)

