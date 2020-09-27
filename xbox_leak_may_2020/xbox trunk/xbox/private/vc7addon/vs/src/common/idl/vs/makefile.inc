$(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vsextid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vsextbs.h: vsext.id
	$(IDHEADER) vsext.id $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vsextid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vsextbs.h $(O)

$(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\idsid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\idsbs.h: ids.id
	$(IDHEADER) ids.id $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\idsid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\idsbs.h $(O)

$(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vbide.h: $(O)\vbide.odl
	$(MKTYPLIB) -nocpp -nologo /DENV=1 /DVJ=1 /DNEWOCX /win32 /h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vbide.h $(O)\vbide.odl

$(O)\vbide.odl: vbide.odl $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vbideid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\wizardid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vbaextid.h $(O)\dte.c $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\dte.tlb
	$(CC_NAME) $(FULL_INCL_PATH) -DVP=1 /D__MKTYPLIB__ /Dc32=c32 /DENV=1 /DVJ=1 /DNEWOCX /DRBY_WIN32 /DRBY_NT /DCMDBARS /DTESTCMD /E /Tc vbide.odl > $(O)\vbide.odl

$(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\vsext.olb: $(O)\vsext.odl
	$(MKTYPLIB) -nocpp -nologo /DENV=1 /DVJ=1 /DNEWOCX /win32 /tlb $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\vsext.olb $(O)\vsext.odl

$(O)\vsext.odl: vbide.odl $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vbideid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\wizardid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vbaextid.h
	$(CC_NAME) $(FULL_INCL_PATH) -DVP=1 /D__MKTYPLIB__ /Dc32=c32 /DENV=1 /DVJ=1 /DNEWOCX /DRBY_WIN32 /DRBY_NT /DCMDBARS /DTESTCMD /DEXTBLDMINTLB=TRUE /E /Tc vbide.odl > $(O)\vsext.odl

$(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vbideid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vbidebs.h: vbide.id
	$(IDHEADER) vbide.id $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vbideid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vbidebs.h $(O)

$(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\wizardid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\wizardbs.h: wizard.id
	$(IDHEADER) wizard.id $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\wizardid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\wizardbs.h $(O)

$(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vbaextid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vbaextbs.h: vbaext.id
	$(IDHEADER) vbaext.id $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vbaextid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vbaextbs.h $(O)


$(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\dte.tlb $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\dteinternal.h $(O)\dte.c: $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vsextid.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\vsextbs.h dte.idl debugger.idl codemodel.idl
    $(MIDL) $(PACKING) $(FULL_INCL_PATH) -char unsigned -ms_ext -c_ext -client none -server none \
        -header $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\dteinternal.h \
        -iid $(PASS0_UUIDDIR)\dte.c \
        -tlb $(PASS0_TLBDIR)\dte.tlb \
        -proxy $(PASS0_PROXYDIR)\dte_p.c \
        -dlldata $(PASS0_DLLDATADIR)\$(DLLDATA)\dte_d.c \
        -cpp_cmd $(TARGET_CPP) $(C_DEFINES) $(USER_C_DEFINES) $(MIDL_FLAGS) $(MIDL_OPTIMIZATION) dte.idl

$(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\dte.h: dte.h
    copy dte.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\dte.h

!ifdef BT_BISLABMACHINE
!ifndef NOLINK

$(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\$(PIAPREFIX)VsMacroHierarchyLib.dll: $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\macroh.tlb
    set OAASSERT=$(VSROOT)\src\common\idl\vs\assert.log
    $(TLBIMP_CMD) /nologo /strictref /namespace:VsMacroHierarchyLib /Primary /PublicKey:$(VSROOT)\src\common\security\FinalPublicKey.snk $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\macroh.tlb /Out:$(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\$(PIAPREFIX)VsMacroHierarchyLib.dll
        $(REGASM_CMD) /regfile:$(O)\$(PIAPREFIX)VsMacroHierarchyLib.reg $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\$(PIAPREFIX)VsMacroHierarchyLib.dll
        $(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\radtools\awk -f $(VSROOT)\public\tools\piafilter.awk < $(O)\$(PIAPREFIX)VsMacroHierarchyLib.reg > $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\$(PIAPREFIX)VsMacroHierarchyLib.vrg
    $(VSPUBLIC)\tools\vsrebase.bat $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\$(PIAPREFIX)VsMacroHierarchyLib.dll
!if "$(BT_BISLABMACHINE)" == ""
    -$(GACUTIL_CMD) -i $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\$(PIAPREFIX)VsMacroHierarchyLib.dll
!endif
    set OAASSERT=
    copy  $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\$(PIAPREFIX)VsMacroHierarchyLib.dll $(URTTARGET)
!else
$(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\$(PIAPREFIX)VSMacroHierarchyLib.dll:
!endif
!else
$(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\$(PIAPREFIX)VSMacroHierarchyLib.dll:
!endif

