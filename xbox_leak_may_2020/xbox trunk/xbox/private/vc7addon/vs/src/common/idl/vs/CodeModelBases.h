#ifndef DONTUSE_NONBROWSABLE
#define NONBROWSABLE ,nonbrowsable
#else
#define NONBROWSABLE
#endif

#define REMOVE(dispid) \
  [id(dispid), HELP(vsextCodeEltRemove) ] \
  HRESULT Remove(VARIANT Element);

#define ADDNAMESPACE(dispid) \
  [id(dispid), HELP(vsextCMAddNamespace)] \
  HRESULT AddNamespace(BSTR Name, VARIANT Location, [optional] VARIANT Position, [out, retval] CodeNamespace **ppCodeNamespace);

#define ADDNAMESPACE_NOFILENAME(dispid) \
  [id(dispid), HELP(vsextCMAddNamespace)] \
  HRESULT AddNamespace(BSTR Name, [optional] VARIANT Position, [out, retval] CodeNamespace **ppCodeNamespace);

#define ADDCLASS_NOFILENAME(dispid) \
  [id(dispid), HELP(vsextCMAddClass)] \
  HRESULT AddClass(BSTR Name, [optional] VARIANT Position, [optional] VARIANT Bases, [optional] VARIANT ImplementedInterfaces, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeClass **ppCodeClass);

#define ADDCLASS(dispid) \
  [id(dispid), HELP(vsextCMAddClass)] \
  HRESULT AddClass(BSTR Name, VARIANT Location, [optional] VARIANT Position, [optional] VARIANT Bases, [optional] VARIANT ImplementedInterfaces, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeClass **ppCodeClass);

#define ADDFUNCTION(dispid) \
  [id(dispid), HELP(vsextCMAddFunc)] \
  HRESULT AddFunction(BSTR Name, VARIANT Location, enum vsCMFunction Kind, VARIANT Type, [optional] VARIANT Position, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeFunction **ppCodeFunction);

#define ADDFUNCTION_NOFILE(dispid) \
  [id(dispid), HELP(vsextCMAddFunc)] \
  HRESULT AddFunction(BSTR Name, enum vsCMFunction Kind, VARIANT Type, [optional] VARIANT Position, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeFunction **ppCodeFunction);

#define ADDFUNCTION_OPTLOC(dispid) \
  [id(dispid), HELP(vsextCMAddFunc)] \
  HRESULT AddFunction(BSTR Name, enum vsCMFunction Kind, VARIANT Type, [optional] VARIANT Position, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [optional] VARIANT Location, [out, retval] CodeFunction **ppCodeFunction);

#define ADDVARIABLE(dispid) \
  [id(dispid), HELP(vsextCMAddVariable)] \
  HRESULT AddVariable(BSTR Name, VARIANT Location, VARIANT Type, [optional] VARIANT Position, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeVariable **ppCodeVariable);

#define ADDVARIABLE_NOFILE(dispid) \
  [id(dispid), HELP(vsextCMAddVariable)] \
  HRESULT AddVariable(BSTR Name, VARIANT Type, [optional] VARIANT Position, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeVariable **ppCodeVariable);

#define ADDVARIABLE_OPTLOC(dispid) \
  [id(dispid), HELP(vsextCMAddVariable)] \
  HRESULT AddVariable(BSTR Name, VARIANT Type, [optional] VARIANT Position, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [optional] VARIANT Location, [out, retval] CodeVariable **ppCodeVariable);

#define ADDATTRIBUTE(dispid) \
  [id(dispid), HELP(vsextCMAddAttribute)] \
  HRESULT AddAttribute(BSTR Name, VARIANT Location, BSTR Value, [optional] VARIANT Position, [out, retval] CodeAttribute **ppCodeAttribute);

#define ADDATTRIBUTE_NOFILENAME(dispid) \
  [id(dispid), HELP(vsextCMAddAttribute)] \
  HRESULT AddAttribute(BSTR Name, BSTR Value, [optional] VARIANT Position, [out, retval] CodeAttribute **ppCodeAttribute);

#define ADDINTERFACE(dispid) \
  [id(dispid), HELP(vsextCMAddInterface)] \
  HRESULT AddInterface(BSTR Name, VARIANT Location, [optional] VARIANT Position, [optional] VARIANT Bases, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeInterface **ppCodeInterface);

#define ADDINTERFACE_NOFILENAME(dispid) \
  [id(dispid), HELP(vsextCMAddInterface)] \
  HRESULT AddInterface(BSTR Name, [optional] VARIANT Position, [optional] VARIANT Bases, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeInterface **ppCodeInterface);

#define ADDPROPERTY(dispid) \
  [id(dispid), HELP(vsextCMAddProperty)] \
  HRESULT AddProperty(BSTR GetterName, BSTR PutterName, VARIANT Location, VARIANT Type, [optional] VARIANT Position, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeProperty **ppCodeProperty);

#define ADDPROPERTY_NOLOC(dispid) \
  [id(dispid), HELP(vsextCMAddProperty)] \
  HRESULT AddProperty(BSTR GetterName, BSTR PutterName, VARIANT Type, [optional] VARIANT Position, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [optional] VARIANT Location, [out, retval] CodeProperty **ppCodeProperty);

#define ADDPROPERTY_OPTLOC(dispid) \
  [id(dispid), HELP(vsextCMAddProperty)] \
  HRESULT AddProperty(BSTR GetterName, BSTR PutterName, VARIANT Type, [optional] VARIANT Position, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [optional] VARIANT Location, [out, retval] CodeProperty **ppCodeProperty);

#define ADDSTRUCT(dispid) \
  [id(dispid), HELP(vsextCMAddStruct)] \
  HRESULT AddStruct(BSTR Name, VARIANT Location, [optional] VARIANT Position, [optional] VARIANT Bases, [optional] VARIANT ImplementedInterfaces, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeStruct **ppCodeStruct);

#define ADDSTRUCT_NOFILENAME(dispid) \
  [id(dispid), HELP(vsextCMAddStruct)] \
  HRESULT AddStruct(BSTR Name, [optional] VARIANT Position, [optional] VARIANT Bases, [optional] VARIANT ImplementedInterfaces, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeStruct **ppCodeStruct);

#define ADDENUM(dispid) \
  [id(dispid), HELP(vsextCMAddEnum)] \
  HRESULT AddEnum(BSTR Name, VARIANT Location, [optional] VARIANT Position, [optional] VARIANT Bases, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeEnum **ppCodeEnum);

#define ADDENUM_NOFILENAME(dispid) \
  [id(dispid), HELP(vsextCMAddEnum)] \
  HRESULT AddEnum(BSTR Name, [optional] VARIANT Position, [optional] VARIANT Bases, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeEnum **ppCodeEnum);

#define ADDDELEGATE(dispid) \
  [id(dispid), HELP(vsextCMAddDelegate)] \
  HRESULT AddDelegate(BSTR Name, VARIANT Location, VARIANT Type, [optional] VARIANT Position, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeDelegate **ppCodeDelegate);

#define ADDDELEGATE_NOFILENAME(dispid) \
  [id(dispid), HELP(vsextCMAddDelegate)] \
  HRESULT AddDelegate(BSTR Name, VARIANT Type, [optional] VARIANT Position, [defaultvalue(vsCMAccessDefault)] enum vsCMAccess Access, [out, retval] CodeDelegate **ppCodeDelegate);

#define ADDBASE(dispid) \
  [id(dispid), HELP(vsextCMAddBase)] \
  HRESULT AddBase(VARIANT Base, [optional] VARIANT Position, [out, retval] CodeElement **ppOut);

#define ADDIMPLEMENTEDINTERFACE(dispid) \
  [id(dispid), HELP(vsextCMAddImplInterface)] \
  HRESULT AddImplementedInterface(VARIANT Base, [optional] VARIANT Position, [out, retval] CodeInterface **ppCodeInterface);

#define ADDMEMBER(dispid) \
  [id(dispid), HELP(vsextCMAddMember)] \
  HRESULT AddMember(BSTR Name, [optional] VARIANT Value, [optional] VARIANT Position, [out, retval] CodeVariable **ppCodeElements);

#define ADDPARAMETER(dispid) \
  [id(dispid), HELP(vsextCMAddParameter)] \
  HRESULT AddParameter(BSTR Name, VARIANT Type, [optional] VARIANT Position, [out, retval] CodeParameter **ppCodeParameter);

#define ADDSTATEMENT(dispid) \
  [id(dispid), propget, HELP(vsextCodeModelAddStatement)] \
  HRESULT AddStatement(IDispatch *Statement, [defaultvalue(VARIANT_FALSE)] VARIANT_BOOL DesignTimeOnly);

#define ADDARGUMENT(dispid) \
  [id(dispid)/*TODO:, HELP()*/] \
  HRESULT AddArgument(IDispatch *Expression);

#define ADDELEMENT(dispid) \
  [id(dispid)/*TODO:, HELP()*/] \
  HRESULT AddElement(IDispatch *Expression);


#define CODEELEMENTBASEMETHODS_V7 \
	[id(1), propget, HELP(vsproDTEProperty) NONBROWSABLE] \
  HRESULT DTE ([out, retval] DTE **DTEObject); \
  [id(2), propget, HELP(vsproCollectionProperty) NONBROWSABLE] \
  HRESULT Collection([out, retval] CodeElements **ppCollection); \
  [id(DISPID_VALUE), propget, HELP(vsproNameProperty)] \
  HRESULT Name([out, retval] BSTR *pVal); \
  [id(DISPID_VALUE), propput, HELP(vsproNameProperty)] \
  HRESULT Name(BSTR NewName); \
  [id(3), propget, HELP(vsextCMFullNameProp)] \
  HRESULT FullName([out, retval] BSTR *pVal); \
  [id(4), propget, HELP(vsproProjectItemProperty) NONBROWSABLE] \
  HRESULT ProjectItem([out, retval] ProjectItem **pProjItem); \
  [id(5), propget, HELP(vsproKindProperty2) NONBROWSABLE] \
  HRESULT Kind([out, retval] enum vsCMElement *pCodeEltKind); \
  [id(6), propget, HELP(vsextCMIsCodeType), NONBROWSABLE] \
  HRESULT IsCodeType([out,retval] VARIANT_BOOL *pIsCodeType); \
  [id(7), propget, HELP(vsextCodeModelInfoLoc) NONBROWSABLE] \
  HRESULT InfoLocation([out, retval] enum vsCMInfoLocation *pInfoLocation); \
  [id(8), propget, HELP(vsextCodeModelChildren) NONBROWSABLE] \
  HRESULT Children([out, retval] CodeElements **ppCodeElements); \
  [id(9), propget, HELP(vsextCodeModelLang) NONBROWSABLE] \
  HRESULT Language([out, retval] BSTR *pLanguage); \
  [id(10), propget, HELP(vsextCodeModelStartPoint) NONBROWSABLE] \
  HRESULT StartPoint([out, retval] TextPoint **ppTextPoint); \
  [id(11), propget, HELP(vsextCodeModelEndPoint) NONBROWSABLE] \
  HRESULT EndPoint([out, retval] TextPoint **ppTextPoint); \
  [id(12), propget NONBROWSABLE, HELP(vsobjExtendersExtenderNamesProperty)] \
  HRESULT ExtenderNames([out, retval] VARIANT *ExtenderNames); \
  [id(13), propget NONBROWSABLE, HELP(vsobjExtendersExtenderProperty)] \
  HRESULT Extender(BSTR ExtenderName, [out, retval] IDispatch **Extender); \
  [id(14), propget NONBROWSABLE, HELP(vsobjExtenderExtenderCATIDProperty)] \
  HRESULT ExtenderCATID ([out, retval] BSTR *pRetval); \
  [id(15), HELP(vsextCodeModelStartPoint) NONBROWSABLE] \
  HRESULT GetStartPoint([in, defaultvalue(10)] enum vsCMPart Part, [out, retval] TextPoint **ppTextPoint); \
  [id(16), HELP(vsextCodeModelEndPoint) NONBROWSABLE] \
  HRESULT GetEndPoint([in, defaultvalue(10)] enum vsCMPart Part, [out, retval] TextPoint **ppTextPoint);

#define LASTCODEELEMENTDISPID_V7 30

#define CODENAMESPACEBASEMETHODS_V7 \
  CODEELEMENTBASEMETHODS_V7\
  [id(LASTCODEELEMENTDISPID_V7+1), propget, HELP(vsproParentProperty) NONBROWSABLE] \
  HRESULT Parent ([out, retval] IDispatch **ParentObject); \
  [id(LASTCODEELEMENTDISPID_V7+2), propget, HELP(vsextCodeModelMembers) NONBROWSABLE] \
  HRESULT Members([out, retval] CodeElements **ppMembers); \
  [id(LASTCODEELEMENTDISPID_V7+5), propget, HELP(vsDocCommentProperty) NONBROWSABLE] \
  HRESULT DocComment([out, retval] BSTR *pDocComment); \
  [id(LASTCODEELEMENTDISPID_V7+5), propput, HELP(vsDocCommentProperty) NONBROWSABLE] \
  HRESULT DocComment(BSTR DocComment); \
  [id(LASTCODEELEMENTDISPID_V7+6), propget, HELP(vsextCodeModelComment) NONBROWSABLE] \
  HRESULT Comment([out, retval] BSTR *pComment); \
  [id(LASTCODEELEMENTDISPID_V7+6), propput, HELP(vsextCodeModelComment) NONBROWSABLE] \
  HRESULT Comment(BSTR Comment); \
  ADDNAMESPACE_NOFILENAME(LASTCODEELEMENTDISPID_V7+7) \
  ADDCLASS_NOFILENAME(LASTCODEELEMENTDISPID_V7+8) \
  ADDINTERFACE_NOFILENAME(LASTCODEELEMENTDISPID_V7+9) \
  ADDSTRUCT_NOFILENAME(LASTCODEELEMENTDISPID_V7+10) \
  ADDENUM_NOFILENAME(LASTCODEELEMENTDISPID_V7+11) \
  ADDDELEGATE_NOFILENAME(LASTCODEELEMENTDISPID_V7+12) \
  REMOVE(LASTCODEELEMENTDISPID_V7+13)

#define LASTCODENAMESPACEDISPID_V7 LASTCODEELEMENTDISPID_V7+30


#define CODETYPEBASEMETHODS_V7 \
  CODEELEMENTBASEMETHODS_V7 \
  [id(LASTCODEELEMENTDISPID_V7+1), propget, HELP(vsproParentProperty) NONBROWSABLE] \
  HRESULT Parent ([out, retval] IDispatch **ParentObject); \
  [id(LASTCODEELEMENTDISPID_V7+2), propget, HELP(vsextCodeModelNamespaceProp) NONBROWSABLE] \
  HRESULT Namespace([out, retval] CodeNamespace **ppCodeNamespace); \
  [id(LASTCODEELEMENTDISPID_V7+3), propget, HELP(vsextCodeModelBaseClasses) NONBROWSABLE] \
  HRESULT Bases([out, retval] CodeElements **ppCodeElements); \
  [id(LASTCODEELEMENTDISPID_V7+4), propget, HELP(vsextCodeModelMembers) NONBROWSABLE] \
  HRESULT Members([out, retval] CodeElements **ppCodeElements); \
  [id(LASTCODEELEMENTDISPID_V7+5), propput, HELP(vsextCodeModelAccess)] \
  HRESULT Access(enum vsCMAccess Access); \
  [id(LASTCODEELEMENTDISPID_V7+5), propget, HELP(vsextCodeModelAccess)] \
  HRESULT Access([out, retval] enum vsCMAccess *pAccess); \
  [id(LASTCODEELEMENTDISPID_V7+6), propget, HELP(vsextCodeModelAttributes) NONBROWSABLE] \
  HRESULT Attributes([out, retval] CodeElements **ppCodeElements); \
  [id(LASTCODEELEMENTDISPID_V7+7), propget, HELP(vsDocCommentProperty) NONBROWSABLE] \
  HRESULT DocComment([out, retval] BSTR *pDocComment); \
  [id(LASTCODEELEMENTDISPID_V7+7), propput, HELP(vsDocCommentProperty) NONBROWSABLE] \
  HRESULT DocComment(BSTR DocComment); \
  [id(LASTCODEELEMENTDISPID_V7+8), propget, HELP(vsextCodeModelComment) NONBROWSABLE] \
  HRESULT Comment([out, retval] BSTR *pComment); \
  [id(LASTCODEELEMENTDISPID_V7+8), propput, HELP(vsextCodeModelComment) NONBROWSABLE] \
  HRESULT Comment(BSTR Comment); \
  ADDBASE(LASTCODEELEMENTDISPID_V7+9) \
  ADDATTRIBUTE_NOFILENAME(LASTCODEELEMENTDISPID_V7+10) \
  [id(LASTCODEELEMENTDISPID_V7+11), HELP(vsextCMRemoveBase)] \
  HRESULT RemoveBase(VARIANT Element); \
  [id(LASTCODEELEMENTDISPID_V7+12), HELP(vsextCMRemoveMember)] \
  HRESULT RemoveMember(VARIANT Element); \
  [id(LASTCODEELEMENTDISPID_V7+13), propget, HELP(vsextCMDerivedFrom)] \
  HRESULT IsDerivedFrom(BSTR FullName, [out, retval] VARIANT_BOOL *pVal); \
  [id(LASTCODEELEMENTDISPID_V7+14), propget, HELP(vsextCMDerivedTypes), NONBROWSABLE] \
  HRESULT DerivedTypes([out, retval] CodeElements **ppCodeElements);

#define LASTCODETYPEDISPID_V7 LASTCODEELEMENTDISPID_V7+30

#define  CODECLASSBASEMETHODS_V7 \
  CODETYPEBASEMETHODS_V7 \
  [id(LASTCODETYPEDISPID_V7+1), propget, HELP(vsextCMImpledInterfaces) NONBROWSABLE] \
  HRESULT ImplementedInterfaces([out, retval] CodeElements **ppCodeElements); \
  [id(LASTCODETYPEDISPID_V7+2), propget, HELP(vsextCodeModelIsAbstract)] \
  HRESULT IsAbstract([out, retval] VARIANT_BOOL *pIsAbstract); \
  [id(LASTCODETYPEDISPID_V7+2), propput, HELP(vsextCodeModelIsAbstract)] \
  HRESULT IsAbstract(VARIANT_BOOL Abstract); \
  ADDIMPLEMENTEDINTERFACE(LASTCODETYPEDISPID_V7+3) \
  ADDFUNCTION_OPTLOC(LASTCODETYPEDISPID_V7+4) \
  ADDVARIABLE_OPTLOC(LASTCODETYPEDISPID_V7+5) \
  ADDPROPERTY_OPTLOC(LASTCODETYPEDISPID_V7+6) \
  ADDCLASS_NOFILENAME(LASTCODETYPEDISPID_V7+7) \
  ADDSTRUCT_NOFILENAME(LASTCODETYPEDISPID_V7+8) \
  ADDENUM_NOFILENAME(LASTCODETYPEDISPID_V7+9) \
  ADDDELEGATE_NOFILENAME(LASTCODETYPEDISPID_V7+10) \
  [id(LASTCODETYPEDISPID_V7+11), HELP(vsextCMRemoveIFace)] \
  HRESULT RemoveInterface(VARIANT Element);

#define LASTCODECLASSEDISPID_V7 LASTCODETYPEDISPID_V7+30


#define CODETYPEREFBASEMETHODS_V7 \
  [id(1), propget, HELP(vsproDTEProperty) NONBROWSABLE] \
  HRESULT DTE ([out, retval] DTE **DTEObject); \
  [id(2), propget, HELP(vsproParentProperty) NONBROWSABLE] \
  HRESULT Parent ([out, retval] IDispatch **ParentObject); \
  [id(3), propget, HELP(vsextCMTypeKind)] \
  HRESULT TypeKind([out, retval] enum vsCMTypeRef *pType); \
  [id(4), propget, HELP(vsextCMCodeTypeProp) NONBROWSABLE] \
  HRESULT CodeType([out, retval] CodeType **ppCodeType); \
  [id(4), propput, HELP(vsextCMCodeTypeProp) NONBROWSABLE] \
  HRESULT CodeType(CodeType *Type); \
  [id(5), propget, HELP(vsextCodeModelType) NONBROWSABLE] \
  HRESULT ElementType([out, retval] CodeTypeRef **ppCodeTypeRef); \
  [id(5), propput, HELP(vsextCodeModelType) NONBROWSABLE] \
  HRESULT ElementType(CodeTypeRef *Type); \
  [id(6), propget, HELP(vsextCMAsStringProp)] \
  HRESULT AsString([out, retval] BSTR *pAsString); \
  [id(7), propget, HELP(vsextCMAsFullNameProp)] \
  HRESULT AsFullName([out, retval] BSTR *pAsFullName); \
  [id(8), propget, HELP(vsextCMRankProp)] \
  HRESULT Rank([out, retval] long *pRank); \
  [id(8), propput, HELP(vsextCMRankProp)] \
  HRESULT Rank(long Rank); \
  [id(9),HELP(vsextCMCreateArray)] \
  HRESULT CreateArrayType([defaultvalue(1)] long Rank, [out, retval] CodeTypeRef **ppTypeRef);

#define CODEFUNCTIONBASEMETHODS_V7 \
  CODEELEMENTBASEMETHODS_V7 \
  [id(LASTCODEELEMENTDISPID_V7+1), propget, HELP(vsproParentProperty) NONBROWSABLE] \
  HRESULT Parent ([out, retval] IDispatch **ParentObject); \
  [id(LASTCODEELEMENTDISPID_V7+2), propget, HELP(vsextCodeModelFcnKind) NONBROWSABLE] \
  HRESULT FunctionKind([out, retval] enum vsCMFunction *ppFunctionKind); \
  [id(LASTCODEELEMENTDISPID_V7+4), propget, HELP(vsextCodeModelPrototype) NONBROWSABLE] \
  HRESULT Prototype([in, defaultvalue(0)] long Flags, [out, retval] BSTR *pFullName); \
  [id(LASTCODEELEMENTDISPID_V7+5), propget, HELP(vsextCodeModelType) NONBROWSABLE] \
  HRESULT Type([out, retval] CodeTypeRef **ppCodeTypeRef); \
  [id(LASTCODEELEMENTDISPID_V7+5), propput, HELP(vsextCodeModelType) NONBROWSABLE] \
  HRESULT Type(CodeTypeRef *pCodeTypeRef); \
  [id(LASTCODEELEMENTDISPID_V7+6), propget, HELP(vsextCodeModelParamsProp) NONBROWSABLE] \
  HRESULT Parameters([out, retval] CodeElements **ppMembers); \
  [id(LASTCODEELEMENTDISPID_V7+8), propput, HELP(vsextCodeModelAccess)] \
  HRESULT Access(enum vsCMAccess Access); \
  [id(LASTCODEELEMENTDISPID_V7+8), propget, HELP(vsextCodeModelAccess)] \
  HRESULT Access([out, retval] enum vsCMAccess *Access); \
  [id(LASTCODEELEMENTDISPID_V7+9), propget, HELP(vsextCodeModelIsOverloaded)] \
  HRESULT IsOverloaded([out, retval] VARIANT_BOOL *pvbOverloaded); \
  [id(LASTCODEELEMENTDISPID_V7+10), propget, HELP(vsextCodeModelIsStatic)] \
  HRESULT IsShared([out, retval] VARIANT_BOOL *Shared); \
  [id(LASTCODEELEMENTDISPID_V7+10), propput, HELP(vsextCodeModelIsStatic)] \
  HRESULT IsShared(VARIANT_BOOL Shared); \
  [id(LASTCODEELEMENTDISPID_V7+11), propget, HELP(vsextCodeModelIsAbstract)] \
  HRESULT MustImplement([out, retval] VARIANT_BOOL *MustImplement); \
  [id(LASTCODEELEMENTDISPID_V7+11), propput, HELP(vsextCodeModelIsAbstract)] \
  HRESULT MustImplement(VARIANT_BOOL MustImplement); \
  [id(LASTCODEELEMENTDISPID_V7+12), propget, HELP(vsextCodeModelOverloadsProp) NONBROWSABLE] \
  HRESULT Overloads([out, retval] CodeElements **ppMembers); \
  [id(LASTCODEELEMENTDISPID_V7+13), propget, HELP(vsextCodeModelAttributes) NONBROWSABLE] \
  HRESULT Attributes([out, retval] CodeElements **ppMembers); \
  [id(LASTCODEELEMENTDISPID_V7+14), propget, HELP(vsDocCommentProperty) NONBROWSABLE] \
  HRESULT DocComment([out, retval] BSTR *pDocComment); \
  [id(LASTCODEELEMENTDISPID_V7+14), propput, HELP(vsDocCommentProperty) NONBROWSABLE] \
  HRESULT DocComment(BSTR DocComment); \
  [id(LASTCODEELEMENTDISPID_V7+15), propget, HELP(vsextCodeModelComment) NONBROWSABLE] \
  HRESULT Comment([out, retval] BSTR *pComment); \
  [id(LASTCODEELEMENTDISPID_V7+15), propput, HELP(vsextCodeModelComment) NONBROWSABLE] \
  HRESULT Comment(BSTR Comment); \
  ADDPARAMETER(LASTCODEELEMENTDISPID_V7+17) \
  ADDATTRIBUTE_NOFILENAME(LASTCODEELEMENTDISPID_V7+18) \
  [id(LASTCODEELEMENTDISPID_V7+19), HELP(vsextCMRemoveParam)] \
  HRESULT RemoveParameter(VARIANT Element); \
  [id(LASTCODEELEMENTDISPID_V7+20), propget, HELP(vsextCMCanOverride)] \
  HRESULT CanOverride([out, retval] VARIANT_BOOL *pCanOverride); \
  [id(LASTCODEELEMENTDISPID_V7+20), propput, HELP(vsextCMCanOverride)] \
  HRESULT CanOverride(VARIANT_BOOL CanOverride);

#define LASTCODEFUNCTIONDISPID_V7 LASTCODEELEMENTDISPID_V7+30

#define CODEVARIABLEBASEMETHODS_V7 \
  CODEELEMENTBASEMETHODS_V7 \
  [id(LASTCODEELEMENTDISPID_V7+1), propget, HELP(vsproParentProperty) NONBROWSABLE] \
  HRESULT Parent ([out, retval] IDispatch **ParentObject); \
  [id(LASTCODEELEMENTDISPID_V7+3), propget, HELP(vsextCodeModelInitialize)] \
  HRESULT InitExpression([out, retval] VARIANT *pExpr); \
  [id(LASTCODEELEMENTDISPID_V7+3), propput, HELP(vsextCodeModelInitialize)] \
  HRESULT InitExpression(VARIANT Expr); \
  [id(LASTCODEELEMENTDISPID_V7+4), propget, HELP(vsextCodeModelPrototype) NONBROWSABLE] \
  HRESULT Prototype([in, defaultvalue(0)] long Flags, [out, retval] BSTR *pVal); \
  [id(LASTCODEELEMENTDISPID_V7+5), propput, HELP(vsextCodeModelType) NONBROWSABLE] \
  HRESULT Type(CodeTypeRef *pCodeTypeRef); \
  [id(LASTCODEELEMENTDISPID_V7+5), propget, HELP(vsextCodeModelType) NONBROWSABLE] \
  HRESULT Type([out, retval] CodeTypeRef **pCodeTypeRef); \
  [id(LASTCODEELEMENTDISPID_V7+6), propput, HELP(vsextCodeModelAccess)] \
  HRESULT Access(enum vsCMAccess Access); \
  [id(LASTCODEELEMENTDISPID_V7+6), propget, HELP(vsextCodeModelAccess)] \
  HRESULT Access([out, retval] enum vsCMAccess *Access); \
  [id(LASTCODEELEMENTDISPID_V7+7), propget, HELP(vsextCodeModelIsConst)] \
  HRESULT IsConstant([out, retval] VARIANT_BOOL *pIsConstant); \
  [id(LASTCODEELEMENTDISPID_V7+7), propput, HELP(vsextCodeModelIsConst)] \
  HRESULT IsConstant(VARIANT_BOOL IsConstant); \
  [id(LASTCODEELEMENTDISPID_V7+8), propget, HELP(vsextCodeModelAttributes) NONBROWSABLE] \
  HRESULT Attributes([out, retval] CodeElements **ppMembers); \
  [id(LASTCODEELEMENTDISPID_V7+9), propget, HELP(vsDocCommentProperty) NONBROWSABLE] \
  HRESULT DocComment([out, retval] BSTR *pDocComment); \
  [id(LASTCODEELEMENTDISPID_V7+9), propput, HELP(vsDocCommentProperty) NONBROWSABLE] \
  HRESULT DocComment(BSTR DocComment); \
  [id(LASTCODEELEMENTDISPID_V7+10), propget, HELP(vsextCodeModelComment) NONBROWSABLE] \
  HRESULT Comment([out, retval] BSTR *pComment); \
  [id(LASTCODEELEMENTDISPID_V7+10), propput, HELP(vsextCodeModelComment) NONBROWSABLE] \
  HRESULT Comment(BSTR Comment); \
  ADDATTRIBUTE_NOFILENAME(LASTCODEELEMENTDISPID_V7+11) \
  [id(LASTCODEELEMENTDISPID_V7+12), propget, HELP(vsextCMIsShared)] \
  HRESULT IsShared([out, retval] VARIANT_BOOL *pShared); \
  [id(LASTCODEELEMENTDISPID_V7+12), propput, HELP(vsextCMIsShared)] \
  HRESULT IsShared(VARIANT_BOOL Shared);

#define LASTCODEVARIABLEDISPID_V7 LASTCODEELEMENTDISPID_V7+30

#define CODEPROPERTYBASEMETHODS_V7 \
  CODEELEMENTBASEMETHODS_V7 \
  [id(LASTCODEELEMENTDISPID_V7+1), propget, HELP(vsproParentProperty) NONBROWSABLE] \
  HRESULT Parent ([out, retval] CodeClass **ParentObject); \
  [id(LASTCODEELEMENTDISPID_V7+3), propget, HELP(vsextCodeModelPrototype) NONBROWSABLE] \
  HRESULT Prototype([in, defaultvalue(0)] long Flags, [out, retval] BSTR *pFullName); \
  [id(LASTCODEELEMENTDISPID_V7+4), propput, HELP(vsextCodeModelType) NONBROWSABLE] \
  HRESULT Type(CodeTypeRef *pCodeTypeRef); \
  [id(LASTCODEELEMENTDISPID_V7+4), propget, HELP(vsextCodeModelType) NONBROWSABLE] \
  HRESULT Type([out, retval] CodeTypeRef **pCodeTypeRef); \
  [id(LASTCODEELEMENTDISPID_V7+5), propget, HELP(vsextCodeModelGetter) NONBROWSABLE] \
  HRESULT Getter([out, retval] CodeFunction **ppCodeFunction); \
  [id(LASTCODEELEMENTDISPID_V7+5), propput, HELP(vsextCodeModelGetter) NONBROWSABLE] \
  HRESULT Getter(CodeFunction *pCodeFunction); \
  [id(LASTCODEELEMENTDISPID_V7+6), propget, HELP(vsextCodeModelSetter) NONBROWSABLE] \
  HRESULT Setter([out, retval] CodeFunction **ppCodeFunction); \
  [id(LASTCODEELEMENTDISPID_V7+6), propput, HELP(vsextCodeModelSetter) NONBROWSABLE] \
  HRESULT Setter(CodeFunction *pCodeFunction); \
  [id(LASTCODEELEMENTDISPID_V7+7), propput, HELP(vsextCodeModelAccess)] \
  HRESULT Access(enum vsCMAccess Access); \
  [id(LASTCODEELEMENTDISPID_V7+7), propget, HELP(vsextCodeModelAccess)] \
  HRESULT Access([out, retval] enum vsCMAccess *Access); \
  [id(LASTCODEELEMENTDISPID_V7+8), propget, HELP(vsextCodeModelAttributes) NONBROWSABLE] \
  HRESULT Attributes([out, retval] CodeElements **ppMembers); \
  [id(LASTCODEELEMENTDISPID_V7+9), propget, HELP(vsDocCommentProperty) NONBROWSABLE] \
  HRESULT DocComment([out, retval] BSTR *pDocComment); \
  [id(LASTCODEELEMENTDISPID_V7+9), propput, HELP(vsDocCommentProperty) NONBROWSABLE] \
  HRESULT DocComment(BSTR DocComment); \
  [id(LASTCODEELEMENTDISPID_V7+10), propget, HELP(vsextCodeModelComment) NONBROWSABLE] \
  HRESULT Comment([out, retval] BSTR *pComment); \
  [id(LASTCODEELEMENTDISPID_V7+10), propput, HELP(vsextCodeModelComment) NONBROWSABLE] \
  HRESULT Comment(BSTR Comment); \
  ADDATTRIBUTE_NOFILENAME(LASTCODEELEMENTDISPID_V7+12)
  

#define LASTCODEPROPERTYDISPID_V7 LASTCODEELEMENTDISPID_V7+30

#define CODEPARAMETERBASEMETHODS_V7 \
  CODEELEMENTBASEMETHODS_V7 \
  [id(LASTCODEELEMENTDISPID_V7+1), propget, HELP(vsproParentProperty) NONBROWSABLE] \
  HRESULT Parent([out, retval] CodeElement **ppCodeElement); \
  [id(LASTCODEELEMENTDISPID_V7+2), propput, HELP(vsextCodeModelType) NONBROWSABLE] \
  HRESULT Type(CodeTypeRef *Type); \
  [id(LASTCODEELEMENTDISPID_V7+2), propget, HELP(vsextCodeModelType) NONBROWSABLE] \
  HRESULT Type([out, retval] CodeTypeRef **pCodeTypeRef); \
  [id(LASTCODEELEMENTDISPID_V7+3), propget, HELP(vsextCodeModelAttributes) NONBROWSABLE] \
  HRESULT Attributes([out, retval] CodeElements **ppMembers); \
  [id(LASTCODEELEMENTDISPID_V7+4), propget, HELP(vsDocCommentProperty) NONBROWSABLE] \
  HRESULT DocComment([out, retval] BSTR *pDocComment); \
  [id(LASTCODEELEMENTDISPID_V7+4), propput, HELP(vsDocCommentProperty) NONBROWSABLE] \
  HRESULT DocComment(BSTR DocComment); \
  ADDATTRIBUTE_NOFILENAME(LASTCODEELEMENTDISPID_V7+5)

#define LASTCODEPARAMETERDISPID_V7 LASTCODEELEMENTDISPID_V7+30

#define CODEATTRIBUTEBASEMETHODS_V7 \
  CODEELEMENTBASEMETHODS_V7 \
  [id(LASTCODEELEMENTDISPID_V7+1), propget, HELP(vsproParentProperty) NONBROWSABLE] \
  HRESULT Parent([out, retval] IDispatch **ppParent); \
  [id(LASTCODEELEMENTDISPID_V7+2), propget, HELP(vsextCodeModelValue)] \
  HRESULT Value([out, retval] BSTR *pValue); \
  [id(LASTCODEELEMENTDISPID_V7+2), propput, HELP(vsproValueProperty)] \
  HRESULT Value(BSTR Value); \
  [id(LASTCODEELEMENTDISPID_V7+3), HELP(vsextRemoveMth)] \
  HRESULT Delete();
  
  

#define LASTCODEATTRIBUTERDISPID_V7 LASTCODEELEMENTDISPID_V7+30

#define CODEINTERFACEBASEMETHODS_V7 \
  CODETYPEBASEMETHODS_V7 \
  ADDFUNCTION_NOFILE(LASTCODETYPEDISPID_V7+1) \
  ADDPROPERTY_NOLOC(LASTCODETYPEDISPID_V7+2)

#define LASTCODEINTERFACEDISPID_V7 LASTCODETYPEDISPID_V7+30

#define CODESTRUCTBASEMETHODS_V7 \
  CODETYPEBASEMETHODS_V7 \
  [id(LASTCODETYPEDISPID_V7+1), propget, HELP(vsextCMImpledInterfaces) NONBROWSABLE] \
  HRESULT ImplementedInterfaces([out, retval] CodeElements **ppCodeElements); \
  [id(LASTCODETYPEDISPID_V7+2), propget, HELP(vsextCodeModelIsDerivedFrom)] \
  HRESULT IsAbstract([out, retval] VARIANT_BOOL *pIsAbstract); \
  [id(LASTCODETYPEDISPID_V7+2), propput, HELP(vsextCodeModelIsDerivedFrom)] \
  HRESULT IsAbstract(VARIANT_BOOL Abstract); \
  ADDIMPLEMENTEDINTERFACE(LASTCODETYPEDISPID_V7+3) \
  ADDFUNCTION_OPTLOC(LASTCODETYPEDISPID_V7+4) \
  ADDVARIABLE_OPTLOC(LASTCODETYPEDISPID_V7+5) \
  ADDPROPERTY_OPTLOC(LASTCODETYPEDISPID_V7+6) \
  ADDCLASS_NOFILENAME(LASTCODETYPEDISPID_V7+7) \
  ADDSTRUCT_NOFILENAME(LASTCODETYPEDISPID_V7+8) \
  ADDENUM_NOFILENAME(LASTCODETYPEDISPID_V7+9) \
  ADDDELEGATE_NOFILENAME(LASTCODETYPEDISPID_V7+10) \
  [id(LASTCODETYPEDISPID_V7+11), HELP(vsextCMRemoveIFace)] \
  HRESULT RemoveInterface(VARIANT Element);

#define LASTCODESTRUCTDISPID_V7 LASTCODETYPEDISPID_V7+30

#define CODEENUMBASEMETHODS_V7 \
  CODETYPEBASEMETHODS_V7 \
  ADDMEMBER(LASTCODETYPEDISPID_V7+1)

#define LASTCODEENUMDISPID_V7 LASTCODETYPEDISPID_V7+30

#define CODEDELEGATEBASEMETHODS_V7 \
  CODETYPEBASEMETHODS_V7 \
  [id(LASTCODETYPEDISPID_V7+1), propget, HELP(vsextCMBaseClass) NONBROWSABLE] \
  HRESULT BaseClass([out, retval] CodeClass **ppCodeClass); \
  [id(LASTCODETYPEDISPID_V7+2), propget, HELP(vsextCodeModelPrototype) NONBROWSABLE] \
  HRESULT Prototype([defaultvalue(0)] long Flags, [out, retval] BSTR *pPrototype); \
  [id(LASTCODETYPEDISPID_V7+3), propget, HELP(vsextCodeModelType) NONBROWSABLE] \
  HRESULT Type([out, retval] CodeTypeRef **pCodeTypeRef); \
  [id(LASTCODETYPEDISPID_V7+3), propput, HELP(vsextCodeModelType) NONBROWSABLE] \
  HRESULT Type(CodeTypeRef *Type); \
  [id(LASTCODETYPEDISPID_V7+4), propget, HELP(vsextCodeModelParamsProp) NONBROWSABLE] \
  HRESULT Parameters([out, retval] CodeElements **ppParameters); \
  ADDPARAMETER(LASTCODETYPEDISPID_V7+5) \
  [id(LASTCODETYPEDISPID_V7+6), HELP(vsextCMRemoveParam)] \
  HRESULT RemoveParameter(VARIANT Element);

#define LASTCODEDELEGATEDISPID_V7 LASTCODETYPEDISPID_V7+30

