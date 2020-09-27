########################################################################
# Makefile for AUTOPANE test application program
#
# Build rules

.SUFFIXES: .c .obj .lib

#       .c   = C source file (producing .obj file)
#		.cpp = C++ source file (producing .obj file)
#       .obj = standard PC object file format
#       .lib = standard PC object library format

!CMDSWITCHES +i

CC = cl

# xbox - change -ZI to zi
CC_FLAGS = -Od -ZI

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   autopane.xbe

autopane:xbe: autopane.exe
	imagebld -debug autopane.exe -out:autopane.xbe

autopane.exe: autopane.lnk autopane.obj
	link -out:autopane.exe -debug:full -debugtype:CV -pdbtype:sept @autopane.lnk

autopane.lnk:                   .\autopane.mak
	echo autopane.obj  > autopane.lnk
	echo xapilib.lib  >> autopane.lnk
	echo ntoskrnl.lib >> autopane.lnk


