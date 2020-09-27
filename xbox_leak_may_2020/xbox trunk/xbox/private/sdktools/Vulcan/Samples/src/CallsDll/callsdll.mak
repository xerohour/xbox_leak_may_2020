# Select your TARGET processor.
#
TARGET=$(PROCESSOR_ARCHITECTURE)
BUILD=debug
CC= cl /nologo
CPPFLAGS=/Zi /Gy /Gf /c /W3 /GX /Ox /D NDEBUG /Fo.\$(BUILD)\callsdll.obj
LFLAGS=/DLL /debug /debugtype:cv,fixup /out:".\$(BUILD)\callsdll.dll" /pdb:"$(BUILD)\callsdll.pdb"

callsdll.dll : callsdll.obj
callsdll.obj : callsdll.cpp $(BUILD)

"$(BUILD)":
    if not exist "$(BUILD)/$(NULL)" mkdir "$(BUILD)"
.cpp.obj:
	$(CC) $(CPPFLAGS) $*.cpp
.obj.dll:
	$(CC) /Fe$@ $(BUILD)\$** /link $(LFLAGS)
		
