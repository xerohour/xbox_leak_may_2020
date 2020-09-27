WINERROR_TARGETS=$(SDK_INC_PATH)\winerror.h   \
                 $(SDK_INC_PATH)\winerror.dbg \
                 $(XDK_INC_PATH)\winerror.h   \
                 $(O)\msg00001.bin            \
                 $(O)\winerror.rc

clean:
    del /Q /F $(WINERROR_TARGETS)

$(SDK_INC_PATH)\winerror.h : $(O)\winerror.h
    copy $** $@

$(XDK_INC_PATH)\winerror.h : $(O)\winerror.h
    copy $** $@
