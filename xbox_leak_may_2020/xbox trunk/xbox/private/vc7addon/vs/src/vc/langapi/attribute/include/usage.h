
#if !defined(__USAGE_H__)
#define __USAGE_H__

static UsageType UsageTypeVector[eUsageCount] = {
#define DAT(e,val,str) e,
#include "usage_dat.h"
#undef DAT
};

static char *UsageStringVector[eUsageCount] = {
#define DAT(e,val,str) #e,
#include "usage_dat.h"
#undef DAT
};

static char* UsageToHumanVector[eUsageCount] = {
#define DAT(e,val,str) str,
#include "usage_dat.h"
#undef DAT
};

enum UsageIndex {
#define DAT(e,val,str) idx_ ## e,
#include "usage_dat.h"
#undef DAT
};

inline char* UsageTo(int usage, char* buf, int startIdx, char* prefix)
{
    DASSERT(eAnyUsage == 0 && eAnyUsage == UsageTypeVector[0]);
    DASSERT(usage >= 0 && usage <= eMaxUsage && buf != 0);
    DASSERT(eCOMInterfaceUsage == UsageTypeVector[idx_eCOMInterfaceUsage]);
    DASSERT(eInterfaceUsage == UsageTypeVector[idx_eInterfaceUsage]);
    DASSERT(idx_eInterfaceUsage == (UsageType) (idx_eCOMInterfaceUsage+1));
    DASSERT(eInterfaceParameterUsage == UsageTypeVector[idx_eInterfaceParameterUsage]);
    DASSERT(eMethodParameterUsage == UsageTypeVector[idx_eMethodParameterUsage]);
    DASSERT(idx_eMethodParameterUsage == (UsageType) (idx_eInterfaceParameterUsage+1));
    DASSERT(eAnyIDLUsage == UsageTypeVector[eUsageCount-1]);

    buf[0] = 0;
    char* pBufEnd = buf;
    bool_t fFirst = true;

    for (int i = startIdx; i < eUsageCount-5 /*Skip eAnyIDLUsage*/; ++i) {
        if ((usage & UsageTypeVector[i]) || (i == 0 && usage == 0)) {

            if ((i == idx_eCOMInterfaceUsage) && (usage & (eInterfaceUsage & ~eCOMInterfaceUsage))) {
                i = idx_eInterfaceUsage;
            }
            if ((i == idx_eInterfaceParameterUsage) && (usage & (eMethodParameterUsage & ~eInterfaceParameterUsage))) {
                i = idx_eMethodParameterUsage; 
            }

            pBufEnd += sprintf(pBufEnd, "%s%s", ((fFirst) ? "" : prefix), UsageToHumanVector[i]);
            fFirst = false;
        }
    }

    return buf;
}

inline char* UsageToHuman(int usage, char* buf)
{
    return UsageTo(usage, buf, idx_eCoClassUsage, ", ");
}

inline char* UsageToString(int usage, char* buf)
{
    return UsageTo(usage, buf, 0, "| ");
}

inline __int64 GetValueKind(const char* val) {
    if (strcmp(val, "eANSIStringValue") == 0) {
        return eANSIStringValue;
    } else if (strcmp(val, "eWideStringValue") == 0) {
        return eWideStringValue;
    } else if (strcmp(val, "eIntegerValue") == 0) {
        return eIntegerValue;
    } else if (strcmp(val, "eInteger64Value") == 0) {
        return eInteger64Value;
    } else if (strcmp(val, "eFloatValue") == 0) {
        return eFloatValue;
    } else if (strcmp(val, "eDoubleValue") == 0) {
        return eDoubleValue;
    } else if (strcmp(val, "eIdentValue") == 0) {
        return eIdentValue;
    } else if (strcmp(val, "eMacroValue") == 0) {
        return eMacroValue;
    } else if (strcmp(val, "eBooleanValue") == 0) {
        return eBooleanValue;
    } else if (strcmp(val, "eGuidValue") == 0) {
        return eGuidValue;
    } else if (strcmp(val, "eSystemTypeValue") == 0) {
        return eSystemTypeValue;
    } else if (strcmp(val, "eVariantValue") == 0) {
        return eVariantValue;
#if VERSP_RELEASE
    } else {
        return eUnknownValue;
    }
#else
    } else if (strcmp(val, "eUnknownValue") == 0) {
        return eUnknownValue;
    } else {
        printf("error in GetValueKind(val=%s)\n", val);
        fflush(stdout);
        DASSERT(UNREACHED);
        return eUnknownValue;
    }
#endif
}

static char* ValueKindVector[eMaxValueKind] = {
    "string",
    "wide string",
    "managed string",
    "integer",
    "integer64",
    "float",
    "double",
    "identifier",
    "macro",
    "boolean",
    "GUID",
    "System::Type",
    "unknown",
    "unknown",
    "unknown"
};

inline __int64 GetUsageType(const char* val) {
    if (strcmp(val, "eAnyUsage") == 0) {
        return eAnyUsage;
    } else if (strcmp(val, "eCoClassUsage") == 0) {
        return eCoClassUsage;
    } else if (strcmp(val, "eCOMInterfaceUsage") == 0) {
        return eCOMInterfaceUsage;
    } else if (strcmp(val, "eInterfaceUsage") == 0) {
        return eInterfaceUsage;
    } else if (strcmp(val, "eMemberUsage") == 0) {
        return eMemberUsage;
    } else if (strcmp(val, "eMethodUsage") == 0) {
        return eMethodUsage;
    } else if (strcmp(val, "eInterfaceMethodUsage") == 0) {
        return eInterfaceMethodUsage;
    } else if (strcmp(val, "eCoClassMemberUsage") == 0) {
        return eCoClassMemberUsage;
    } else if (strcmp(val, "eCoClassMethodUsage") == 0) {
        return eCoClassMethodUsage;
    } else if (strcmp(val, "eGlobalMethodUsage") == 0) {
        return eGlobalMethodUsage;
    } else if (strcmp(val, "eGlobalDataUsage") == 0) {
        return eGlobalDataUsage;
    } else if (strcmp(val, "eClassUsage") == 0) {
        return eClassUsage;
    } else if (strcmp(val, "eInterfaceParameterUsage") == 0) {
        return eInterfaceParameterUsage;
    } else if (strcmp(val, "eMethodParameterUsage") == 0) {
        return eMethodParameterUsage;
    } else if (strcmp(val, "eIDLModuleUsage") == 0) {
        return eIDLModuleUsage;
    } else if (strcmp(val, "eAnonymousUsage") == 0) {
        return eAnonymousUsage;
    } else if (strcmp(val, "eTypedefUsage") == 0) {
        return eTypedefUsage;
    } else if (strcmp(val, "eUnionUsage") == 0) {
        return eUnionUsage;
    } else if (strcmp(val, "eEnumUsage") == 0) {
        return eEnumUsage;
    } else if (strcmp(val, "eDefineTagUsage") == 0) {
        return eDefineTagUsage;
    } else if (strcmp(val, "eStructUsage") == 0) {
        return eStructUsage;
    } else if (strcmp(val, "eLocalUsage") == 0) {
        return eLocalUsage;
    } else if (strcmp(val, "ePropertyUsage") == 0) {
        return ePropertyUsage;
    } else if (strcmp(val, "eEventUsage") == 0) {
        return eEventUsage;
    } else if (strcmp(val, "eTemplateUsage") == 0) {
        return eTemplateUsage;
    } else if (strcmp(val, "eModuleUsage") == 0) {
        return eModuleUsage;
    } else if (strcmp(val, "eIllegalUsage") == 0) {
        return eIllegalUsage;
    } else if (strcmp(val, "eAsynchronousUsage") == 0) {
        return eAsynchronousUsage;
    } else if (strcmp(val, "eAnyIDLUsage") == 0) {
        return eAnyIDLUsage;
    } else {
        char buf[512];
        _snprintf(buf, 512, "%s: incorrect format for attribute \'usage\'", val);
#if VERSP_RELEASE || VERSP_TEST || VERSP_DEBUG
        error(C2338, buf);
#else
        printf("%s\n", buf);
#endif
        DASSERT(UNREACHED);
        return eAnyUsage;
    }
}

#endif // __USAGE_H__
