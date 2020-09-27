########################################################################
# Makefile for TESTGO test application program

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -Od -ZI 

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.c{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   testgo01.exe

testgo01.exe: testgo01.lnk testgo.obj
	link -out:testgo01.exe -debug:full -debugtype:CV -pdbtype:sept @testgo01.lnk

testgo01.lnk:                   .\testgo01.mak
	echo testgo.obj  > testgo01.lnk

