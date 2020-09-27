MAJORCOMP=xgraphics
MINORCOMP=xgraphics
TARGETNAME=xgraphics$(D)

TARGETTYPE=LIBRARY

SYNCHRONIZE_BLOCK=1
SYNCHRONIZE_DRAIN=1

OBJLIBFILES = \
    ..\..\pushbuffer\$(WHICHBUILD)\obj\*\pushbuffer$(D).lib \
    ..\..\shadeasm\$(WHICHBUILD)\obj\*\shadeasm$(D).lib \
    ..\..\swizzler\$(WHICHBUILD)\obj\*\swizzler$(D).lib \
    ..\..\math\$(WHICHBUILD)\obj\*\xgmath$(D).lib \
    ..\..\misc\$(WHICHBUILD)\obj\*\misc$(D).lib \
    ..\..\dxtc\$(WHICHBUILD)\obj\*\dxtc$(D).lib \
    ..\..\..\xfont\library\$(WHICHBUILD)\obj\*\xfont$(D).lib \
    ..\..\..\xfont\scaler\$(WHICHBUILD)\obj\*\xfonttt$(D).lib

