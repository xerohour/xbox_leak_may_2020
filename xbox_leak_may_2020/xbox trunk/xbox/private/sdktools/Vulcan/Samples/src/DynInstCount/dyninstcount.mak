# Select your TARGET processor.
#
SAMPLE=DynInstCount
SAMPLEDLL=DynInstCountDll
TARGET=$(PROCESSOR_ARCHITECTURE)
BUILD=debug
CC= cl /nologo
CPPFLAGS=/MD /Zi /Gy /Gf /c /W3 /GX /Oy- /D NDEBUG
LFLAGS=/debug /debugtype:cv,fixup /pdb:"$(BUILD)\$*.pdb" /MACHINE:IX86

!include "..\samples.mak"

all: $(SAMPLE).exe $(SAMPLEDLL).dll

$(SAMPLE).exe : $(SAMPLE).obj
$(SAMPLE).obj : $(SAMPLE).cpp $(BUILD)

$(SAMPLEDLL).dll : $(SAMPLEDLL).obj $(SAMPLEDLL).res 
$(SAMPLEDLL).obj : $(SAMPLEDLL).cpp $(BUILD)
$(SAMPLEDLL).res : $(SAMPLEDLL).rc

"$(BUILD)":
    if not exist "$(BUILD)/$(NULL)" mkdir "$(BUILD)"
.cpp.obj:
	$(CC) $(VINC) $(CPPFLAGS) $*.cpp /Fo.\$(BUILD)\$*.obj
.obj.exe:
	$(CC) /Fe$@ $(BUILD)\$** /link /out:".\$(BUILD)\$*.exe" $(LFLAGS) $(VLIB) \
		vulcan.lib
.obj.dll:
	$(CC) /LD /Fe$@ $(BUILD)\$*.obj /link /out:".\$(BUILD)\$*.dll" .\$(BUILD)\$(SAMPLEDLL).res $(LFLAGS) $(VLIB) /dll \
		vulcan.lib

.rc.res:
	$(RC) /r /fo .\$(BUILD)\$*.res $*.rc 
