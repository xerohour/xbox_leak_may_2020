TARGETSRCS0= \
        $(SDK_INC_PATH)\ntddstor.h \
        $(SDK_INC_PATH)\ntdddisk.h \
        $(SDK_INC_PATH)\ntddscsi.h \
        $(SDK_INC_PATH)\ntddcdrm.h

clean:
	-del /F /Q $(NTTARGETFILE0)

#
# ntdd*
#

$(SDK_INC_PATH)\ntddstor.h : ntddstor.x
    hsplit -o $@ nul -bt2 BEGIN_MSINTERNAL END_MSINTERNAL -c @@ ntddstor.x

$(SDK_INC_PATH)\ntdddisk.h : ntdddisk.x
    hsplit -o $@ nul -bt2 BEGIN_MSINTERNAL END_MSINTERNAL -c @@ ntdddisk.x

$(SDK_INC_PATH)\ntddcdrm.h : ntddcdrm.x
    hsplit -o $@ nul -bt2 BEGIN_MSINTERNAL END_MSINTERNAL -c @@ ntddcdrm.x

$(SDK_INC_PATH)\ntddscsi.h : ntddscsi.x
    hsplit -o $@ nul -bt2 BEGIN_MSINTERNAL END_MSINTERNAL -c @@ ntddscsi.x

#
# usb.h, usbd.h, and hcdi.h
#

$(USBD_PATH)\usbd.h: usb.x
    hsplit -ta usbd -ts usb -ts hcdi -ts usbxapi -ta usbandusbd -ta hcdiandusbd -ta usbxapiandusbd -ta nohcdi -ta nousbxapi -o $@ -i usb.x

$(USB_INC_PATH)\hcdi.h: usb.x
    hsplit -ts usbd -ts usb -ta hcdi -ts usbxapi -ts usbandusbd -ta hcdiandusbd -ts usbxapiandusbd -ts nohcdi -ta nousbxapi -o $@ -i usb.x

$(DDK_INC_PATH)\usb.h: usb.x
    hsplit -ts usbd -ta usb -ts hcdi -ts usbxapi -ta usbandusbd -ts hcdiandusbd -ts usbxapiandusbd -ta nohcdi -ta nousbxapi -o $@ -i usb.x

$(PRIVATE_INC_PATH)\usbxapi.h: usb.x
    hsplit -ts usbd -ts usb -ts hcdi -ta usbxapi -ts usbandusbd -ts hcdiandusbd -ta usbxapiandusbd -ta nohcdi -ts nousbxapi -o $@ -i usb.x


#Inference rules
.SUFFIXES :.h .ref	

# Copy .h files from here to public\sdk\inc
{}.h{$(SDK_INC_PATH)}.h:
	 copy $** $@

# Copy .ref files from here to public\sdk\inc
{}.ref{$(SDK_INC_PATH)}.ref:
	 copy $** $@


