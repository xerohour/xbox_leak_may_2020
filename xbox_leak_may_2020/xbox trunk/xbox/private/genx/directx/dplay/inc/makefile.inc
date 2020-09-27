dplay8.h: dplay8.x
    sed "/@@BEGIN_MSINTERNAL/,/@@END_MSINTERNAL/D" $? > $@

$(PRIVATE_INC_PATH)\dplay8p.h: dplay8.x
    copy $? $@

dpaddr.h: dpaddr.x
    sed "/@@BEGIN_MSINTERNAL/,/@@END_MSINTERNAL/D" $? > $@

$(PRIVATE_INC_PATH)\dpaddrp.h: dpaddr.x
    copy $? $@

$(PRIVATE_INC_PATH)\dpsp8p.h: dpsp8.x
    copy $? $@

{}.h{$(SDK_INC_PATH)}.h:
    copy $** $@

{}.h{$(XDK_INC_PATH)}.h:
    copy $** $@

clean:
	del /F /Q $(NTTARGETFILE0)
