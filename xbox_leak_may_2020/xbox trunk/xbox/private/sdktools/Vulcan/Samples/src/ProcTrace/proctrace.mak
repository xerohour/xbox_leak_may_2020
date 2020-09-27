# Select your TARGET processor.
#
SAMPLE=ProcTrace
SAMPLEDLL=ProcTraceDll
TARGET=$(PROCESSOR_ARCHITECTURE)
BUILD=debug
CC= cl /nologo
CPPFLAGS=/MD /Zi /Gy /Gf /c /W3 /GX /Ox /Oy- /D NDEBUG
LFLAGS=/debug /debugtype:cv,fixup /pdb:"$(BUILD)\$*.pdb" /MACHINE:IX86

!include "..\samples.mak"

all: $(SAMPLE).exe $(SAMPLEDLL).dll

$(SAMPLE).exe : $(SAMPLE).obj
$(SAMPLE).obj : $(SAMPLE).cpp $(BUILD)

$(SAMPLEDLL).dll : $(SAMPLEDLL).obj
$(SAMPLEDLL).obj : $(SAMPLEDLL).cpp $(BUILD)


"$(BUILD)":
    if not exist "$(BUILD)/$(NULL)" mkdir "$(BUILD)"
.cpp.obj:
	$(CC) $(VINC) $(CPPFLAGS) $*.cpp /Fo.\$(BUILD)\$*.obj
.obj.exe:
	$(CC) /Fe$@ $(BUILD)\$** /link /out:".\$(BUILD)\$*.exe" $(LFLAGS) $(VLIB) \
		vulcan.lib
.obj.dll:
	$(CC) /LD /Fe$@ $(BUILD)\$*.obj /link /out:".\$(BUILD)\$*.dll" $(LFLAGS) $(VLIB) /dll \
		vulcan.lib
