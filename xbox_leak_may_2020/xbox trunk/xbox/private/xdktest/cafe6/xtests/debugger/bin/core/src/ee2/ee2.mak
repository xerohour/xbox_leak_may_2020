########################################################################
# Makefile for EE2 test application program

!CMDSWITCHES +i

CC = cl

#xbox CC_FLAGS = -Od -ZI -DWIN32 -D_DEBUG -D_CONSOLE -D_MBCS
CC_FLAGS = -Od -Zi -DWIN32 -D_DEBUG -D_CONSOLE -D_MBCS -Zvc6

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   ee2.xbe

ee2.xbe: ee2.exe
	imagebld -debug ee2.exe -out:ee2.xbe

!IF "$(CFG)" == "INCREMENTAL"
!MESSAGE Linking ee2 incrementally

ee2.exe: ee2.lnk eetest.obj
	link -out:ee2.exe -debug:full -debugtype:vc6 -subsystem:xbox -fixed:no /incremental:yes @ee2.lnk
!ELSE
!MESSAGE Linking ee2 non-incrementally

ee2.exe: ee2.lnk eetest.obj
	link -out:ee2.exe -debug:full -debugtype:vc6 -subsystem:xbox -fixed:no -incremental:no @ee2.lnk
!ENDIF

ee2.lnk:                   .\ee2.mak
	echo eetest.obj   > ee2.lnk
	echo xapilib.lib  >> ee2.lnk
	echo xboxkrnl.lib >> ee2.lnk
	echo d3dx8d.lib   >> ee2.lnk
