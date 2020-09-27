$(_NT386TREE)\winx\dump\$(TARGETNAME).$(TARGETEXT): $(TARGET)
    copy $** $@

$(_NT386TREE)\winx\dump\$(TARGETNAME).inf: oemsetup.inf
    copy $** $@