# Select your TARGET processor.
#
TARGET=$(PROCESSOR_ARCHITECTURE)
BUILD=debug
CC= cl /nologo
CPPFLAGS=/Zi /Gy /Gf /c /W3 /GX /Ox /D NDEBUG /Fo.\$(BUILD)\calls.obj
LFLAGS=/debug /debugtype:cv,fixup /out:".\$(BUILD)\calls.exe" /pdb:"$(BUILD)\calls.pdb"

!include "..\samples.mak"

calls.exe : calls.obj
calls.obj : calls.cpp $(BUILD)

"$(BUILD)":
    if not exist "$(BUILD)/$(NULL)" mkdir "$(BUILD)"
.cpp.obj:
	$(CC) $(VINC) $(CPPFLAGS) $*.cpp
.obj.exe:
	$(CC) /Fe$@ $(BUILD)\$** /link $(LFLAGS) $(VLIB) \
		vulcan.lib
