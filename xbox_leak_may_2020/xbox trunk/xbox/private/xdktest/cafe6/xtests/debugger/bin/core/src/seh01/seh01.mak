########################################################################
# Makefile for seh01 test application program

!CMDSWITCHES +i

CC = cl

# xbox - change -ZI to -Zi
CC_FLAGS = -GX -Od -Zi -Zvc6

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   seh01.xbe

seh01.xbe: seh01.exe
    imagebld -debug seh01.exe -testname:"Structured Exception Handling (SEH) test title" -out:seh01.xbe

seh01.exe: seh01.lnk seh01.obj sehutil.obj
	link -out:seh01.exe -debug:full -debugtype:vc6 -subsystem:xbox -fixed:no @seh01.lnk

seh01.lnk:                   .\seh01.mak
	echo seh01.obj  > seh01.lnk
	echo sehutil.obj  >> seh01.lnk
    echo xapilib.lib  >> seh01.lnk
    echo xboxkrnl.lib >> seh01.lnk

