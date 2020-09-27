$(TARGETPATH)\xipsums.bin: $(XIPS)
    -md obj
    xipsign $@ $(XIPS)

