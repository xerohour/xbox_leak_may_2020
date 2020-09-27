dmusici.h: dmusici.x
    sed "/@@BEGIN_MSINTERNAL/,/@@END_MSINTERNAL/D" $? > $@

$(PRIVATE_INC_PATH)\dmusicip.h: dmusici.x
    copy $? $@

{}.h{$(PRIVATE_INC_PATH)}.h:
    copy $** $@

{}.h{$(SDK_INC_PATH)}.h:
    copy $** $@

{}.h{$(XDK_INC_PATH)}.h:
    copy $** $@

clean:
	-del /F /Q $(NTTARGETFILE0)

