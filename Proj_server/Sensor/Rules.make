# Common rules for project's makefiles #
# $Id: Rules.make 373 2011-04-07 17:09:48Z jose.goncalves $

ifeq ($(strip "${TARGET}"),"")

# Native compilation
PREFIX               = /usr/local
CONF_PREFIX          = $(PREFIX)/etc
VAR_PREFIX           = /var/local

else

# Cross compilation
CROSS_COMPILE        = $(TARGET)-
DESTDIR              = $(PRJROOT)/rootfs
PREFIX               = /usr
CONF_PREFIX          = /etc
VAR_PREFIX           = /var
ifeq ("${TARGET}","arm-linux")
COMFLAGS            += -march=armv5te -mtune=arm926ej-s
endif

endif

DATA_PREFIX          = $(PREFIX)/share

# Tools
export AS            = $(CROSS_COMPILE)as
export AR            = $(CROSS_COMPILE)ar
export CC            = $(CROSS_COMPILE)gcc
export CXX           = $(CROSS_COMPILE)g++
export CPP           = $(CC) -E
export LD            = $(CROSS_COMPILE)ld
export STRIP         = $(CROSS_COMPILE)strip

# Auxiliary tools
export RM            = rm -f
export INSTALL       = install

# Tool flags
COMFLAGS            += -D_XOPEN_SOURCE=600 \
                       -D'DIR_CONF_PREFIX="$(CONF_PREFIX)/"' \
                       -D'DIR_DATA_PREFIX="$(DATA_PREFIX)/"' \
                       -D'DIR_VAR_PREFIX="$(VAR_PREFIX)/"'
COMFLAGS            += -Wall -Wextra
COMFLAGS            += -O2 -fomit-frame-pointer -pipe
CSTD                 = -std=c99
CXXSTD               = -std=c++98
export CFLAGS       += $(CSTD) $(COMFLAGS)
export CXXFLAGS     += $(CXXSTD) $(COMFLAGS)

# Installation directories
export LIB_DIR       = $(DESTDIR)$(PREFIX)/lib
export BIN_DIR       = $(DESTDIR)$(PREFIX)/bin
export SBIN_DIR      = $(DESTDIR)$(PREFIX)/sbin
export CONF_DATA_DIR = $(DESTDIR)$(CONF_PREFIX)
export STAT_DATA_DIR = $(DESTDIR)$(DATA_PREFIX)
export VAR_DATA_DIR  = $(DESTDIR)$(VAR_PREFIX)
export SRV_DATA_DIR  = $(DESTDIR)/srv

