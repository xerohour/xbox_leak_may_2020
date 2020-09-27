# Additional rules
# 
# $(TARGETPATHUI)\$(TARGETNAME)ui.dll : $(O)\solution.res
# 	@if not exist $(TARGETPATHUI) mkdir $(TARGETPATHUI)
# 	$(LINK_NAME) $(LINKER_VC6PDBS) -out:$(TARGETPATHUI)\$(TARGETNAME)ui.dll -machine:$(MACHINE_TYPE) -base:@$(COFFBASE_TXT_FILE),vsslnui -noentry -nodefaultlib -dll -subsystem:windows,4.00 $(O)\solution.res
# !if "$(BUILD_RETAIL)"=="yes"
# 	$(COPYEXE_CMD)
# !endif
# 
# $(O)\solution.res : solution.rc $(O)\solnmenu.cto $(O)\vssln.tlb spkg.rgs res\build1.bmp
# 
# $(O)\solnmenu.cto: solnmenu.ctc solnmnid.h
