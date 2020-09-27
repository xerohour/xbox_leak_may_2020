# Select your TARGET processor.
#
TARGET=$(PROCESSOR_ARCHITECTURE)
BUILD=debug
CC= cl /nologo
CPPFLAGS=/Zi /Gy /Gf /c /W3 /GX /Ox /D NDEBUG /Fo.\$(BUILD)\inststat.obj
LFLAGS=/debug /debugtype:cv,fixup /out:".\$(BUILD)\inststat.exe" /pdb:"$(BUILD)\inststat.pdb"

!include "..\samples.mak"

inststat.exe : inststat.obj
inststat.obj : inststat.cpp $(BUILD)

"$(BUILD)":
    if not exist "$(BUILD)/$(NULL)" mkdir "$(BUILD)"
.cpp.obj:
	$(CC) $(VINC) $(CPPFLAGS) $*.cpp
.obj.exe:
	$(CC) /Fe$@ $(BUILD)\$** /link $(LFLAGS) $(VLIB) \
		vulcan.lib
