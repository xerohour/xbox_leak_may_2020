!if 0
Copyright (c) 1998 Microsoft Corporation
!endif

PLAT_DIR           = win9x
ALT_PROJECT_TARGET = win9x

USE_MAPSYM = 1

C_DEFINES    = $(C_DEFINES) /UWINNT /DWIN95

!ifdef NTDEBUG
C_DEFINES = $(C_DEFINES) /DDEBUG_RETAIL /DDEBUG
!endif
