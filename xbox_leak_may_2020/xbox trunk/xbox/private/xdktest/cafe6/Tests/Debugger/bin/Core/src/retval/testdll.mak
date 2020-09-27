########################################################################
# Makefile for TESTDLL.DLL (RETVAL test application program)

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -GX -Od -ZI 

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:  testdll.dll	

testdll.dll: testdll.lnk testdll.obj
	link -out:testdll.dll -dll -implib:"testdll.lib" -debug:full -debugtype:CV -pdbtype:sept @testdll.lnk
	
testdll.lnk:                   .\testdll.mak
	echo testdll.obj  > testdll.lnk


