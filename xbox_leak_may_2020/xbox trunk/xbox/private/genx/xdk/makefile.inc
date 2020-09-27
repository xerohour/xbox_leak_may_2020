
TARGETSRCS= \
             $(XDK_INC_PATH)\BaseTsd.h       \
             $(XDK_INC_PATH)\CGuid.h         \
             $(XDK_INC_PATH)\GuidDef.h       \
             $(XDK_INC_PATH)\PopPack.h       \
             $(XDK_INC_PATH)\PshPack1.h      \
             $(XDK_INC_PATH)\PshPack2.h      \
             $(XDK_INC_PATH)\PshPack4.h      \
             $(XDK_INC_PATH)\PshPack8.h      \
             $(XDK_INC_PATH)\XObjBase.h


xdkinc: $(TARGETSRCS)

clean: cleansrc

cleansrc:
    -del /q /f $(TARGETSRCS) 2>nul

{$(SDK_INC_PATH)}.h{$(XDK_INC_PATH)}.h:
    copy $** $@
