!if 0
Copyright (c) 1998-1999 Microsoft Corporation
!endif

PLAT_DIR           = daytona
# ALT_PROJECT_TARGET = .

!if defined(NTDEBUG) && "$(NTDEBUG)" != "ntsdnodebug"
DEBUG=1
!endif
