TARGETFILES= \
    $(O)\d3d8.h \
    $(O)\xmetal.h \
    $(O)\d3d8caps.h \
    $(O)\d3d8types.h \
    $(O)\d3d8perf.h \
    $(O)\d3d8sddi.h \
    $(O)\dx7todx8.h \
    $(O)\d3dx8mesh.h \
    $(O)\d3dx8.h \
    $(O)\xfont.h \
    $(O)\xfontformat.h \
    $(D3DX8_INC_PATH)\d3dx8meshp.h \
    $(D3DX8_INC_PATH)\d3dx8p.h \
    $(PROJECT_INC_PATH)\d3d8sddi.h \
    $(SDK_INC_PATH)\dx7todx8.h \
    $(SDK_INC_PATH)\d3d8.h \
    $(SDK_INC_PATH)\xmetal.h \
    $(SDK_INC_PATH)\d3d8caps.h \
    $(SDK_INC_PATH)\d3d8types.h \
    $(SDK_INC_PATH)\d3d8perf.h \
    $(SDK_INC_PATH)\d3dx8.h \
    $(SDK_INC_PATH)\d3dx8mesh.h \
    $(SDK_INC_PATH)\d3dx8core.h \
    $(SDK_INC_PATH)\d3dx8effect.h \
    $(SDK_INC_PATH)\d3dx8math.h \
    $(SDK_INC_PATH)\d3dx8math.inl \
    $(SDK_INC_PATH)\d3dx8shape.h \
    $(SDK_INC_PATH)\d3dx8tex.h \
    $(SDK_INC_PATH)\xgraphics.h \
    $(SDK_INC_PATH)\xgmath.h \
    $(SDK_INC_PATH)\xgmath.inl \
    $(XDK_INC_PATH)\d3d8.h \
    $(XDK_INC_PATH)\xmetal.h \
    $(XDK_INC_PATH)\d3d8caps.h \
    $(XDK_INC_PATH)\d3d8types.h \
    $(XDK_INC_PATH)\d3d8perf.h \
    $(XDK_INC_PATH)\d3dx8.h \
    $(XDK_INC_PATH)\d3dx8mesh.h \
    $(XDK_INC_PATH)\d3dx8core.h \
    $(XDK_INC_PATH)\d3dx8effect.h \
    $(XDK_INC_PATH)\d3dx8math.h \
    $(XDK_INC_PATH)\d3dx8math.inl \
    $(XDK_INC_PATH)\d3dx8shape.h \
    $(XDK_INC_PATH)\d3dx8tex.h \
    $(XDK_INC_PATH)\xgraphics.h \
    $(XDK_INC_PATH)\xgmath.h \
    $(XDK_INC_PATH)\xgmath.inl \
    $(WSDK_INC_PATH)\d3d8-xbox.h \
    $(WSDK_INC_PATH)\d3d8types-xbox.h \
    $(WSDK_INC_PATH)\xgraphics.h \
    $(WSDK_INC_PATH)\xgmath.h \
    $(WSDK_INC_PATH)\xgmath.inl \
    $(PRIVATE_INC_PATH)\d3d8perfp.h \
    $(SDK_INC_PATH)\xfont.h \
    $(SDK_INC_PATH)\xfontformat.h \
    $(XDK_INC_PATH)\xfont.h \
    $(XDK_INC_PATH)\xfontformat.h \
    $(WSDK_INC_PATH)\xfont-pc.h \
    $(WSDK_INC_PATH)\xfontformat-pc.h

processedfiles: \
    $(TARGETFILES)

clean:
    -del /q /f $(TARGETFILES) 2>nul

DX8HSPLITFLAGS = -ts dx3 -ts dx4 -ts dx5 -ts dx6 -ts dx7 -ta dx8 -v 800
DX8HSPLITSDKFLAGS = $(DX8HSPLITFLAGS) -ta external -ts internal
DX8HSPLITINTERNALFLAGS = $(DX8HSPLITFLAGS) -ts external -ta internal

$(O)\d3d8.h : d3d8.x
    hsplit $(DX8HSPLITSDKFLAGS) -ta xboxonly -ts xboxwindows -ts january -o $@ tmp.px d3d8.x
    del tmp.px

$(O)\xmetal.h : xmetal.x
    hsplit $(DX8HSPLITSDKFLAGS) -ta xboxonly -ts xboxwindows -ts january -o $@ tmp.px xmetal.x
    del tmp.px

$(O)\d3d8caps.h :d3d8caps.x
    hsplit $(DX8HSPLITSDKFLAGS) -o $@ tmp.px d3d8caps.x
    del tmp.px

$(O)\d3d8types.h : d3d8typs.x
    hsplit $(DX8HSPLITSDKFLAGS) -o $@ tmp.px d3d8typs.x
    del tmp.px

$(O)\d3d8perf.h : d3d8perf.x
    cl /EP /C d3d8perf.x > tmp.cx
    hsplit -ta external -ts internal -ts remove -o $@ tmp.px tmp.cx
    del tmp.px
    del tmp.cx

$(PRIVATE_INC_PATH)\d3d8perfp.h : d3d8perf.x
    hsplit -ts external -ta internal -ts remove -o $@ tmp.px d3d8perf.x
    del tmp.px

$(O)\d3d8sddi.h : d3d8sddi.x
    copy $? $@

$(O)\dx7todx8.h : dx7todx8.x
    copy $? $@

$(O)\d3dx8mesh.h : d3dx8mesh.w d3dmkhdr.m4
    m4  d3dx8mesh.w > tmp.wx
    hsplit $(DX8HSPLITSDKFLAGS) -o $@ tmp.px tmp.wx
    del tmp.wx
    del tmp.px

$(D3DX8_INC_PATH)\d3dx8meshp.h : d3dx8mesh.w d3dmkhdr.m4
    m4  d3dx8mesh.w > tmp.wx
    hsplit $(DX8HSPLITINTERNALFLAGS) -o $@ tmp.px tmp.wx
    del tmp.wx
    del tmp.px

$(O)\d3dx8.h : d3dx8.w d3dmkhdr.m4
    m4  d3dx8.w > tmp.wx
    hsplit $(DX8HSPLITSDKFLAGS) -o $@ tmp.px tmp.wx
    del tmp.wx
    del tmp.px

$(D3DX8_INC_PATH)\d3dx8p.h : d3dx8.w d3dmkhdr.m4
    m4  d3dx8.w > tmp.wx
    hsplit $(DX8HSPLITINTERNALFLAGS) -o $@ tmp.px tmp.wx
    del tmp.wx
    del tmp.px
    
$(O)\xfont.h : xfont.x
    hsplit $(DX8HSPLITSDKFLAGS) -ta xboxonly -ts xboxwindows -o $@ tmp.px xfont.x
    del tmp.px

$(O)\xfontformat.h : xfontformat.x
    hsplit $(DX8HSPLITSDKFLAGS) -ta xboxonly -ts xboxwindows -o $@ tmp.px xfontformat.x
    del tmp.px

$(SDK_INC_PATH)\dx7todx8.h    :      $(O)\dx7todx8.h    
    copy $? $@

$(SDK_INC_PATH)\d3d8.h        :      $(O)\d3d8.h        
    copy $? $@

$(SDK_INC_PATH)\xmetal.h      :      $(O)\xmetal.h        
    copy $? $@

$(SDK_INC_PATH)\d3d8caps.h    :      $(O)\d3d8caps.h    
    copy $? $@

$(SDK_INC_PATH)\d3d8types.h   :      $(O)\d3d8types.h   
    copy $? $@

$(SDK_INC_PATH)\d3d8perf.h   :       $(O)\d3d8perf.h   
    copy $? $@

$(PROJECT_INC_PATH)\d3d8sddi.h:      $(O)\d3d8sddi.h    
    copy $? $@

$(SDK_INC_PATH)\d3dx8.h       :      $(O)\d3dx8.h       
    copy $? $@

$(SDK_INC_PATH)\d3dx8mesh.h   :      $(O)\d3dx8mesh.h   
    copy $? $@

$(SDK_INC_PATH)\d3dx8core.h   :      d3dx8core.h       
    copy $? $@

$(SDK_INC_PATH)\d3dx8effect.h :      d3dx8effect.h     
    copy $? $@

$(SDK_INC_PATH)\d3dx8math.h   :      d3dx8math.h       
    copy $? $@

$(SDK_INC_PATH)\d3dx8math.inl :      d3dx8math.inl     
    copy $? $@

$(SDK_INC_PATH)\d3dx8shape.h  :      d3dx8shape.h      
    copy $? $@

$(SDK_INC_PATH)\d3dx8tex.h    :      d3dx8tex.h        
    copy $? $@

$(SDK_INC_PATH)\xgmath.h      :      xgmath.h        
    copy $? $@

$(SDK_INC_PATH)\xgmath.inl    :      xgmath.inl
    copy $? $@

{$(SDK_INC_PATH)}.h{$(XDK_INC_PATH)}.h:
    copy $** $@

$(SDK_INC_PATH)\xfont.h    :      $(O)\xfont.h    
    copy $? $@

$(SDK_INC_PATH)\xfontformat.h    :      $(O)\xfontformat.h    
    copy $? $@

$(XDK_INC_PATH)\xfont.h    :      $(O)\xfont.h    
    copy $(O)\xfont.h $(XDK_INC_PATH)\xfont.h

$(XDK_INC_PATH)\xfontformat.h    :      $(O)\xfontformat.h    
    copy $(O)\xfontformat.h $(XDK_INC_PATH)\xfontformat.h

$(XDK_INC_PATH)\d3dx8math.inl : $(SDK_INC_PATH)\d3dx8math.inl
    copy $** $@

$(XDK_INC_PATH)\xgmath.inl    : $(SDK_INC_PATH)\xgmath.inl
    copy $** $@

$(SDK_INC_PATH)\xgraphics.h : xgraphics.x
    hsplit $(DX8HSPLITSDKFLAGS) -ta xboxonly -ts xboxwindows -ts january -o $@ tmp.px xgraphics.x
    del tmp.px

$(WSDK_INC_PATH)\xgraphics.h : xgraphics.x
    hsplit $(DX8HSPLITSDKFLAGS) -ts xboxonly -ta xboxwindows -ts january -o $@ tmp.px xgraphics.x
    del tmp.px

$(WSDK_INC_PATH)\xgmath.h   :    xgmath.h
    copy $? $@
    
$(WSDK_INC_PATH)\xgmath.inl :    xgmath.inl
    copy $? $@

$(WSDK_INC_PATH)\d3d8-xbox.h       :      d3d8.x
    hsplit $(DX8HSPLITSDKFLAGS) -ts xboxonly -ta xboxwindows -ts january -o $@ tmp.px d3d8.x
    del tmp.px

$(WSDK_INC_PATH)\d3d8types-xbox.h  :      $(O)\d3d8types.h   
    copy $? $@

$(WSDK_INC_PATH)\xfont-pc.h   :    xfont.x
    hsplit $(DX8HSPLITSDKFLAGS) -ts xboxonly -ta xboxwindows -o $@ tmp.px xfont.x
    del tmp.px
    
$(WSDK_INC_PATH)\xfontformat-pc.h   :    xfontformat.x
    hsplit $(DX8HSPLITSDKFLAGS) -ts xboxonly -ta xboxwindows -o $@ tmp.px xfontformat.x
    del tmp.px
    
