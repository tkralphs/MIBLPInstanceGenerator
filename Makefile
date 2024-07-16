##############################################################################
##############################################################################
#                                                                            #
# This file is part of the SYMPHONY MILP Solver Framework.                   #
#                                                                            #
# SYMPHONY was jointly developed by Ted Ralphs (tkralphs@lehigh.edu) and     #
# Laci Ladanyi (ladanyi@us.ibm.com).                                         #
#                                                                            #
# (c) Copyright 2005-2007 Lehigh University. All Rights Reserved.            #
#                                                                            #
# This software is licensed under the Common Public License. Please see      #
# accompanying file for terms.                                               #
#                                                                            #
# The author of this file is Menal Guzelsoy (megb@lehigh.edu)                #
#                                                                            #
##############################################################################
##############################################################################

CC = g++
OPT = -g #-fPIC
SHLINKPREFIX = -Wl,-rpath,

COINROOT = /home/ted/Projects/build-mibs-1.2-debug

#SOLINCDIR = $(COINROOT)/include/coin #$(SYMROOT)/include
LIBPATHS = $(COINROOT)/lib
LPLIB = `PKG_CONFIG_PATH=$(COINROOT)/lib/pkgconfig pkg-config --libs coinutils`

#INCDIR        = $(addprefix -I,${SOLINCDIR})
INCDIR = `PKG_CONFIG_PATH=$(COINROOT)/lib/pkgconfig pkg-config --cflags coinutils`
#LDFLAGS       = $(addprefix -L,${LIBPATHS})
#LDFLAGS      += $(SYMDEFS)

#LINKFLAGS += $(addprefix ${SHLINKPREFIX},${LIBPATHS})

EXAMPLES = milpGen

examples: $(EXAMPLES)

milpGen : milpGen.cpp	

	$(CC) $(OPT) $(LDFLAGS) $(INCDIR) $(LINKFLAGS) $< -o $@ $(LPLIB)

clean : 

	rm -rf $(EXAMPLES)

#.SILENT:
