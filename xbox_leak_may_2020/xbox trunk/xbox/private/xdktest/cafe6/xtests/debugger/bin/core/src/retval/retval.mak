########################################################################
# Makefile for RETVAL test application program

!CMDSWITCHES +i

CC = cl

# original VC6 test had -ZI but xbox doesn't do "edit and continue"
# so we use -Zi instead - emmang@xbox

CC_FLAGS = -GX -Od -Zi -Zvc6

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal: retval.xbe	

retval.xbe: retval.exe
	imagebld /debug retval.exe -out:retval.xbe

retval.exe: retval.lnk retval.obj lineonly.obj nodebug.obj
	cl -c -Od -Zd -GX lineonly.cpp 
	cl -c -GX nodebug.cpp 
	link -nodefaultlib -out:retval.exe -debug:full -debugtype:vc6 -subsystem:xbox -fixed:no @retval.lnk

retval.lnk: retval.mak
	echo retval.obj  > retval.lnk
	echo lineonly.obj  >> retval.lnk
	echo nodebug.obj  >> retval.lnk
	echo xapilib.lib  >> retval.lnk
	echo libcmt.lib  >> retval.lnk
	echo xboxkrnl.lib  >> retval.lnk

