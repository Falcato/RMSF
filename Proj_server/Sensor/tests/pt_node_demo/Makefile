# $Id: Makefile 574 2011-09-02 17:04:42Z jose.goncalves $

PROG       = pt_node_demo.sh

include ../../Rules.make

all: $(PROG)

install: all
	$(INSTALL) -d -m 755 $(BIN_DIR)
	$(INSTALL) -m 755 $(PROG) $(BIN_DIR)

clean:
	$(RM) *.o *~ *.bak

