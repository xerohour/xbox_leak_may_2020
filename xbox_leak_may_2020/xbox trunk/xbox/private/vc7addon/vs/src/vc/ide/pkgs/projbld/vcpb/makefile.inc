!include $(_NTROOT)\private\vc7addon\vsenv.inc
!include $(VSROOT)\Src\VC\IDE\makefile.inc


$(O)\$(RCFILE).res: $(RCFILE).rc $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\vcpb2.tlb

!ifdef URTTARGET

$(URTTARGET)\Microsoft.VisualStudio.VCProjectEngine.dll: $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\VCPB2.tlb
    echo Building and signing Microsoft.VisualStudio.VCProjectEngine.dll (com+ Metadata) from vcpb2.tlb > con
    SET OLDPATHBACK=%%PATH%%
    set PATH=%%PATH%%;$(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY);$(URTTARGET)
    $(TLBIMP_CMD)  /nologo /strictref /PublicKey:$(COMMON_ROOT)\security\FinalPublicKey.snk  /Primary $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\VCPB2.tlb /Out:$(URTTARGET)\Microsoft.VisualStudio.VCProjectEngine.dll
    $(REGASM_CMD) /regfile:$(O)\Microsoft.VisualStudio.VCProjectEngine.reg $(URTTARGET)\Microsoft.VisualStudio.VCProjectEngine.dll
    $(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\radtools\perl.exe $(VSROOT)\public\tools\reg2vrg.pl < $(O)\Microsoft.VisualStudio.VCProjectEngine.reg > $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\Microsoft.VisualStudio.VCProjectEngine.vrg

    $(VSPUBLIC)\tools\vsrebase.bat $(URTTARGET)\Microsoft.VisualStudio.VCProjectEngine.dll
    SET PATH=%%OLDPATHBACK%%

!endif