
#if !defined(__GROUP_H__)
#define __GROUP_H__

typedef enum tagGroupType {
    eCPlusPlusGroup         = 0x0,
    eModuleGroup            = 0x1,
    eInterfaceGroup         = 0x2,
    eComGroup               = 0x3,
    eComPlusMetaDataGroup   = 0x4,
    eControlGroup           = 0x5,
    eWindowGroup            = 0x6,
    eRegistryGroup          = 0x7,
    eDBConsumerGroup        = 0x8,
    eDBProviderGroup        = 0x9,
    eStockPMEGroup          = 0xA,
    eDebugGroup             = 0xB,
    eDHTMLGroup             = 0xC,
    eCompilerGroup          = 0xD,
    eIDLGroup               = 0xE,
    eUserDefinedGroup       = 0xF,
    eHPS                    = 0x10,
    eMaxGroup               = 0x11
} GroupType;

static GroupType GroupTypeVector[eMaxGroup] = {
    eCPlusPlusGroup,
    eModuleGroup,
    eInterfaceGroup,
    eComGroup,
    eComPlusMetaDataGroup,
    eControlGroup,
    eWindowGroup,
    eRegistryGroup,
    eDBConsumerGroup,
    eDBProviderGroup,
    eStockPMEGroup,
    eDebugGroup,
    eDHTMLGroup,
    eCompilerGroup,
    eIDLGroup,
    eUserDefinedGroup,
    eHPS
};

static const char* GroupTypeStrings[eMaxGroup] = {
    "C++",
    "Module",
    "Interface",
    "COM",
    "COM+",
    "Controls",
    "Windowing",
    "Registry",
    "OLE DB Consumer",
    "OLE DB Provider",
    "Stock PME",
    "Internal/Debug",
    "DHTML",
    "Compiler",
    "IDL",
    "User Defined",
    "ATL Server"
};

inline GroupType GetGroupType(char* val) {
    if (strcmp(val, "eCPlusPlusGroup") == 0) {
            return eCPlusPlusGroup;
    } else if (strcmp(val, "eModuleGroup") == 0) {
            return eModuleGroup;
    } else if (strcmp(val, "eInterfaceGroup") == 0) {
            return eInterfaceGroup;
    } else if (strcmp(val, "eComGroup") == 0) {
            return eComGroup;
    } else if (strcmp(val, "eComPlusMetaDataGroup") == 0) {
            return eComPlusMetaDataGroup;
    } else if (strcmp(val, "eControlGroup") == 0) {
            return eControlGroup;
    } else if (strcmp(val, "eWindowGroup") == 0) {
            return eWindowGroup;
    } else if (strcmp(val, "eRegistryGroup") == 0) {
            return eRegistryGroup;
    } else if (strcmp(val, "eDBConsumerGroup") == 0) {
            return eDBConsumerGroup;
    } else if (strcmp(val, "eDBProviderGroup") == 0) {
            return eDBProviderGroup;
    } else if (strcmp(val, "eStockPMEGroup") == 0) {
            return eStockPMEGroup;
    } else if (strcmp(val, "eDebugGroup") == 0) {
            return eDebugGroup;
    } else if (strcmp(val, "eDHTMLGroup ") == 0) {
            return eDHTMLGroup  ;
    } else if (strcmp(val, "eCompilerGroup") == 0) {
            return eCompilerGroup;
    } else if (strcmp(val, "eIDLGroup") == 0) {
            return eIDLGroup;
    } else if (strcmp(val, "eUserDefinedGroup") == 0) {
            return eUserDefinedGroup;
    } else if (strcmp(val, "eHPS") == 0) {
            return eHPS;
    } else {
        char buf[512];
        sprintf(buf, "'%s': incorrect format for attribute \'group\'", val);
#if VERSP_RELEASE || VERSP_TEST || VERSP_DEBUG
        error(C2338, buf);
#else
        printf("%s\n", buf);
#endif
        DASSERT(UNREACHED);
        return eCPlusPlusGroup;
    }
}

#endif // __GROUP_H__
