# $Id: Makefile 267 2010-11-20 11:06:40Z jose.goncalves $

SRC        = $(shell find -maxdepth 1 -name "*.c" -printf "%f ")
OBJS       = $(SRC:.c=.o)
LIBNAME    = libv4l2_control

MAJVER     = 1
MINVER     = 0
VERSION    = $(MAJVER).$(MINVER)

include ../Rules.make

CFLAGS    += -fpic -Wno-missing-field-initializers -DDEBUG
# The V4L2 interface uses nameless unions, that are not in the C99 standard.
CFLAGS    += -std=gnu89 -DDEBUG

all: $(LIBNAME).so

$(LIBNAME).so: $(LIBNAME).so.$(MAJVER)

$(LIBNAME).so.$(MAJVER): $(LIBNAME).so.$(VERSION)

$(LIBNAME).so $(LIBNAME).so.$(MAJVER):
	ln -sf $< $@

$(LIBNAME).so.$(VERSION): $(OBJS)
	$(CC) -shared -Wl,-soname,$(LIBNAME).so.$(MAJVER) $(LDFLAGS) $^ $(LDLIBS) -o $@

install: all
	$(INSTALL) -d -m 755 $(LIB_DIR)
	$(INSTALL) -m 755 $(LIBNAME).so.$(VERSION) $(LIB_DIR)
	$(STRIP) $(LIB_DIR)/$(LIBNAME).so.$(VERSION)
	cp -d $(LIBNAME).so.$(MAJVER) $(LIB_DIR)

clean:
	$(RM) *.o *~ *.bak
	$(RM) $(LIBNAME).so*

