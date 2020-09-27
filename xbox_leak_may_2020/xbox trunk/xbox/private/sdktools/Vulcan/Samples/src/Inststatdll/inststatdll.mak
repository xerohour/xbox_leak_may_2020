# Select your TARGET processor.
#
TARGET=$(PROCESSOR_ARCHITECTURE)
BUILD=debug
CC= cl /nologo
CPPFLAGS=/Zi /Gy /Gf /c /W3 /GX /Ox /D NDEBUG /Fo.\$(BUILD)\inststatdll.obj
LFLAGS=/DLL /debug /debugtype:cv,fixup /out:".\$(BUILD)\inststatdll.dll" /pdb:"$(BUILD)\inststatdll.pdb"

inststatdll.dll : inststatdll.obj
inststatdll.obj : inststatdll.cpp $(BUILD)

"$(BUILD)":
    if not exist "$(BUILD)/$(NULL)" mkdir "$(BUILD)"
.cpp.obj:
	$(CC) $(CPPFLAGS) $*.cpp
.obj.dll:
	$(CC) /Fe$@ $(BUILD)\$** /link $(LFLAGS) 
