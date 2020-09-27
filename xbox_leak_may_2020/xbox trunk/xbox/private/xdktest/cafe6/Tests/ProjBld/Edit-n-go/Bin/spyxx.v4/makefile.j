!message NOTE: Must be running on NT-J for correct compilation of Japanese resources !

!IFNDEF DEBUG
DEBUGFLAG=WinRelease
!ELSE
DEBUGFLAG=WinDebug
!ENDIF

!IF "$(CPU)" == "MIPS"
RTEXE=_mips
!ELSE
RTEXE=
!ENDIF

# this is done to work around MIPS-J compiler because it's not DBC-enabled
!IF "$(CPU)" == "MIPS"
RTESCAPE=-b
!ELSE
RTESCAPE=-h
!ENDIF

# now that we split RC files, $(TRANSLATE) switch is for real
#!IFNDEF TRANSLATE
#TRANSLATE=1
#!ENDIF

!IFNDEF RTTOOL
RTTOOL_TMP=\SUSHI\LOCALIZE\rt$(RTEXE)
!ELSE
RTTOOL_TMP=$(RTTOOL)$(RTEXE)
!ENDIF

!IF "$(TRANSLATE)" == "1"
SPYJ_RC=$(RTTOOL_TMP) -B spyj.lng spy.cr spy.rc
!ELSE
SPYJ_RC=copy spyj.rc spy.rc
!ENDIF

all:
	attrib -r spy.rc
	copy spy.rc spy.cr
	$(SPYJ_RC)
	copy spy.rc rc.out
	touch spy.rc

	nmake /f spy.mak CFG="$(DEBUGFLAG)"

	copy spy.cr spy.rc
