#
# The fin and fur vertex shaders come in several configurations, built from
# the same source files with different defines.
#
$(FURVERTEXSHADERS) : $(VSPATH)\fur.vsh $(VSPATH)\eyelighthalf.vsh $(VSPATH)\hairlighting.vsh $(VSPATH)\wind.vsh
    @echo Assembling fur vertex shader.
	xsasm -nologo -l -D WIND -D LOCAL_LIGHTING -D SELF_SHADOWING $(VSPATH)\fur.vsh $(VSPATH)\fur_wind1_local1_self1.xvu "" $(O)\fur_wind1_local1_self1.lst
	xsasm -nologo -l -D WIND -D LOCAL_LIGHTING                   $(VSPATH)\fur.vsh $(VSPATH)\fur_wind1_local1_self0.xvu "" $(O)\fur_wind1_local1_self0.lst
	xsasm -nologo -l -D WIND                   -D SELF_SHADOWING $(VSPATH)\fur.vsh $(VSPATH)\fur_wind1_local0_self1.xvu "" $(O)\fur_wind1_local0_self1.lst
	xsasm -nologo -l -D WIND                                     $(VSPATH)\fur.vsh $(VSPATH)\fur_wind1_local0_self0.xvu "" $(O)\fur_wind1_local0_self0.lst
	xsasm -nologo -l         -D LOCAL_LIGHTING -D SELF_SHADOWING $(VSPATH)\fur.vsh $(VSPATH)\fur_wind0_local1_self1.xvu "" $(O)\fur_wind0_local1_self1.lst
	xsasm -nologo -l         -D LOCAL_LIGHTING                   $(VSPATH)\fur.vsh $(VSPATH)\fur_wind0_local1_self0.xvu "" $(O)\fur_wind0_local1_self0.lst
	xsasm -nologo -l                           -D SELF_SHADOWING $(VSPATH)\fur.vsh $(VSPATH)\fur_wind0_local0_self1.xvu "" $(O)\fur_wind0_local0_self1.lst
	xsasm -nologo -l                                             $(VSPATH)\fur.vsh $(VSPATH)\fur_wind0_local0_self0.xvu "" $(O)\fur_wind0_local0_self0.lst

$(FINVERTEXSHADERS) : $(VSPATH)\fin.vsh $(VSPATH)\eyelighthalf.vsh $(VSPATH)\hairlighting.vsh $(VSPATH)\wind.vsh
    @echo Assembling fin vertex shader.
	xsasm -nologo -l -D WIND -D LOCAL_LIGHTING -D SELF_SHADOWING $(VSPATH)\fin.vsh $(VSPATH)\fin_wind1_local1_self1.xvu "" $(O)\fin_wind1_local1_self1.lst
	xsasm -nologo -l -D WIND -D LOCAL_LIGHTING                   $(VSPATH)\fin.vsh $(VSPATH)\fin_wind1_local1_self0.xvu "" $(O)\fin_wind1_local1_self0.lst
	xsasm -nologo -l -D WIND                   -D SELF_SHADOWING $(VSPATH)\fin.vsh $(VSPATH)\fin_wind1_local0_self1.xvu "" $(O)\fin_wind1_local0_self1.lst
	xsasm -nologo -l -D WIND                                     $(VSPATH)\fin.vsh $(VSPATH)\fin_wind1_local0_self0.xvu "" $(O)\fin_wind1_local0_self0.lst
	xsasm -nologo -l         -D LOCAL_LIGHTING -D SELF_SHADOWING $(VSPATH)\fin.vsh $(VSPATH)\fin_wind0_local1_self1.xvu "" $(O)\fin_wind0_local1_self1.lst
	xsasm -nologo -l         -D LOCAL_LIGHTING                   $(VSPATH)\fin.vsh $(VSPATH)\fin_wind0_local1_self0.xvu "" $(O)\fin_wind0_local1_self0.lst
	xsasm -nologo -l                           -D SELF_SHADOWING $(VSPATH)\fin.vsh $(VSPATH)\fin_wind0_local0_self1.xvu "" $(O)\fin_wind0_local0_self1.lst
	xsasm -nologo -l                                             $(VSPATH)\fin.vsh $(VSPATH)\fin_wind0_local0_self0.xvu "" $(O)\fin_wind0_local0_self0.lst

!include "..\..\makefile.inc"
