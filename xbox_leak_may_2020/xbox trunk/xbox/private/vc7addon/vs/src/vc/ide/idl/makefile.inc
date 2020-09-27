!include ..\makefile.inc

$(PASS0_HEADERDIR)\VCCodeModels.h : VCCodeModels.idl $(PASS0_TLBDIR)\DTE.tlb

$(PASS0_HEADERDIR)\VCWiz.h : VCWiz.idl $(PASS0_TLBDIR)\DTE.tlb

$(PASS0_HEADERDIR)\ProjBld2.h : ProjBld2.idl $(PASS0_TLBDIR)\DTE.tlb

$(VSBUILT)\$(BUILT_FLAVOR)\Inc\$(TARGET_DIRECTORY)\VCCodeModelsHelpID.h $(VSBUILT)\$(BUILT_FLAVOR)\Inc\$(TARGET_DIRECTORY)\VCCodeModelsHelpBS.h : VCCodeModelsHelp.id $(COMMON_ROOT)\idl\vs\vsext.id 
	@type VCCodeModelsHelp.id $(COMMON_ROOT)\idl\vs\vsext.id > tempid.id
	$(IDHEADER) tempid.id $(VSBUILT)\$(BUILT_FLAVOR)\Inc\$(TARGET_DIRECTORY)\VCCodeModelsHelpID.h $(VSBUILT)\$(BUILT_FLAVOR)\Inc\$(TARGET_DIRECTORY)\VCCodeModelsHelpBS.h $(O)
	@del tempid.id
