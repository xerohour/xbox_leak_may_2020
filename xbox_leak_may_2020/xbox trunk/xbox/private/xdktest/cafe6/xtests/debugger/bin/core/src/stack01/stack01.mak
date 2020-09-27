########################################################################
# Makefile for STACK01 test application program

!CMDSWITCHES +i

CC = cl

# xbox changed -ZI to -zi
C_FLAGS = -Od -MLd -Zi -Zvc6
CPP_FLAGS = -Od -MLd -Zi -Zvc6

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

goal:   stack01.xbe

stack01.xbe: stack01.exe
	imagebld -debug stack01.exe -stack:0x10000 -testname:stack01 -out:stack01.xbe

stack01.exe: stack01.lnk cstack.obj cppstack.obj
	link -out:stack01.exe -debug:full -debugtype:vc6 -subsystem:xbox -fixed:no @stack01.lnk

stack01.lnk:                   .\stack01.mak
	echo cstack.obj    > stack01.lnk
	echo cppstack.obj  >> stack01.lnk
	echo xapilibd.lib   >> stack01.lnk
	echo xboxkrnl.lib  >> stack01.lnk
