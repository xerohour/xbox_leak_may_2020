
#include <unknwn.h>

[emitidl];

[export]
typedef enum tagEvent
{
    seenNoEvent = 0x0,        
    seenDefineTag = 0x1,
    seenAllBases = 0x2,
    seenFunctionFormal = 0x3,
    seenMember = 0x4, 
    seenAllMembers = 0x5,
    seenEndTag = 0x6,                      
    seenAllSource = 0x7,        
    seenDumpGlobals = 0x8,
    MAX_EVENT = 0x9
} Event;

// Must be kept in sync with Event
[export]
typedef enum tagEvent2 {
    seenNoEvent2        = (1 << seenNoEvent),
    seenDefineTag2      = (1 << seenDefineTag),
    seenAllBases2       = (1 << seenAllBases),
    seenFunctionFormal2 = (1 << seenFunctionFormal),
    seenMember2         = (1 << seenMember),
    seenAllMembers2     = (1 << seenAllMembers),
    seenEndTag2         = (1 << seenEndTag),
    seenAllSource2      = (1 << seenAllSource),
    seenDumpGlobals2    = (1 << seenDumpGlobals)
} Event2;
    
[export]
typedef enum tagMemberAttrs
{
    eNone = 0x0,
    ePrivate = 0x1,
    eProtected = 0x2,
    ePublic = 0x3,
    eAccMask = 0x3,
    eVirtual = 0x4,
    eStatic = 0x8,
    eConst = 0x10,
    eMethod = 0x20,
    eField = 0x40,
    eBase = 0x80,
    eOverload = 0x100,
    eCdecl = 0x200,
    eStdcall = 0x400,
    eThiscall = 0x800,
    eInterface = 0x1000,
    eCoclass = 0x2000,
    eClass = 0x4000,
    eStruct = 0x8000,
    eEnum = 0x10000,
    eUnion = 0x20000,
    eNamespace = 0x40000,
    eDefined = 0x80000,
    eQueuable = 0x100000,
    ePure = 0x200000,
    eOtherCall = 0x400000,
    eSymbol = 0x800000  // should be typedef
} MemberAttrs;

[export]
typedef enum tagInterfaceType
{
    eNoInterface = 0x0,
    eDual = 0x1,
    eDispinterface = 0x2,
    eCustom = 0x3,
    eCOMPlus = 0x4
} InterfaceType;

// scopes
[export]
typedef enum tagScopeType
{
    eNoScope = 0x0,
    eGlobalScope = 0x1,
    eClassScope = 0x2,
    eNamespaceScope = 0x4,
    eMemberScope = 0x8,
    eMethodScope = 0x10,
    eFormalScope = 0x20,
    eSymbolScope = 0x40,    // typedefs, etc.
    eMAXSCOPE
} ScopeType;

[export]
typedef enum tagPropertyType
{
    ePropNone = 0x0,
    ePropPut = 0x1,
    ePropGet = 0x2,
    ePropPutRef = 0x3
} PropertyType;

[export]
typedef enum tagUsageType
{   
    eAnyUsage                = 0x0,
    eCoClassUsage            = 0x1,
    eCPPInterfaceUsage       = 0x2,        
    eInterfaceUsage          = 0x4,
    eAnyInterfaceUsage       = 0x6,
    eMemberUsage             = 0x8,
    eMethodUsage             = 0x10,
    eInterfaceMethodUsage    = 0x20,
    eCoClassMemberUsage      = 0x40,
    eCoClassMethodUsage      = 0x80,
    eGlobalMethodUsage       = 0x100,  // global funcs
    eGlobalDataUsage         = 0x200,  // global data
    eClassUsage              = 0x400,  // any struct or class (non-coclass) but not typedef, union, enum, coclass or interface
    eInterfaceParameterUsage = 0x800,  // interface function formals only
    eCoClassParameterUsage   = 0x1000,  // coclass function formals only
    eIDLModuleUsage          = 0x2000,
    eAnonymousUsage          = 0x4000,        
    eNamespaceUsage          = 0x8000,
    eTypedefUsage            = 0x10000,
    eUnionUsage              = 0x20000,
    eEnumUsage               = 0x40000,
    eDefineTagUsage          = 0x80000,
    eStructUsage             = 0x100000,           
    eLocalUsage              = 0x200000,           
    eModuleUsage             = 0x400000, // Not real -- nothing should have module usage 
    eIllegalUsage            = 0x800000,          
    eAnyIDLUsage             = 0x3FBFFF,  // eAnyUsage - eAnonymousUsage
    eMaxUsage                = 0x7FFFFF,
    eUsageCount              = 0x1A // 3 less than the number of enum entries
} UsageType;

[export]
typedef enum tagGroupType
{
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
    eMaxGroup               = 0x10
} GroupType;

[export]
typedef enum tagProviderFlags
{
    eNoDefaultBehavior      = 0x0,
    eWarningUponDup         = 0x1
} ProviderFlags;

[export]
typedef struct StrToStrList
{
    [string] char* str;
    [size_is(strlist_len)] char** strlist;
    int strlist_len;
} StrToStrList;

[export]
struct AttributeSpecs
{
    [string] char* attribute;

    [size_is(arg_list_len)] char** argument_list;
    int arg_list_len;

    [size_is(arg_list_len)] char** argument_type;

    [size_is(req_args_len)] char** required_arguments;
    int req_args_len;

    [size_is(req_attrs_len)] char** required_class_attributes;
    int req_class_attrs_len;

    [size_is(req_attrs_len)] char** required_attributes;
    int req_attrs_len;
    [size_is(prop_req_list_len)] char** prop_required_attribute_keys;
    [size_is(prop_req_list_len)] char*** prop_required_attributes;
    [size_is(prop_req_list_len)] int* prop_req_attrs_len;
    int prop_req_list_len;

    [size_is(inv_attrs_len)] char** invalid_class_attributes;
    int inv_class_attrs_len;

    [size_is(inv_attrs_len)] char** invalid_attributes;
    int inv_attrs_len;
    [size_is(prop_inv_list_len)] char** prop_invalid_attribute_keys;
    [size_is(prop_inv_list_len)] char*** prop_invalid_attributes;
    [size_is(prop_inv_list_len)] int* prop_inv_attrs_len;
    int prop_inv_list_len;

    [size_is(imp_attrs_len)] char** implied_attributes;
    int imp_attrs_len;

    [size_is(arg_list_len)] char*** check_argument;
    [size_is(arg_list_len)] int* chk_arg_len;

    [size_is(arg_list_len)] char** default_value;

    [string] char* help_string;
    [size_is(arg_list_len)] char** prop_help_strings;

    [size_is(num_meta_types)] char** meta_types;
    [size_is(num_meta_types)] char** meta;
    int num_meta_types;

    [string] char* usage;   // usually UsageType, but user extendible -- not yet though
    [string] char* group;   // usually GroupType, but user extendible -- not yet though

    [string] char* listen_to;

    int repeatable;     // usually boolean  
    [size_is(arg_list_len)] int* arg_repeatable; // usually boolean 
    
    [size_is(arg_list_len)] int* multivalue;

    int frequently_used;// usually boolean
    int shipping;       // usually boolean
    int variable_args;  // required # of props+defaults is not fixed
    int explicit_args;  // usually boolean

    [string] char* qa;
    [string] char* ue;
    [string] char* pm;
    [string] char* dev;
};

[export]
struct MemberSpecs
{
    [string] char* name;
    [string] char* desc;
    uintptr_t attrs;
    uintptr_t alias;
};

[export]
typedef enum tagMetadataTypes
{
    eClassDef = 0,
    eClassRef = 1,
    eMethodDef = 2,
    eMethodRef = 3,
    eMethodImpl = 4
} MetadataTypes;

[export]
typedef enum tagValueKind
{
    eANSIStringValue = 0,
    eWideStringValue = 1,
    eIntegerValue = 2,
    eInteger64Value = 3,
    eFloatValue = 4,
    eDoubleValue = 5,
    eIdentValue = 6,
    eMacroValue = 7,
    eBooleanValue = 8,
    eGuidValue = 9,
    eUnknownValue = 10,
    eMaxValueKind = 11
} ValueKind;

[export]
typedef enum tagForceIncludeKind
{
    eInclude = 0,
    eSysInclude = 1,
    eImport = 2,
    eSysImport = 3
} ForceIncludeKind;

#if 0
[export]
struct SizedValue
{
    [switch_type(char), switch_is(kind)] union {        
    [case(1)]
        [string] wchar_t* wval;
    [default]
        [string] char* val;
    };
    char kind;
};
#else
[export]
struct SizedValue
{
    [string] char* val;
    char kind;
};
#endif

[export]
struct AttributeArg
{
    [string] char* name;
    [size_is(valcount)] struct SizedValue* vals;
    int valcount;
};

[export]
struct Attribute
{
    [string] char* name;
    [size_is(argcount)] struct AttributeArg* args;
    int argcount;
    DWORD vPos;
    DWORD vEndPos;
};

[
    object,
    uuid(64626784-83F5-11d2-B8DA-00C04F799BBB),
    helpstring("ICompiler Interface"),
    pointer_default(unique)
]
__interface ICompiler : IUnknown
{
    [id(1)] HRESULT AddBaseClass([in] struct MemberSpecs* specs, [in, defaultvalue(FALSE)] const boolean backpatch);
    [id(2)] HRESULT AddtoMemberList([in, string] const char* toks);
    [id(3)] HRESULT AddtoGlobalList([in, string] const char* toks, [in, defaultvalue(seenNoEvent)] Event e);
    [id(4)] HRESULT QueryScope([out] int* pScope, [out, retval] char** pScopename);
    [id(5)] HRESULT EmitGlobalsNow([in, defaultvalue(FALSE)] const boolean freeMemory);
    [id(6)] HRESULT InjectAtBeginningOfFunction([in, string] const char* toks, [in] const uintptr_t alias);
    [id(7)] HRESULT InjectAtEndOfFunction([in, string] const char* toks, [in] const uintptr_t alias);
    [id(8)] HRESULT InjectIntoFunctionHere([in, string] const char* toks);
    [id(9)] HRESULT AlterName([in, string] const char* name, [in] const uintptr_t alias);
    [id(10)] HRESULT QueryBases([in, out, size_is( , *count)] struct MemberSpecs** specs, [in, out] int* count, [in, defaultvalue(FALSE)] const boolean recursive);
    [id(11)] HRESULT QueryCurrentClass([out, retval] struct MemberSpecs* spec);
    [id(12)] HRESULT QueryCurrentLibrary([out, retval] struct MemberSpecs* spec);
    [id(13)] HRESULT QueryCurrentMember([out, retval] struct MemberSpecs* spec);        
    [id(14)] HRESULT QueryMembers([in, out, size_is( , *count)] struct MemberSpecs** specs, [in, out] int* count);
    [id(15)] HRESULT QueryMacro([in, string] const char* macro, [out, retval] char** expansion);
    [id(16)] HRESULT QuerySymbol([in, out] struct MemberSpecs* specs, [out] boolean* isDefd, [in] const boolean lookonlyinclass, [in] const uintptr_t enclosingclass);
    [id(17)] HRESULT QueryAttributes([in, out, size_is( , *pcount)] struct Attribute** ppattr, [in, out] int* pcount, [in] const uintptr_t alias);
    [id(18)] HRESULT EmitMetadata([out] uintptr_t* emit_alias, [out] uintptr_t* import_alias); 
    [id(19)] HRESULT Guid([out] LPGUID* ppGuid, [out] struct SizedValue* pid, [in] const uintptr_t alias);
    [id(20)] HRESULT EmitWarning([in] const int level, [in, string] const char* warn);
    [id(21)] HRESULT EmitError([in, string] const char* error);      
    [id(22)] HRESULT ForceInclude([in, string] const char* fullpath, [in] const ForceIncludeKind e, [in, defaultvalue(FALSE)] const boolean freeMemory);
    [id(23)] HRESULT Pragma([in, string] const char* pragma);
    [id(24)] HRESULT EmitMembersNow();
    [id(25)] HRESULT IntegralExpressionEvaluator([in, string] const char* toks, [out, retval] int* out);    
    [id(26)] HRESULT GetValue([in] struct SizedValue* sv, [string, out, retval] char** val);
    [id(27)] HRESULT GetDispid([in] uintptr_t alias, [out] long* dispid, [out] PropertyType* prop, [out] long* vfoffset);
    [id(28)] HRESULT QueryStunDepth([out, retval] intptr_t *sd);
    [id(29)] HRESULT QueryPosition([in] struct Attribute* attr, [in] const uintptr_t alias, [out, string] char** file, [out] uintptr_t *line);
    [id(30)] HRESULT QueryName([in, string] const char* decorname, [out, string] char** undecorname);
    [id(31)] HRESULT QueryToken([in] uintptr_t alias, [in] MetadataTypes e, [out] uintptr_t* token);
    [id(32)] HRESULT QueryFullPath([in, string] const char* filename, [out, string] char** fullpath); 
};

[
    hidden,
    restricted,
    object,
    uuid(64626785-83F5-11d2-B8DA-00C04F799BBB),
    helpstring("ICompiler2 Interface"),
    pointer_default(unique)
]
__interface ICompiler2 : ICompiler
{
    [id(1)] HRESULT QueryFunctionFormal([out, retval] uintptr_t* ppsym);
    [id(2)] HRESULT AttributeMap([in, string] const char* scope, [in, string] const char* attr);    
    [id(3)] HRESULT SetGuid([in] struct SizedValue *pValue);
    [id(4)] HRESULT SetModule([in] struct Attribute* pattr);
    [id(5)] HRESULT SetThreading([in] struct Attribute* pattr);
    [id(6)] HRESULT SetInterface([in] InterfaceType einterface);
    [id(7)] HRESULT SetVersion([in] struct Attribute* ppattr);
    [id(8)] HRESULT SetControl([in] const boolean isdefined);
    [id(9)] HRESULT EmitGlobalsAt(Event e, const boolean freeMemory);   
    [id(10)] HRESULT SetProgid([in] struct SizedValue *pValue);
    [id(11)] HRESULT SetProperty([in] struct Attribute* pattr);
    [id(12)] HRESULT QueryCurrentSymbol([out, retval] struct MemberSpecs* spec);
    [id(13)] HRESULT SetPropput([in] const boolean isProperty);
    [id(14)] HRESULT SetPropget([in] const boolean isProperty);
    [id(15)] HRESULT SetPropputref([in] const boolean isProperty);
    [id(16)] HRESULT SetCOM([in] const boolean isCOM);
    [id(17)] HRESULT SetObject([in] const boolean isObject);
    [id(18)] HRESULT SetCoClass([in] const boolean isCoClass);
#if CC_RAW
    [id(19)] HRESULT SetRaw([in] const boolean isRaw);
#endif  // CC_RAW
#if CC_COR
    [id(20)] HRESULT SetManaged([in] const boolean isManaged);
    [id(21)] HRESULT SetDelegate([in] const boolean isDelegate);
    [id(22)] HRESULT SetMulticastDelegate([in] const boolean isMulticastDelegate);
#endif  // CC_COR
#if CC_COMARRAY
    [id(23)] HRESULT SetComArray([in] const boolean isComArray);
#endif  // CC_COMARRAY
#if CC_COR
    [id(24)] HRESULT SetFinal([in] const boolean isFinal);
#endif  // CC_COR
    [id(25)] HRESULT RegisterHandler([in] LPGUID pClsid, [out] uintptr_t* ppHandler);
#if CC_COR
    [id(26)] HRESULT SetPDirect([in] struct Attribute* pattr);
#endif  // CC_COR
};


extern "C" const __declspec(selectany) GUID CATID_AttributeProvider = { 0xd57875f0, 0x2f34, 0x11d3, { 0xbe, 0x70, 0x0, 0xa0, 0xc9, 0xa3, 0xa5, 0x9b } };
    

[
    object,
    uuid(64626787-83F5-11d2-B8DA-00C04F799BBB),
    helpstring("IAttributeGrammar Interface"),
    pointer_default(unique)
]
__interface IAttributeGrammar : IUnknown
{       
    [id(1)] HRESULT GetAllAttributes([out] char*** ppszAttributes, [out] int* pnCount);
    [id(2)] HRESULT GetListenToAttributes([out] char*** ppszAttributes, [out] int* pnCount);
    [id(3)] HRESULT GetAttributeList([in] UsageType usage, [out] char*** ppszAttributes, [out] int* pnCount);
    [id(4)] HRESULT GetArgumentList([in] int nAttribute, [out] char*** ppszArguments, [out] int* pnArgCount);
    [id(5)] HRESULT GetArgumentValueList([in] int nAttribute, [in] int nArgument, [out] char*** ppszValues, [out] int* pnValCount);
    [id(6)] HRESULT GetAttributeUsage([in] int nAttribute, [out] int* pUsageType, [out] BSTR* pUsageStr);
    [id(7)] HRESULT GetArgumentDefault([in] int nAttribute, [in] int nArgument, [out] char** pszDefault);
    [id(8)] HRESULT GetAttributeCategory([in] int nAttribute, [out] char** pszCategory);
    [id(9)] HRESULT GetRepeatable([in] int nAttribute, [out] BOOL* pbRepeatable);
    [id(10)] HRESULT GetArgumentType([in] int nAttribute, [in] int nArgument, [out] int* pbType);
    [id(11)] HRESULT GetRequiredArguments([in] int nAttribute, [out] char*** pszReqArguments, [out] int* pnCount);
    [id(12)] HRESULT GetRequiredAttributes([in] int nAttribute, [out] char*** pszReqAttributes, [out] int* pnCount);
    [id(13)] HRESULT GetRequiredClassAttributes([in] int nAttribute, [out] char*** pszReqClassAttrs, [out] int* pnCount);
    [id(14)] HRESULT GetArgumentRequiredAttributes([in] int nAttribute, [out] char*** keys, [out] char*** reqargs, [out] int* maxdim, [out] int** pcount, [out] int* count);
    [id(15)] HRESULT GetInvalidAttributes([in] int nAttribute, [out] char*** pszReqAttributes, [out] int* pnCount);
    [id(16)] HRESULT GetInvalidClassAttributes([in] int nAttribute, [out] char*** pszReqClassAttrs, [out] int* pnCount);
    [id(17)] HRESULT GetArgumentInvalidAttributes([in] int nAttribute, [out] char*** keys, [out] char*** invargs, [out] int* maxdim, [out] int** pcount, [out] int* count);        
    [id(18)] HRESULT GetHelpString([in] int nAttribute, [in] int nArgument, [out] BSTR* pszHelpString);
    [id(19)] HRESULT GetAttributeHelpString([in] int nAttribute, [out] char** pszHelpString);
    [id(20)] HRESULT GetArgumentHelpString([in] int nAttribute, [in] int nArgument, [out] char** pszHelpString);
    [id(21)] HRESULT GetMeta([in] int nAttribute, [in]char* pszMetaType, [out] char** ppszArguments);
    [id(22)] HRESULT GetVariableArguments([in] int nAttribute, [out] BOOL* pbVarArgs);
    [id(23)] HRESULT AttributeToInt([in] const char* attr, [out, retval] int* idx);
    [id(24)] HRESULT GetShipping([in] int nAttribute, [out] BOOL* pbShipping);
    [id(25)] HRESULT GetVersion([out] DWORD* pmajor, [out] DWORD* pminor);
    [id(26)] HRESULT GetNames([out] char** provname, [out] char** provguid);
    [id(27)] HRESULT GetMultiValue([in] int nAttribute, [in] int nArgument, [out] BOOL* pbMultivalue);
    [id(28)] HRESULT GetArgumentRepeatable([in] int nAttribute, [in] int nArgument, [out] BOOL* pbRepeatable);
    [id(29)] HRESULT GetExplicitArguments([in] int nAttribute, [out] BOOL* pbExplicitArgs);
};

[
    object,
    uuid(64626786-83F5-11d2-B8DA-00C04F799BBB),
    helpstring("IAttributeHandler Interface"),
    pointer_default(unique)
]
__interface IAttributeHandler : IUnknown
{
    [id(1), helpstring("method ParseAttributes")] 
    HRESULT ParseAttributes(
        [in] ICompiler* pcxxfe, 
        [in, size_is(count)] struct Attribute* attr,
        [in] int count,
        [in] int usage);
    [id(2), helpstring("method OnEvent")] 
    HRESULT OnEvent([in] ICompiler* pcxxfe, [in] Event e);
    [id(3), helpstring("method CreateInstance")] 
    HRESULT CreateInstance(
        [in] int eFlags, 
        [out] IAttributeHandler** ppHandler, 
        [out] IAttributeGrammar** ppGrammar);
};


[
    module(name="CompilerLib",
           uuid="6462678F-83F5-11d2-B8DA-00C04F799BBB",
           version="1.0",
           helpstring="Compiler 1.0 Type Library")
];

CASSERT(CC_BLDWITH61_IDLATTR);

#define Compiler CCompiler
#define DefaultHandler CDefaultHandler
#define ProvProv CProvProv

