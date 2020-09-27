########################################################################
# Makefile for cons01 test application program
#
# Build rules

.SUFFIXES: .c .obj .lib

#       .c = CMerge source file (producing .obj file)
#       .obj = standard PC object file format
#       .lib = standard PC object library format

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -Od -ZI 

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   cons01.exe

cons01.exe: cons01.lnk cons01.obj
	link -out:cons01.exe -debug:full -debugtype:CV -pdbtype:sept @cons01.lnk

cons01.lnk:                   .\cons01.mak
	echo cons01.obj  > cons01.lnk


