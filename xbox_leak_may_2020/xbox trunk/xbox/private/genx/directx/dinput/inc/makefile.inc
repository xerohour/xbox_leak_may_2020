#
# The contents of this file was moved from windows\directx\dinput\makefile0
#

COPYRIGHT_STRING = Copyright (c) 1990-1999  Microsoft Corporation


OUTPUT_HDRS = $(SDK_INC_PATH)\dinput.h \
              $(PRIVATE_INC_PATH)\dinputp.h

dinput: $(OUTPUT_HDRS)

clean: cleansrc

cleansrc:
    del $(OUTPUT_HDRS)

#
#   Yuck!  We can't use -u because it causes all trail-commented lines to
#   be deleted!  Instead, we must explicitly exclude DX3 rather than saying
#   "Anything that isn't DX5 should be excluded".
#
#   -u      = skip unknown flags
#   -ts foo = skip `foo' tags
#   -ta foo = include `foo' tags
#   -v  500 = this is version 500
#
#   Tags:
#
#       dx3  = DX3
#       dx5  = DX5
#       dx5  = DX5
#       dx5a = DX5a     (Win98 Gold)
#       dx5b2= DX5b2    (NT5 beta 2)
#       dx6  = DX6.1a   (Win98 OSR1)
#       dx7  = DX7      (Win2000 Gold)
#       dx8  = DX8
#
#HSPLITFLAGS=-u -ta dx5 -v 500      # Can't use this (See above)

# The following line builds the DX5 header
#HSPLITFLAGS=-ts dx3 -ta dx5 -ts dx5a -ts dx5b2 -ts dx6 -ts dx7 -ts dx8 -v 500

# The following line builds the DX5a header; note that DX5a includes DX5
#HSPLITFLAGS=-ts dx3 -ta dx5 -ta dx5a -ts dx5b2 -ts dx6 -ts dx7 -ts dx8 -v 50A

# The following line builds the DX5B2 header
#HSPLITFLAGS=-ts dx3 -ts dx5 -ts dx5a -ta dx5b2 -ts dx6 -ts dx7 -ts dx8 -v 5B2

# The following line builds the DX6.1a header
#HSPLITFLAGS=-ts dx3 -ts dx5 -ts dx5a -ts dx5b2 -ta dx6 -ts dx7 -ts dx8 -v 600

# The following line builds the DX7 header
#HSPLITFLAGS=-ts dx3 -ts dx5 -ts dx5a -ts dx5b2 -ts dx6 -ta dx7 -ts dx8 -v 700

# The following line builds the DX8 header
#HSPLITFLAGS=-ts dx3 -ts dx5 -ts dx5a -ts dx5b2 -ts dx6 -ts dx7 -ta dx8 -v 800

# The following line builds the DX8 for XBOX header
HSPLITFLAGS=-ts dx3 -ts dx5 -ts dx5a -ts dx5b2 -ts dx6 -ta dx7 -ts dx8 -v 800 -ts noxbox -ta dx8x


$(SDK_INC_PATH)\dinput.h $(PRIVATE_INC_PATH)\dinputp.h: dinput.w dimkhdr.m4
        m4 dinput.w >dinput.wx
        hsplit $(HSPLITFLAGS) -o dinput.x dinputp.x dinput.wx
        del dinput.wx
        wcshdr.exe < dinput.x > $(SDK_INC_PATH)\dinput.h
        del dinput.x
        wcshdr.exe < dinputp.x > $(PRIVATE_INC_PATH)\dinputp.h
        del dinputp.x


$(SDK_INC_PATH)\dinputd.h $(PRIVATE_INC_PATH)\dinputdp.h: dinputd.w dimkhdr.m4
        m4 dinputd.w >dinputd.wx
        hsplit $(HSPLITFLAGS) -o dinputd.x dinputdp.x dinputd.wx
        del dinputd.wx
        wcshdr.exe < dinputd.x > $(SDK_INC_PATH)\dinputd.h
        del dinputd.x
        wcshdr.exe < dinputdp.x > $(PRIVATE_INC_PATH)\dinputdp.h
        del dinputdp.x
