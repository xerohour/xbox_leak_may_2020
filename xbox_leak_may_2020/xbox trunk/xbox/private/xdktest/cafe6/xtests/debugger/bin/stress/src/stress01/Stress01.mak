########################################################################
# Makefile for Stress test application program

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

goal:   Stress01.xbe

Stress01.xbe: Stress01.exe
    imagebld -debug Stress01.exe -testname:Stress01 -out:Stress01.xbe

Stress01.exe: Stress01.lnk Stress01.obj
	link -out:Stress01.exe -debug:full -debugtype:vc6 -subsystem:xbox -fixed:no @Stress01.lnk

Stress01.lnk:                   .\Stress01.mak
	echo Stress01.obj  > Stress01.lnk
    echo xapilib.lib >> Stress01.lnk
    echo xboxkrnl.lib >> Stress01.lnk

