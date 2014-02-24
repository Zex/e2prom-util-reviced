#
# Makefile for E2PROM utility
#
# Author: Zex <top_zlynch@yahoo.com>
#
PROJECT		= e2prom #$(shell basename $(shell pwd))
VERSION		= 3.6.6
INCLUDES	= include build/src
CXXFLAGS	= -std=c++11 -pg -g3 -O0 #-DDEBUG
#SOURCES		= $(basename $(wildcard lib/*))
SOURCES		= lib/tools lib/utils
TARGET		= $(TOOLS)/ascii2bin $(TOOLS)/bin2ascii

#ifndef TOOLCHAIN-$(PLATFORM)
#export TOOLCHAIN-$(PLATFORM) = i686-pc-linux-gnu-
#endif

#include extern/makefiles/presets.mk

all: tools

e2prom:
	@$(MAKE) -f extern/makefiles/so.mk

tools: tools/ascii2bin tools/bin2ascii tools/update-xtype tools/temp2bin

tools/%: e2prom
	@$(MAKE) 'PROJECT_DIR=..' 'PROJECT_SUB=tools' -C tools -f Makefile $(@F)

examples: $(basename $(wildcard examples/*.cpp))

examples/%: e2prom
	@$(MAKE) 'PROJECT_DIR=..' 'PROJECT_SUB=examples' -C examples -f Makefile $(@F)

#install_lib :
#	@echo "Installing lib to $(PREFIX)/lib"
#	@mkdir -p $(PREFIX)/lib 2> /dev/null
#	@cp -a $(LIB) $(PREFIX)/lib
#
#install_bin :
#	@echo "Installing bin to $(PREFIX)/bin"
#	@mkdir -p $(PREFIX)/bin 2> /dev/null
#	@cp -a $(INSTALL_BIN) $(PREFIX)/bin
#
#install : install_lib install_bin

clean:
	@$(MAKE) -f extern/makefiles/so.mk clean
	@$(MAKE) -f extern/makefiles/lcov.mk clean
	@$(RM) -f *.out

clean-tool-output:
	@$(RM) -f *xxd *bin *ascii
