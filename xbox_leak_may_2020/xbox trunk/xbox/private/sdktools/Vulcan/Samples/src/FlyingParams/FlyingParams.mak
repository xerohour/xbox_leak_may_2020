# Select your TARGET processor.
#
SAMPLE=FlyingParams
SAMPLEDLL=FlyingParamsDll
TARGET=$(PROCESSOR_ARCHITECTURE)
BUILD=debug
CC= cl /nologo
CPPFLAGS=/MD /Zi /Gy /Gf /c /W3 /GX /Oy- /D NDEBUG 
LFLAGS=/debug /debugtype:cv,fixup /pdb:"$(BUILD)\$*.pdb" /MACHINE:IX86 

!include "..\samples.mak"

all: $(SAMPLE).exe $(SAMPLEDLL).dll

$(SAMPLE).exe : $(SAMPLE).obj FlyParamsUI.obj $(SAMPLE).res
$(SAMPLE).obj : $(SAMPLE).cpp $(BUILD)
FlyParamsUI.obj : FlyParamsUI.cpp $(BUILD)
$(SAMPLE).res : $(SAMPLE).rc

$(SAMPLEDLL).dll : $(SAMPLEDLL).obj
$(SAMPLEDLL).obj : $(SAMPLEDLL).cpp $(BUILD)

"$(BUILD)":
    if not exist "$(BUILD)/$(NULL)" mkdir "$(BUILD)"
.cpp.obj:
	$(CC) $(VINC) $(CPPFLAGS) $*.cpp /Fo.\$(BUILD)\$*.obj
.obj.exe:
	$(CC) /Fe$@ .\$(BUILD)\$(SAMPLE).obj ./\$(BUILD)\FlyParamsUI.obj /link /out:".\$(BUILD)\$*.exe" $(LFLAGS) $(VLIB) .\$(BUILD)\$(SAMPLE).res \
		vulcan.lib
.obj.dll:
	$(CC) /LD /Fe$@ $(BUILD)\$*.obj /link /out:".\$(BUILD)\$*.dll" $(LFLAGS) /dll 

.rc.res:
	$(RC) /r /fo .\$(BUILD)\$*.res $*.rc 
