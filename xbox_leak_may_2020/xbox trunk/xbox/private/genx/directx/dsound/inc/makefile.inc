headers: clean \
         $(PRIVATE_INC_PATH)\dsoundp.h \
         $(SDK_INC_PATH)\dsound.h \
         $(XDK_INC_PATH)\dsound.h \
         $(PRIVATE_INC_PATH)\dsfxparmp.h \
         $(SDK_INC_PATH)\dsfxparm.h \
         $(XDK_INC_PATH)\dsfxparm.h \
         $(SDK_INC_PATH)\wmvxmo.h \
         $(XDK_INC_PATH)\wmvxmo.h \
         $(SDK_INC_PATH)\wavbndlr.h \
         $(XDK_INC_PATH)\wavbndlr.h

$(PRIVATE_INC_PATH)\dsoundp.h: dsound.x
    copy $? $@

$(SDK_INC_PATH)\dsound.h: dsound.x
    sed "/@@BEGIN_MSINTERNAL/,/@@END_MSINTERNAL/D" $? > $@

$(XDK_INC_PATH)\dsound.h: $(SDK_INC_PATH)\dsound.h
    copy $? $@

$(PRIVATE_INC_PATH)\dsfxparmp.h: dsfxparm.x
    copy $? $@

$(SDK_INC_PATH)\dsfxparm.h: dsfxparm.x
    sed "/@@BEGIN_MSINTERNAL/,/@@END_MSINTERNAL/D" $? > $@

$(XDK_INC_PATH)\dsfxparm.h: $(SDK_INC_PATH)\dsfxparm.h
    copy $? $@

$(SDK_INC_PATH)\wmvxmo.h: wmvxmo.h
    copy $? $@

$(XDK_INC_PATH)\wmvxmo.h: $(SDK_INC_PATH)\wmvxmo.h
    copy $? $@

$(SDK_INC_PATH)\wavbndlr.h: wavbndlr.h
    copy $? $@

$(XDK_INC_PATH)\wavbndlr.h: $(SDK_INC_PATH)\wavbndlr.h
    copy $? $@

clean:
    if exist $(PRIVATE_INC_PATH)\dsoundp.h      del /f /q $(PRIVATE_INC_PATH)\dsoundp.h
    if exist $(SDK_INC_PATH)\dsound.h           del /f /q $(SDK_INC_PATH)\dsound.h
    if exist $(XDK_INC_PATH)\dsound.h           del /f /q $(XDK_INC_PATH)\dsound.h
    if exist $(PRIVATE_INC_PATH)\dsfxparmp.h    del /f /q $(PRIVATE_INC_PATH)\dsfxparmp.h
    if exist $(SDK_INC_PATH)\dsfxparm.h         del /f /q $(SDK_INC_PATH)\dsfxparm.h
    if exist $(XDK_INC_PATH)\dsfxparm.h         del /f /q $(XDK_INC_PATH)\dsfxparm.h
    if exist $(SDK_INC_PATH)\wmvxmo.h           del /f /q $(SDK_INC_PATH)\wmvxmo.h
    if exist $(XDK_INC_PATH)\wmvxmo.h           del /f /q $(XDK_INC_PATH)\wmvxmo.h
    if exist $(SDK_INC_PATH)\wavbnldr.h         del /f /q $(SDK_INC_PATH)\wavbnldr.h
    if exist $(XDK_INC_PATH)\wavbnldr.h         del /f /q $(XDK_INC_PATH)\wavbnldr.h
