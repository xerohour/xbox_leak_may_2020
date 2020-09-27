TARGET = $(TARGETPATH)\$(TARGET_DIRECTORY)\$(TARGETNAME).$(TARGETEXT)

$(DESTFILE): $(TARGET)
    chmode -r $@
    copy $? $@
    chmode +r $@
