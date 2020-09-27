TARGETPATH=obj
TARGETTYPE=PROGRAM
UMTYPE=xboxapp

INCLUDES=$(INCLUDES);$(PRIVATE_INC_PATH);$(SDK_INC_PATH);..\lib

TARGETLIBS= \
    ..\lib\$(O)\xnettest.lib \
    $(LIBRARY_PATH)\xnet$(D).lib

