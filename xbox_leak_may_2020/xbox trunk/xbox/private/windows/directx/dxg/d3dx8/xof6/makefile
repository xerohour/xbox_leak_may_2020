!if "$(RLBUILDENV)" == "Memphis"
#
# Win 95
#

!if "$(RLROOT)" == ""
!error Environment variable RLROOT must be set to point at Retained Mode Root
!endif

!if "$(MANROOT)" == ""
!if "$(DXSDKROOT)" == ""
!error Environment variable DXSDKROOT must be set to point at DirectX SDK Root
!else
MANROOT = $(RLROOT)
!endif
!endif

!include $(MANROOT)\build.mk

!else
#
# Windows NT or IE build
#

!include $(NTMAKEENV)\makefile.def
!endif
