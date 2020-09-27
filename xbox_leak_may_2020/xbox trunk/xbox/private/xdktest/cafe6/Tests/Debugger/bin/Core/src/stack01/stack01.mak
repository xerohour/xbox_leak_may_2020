########################################################################
# Makefile for STACK01 test application program

!CMDSWITCHES +i

CC = cl

C_FLAGS = -Od -MLd -ZI 
CPP_FLAGS = -Od -MLd -ZI

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.c{}.obj:
	$(CC) -c $(C_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CPP_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   stack01.exe


stack01.exe: stack01.lnk cstack.obj cppstack.obj
	link -out:stack01.exe -debug:full -debugtype:CV -pdbtype:sept @stack01.lnk

stack01.lnk:                   .\stack01.mak
	echo cstack.obj  > stack01.lnk
	echo cppstack.obj  >> stack01.lnk
