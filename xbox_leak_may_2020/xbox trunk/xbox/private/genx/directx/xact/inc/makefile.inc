headers: clean \
         $(PRIVATE_INC_PATH)\xactp.h \
         $(SDK_INC_PATH)\xact.h \
         $(XDK_INC_PATH)\xact.h 

$(PRIVATE_INC_PATH)\xactp.h: xact.x
    copy $? $@

$(SDK_INC_PATH)\xact.h: xact.x
    sed "/@@BEGIN_MSINTERNAL/,/@@END_MSINTERNAL/D" $? > $@

$(XDK_INC_PATH)\xact.h: xact.x
    sed "/@@BEGIN_MSINTERNAL/,/@@END_MSINTERNAL/D" $? > $@

clean:
    if exist $(PRIVATE_INC_PATH)\xactp.h      del /f /q $(PRIVATE_INC_PATH)\xactp.h
    if exist $(SDK_INC_PATH)\xact.h           del /f /q $(SDK_INC_PATH)\xact.h
    if exist $(XDK_INC_PATH)\xact.h           del /f /q $(XDK_INC_PATH)\xact.h
