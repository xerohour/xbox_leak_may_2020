xvchat.h: xvchat.x
    sed "/@@BEGIN_MSINTERNAL/,/@@END_MSINTERNAL/D" $? > $@

$(PRIVATE_INC_PATH)\xvchatp.h: xvchat.x
    copy $? $@

{}.h{$(SDK_INC_PATH)}.h:
    copy $** $@

{}.h{$(XDK_INC_PATH)}.h:
    copy $** $@

clean:
	del /F /Q $(NTTARGETFILE0)
