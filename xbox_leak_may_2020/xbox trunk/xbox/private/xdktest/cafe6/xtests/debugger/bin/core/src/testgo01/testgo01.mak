########################################################################
# Makefile for TESTGO test application program

!CMDSWITCHES +i

CC = cl

# xbox - change -ZI to -Zi
CC_FLAGS = -Od -Zi -Zvc6

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.c{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   testgo01.xbe

testgo01.xbe: testgo01.exe
    imagebld -debug testgo01.exe -testname:testgo01 -out:testgo01.xbe

testgo01.exe: testgo01.lnk testgo.obj
	link -out:testgo01.exe -debug:full -debugtype:vc6 -subsystem:xbox -fixed:no @testgo01.lnk

testgo01.lnk:                   .\testgo01.mak
	echo testgo.obj  > testgo01.lnk
    echo xapilib.lib >> testgo01.lnk
    echo xboxkrnl.lib >> testgo01.lnk

