#
# Do not include this file directly.  Instead, use win32.inc, console.inc, 
# xbox.inc or dsound.inc
#

TARGETTYPE=LIBRARY

DXBASEDIR=$(BASEDIR)\private\windows\directx
DSBASEDIR=$(DXBASEDIR)\dsound

!ifdef DPFLVL_DEFAULT
C_DEFINES=$(C_DEFINES) -DDPFLVL_DEFAULT="$(DPFLVL_DEFAULT)"
!endif

!ifdef DPFLVL_DEFAULT_BREAK
C_DEFINES=$(C_DEFINES) -DDPFLVL_DEFAULT_BREAK="$(DPFLVL_DEFAULT_BREAK)"
!endif

NTDBGFILES=1
USE_MAPSYM=1

USER_C_FLAGS=$(USER_C_FLAGS) /FAcs

!if !$(FREEBUILD)
MSC_OPTIMIZATION=$(MSC_OPTIMIZATION) /Od
!endif

INCLUDES=\
    $(DSBASEDIR)\common
