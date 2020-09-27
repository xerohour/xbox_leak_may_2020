!include $(VSROOT)\Src\VC\IDE\makefile.inc

$(URTTARGET)\Microsoft.VisualStudio.VCProject.dll: $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\ProjBld2.tlb
    echo Building and signing Microsoft.VisualStudio.VCProject.dll (com+ Metadata) from ProjBld2.tlb > con
    SET OLDPATHBACK=%%PATH%%
    set PATH=%%PATH%%;$(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY);$(URTTARGET)
    $(TLBIMP_CMD)  /nologo /strictref /PublicKey:$(COMMON_ROOT)\security\FinalPublicKey.snk  /Primary $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\projbld2.tlb /Out:$(URTTARGET)\Microsoft.VisualStudio.VCProject.dll
    $(REGASM_CMD) /regfile:$(O)\Microsoft.VisualStudio.VCProject.reg $(URTTARGET)\Microsoft.VisualStudio.VCProject.dll
    $(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\radtools\perl.exe $(VSROOT)\public\tools\reg2vrg.pl < $(O)\Microsoft.VisualStudio.VCProject.reg > $(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\Microsoft.VisualStudio.VCProject.vrg
    $(VSPUBLIC)\tools\vsrebase.bat $(URTTARGET)\Microsoft.VisualStudio.VCProject.dll
    SET PATH=%%OLDPATHBACK%%
