########################################################################
# Makefile for NLG test application program

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -GX -Od -ZI 

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   nlg.exe

nlg.exe: nlg.lnk nlgmain.obj eh01.obj eh02.obj ehutil.obj ljmp01.obj ljmp02.obj 
	link -out:nlg.exe -debug:full -debugtype:CV -pdbtype:sept @nlg.lnk

nlg.lnk:                   .\nlg.mak
	echo nlgmain.obj  > nlg.lnk
	echo eh01.obj  >> nlg.lnk
	echo eh02.obj  >> nlg.lnk
	echo ehutil.obj  >> nlg.lnk
	echo ljmp01.obj  >> nlg.lnk
	echo ljmp02.obj  >> nlg.lnk
