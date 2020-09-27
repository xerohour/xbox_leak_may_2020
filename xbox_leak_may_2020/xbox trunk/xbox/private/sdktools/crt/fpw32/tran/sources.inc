!INCLUDE ..\..\fp32.inc

MAJORCOMP=fp
MINORCOMP=tran

TARGETNAME=tran

INCLUDES=..\..\include;..\..\inc;..\..\inc\i386;..\..\..\crtw32\h;$(INCLUDES)

C_FLAGS=$(C_FLAGS) -Op

#removed *pentium4.asm cpu_disp.c

SOURCES=\
    ..\bessel.c    \
    ..\fpexcept.c  \
    ..\frexp.c     \
    ..\ieeemisc.c  \
    ..\ldexp.c     \
    ..\matherr.c   \
    ..\util.c

i386_SOURCES=\
    ..\87cdisp.asm   \
    ..\87ctran.asm   \
    ..\87ctriga.asm  \
    ..\87ctrigh.asm  \
    ..\87disp.asm    \
    ..\87except.c  \
    ..\87fmod.asm    \
    ..\87sqrt.asm    \
    ..\87tran.asm    \
    ..\87trig.asm    \
    ..\87triga.asm   \
    ..\87trigh.asm   \
    ..\adj_fdiv.asm  \
    ..\ceil.asm      \
    ..\common.asm    \
    ..\fabs.asm      \
    ..\filter.c    \
    ..\filter_simd.c    \
    ..\floor.asm     \
    ..\fpctrl.c    \
    ..\frnd.asm      \
    ..\fsqrt.asm     \
    ..\ftol.asm      \
    ..\ftol2.asm     \
    ..\genexcep.asm  \
    ..\huge.asm      \
    ..\hypot.c     \
    ..\ieee87.c    \
    ..\modf.asm      \
    ..\powhlp.asm    \
    ..\testfdiv.c  \
    ..\asin.asm      \
    ..\acos.asm      \
    ..\atan.asm      \
    ..\sin.asm       \
    ..\cos.asm       \
    ..\tan.asm       \
    ..\log.asm       \
    ..\log10.asm     \
    ..\pow.asm       \
    ..\sqrt.asm      \
    ..\xmmi_fp_emul.c      \
    ..\xmmi2_fp_emul.c  \
    ..\atan_table.c  \
    ..\libm_error.c
