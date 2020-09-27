TARGETNAME=XQuake$(LIBEXT)
TARGETTYPE=PROGRAM
TARGETPATH=obj

# PERFFLAGS=/fastcap

# Optimize for speed

!if "$(MSC_OPTIMIZATION)"==""
MSC_OPTIMIZATION=/Ox
!endif

INCLUDES=$(INCLUDES);..

SOURCES = \
	cd_win.c \
	chase.c \
	cl_demo.c \
	cl_input.c \
	cl_main.c \
	cl_parse.c \
	cl_tent.c \
	cmd.c \
	common.c \
	console.c \
	crc.c \
	cvar.c \
	gl_draw.c \
	gl_fakegl.cpp \
	gl_mesh.c \
	gl_model.c \
	gl_refrag.c \
	gl_rlight.c \
	gl_rmain.c \
	gl_rmisc.c \
	gl_rsurf.c \
	gl_screen.c \
	gl_test.c \
	gl_vidnt.c \
	gl_warp.c \
	host.c \
	host_cmd.c \
	in_win.c \
	in_xbox.cpp \
	keys.c \
	mathlib.c \
	menu.c \
	net_dgrm.c \
	net_loop.c \
	net_main.c \
	net_vcr.c \
	net_win.c \
	net_wins.c \
	pr_cmds.c \
	pr_edict.c \
	pr_exec.c \
	r_part.c \
	sbar.c \
	snd_dma.c \
	snd_mem.c \
	snd_mix.c \
	snd_win.c \
	sv_main.c \
	sv_move.c \
	sv_phys.c \
	sv_user.c \
	sys_win.c \
	view.c \
	wad.c \
	world.c \
	xgc.cpp \
	zone.c \
    dmusic.cpp \

C_DEFINES= $(C_DEFINES) -DNO_ASSEMBLY -DNO_MGRAPH -DGLQUAKE -DD3DQUAKE

UMTYPE=xboxapp

USE_LIBCMT=1

INCLUDES=$(INCLUDES);$(BASEDIR)\private\inc

LINKER_FLAGS=$(LINKER_FLAGS) -map
XE_FLAGS=$(XE_FLAGS) -NOPRELOAD:XNET

LINKLIBS=\
    $(LIBRARY_PATH)\d3d8$(LIBEXT).lib \
    $(LIBRARY_PATH)\d3dx8$(D).lib \
    $(LIBRARY_PATH)\dsound$(D).lib \
    $(LIBRARY_PATH)\xnet$(D).lib \
    $(LIBRARY_PATH)\xgraphics$(D).lib \
    $(XDK_LIB_PATH)\xbdm.lib \

# Allow Debug Keyboard support

C_DEFINES=$(C_DEFINES) -DDEBUG_KEYBOARD

LINKLIBS=$(LINKLIBS) \
    $(LIBRARY_PATH)\xkbd$(D).lib \

# XQUAKE_DMUSIC tests DMUSIC overhead

!if defined(XQUAKE_DMUSIC)

C_DEFINES=$(C_DEFINES) -DXQUAKE_DMUSIC

LINKLIBS=$(LINKLIBS) \
	$(LIBRARY_PATH)\dmusic$(LIBEXT).lib \

!endif

# XQUAKE_WMA tests WMA overhead

!if defined(XQUAKE_WMA)

C_DEFINES=$(C_DEFINES) -DXQUAKE_WMA

SOURCES=\
	$(SOURCES) \
    wma.cpp \

!endif
