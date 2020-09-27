# Select your TARGET processor.
#
TARGET=$(PROCESSOR_ARCHITECTURE)
BUILD=debug
CC= cl /nologo
CPPFLAGS=/MD /Zi /Gy /Gf /c /W3 /GX /Ox /Gz /D NDEBUG /Fo.\$(BUILD)\capsize.obj
LFLAGS=/debug /debugtype:cv,fixup /out:".\$(BUILD)\capsize.exe" /pdb:"$(BUILD)\capsize.pdb"

!include "..\samples.mak"

capsize.exe : capsize.obj
capsize.obj : capsize.cpp $(BUILD)

"$(BUILD)":
    if not exist "$(BUILD)/$(NULL)" mkdir "$(BUILD)"
.cpp.obj:
	$(CC) $(VINC) $(CPPFLAGS) $*.cpp
.obj.exe:
	$(CC) /Fe$@ $(BUILD)\$** /link $(LFLAGS) $(VLIB) \
		vulcan.lib
