INCLUDES=\
    $(PRIVATE_INC_PATH); \
    $(BASEDIR)\private\ntos\inc; \
    $(BASEDIR)\private\windows\directx\dplay\protocol; \
    $(BASEDIR)\private\windows\directx\dplay\common; \
    $(BASEDIR)\private\windows\directx\dplay\dnaddress; \
    $(BASEDIR)\private\windows\directx\dplay\core; \
    $(BASEDIR)\private\windows\directx\dplay\sp\wsock; \
    $(BASEDIR)\private\online\client\inc


C_DEFINES=\
    -D_UNICODE \
    -DUNICODE \
    -DNT \
    -DMICHAEL \
    -DCINTERFACE \
    -DDPLAY_DOWORK

MSC_WARNING_LEVEL=/W3 /WX

!ifdef DEBUG_DPLAY
MSC_OPTIMIZATION=$(MSC_OPTIMIZATION) /FAcs
!endif
