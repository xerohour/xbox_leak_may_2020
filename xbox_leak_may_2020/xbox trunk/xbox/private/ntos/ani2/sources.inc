


# C_DEFINES=$(C_DEFINES) -DPROFILE
# C_DEFINES=$(C_DEFINES) -DDETERMINISTIC

LINKER_FLAGS=$(LINKER_FLAGS) /MAP

SOURCES=\
CamControl.cpp \
camera.cpp \
GreenFog.cpp \
logo_renderer.cpp \
qrand.cpp \
renderer.cpp \
scene_renderer.cpp \
Shield.cpp \
tex_gen.cpp \
VBlob.cpp \
xbs_app.cpp \
fastmath.cpp \
placementdoodad.cpp \
bootsound.cpp \
mslogo.cpp \
dev.c \
cf.c \
evf.c \
globals.c \
sos.c \
proc.c \
stboot.c
