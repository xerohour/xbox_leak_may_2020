# Select your TARGET processor.
#
SAMPLE=FlyingICount
SAMPLEUI=FlyingICountUI
TARGET=$(PROCESSOR_ARCHITECTURE)
BUILD=debug
CC= cl /nologo
CPPFLAGS=/MD /Zi /Gy /Gf /c /W3 /GX /Oy- /D NDEBUG
LFLAGS=/debug /debugtype:cv,fixup /pdb:"$(BUILD)\$*.pdb" /MACHINE:IX86

!include "..\samples.mak"

all: $(SAMPLE).exe

$(SAMPLE).exe : $(SAMPLE).obj $(SAMPLEUI).obj $(SAMPLE).res
$(SAMPLEUI).obj : $(SAMPLEUI).cpp $(BUILD)
$(SAMPLE).obj : $(SAMPLE).cpp $(BUILD)
$(SAMPLE).res : $(SAMPLE).rc

"$(BUILD)":
    if not exist "$(BUILD)/$(NULL)" mkdir "$(BUILD)"
.cpp.obj:
	$(CC) $(VINC) $(CPPFLAGS) $*.cpp /Fo.\$(BUILD)\$*.obj
.obj.exe:
	$(CC) /Fe$@ .\$(BUILD)\$(SAMPLE).obj .\$(BUILD)\$(SAMPLEUI).obj /link /out:".\$(BUILD)\$*.exe" $(LFLAGS) $(VLIB) .\$(BUILD)\$(SAMPLE).res \
		vulcan.lib
.rc.res:
	$(RC) /r /fo .\$(BUILD)\$*.res $*.rc 
