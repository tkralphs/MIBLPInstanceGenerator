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

#SYMROOT = $(HOME)/Sym-Trunk/SYMPHONY
#COINROOT =$(HOME)/Sym-Trunk/build

COINROOT = /home/ted/Projects/build-mibs-1.2/debug

SOLINCDIR = $(COINROOT)/include/coin #$(SYMROOT)/include
LIBPATHS = $(COINROOT)/lib
LPLIB = -lCoinUtils -lbz2 -lz -llapack -lblas -lm

INCDIR        = $(addprefix -I,${SOLINCDIR})
LDFLAGS       = $(addprefix -L,${LIBPATHS})
LDFLAGS      += $(SYMDEFS)

LINKFLAGS += $(addprefix ${SHLINKPREFIX},${LIBPATHS})

EXAMPLES = milpGen

examples: $(EXAMPLES)

milpGen : milpGen.cpp	

	$(CC) $(OPT) $(LDFLAGS) $(INCDIR) $(LINKFLAGS) $< -o $@ $(LPLIB)

clean : 

	rm -rf $(EXAMPLES)

#.SILENT:
