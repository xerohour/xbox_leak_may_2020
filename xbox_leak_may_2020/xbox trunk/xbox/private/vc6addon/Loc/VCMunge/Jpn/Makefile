############################################################################################
## This makefile will munge all template, and other non standeard resource
## from a resource only stub (.stb) dll to an existing .dll.
##
##   $(IDELOC) = path for the root directory of the D IDE files
##             Example: SET IDELOC=D:\MSDEV
##
############################################################################################

!ifndef IDELOC
!MESSAGE IDELOC is not defined.
!ERROR	 Example: SET IDELOC=D:\MSDEV (pointing to the ..\MSDEV directory of the J product)
!endif

!ifndef RELEASE
RELEASE=0
!endif

!ifndef DEBUG
DEBUG=1
!endif

!IF "$(DEBUG)"=="0"
!UNDEF DEBUG
!ENDIF

!IF "$(OS)" == "Windows_NT"
LOGNT=
!ELSE
!UNDEF LOGNT
!ERROR Resource copy requires Windows NT!!
!ENDIF

!IFDEF DEBUG
LOGDIR=..\bldlogs.dbg
!ELSE
LOGDIR=..\bldlogs.ret
!ENDIF

FILE_LOG=results.log
ERRORLOG= \
!       IFDEF VERB
		| tee $(FILE_LOG) 2>&1
!       ELSE
		>> $(FILE_LOG) 2>&1
!       ENDIF

!IFDEF LOGNT
STDERROUT=$(ERRORLOG) || copy $(FILE_LOG) $(LOGDIR)\$@.log
!ELSE
STDERROUT=
!ENDIF

all:  mfcapwz done




always: logdir
	set PATH=$(PATH)
	set IDELOC=$(IDELOC)
	set IDE=$(IDE)
	set INCLUDE=$(IDE)\include;$(IDE)\MFC\include;$(INCLUDE)
	set LIB=$(IDE)\lib;$(IDE)\MFC\lib;$(LIB)
	set CPU=$(CPU)
	set MAKEFLAGS=K
	set WIN32=1

logdir:
	if not exist $(LOGDIR) md $(LOGDIR)

cleanbin:
	-attrib /S -r bin\*.*
	-del /s bin\*.stb

clean: clean_mfcapwz 


#######################################################
###		CLEAN DEV-STUDIO
#######################################################
##############
clean_mfcapwz: 
	-@echotime /t  Cleaning $@ stub
	cd mfcapwz

!IFDEF DEBUG
	$(MAKE) /k DEBUG=1 clean $(STDERROUT)
	delnode /q Debug
!ELSE
	$(MAKE) /k DEBUG=0 clean $(STDERROUT)
	delnode /q Release
!ENDIF DEBUG
	cd ..\


#######################################################
###		END CLEANING DEV-STUDIO
#######################################################



#######################################################
###		DEV-STUDIO TEMPLATE MUNGE
#######################################################


################
mfcapwz: always
	-@echotime /t  Building $@ stub

	cd mfcapwz
	$(MAKE) /k DEBUG=0 /all $(STDERROUT)

	cd ..\

!IFDEF DEBUG
	$(MAKE) /k DEBUG=1 $(STDERROUT)
	rescopy .\bin\mfcapwz.stb $(IDELOC)\bind\ided\mfcapwzd.dll "TEMPLATE":all
!ELSE
	rescopy .\bin\mfcapwz.stb $(IDELOC)\mfcapwz.dll "TEMPLATE":all
!ENDIF
	-@echotime /t  Done Building $@ stub

###############

done: always
	-@echotime /t  End of resource munging.


#######################################################
###		END DEV-STUDIO TEMPLATE MUNGE
#######################################################


help:
!MESSAGE
!MESSAGE TARGETS:
!MESSAGE        mfcapwz:    mfcapwz resource only stub dll
!MESSAGE        all:        all above
!MESSAGE SWITCHES:
!MESSAGE        VERB=     Don't silence NMAKE
!MESSAGE        DEBUG=0   Build the release version (defaults to release)
!MESSAGE        DEBUG=1   Build the debug version (defaults to release)
