# obj\$(TARGET_DIRECTORY)\xapi.def: xapi.src
#     $(TARGET_CPP) /EP $(CDEFINES) xapi.src > obj\$(TARGET_DIRECTORY)\xapi.def

$(XDK_LIB_PATH)\$(TARGETNAME).lib: $(SDK_LIB_PATH)\$(TARGETNAME).lib
    copy $** $@

# $(XDK_LIB_PATH)\$(TARGETNAME)lib$(D).lib: $(SDK_LIB_PATH)\$(TARGETNAME)lib$(D).lib
#     copy $** $@

