# Select your TARGET processor.
#
TARGET=$(PROCESSOR_ARCHITECTURE)
BUILD=debug
CC= cl /nologo
CPPFLAGS=/Zi /Gy /Gf /c /W3 /GX /Ox /D NDEBUG /Fo.\$(BUILD)\peephole.obj
LFLAGS=/debug /debugtype:cv,fixup /out:".\$(BUILD)\peephole.exe" /pdb:"$(BUILD)\peephole.pdb"

!include "..\samples.mak"

peephole.exe : peephole.obj
peephole.obj : peephole.cpp $(BUILD)

"$(BUILD)":
    if not exist "$(BUILD)/$(NULL)" mkdir "$(BUILD)"
.cpp.obj:
	$(CC) $(VINC) $(CPPFLAGS) $*.cpp
.obj.exe:
	$(CC) /Fe$@ $(BUILD)\$** /link $(LFLAGS) $(VLIB) \
		vulcan.lib
