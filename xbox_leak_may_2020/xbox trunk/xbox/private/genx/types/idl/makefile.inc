$(PASS0_UUIDDIR)\wtypes.c:
    echo "/* dummy file for build*/" >$(PASS0_UUIDDIR)\wtypes.c

$(PASS0_SOURCEDIR)\wtypes_p.c:
    echo "/* dummy file for build*/" >$(PASS0_SOURCEDIR)\wtypes_p.c

ocidl.idl   : ..\..\public\ocidl.idl
wtypes.idl  : ..\..\public\wtypes.idl
unknwn.idl  : ..\..\public\unknwn.idl
objidl.idl  : ..\..\public\objidl.idl
propidl.idl : ..\..\public\propidl.idl
pbagex.idl  : ..\..\public\pbagex.idl
oleidl.idl  : ..\..\public\oleidl.idl
oaidl.idl   : ..\..\public\oaidl.idl
storext.idl : ..\..\public\storext.idl
