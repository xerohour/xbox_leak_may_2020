!include $(_NTROOT)\private\vc7addon\vsenv.inc

!include $(VCROOT)\makefile.inc

$(O)\$(RCFILE).res: $(RCFILE).rc $(O)\$(CMDTABLE).cto

$(O)\$(CMDTABLE).cto: $(CMDTABLE).cmd