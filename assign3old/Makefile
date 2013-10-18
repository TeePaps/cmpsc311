#
# CMPSC311 - Starter Makefile

# Variables
CC=gcc 
LINK=gcc
CFLAGS=-c -Wall -I. -fpic -g
LINKFLAGS=-L. -g
LIBFLAGS=-shared -Wall
LINKLIBS=-lcmpsc311 -lsmsa -lgcrypt

# Files to build
SASIM_OBJFILES=		smsa_sim.o \
			smsa_driver.o 
TARGETS=		smsasim
					
# Suffix rules
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS)  -o $@ $<

# Productions

dummy : $(TARGETS) 

smsasim : $(SASIM_OBJFILES) $(LIBS)
	$(LINK) $(LINKFLAGS) -o $@ $(SASIM_OBJFILES) $(LINKLIBS) 
	
clean:
	rm -f $(TARGETS) $(SASIM_OBJFILES)
  
# Dependancies
