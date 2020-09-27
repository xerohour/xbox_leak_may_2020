DAT(eAnyUsage,                0x000000, "anything")
DAT(eCoClassUsage,            0x000001, "coclasses")
DAT(eCOMInterfaceUsage,       0x000002, "COM interfaces")
DAT(eInterfaceUsage,          0x000004|eCOMInterfaceUsage, "interfaces")
DAT(eMemberUsage,             0x000008, "members")
DAT(eMethodUsage,             0x000010, "methods")
DAT(eInterfaceMethodUsage,    0x000020, "interface methods")
DAT(eCoClassMemberUsage,      0x000040, "coclass members")
DAT(eCoClassMethodUsage,      0x000080, "coclass methods")
DAT(eGlobalMethodUsage,       0x000100, "global methods")
DAT(eGlobalDataUsage,         0x000200, "global data")
DAT(eClassUsage,              0x000400, "classes") // any struct or class (non-coclass) but not typedef, union, enum, coclass or interface
DAT(eInterfaceParameterUsage, 0x000800, "interface method parameters") // interface function formals only
DAT(eMethodParameterUsage,    0x001000|eInterfaceParameterUsage, "method parameters") // function formals only
DAT(eIDLModuleUsage,          0x002000, "idl_module methods")
DAT(eAnonymousUsage,          0x004000, "anonymous attribute blocks")
DAT(eTypedefUsage,            0x008000, "typedefs")
DAT(eUnionUsage,              0x010000, "unions")
DAT(eEnumUsage,               0x020000, "enums")
DAT(eDefineTagUsage,          0x040000, "UDT definitions")
DAT(eStructUsage,             0x080000, "structs")
DAT(eLocalUsage,              0x100000, "local variables")
DAT(ePropertyUsage,           0x200000, "properties")
DAT(eEventUsage,              0x400000, "events")
DAT(eTemplateUsage,           0x800000, "templates")
DAT(eModuleUsage,             0x1000000, "modules") // Not real -- nothing should have module usage
DAT(eIllegalUsage,            0x2000000, "illegal usage")
DAT(eAsynchronousUsage,       0x4000000, "asynchronous usage")
DAT(eAnyIDLUsage,             0x1FBFFF, "anything but anonymous, property, event, asynchronous and template") // eAnyUsage - eAnonymousUsage - eTemplateUsage - ePropertyUsage - eEventUsage - eAsynchronousUsage
