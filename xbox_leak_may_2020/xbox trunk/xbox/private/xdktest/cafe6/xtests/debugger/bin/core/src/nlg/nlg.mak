########################################################################
# Makefile for NLG test application program

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

goal:   nlg.xbe

nlg.xbe: nlg.exe
    imagebld -debug nlg.exe -testname:"Non-Local Goto (NLG) test title" -out:nlg.xbe

# xbox - use -nodefaultlib, else keep getting libcpmt.lib
nlg.exe: nlg.lnk nlgmain.obj eh01.obj eh02.obj ehutil.obj ljmp01.obj ljmp02.obj 
	link -nodefaultlib -out:nlg.exe -debug:full -debugtype:vc6 -subsystem:xbox -fixed:no @nlg.lnk

nlg.lnk:                   .\nlg.mak
	echo nlgmain.obj  > nlg.lnk
	echo eh01.obj  >> nlg.lnk
	echo eh02.obj  >> nlg.lnk
	echo ehutil.obj  >> nlg.lnk
	echo ljmp01.obj  >> nlg.lnk
	echo ljmp02.obj  >> nlg.lnk
    echo xapilib.lib >> nlg.lnk
    echo libcmt.lib >> nlg.lnk
    echo xboxkrnl.lib >> nlg.lnk
