  ########################################################################
# Makefile for DLL.DLL & drivedll.EXE (EE test application program)
#

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -Od -ZI -D"WIN32" -DDLL_CLASS

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
{}.c{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
{}.cxx{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal: fibondll.dll drivedll.exe

fibondll.dll: fibondll.lnk dllmain.obj fibon.obj
	link -out:fibondll.dll -dll -implib:"fibondll.lib" -debug:full -debugtype:CV -pdbtype:sept @fibondll.lnk
	
fibondll.lnk:                   .\drivedll.mak
	xcopy dll.cpp .\dllmain.*
	echo dllmain.obj  > fibondll.lnk
	echo fibon.obj  >> fibondll.lnk

drivedll.exe: drivedll.lnk fibondll.lib driverdll.obj 
	link -out:driverdll.exe -debug:full -debugtype:CV -pdbtype:sept @driverdll.lnk

drivedll.lnk:                   .\drivedll.mak
	echo driverdll.obj  > driverdll.lnk
	echo fibondll.lib  >> driverdll.lnk
