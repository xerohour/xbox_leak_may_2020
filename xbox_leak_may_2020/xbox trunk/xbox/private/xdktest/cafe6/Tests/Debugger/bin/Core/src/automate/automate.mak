########################################################################
# Makefile for AUTOMATE test application program
#
# Build rules

.SUFFIXES: .c .a .obj .lib

#       .c = CMerge source file (producing .obj file)
#       .a = ASM68 source file (producing .obj file)
#       .obj = standard PC object file format
#       .lib = standard PC object library format

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -Od -Zi

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   automate.exe

automate.exe: automate.lnk automate.obj
	link -out:automate.exe -debug:full -debugtype:CV -pdbtype:sept @automate.lnk

automate.lnk:                   .\automate.mak
	echo automate.obj  > automate.lnk


