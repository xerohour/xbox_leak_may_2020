TARGETTYPE=LIBRARY

INCLUDES=..\..\scaler

C_DEFINES=$(C_DEFINES) -DUNICODE

SOURCES=\
    bitmap.cpp \
    painttext.cpp \
    truetype.cpp \
    xfont.c

