cl /nologo /ML /W3 /Od /c /Zl /vd0 ConductorGUID.c
cl /nologo /ML /W3 /Od /c /Zl /vd0 DMUSProdGUID.c
cl /nologo /ML /W3 /Od /c /Zl /vd0 SegmentDesignerGUID.c
cl /nologo /ML /W3 /Od /c /Zl /vd0 TimelineGUID.c
lib DMUSProdGUID.obj /link50compat /nologo /out:DMUSProdGUID.lib
lib ConductorGUID.obj /link50compat /nologo /out:ConductorGUID.lib
lib SegmentDesignerGUID.obj /link50compat /nologo /out:SegmentDesignerGUID.lib
lib TimelineGUID.obj /link50compat /nologo /out:TimelineGUID.lib
