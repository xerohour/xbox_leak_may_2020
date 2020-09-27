# Select your TARGET processor.
#
SAMPLE=RegsUseDef
TARGET=$(PROCESSOR_ARCHITECTURE)
BUILD=debug
CC= cl /nologo
CPPFLAGS=/MD /Zi /Gy /Gf /c /W3 /GX /Ox /D NDEBUG /Fo.\$(BUILD)\$(SAMPLE).obj
LFLAGS=/debug /debugtype:cv,fixup /out:".\$(BUILD)\$(SAMPLE).exe" /pdb:"$(BUILD)\$(SAMPLE).pdb"

!include "..\samples.mak"

$(SAMPLE).exe : $(SAMPLE).obj
$(SAMPLE).obj : $(SAMPLE).cpp $(BUILD)

"$(BUILD)":
    if not exist "$(BUILD)/$(NULL)" mkdir "$(BUILD)"
.cpp.obj:
	$(CC) $(VINC) $(CPPFLAGS) $*.cpp
.obj.exe:
	$(CC) /Fe$@ $(BUILD)\$** /link $(LFLAGS) $(VLIB) \
		vulcan.lib
