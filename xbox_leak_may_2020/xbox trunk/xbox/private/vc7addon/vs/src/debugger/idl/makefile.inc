$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\ad1.lib : $(AD1_LIBOBJECTS)
	$(LIB_NAME) -out:$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\ad1.lib -nodefaultlib $(AD1_LIBOBJECTS)

$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\ad2ide.lib : $(AD2IDE_LIBOBJECTS)
	$(LIB_NAME) -out:$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\ad2ide.lib -nodefaultlib $(AD2IDE_LIBOBJECTS)

$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\ad2de.lib : $(AD2DE_LIBOBJECTS)
	$(LIB_NAME) -out:$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\ad2de.lib -nodefaultlib $(AD2DE_LIBOBJECTS)

$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\ad2priv.lib : $(AD2PRIV_LIBOBJECTS)
	$(LIB_NAME) -out:$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\ad2priv.lib -nodefaultlib $(AD2PRIV_LIBOBJECTS)

$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\sqlinit.lib : $(SQLINIT_LIBOBJECTS)
	$(LIB_NAME) -out:$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\sqlinit.lib -nodefaultlib $(SQLINIT_LIBOBJECTS)

$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\sleinternal.lib : $(SLEINTERNAL_LIBOBJECTS)
	$(LIB_NAME) -out:$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\sleinternal.lib -nodefaultlib $(SLEINTERNAL_LIBOBJECTS)

$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\sdminternal.lib : $(SDMINTERNAL_LIBOBJECTS)
	$(LIB_NAME) -out:$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\sdminternal.lib -nodefaultlib $(SDMINTERNAL_LIBOBJECTS)

# compile-time PDB needs to be available for those who statically link with ad*.lib
$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\$(TARGETNAME)_comp.pdb : $(O)\$(TARGETNAME)_comp.pdb
	copy $(O)\$(TARGETNAME)_comp.pdb $(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)\$(TARGETNAME)_comp.pdb

$(PASS0_HEADERDIR)\dispex.h : dispex.idl
	$(MIDL) $(PACKING) $(FULL_INCL_PATH) -char unsigned -ms_ext -c_ext -client none -server none -header $@ \
	-iid $(PASS0_UUIDDIR)\dispex.c \
	-tlb $(PASS0_TLBDIR)\dispex.tlb \
	-proxy $(PASS0_PROXYDIR)\dispex_p.c \
	-dlldata $(PASS0_DLLDATADIR)\dispex_d.c \
	-cpp_cmd $(TARGET_CPP) $(C_DEFINES) $(USER_C_DEFINES) $(MIDL_FLAGS) $(MIDL_OPTIMIZATION) dispex.idl

$(PASS0_HEADERDIR)\adnots.h : adnots.idl
	$(MIDL) $(PACKING) $(FULL_INCL_PATH) -char unsigned -ms_ext -c_ext -client none -server none -header $@ \
	-iid $(PASS0_UUIDDIR)\adnots.c \
	-tlb $(PASS0_TLBDIR)\adnots.tlb \
	-proxy $(PASS0_PROXYDIR)\adnots_p.c \
	-dlldata $(PASS0_DLLDATADIR)\adnots_d.c \
	-cpp_cmd $(TARGET_CPP) $(C_DEFINES) $(USER_C_DEFINES) $(MIDL_FLAGS) $(MIDL_OPTIMIZATION) adnots.idl

$(PASS0_HEADERDIR)\v6.h : v6.idl
	$(MIDL) $(PACKING) $(FULL_INCL_PATH) -char unsigned -ms_ext -c_ext -client none -server none -header $@ \
	-iid $(PASS0_UUIDDIR)\v6.c \
	-tlb $(PASS0_TLBDIR)\v6.tlb \
	-proxy $(PASS0_PROXYDIR)\v6_p.c \
	-dlldata $(PASS0_DLLDATADIR)\v6_d.c \
	-cpp_cmd $(TARGET_CPP) $(C_DEFINES) $(USER_C_DEFINES) $(MIDL_FLAGS) $(MIDL_OPTIMIZATION) v6.idl

$(PASS0_HEADERDIR)\enc_temp.h : enc_temp.idl
	$(MIDL) -DENC_TEMP_COMPILE $(PACKING) $(FULL_INCL_PATH) -char unsigned -ms_ext -c_ext -client none -server none -header $@ \
	-iid $(PASS0_UUIDDIR)\enc_temp.c \
	-tlb $(PASS0_TLBDIR)\enc_temp.tlb \
	-proxy $(PASS0_PROXYDIR)\enc_temp_p.c \
	-dlldata $(PASS0_DLLDATADIR)\enc_temp_d.c \
	-cpp_cmd $(TARGET_CPP) $(C_DEFINES) $(USER_C_DEFINES) $(MIDL_FLAGS) $(MIDL_OPTIMIZATION) enc_temp.idl

$(PASS0_HEADERDIR)\mdsqldbg.h : mdsqldbg.idl
	$(MIDL) $(PACKING) $(FULL_INCL_PATH) -char unsigned -ms_ext -c_ext -client none -server none -header $@ \
	-iid $(PASS0_UUIDDIR)\mdsqldbg.c \
	-tlb $(PASS0_TLBDIR)\mdsqldbg.tlb \
	-proxy $(PASS0_PROXYDIR)\mdsqldbg_p.c \
	-dlldata $(PASS0_DLLDATADIR)\mdsqldbg_d.c \
	-cpp_cmd $(TARGET_CPP) $(C_DEFINES) $(USER_C_DEFINES) $(MIDL_FLAGS) $(MIDL_OPTIMIZATION) mdsqldbg.idl

$(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\dbgmetric.h : ..\inc\dbgmetric.h
	copy ..\inc\dbgmetric.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\dbgmetric.h

$(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\toolhrt.h : ..\inc\toolhrt.h
	copy ..\inc\toolhrt.h $(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)\toolhrt.h
