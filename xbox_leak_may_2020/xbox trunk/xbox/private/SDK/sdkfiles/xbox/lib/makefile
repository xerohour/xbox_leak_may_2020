VCLIBDIR=c:\progra~1\micros~3\vc98\lib
DXLIBDIR=dxlibs

VCLIBS = \
    $(VCLIBDIR)\kernel32.lib \
    $(VCLIBDIR)\user32.lib \
    $(VCLIBDIR)\gdi32.lib \
    $(VCLIBDIR)\advapi32.lib \
    $(VCLIBDIR)\shell32.lib \
    $(VCLIBDIR)\winmm.lib

DXLIBS = \
    $(DXLIBDIR)\d3d8.lib \
    $(DXLIBDIR)\dinput8.lib \
    $(DXLIBDIR)\dplayx.lib \
    $(DXLIBDIR)\dsound.lib \
    $(DXLIBDIR)\dxguid.lib

OBJS = \
    unknwn.obj

xbox.lib: $(VCLIBS) $(DXLIBS) $(OBJS)
    link -lib -out:$@ $(VCLIBS) $(DXLIBS) $(OBJS)
