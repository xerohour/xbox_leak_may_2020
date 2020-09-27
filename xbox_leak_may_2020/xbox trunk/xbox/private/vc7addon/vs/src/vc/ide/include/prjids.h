// this file contains the definitions for the project model IDs

// This ID range is reserved for vendor tools.  If it isn't big enough, then either make room for more 
// vendor tool IDs in the range or find a way to chunk in more reserved IDs in the project engine code.
#define VCVENDORTOOL_MIN_DISPID	 					5000
#define VCVENDORTOOL_MAX_DISPID						7499

// Project Engine Object
#define VCPROJECTENGINE_MIN_DISPID					200
#define VCPROJECTENGINE_MAX_DISPID					299

// this range is for stuff that appears in the build options page
#define BUILDOPTIONS_MIN_DISPID						200
#define BUILDOPTIONS_MAX_DISPID						205

#define PROJENGID_BuildLogging						200
#define PROJENGID_BuildTiming						201
#define PROJENGID_RedirectedOutputFile				202
#define PROJENGID_Platforms							210
#define PROJENGID_Projects							211
#define PROJENGID_StyleSheets						212
#define PROJENGID_CreateProject						213
#define PROJENGID_LoadProject						214
#define PROJENGID_RemoveProject						215
#define PROJENGID_Initialize						216
#define PROJENGID_LoadStyleSheet					217
#define PROJENGID_RemoveStyleSheet					218
#define PROJENGID_CreateStyleSheet					219
#define PROJENGID_PerformanceLogging				220
#define PROJENGID_LoadVBScriptProject				221
#define PROJENGID_IsSystemInclude					222
#define PROJENGID_CPPExtensions						223
#define PROJENGID_Events							224
#define PROJENGID_Evaluate							225

// Project Engine Events
#define ENGEVENTID_ItemAdded						275
#define ENGEVENTID_ItemRemoved						276
#define ENGEVENTID_ItemRenamed						277
#define ENGEVENTID_ItemMoved						278
#define ENGEVENTID_ItemPropertyChange				279
#define ENGEVENTID_SccEvent							280
#define ENGEVENTID_ReportError						281
#define ENGEVENTID_ProjectBuildStarted				282
#define ENGEVENTID_ProjectBuildFinished				283

// Build Engine Object
#define IVCBUILDENGINE_MIN_DISPID					300
#define IVCBUILDENGINE_MAX_DISPID					399

#define BLDENGID_SpawnerActive						300
#define BLDENGID_CanSpawnBuild						301
#define BLDENGID_UserStoppedBuild					302
#define BLDENGID_HaveLogging						303
#define BLDENGID_LogTrace							304
#define BLDENGID_CurrentLogSection					305
#define BLDENGID_FormCommandLine					306
#define BLDENGID_FormBatchFile						307
#define BLDENGID_FormTempFile						308
#define BLDENGID_FormXMLCommand						309					
#define BLDENGID_ExecuteCommandLines				310
#define BLDENGID_IncrementErrorCount				311
#define BLDENGID_IncrementWarningCount				312
#define BLDENGID_ErrorCount							313
#define BLDENGID_WarningCount						314
#define BLDENGID_IncrementProjectErrorCount			315
#define BLDENGID_IncrementProjectWarningCount		316
#define BLDENGID_AddMissingEnvVar					317
#define BLDENGID_ClearPending						318
#define BLDENGID_AddToOutputDirs					319
#define BLDENGID_StopBuild							320
#define BLDENGID_ReuseLogFile						321

// VC File Object, see also VCProjectItem
#define VCFILE_MIN_DISPID							400
#define VCFILE_MAX_DISPID							499

#define VCFILEID_FileConfigurations					400
#define VCFILEID_FullPath							401
#define VCFILEID_RelativePath						402
#define VCFILEID_Name								403
#define VCFILEID_Remove								404
#define VCFILEID_FileCodeModel						405
#define VCFILEID_DeploymentContent					406
#define VCFILEID_Extension							407
#define VCFILEID_Move								408
#define VCFILEID_CanMove							409

// File Configuration Object
#define VCFILECONFIGURATION_MIN_DISPID				500
#define VCFILECONFIGURATION_MAX_DISPID				599

#define VCFCFGID_Tool								500
#define VCFCFGID_ExcludedFromBuild					501
#define VCFCFGID_File								502
#define VCFCFGID_MatchName							510
#define VCFCFGID_Parent								511
#define VCFCFGID_Name								512
#define VCFCFGID_Evaluate							513
#define VCFCFGID_VCProjectEngine					514

// Style Sheet Object
#define VCSTYLESHEET_MIN_DISPID						600
#define VCSTYLESHEET_MAX_DISPID						649

#define VCSTYLEID_Name								600
#define VCSTYLEID_Platform							601
#define VCSTYLEID_PlatformName						602
#define VCSTYLEID_MatchName							603
#define VCSTYLEID_StyleSheetName					604
#define VCSTYLEID_StyleSheetFile					605
#define VCSTYLEID_IsDirty							606
#define VCSTYLEID_Tools								607
#define VCSTYLEID_FileTools							608
#define VCSTYLEID_ToolSet							609
#define VCSTYLEID_StyleSheets						610

// Generic Item Object
#define VCPROJECTITEM_MIN_DISPID					675
#define VCPROJECTITEM_MAX_DISPID					699

#define VCITMID_MatchName							675
#define VCITMID_Project								676
#define VCITMID_Parent								677
#define VCITMID_ItemName							678
#define VCITMID_Kind								679
#define VCITMID_VCProjectEngine						680

// Configuration Object
#define VCCONFIGURATION_MIN_DISPID					700
#define VCCONFIGURATION_MAX_DISPID					799

#define VCCFGID_ConfigurationName					700
#define VCCFGID_Project								701
#define VCCFGID_OutputDirectory						702
#define VCCFGID_IntermediateDirectory				703
#define VCCFGID_DeleteExtensionsOnClean				704
#define VCCFGID_Name								705
#define VCCFGID_Platform							706
#define VCCFGID_Tools								707
#define VCCFGID_FileTools							708
#define VCCFGID_Evaluate							709
#define VCCFGID_VCProjectEngine						710

#define VCCFGID_PrimaryOutput						721
#define VCCFGID_ImportLibrary						722
#define VCCFGID_ProgramDatabase						723
#define VCCFGID_Delete								724
#define VCCFGID_Build								725
#define VCCFGID_Rebuild								726
#define VCCFGID_Clean								727
#define VCCFGID_DebugSettings						728
#define VCCFGID_MatchName							729
#define VCCFGID_GeneralSettings						730
#define VCCFGID_UpToDate							731
#define VCCFGID_ConfigurationType					732
#define VCCFGID_CopyTo								733
#define VCCFGID_AppliedStyleSheets					734
#define VCCFGID_BuildBrowserInformation				735
#define VCCFGID_UseOfMFC							736
#define VCCFGID_UseOfATL							737
#define VCCFGID_ATLMinimizesCRunTimeLibraryUsage	738
#define VCCFGID_CharacterSet						739
#define VCCFGID_ManagedExtensions					740	
#define VCCFGID_WholeProgramOptimization			741
#define VCCFGID_RegisterOutput						742
#define VCCFGID_StyleSheets							743
#define VCCFGID_IntrinsicAppliedStyleSheets			744		// virtual prop: list of intrinsic applied style sheets present
#define VCCFGID_AllOutputsDirty						745		// virtual prop; have we already dirtied the command line outputs for the build?
#define VCCFGID_PrimaryOutputDirty					746		// virtual prop; have we already dirtied the primary output (or def extension)?
#define VCCFGID_AnyOutputDirty						747		// virtual prop; has any tool at any level dirtied its output since the last build?
#define VCCFGID_AnyToolDirty						748		// virtual prop; has any tool at any level been dirtied since the last build?
#define VCCFGID_DirtyDuringBuild					749		// virtual prop; are we dirtying any tool at any level during a build?

// Project Object
#define VCPROJECT_MIN_DISPID						800
#define VCPROJECT_MAX_DISPID						899

#define VCPROJID_ProjectType						801
#define VCPROJID_ProjectDirectory					802
#define VCPROJID_AddFile							803
#define VCPROJID_CanAddFile							804
#define VCPROJID_AddConfiguration					805
#define VCPROJID_RemoveFile							806
#define VCPROJID_Files								807
#define VCPROJID_Configurations						808
#define VCPROJID_Save								809
#define VCPROJID_MoveInto							810
#define VCPROJID_AddPlatform						811
#define VCPROJID_RemovePlatform						812
#define VCPROJID_RemoveFilter						813
#define VCPROJID_AddFilter							814
#define VCPROJID_CanAddFilter						815
#define VCPROJID_ProjectFile						816
#define VCPROJID_DeleteConfiguration				817
#define VCPROJID_Filters							818
#define VCPROJID_IsDirty							819
#define VCPROJID_SaveProjectOptions					820
#define VCPROJID_LoadProjectOptions					821
#define VCPROJID_Platforms							822
#define VCPROJID_RemoveConfiguration				823
#define VCPROJID_FileFormat							824
#define VCPROJID_Items								825
#define VCPROJID_AddWebRef							826
#define VCPROJID_FileEncoding						827
#define VCPROJID_VCProjectEngine					828

#define VCPROJID_Name								840

#define VCPROJID_SccProjectName						850
#define VCPROJID_SccAuxPath							851
#define VCPROJID_SccLocalPath						852
#define VCPROJID_SccProvider						853

#define VCPROJID_Keyword							855

#define VCPROJID_Object								864
#define VCEXEPRJID_Object							865

#define VCPROJID_OwnerKey							866
#define VCPROJID_ProjectGUID							867

// C/C++ compiler (general)
// NOTE: If the range of dispids goes beyond VCCLCOMPILERTOOL_MAX_DISPID, keep it updated.
#define VCCLCOMPILERTOOL_MIN_DISPID					2000
#define VCCLCOMPILERTOOL_MAX_DISPID					2999

// C/C++ compiler, General
#define CLGENERAL_MIN_DISPID						2000	// MIN/MAX used for property page ranges
#define CLGENERAL_MAX_DISPID						2029

#define VCCLID_ToolName								2000
#define VCCLID_AdditionalOptions					2001
#define VCCLID_VCProjectEngine						2002

#define VCCLID_AdditionalIncludeDirectories			2010
#define VCCLID_AdditionalUsingDirectories			2011
#define VCCLID_DebugInformationFormat				2012
#define VCCLID_CompileAsManaged						2013
#define VCCLID_SuppressStartupBanner				2014
#define VCCLID_WarningLevel							2015
#define VCCLID_Detect64BitPortabilityProblems		2016
#define VCCLID_WarnAsError							2017
#define VCCLID_CompileOnly							2018

// C/C++ compiler, Optimization
#define CLOPTIMIZATION_MIN_DISPID					2030	// MIN/MAX used for property page ranges
#define CLOPTIMIZATION_MAX_DISPID					2059

#define VCCLID_Optimization							2030
#define VCCLID_GlobalOptimizations					2031
#define VCCLID_InlineFunctionExpansion				2032
#define VCCLID_EnableIntrinsicFunctions				2033
#define VCCLID_ImproveFloatingPointConsistency		2034
#define VCCLID_FavorSizeOrSpeed						2035
#define VCCLID_OmitFramePointers					2036
#define VCCLID_EnableFiberSafeOptimizations			2037
#define VCCLID_WholeProgramOptimization				2038	// not on the page; set via general cfg page
#define VCCLID_OptimizeForProcessor					2039
#define VCCLID_OptimizeForWindowsApplication		2040

// C/C++ compiler, Preprocessor
#define CLPREPROCESS_MIN_DISPID						2060	// MIN/MAX used for property page ranges
#define CLPREPROCESS_MAX_DISPID						2079

#define VCCLID_PreprocessorDefinitions				2060
#define VCCLID_IgnoreStandardIncludePath			2061
#define VCCLID_GeneratePreprocessedFile				2062
#define VCCLID_KeepComments							2063

// C/C++ Compiler, Advanced
#define CLADV_MIN_DISPID							2080	// MIN/MAX used for property page ranges
#define CLADV_MAX_DISPID							2109

#define VCCLID_CallingConvention					2080
#define VCCLID_CompileFileAs						2081
#define VCCLID_CompileAs							2082
#define VCCLID_DisableSpecificWarnings				2083
#define VCCLID_ForcedIncludeFiles					2084
#define VCCLID_ForcedUsingFiles						2085
#define VCCLID_ShowIncludes							2086
#define VCCLID_UndefinePreprocessorDefinitions		2087
#define VCCLID_UndefineAllPreprocessorDefinitions	2088

// C/C++ compiler, Code Generation
#define CLCODEGEN_MIN_DISPID						2120	// MIN/MAX used for property page ranges
#define CLCODEGEN_MAX_DISPID						2139

#define VCCLID_StringPooling						2120
#define VCCLID_MinimalRebuild						2121
#define VCCLID_ExceptionHandling					2122
#define VCCLID_SmallerTypeCheck						2123
#define VCCLID_BasicRuntimeChecks					2124
#define VCCLID_RuntimeLibrary						2125
#define VCCLID_StructMemberAlignment				2126
#define VCCLID_BufferSecurityCheck					2127
#define VCCLID_EnableFunctionLevelLinking			2128

// C/C++ compiler, Language
#define CLLANG_MIN_DISPID							2140	// MIN/MAX used for property page ranges
#define CLLANG_MAX_DISPID							2159

#define VCCLID_DisableLanguageExtensions			2140
#define VCCLID_DefaultCharIsUnsigned				2141
#define VCCLID_TreatWChar_tAsBuiltInType			2142
#define VCCLID_ForceConformanceInForLoopScope		2143
#define VCCLID_RuntimeTypeInfo						2144

// C/C++ compiler, Precompiled Headers
#define CLPCH_MIN_DISPID							2160	// MIN/MAX used for property page ranges
#define CLPCH_MAX_DISPID							2169

#define VCCLID_UsePrecompiledHeader					2161
#define VCCLID_PrecompiledHeaderThrough				2162
#define VCCLID_PrecompiledHeaderFile				2163

// C/C++ compiler, Output Files
#define CLOUTPUT_MIN_DISPID							2170	// MIN/MAX used for property page ranges
#define CLOUTPUT_MAX_DISPID							2179

#define VCCLID_ExpandAttributedSource				2170
#define VCCLID_AssemblerOutput						2171
#define VCCLID_AssemblerListingLocation				2172
#define VCCLID_ObjectFile							2173
#define VCCLID_ProgramDataBaseFileName				2174

// C/C++ compiler (general), Browse info helpers
#define CLBROWSE_MIN_DISPID							2180	// MIN/MAX used for property page ranges
#define CLBROWSE_MAX_DISPID							2189

#define VCCLID_BrowseInformation					2181
#define VCCLID_BrowseInformationFile				2182

// C/C++ compiler (general), Automation properties
#define VCCLID_ToolPath								2220
#define VCCLID_PropertyOption						2221
#define VCCLID_FullIncludePath						2222
#define VCCLID_CmdLineOptionsDirty					2223	// virtual prop; have we already dirtied the command line for this tool?
#define VCCLID_OutputsDirty							2224	// virtual prop; have we already dirtied the outputs for this tool?
#define VCCLID_ResolvedPdbName						2225	// virtual prop; keep this in sync with pdb file name


// Linker Tool
// NOTE: If the range of dispids goes beyond VCLINKERTOOL_MAX_DISPID, keep it updated.
#define VCLINKERTOOL_MIN_DISPID						3000
#define VCLINKERTOOL_MAX_DISPID						3299

// Linker Tool, General
#define LINKGENERAL_MIN_DISPID						3000	// MIN/MAX used for property page ranges
#define LINKGENERAL_MAX_DISPID						3019

#define VCLINKID_ToolName							3000
#define VCLINKID_AdditionalOptions					3001
#define VCLINKID_OutputFile							3002
#define VCLINKID_ShowProgress						3003
#define VCLINKID_Version							3004
#define VCLINKID_LinkIncremental					3005
#define VCLINKID_SuppressStartupBanner				3006
#define VCLINKID_IgnoreImportLibrary				3007
#define VCLINKID_RegisterOutput						3008
#define VCLINKID_AdditionalLibraryDirectories		3009

// Linker Tool, Input
#define LINKINPUT_MIN_DISPID						3020	// MIN/MAX used for property page ranges
#define LINKINPUT_MAX_DISPID						3039

#define VCLINKID_AdditionalDependencies				3020
#define VCLINKID_IgnoreAllDefaultLibraries			3021
#define VCLINKID_IgnoreDefaultLibraryNames			3022
#define VCLINKID_ModuleDefinitionFile				3023
#define VCLINKID_AddModuleNamesToAssembly			3024
#define VCLINKID_EmbedManagedResourceFile			3025
#define VCLINKID_ForceSymbolReferences				3026
#define VCLINKID_DelayLoadDLLs						3127
#define VCLINKID_LinkDLL							3028
#define VCLINKID_DependentInputs					3029	// virtual prop; additional inputs picked up via project dependencies

// Linker Tool, MIDL
#define LINKMIDL_MIN_DISPID							3040	// MIN/MAX used for property page ranges
#define LINKMIDL_MAX_DISPID							3049

#define VCLINKID_MidlCommandFile					3040
#define VCLINKID_IgnoreEmbeddedIDL					3041
#define VCLINKID_MergedIDLBaseFileName				3042
#define VCLINKID_TypeLibraryFile					3043
#define VCLINKID_TypeLibraryResourceID				3044

// Linker Tool, Debug
#define LINKDEBUG_MIN_DISPID						3050	// MIN/MAX used for property page ranges
#define LINKDEBUG_MAX_DISPID						3059

#define VCLINKID_GenerateDebugInformation			3050
#define VCLINKID_ProgramDatabaseFile				3051
#define VCLINKID_StripPrivateSymbols				3052
#define VCLINKID_GenerateMapFile					3053
#define VCLINKID_MapFileName						3054
#define VCLINKID_MapExports							3055
#define VCLINKID_MapLines							3056

// Linker Tool, System
#define LINKSYSTEM_MIN_DISPID						3065	// MIN/MAX used for property page ranges
#define LINKSYSTEM_MAX_DISPID						3079

#define VCLINKID_SubSystem							3065
#define VCLINKID_HeapReserveSize					3066
#define VCLINKID_HeapCommitSize						3067
#define VCLINKID_StackReserveSize					3068
#define VCLINKID_StackCommitSize					3069
#define VCLINKID_LargeAddressAware					3070
#define VCLINKID_TerminalServerAware				3071
#define VCLINKID_SwapRunFromCD						3072
#define VCLINKID_SwapRunFromNet						3073

// Linker Tool, Optimization
#define LINKOPTIMIZATION_MIN_DISPID					3080	// MIN/MAX used for property page ranges
#define LINKOPTIMIZATION_MAX_DISPID					3099

#define VCLINKID_OptimizeReferences					3080
#define VCLINKID_EnableCOMDATFolding				3081
#define VCLINKID_OptimizeForWindows98				3082
#define VCLINKID_FunctionOrder						3083
#define VCLINKID_LinkTimeCodeGeneration				3084	// not on the page; set via cfg general page

// Linker Tool, Advanced
#define LINKADVANCED_MIN_DISPID						3100	// MIN/MAX used for property page ranges
#define LINKADVANCED_MAX_DISPID						3129

#define VCLINKID_EntryPointSymbol					3100
#define VCLINKID_ResourceOnlyDLL					3101
#define VCLINKID_SetChecksum						3102
#define VCLINKID_BaseAddress						3103
#define VCLINKID_TurnOffAssemblyGeneration			3104
#define VCLINKID_SupportUnloadOfDelayLoadedDLL		3105
#define VCLINKID_ImportLibrary						3106
#define VCLINKID_MergeSections						3107
#define VCLINKID_TargetMachine						3108

// Linker Tool, Automation properties
#define VCLINKID_ToolPath							3120
#define VCLINKID_PropertyOption						3121
#define VCLINKID_VCProjectEngine					3122
#define VCLINKID_CmdLineOptionsDirty				3123	// virtual prop; have we already dirtied the command line for this tool?
#define VCLINKID_OutputsDirty						3124	// virtual prop; have we already dirtied the outputs for this tool?
#define VCLINKID_LinkerCommandLineGenerated			3125	// virtual prop; has this linker command line ever been generated (this IDE session?)

// Library Tool
// NOTE: If the range of dispids goes beyond VCLIBRARIANTOOL_MAX_DISPID, keep it updated.
#define VCLIBRARIANTOOL_MIN_DISPID					3300
#define VCLIBRARIANTOOL_MAX_DISPID					3499

// Library Tool, General
#define LIBGENERAL_MIN_DISPID						3300	// MIN/MAX used for property page ranges
#define LIBGENERAL_MAX_DISPID						3319

#define VCLIBID_ToolName							3300
#define VCLIBID_AdditionalOptions					3301
#define VCLIBID_OutputFile							3302
#define VCLIBID_AdditionalDependencies				3303
#define VCLIBID_AdditionalLibraryDirectories		3304
#define VCLIBID_SuppressStartupBanner				3305
#define VCLIBID_ModuleDefinitionFile				3306
#define VCLIBID_IgnoreAllDefaultLibraries			3307
#define VCLIBID_IgnoreDefaultLibraryNames			3308
#define VCLIBID_ExportNamedFunctions				3309
#define VCLIBID_ForceSymbolReferences				3310
#define VCLIBID_DependentInputs						3311	// virtual prop; additional inputs picked up via project dependencies

// Library Tool, Automation properties
#define VCLIBID_ToolPath							3340
#define VCLIBID_PropertyOption						3341
#define VCLIBID_VCProjectEngine						3342
#define VCLIBID_CmdLineOptionsDirty					3343	// virtual prop; have we already dirtied the command line for this tool?
#define VCLIBID_OutputsDirty						3344	// virtual prop; have we already dirtied the outputs for this tool?
#define VCLIBID_LibCommandLineGenerated				3345	// virtual prop; has this librarian command line ever been generated (this IDE session?)

// MIDL Tool
// NOTE: If the range of dispids goes beyond VCMIDLTOOL_MAX_DISPID, keep it updated.
#define VCMIDLTOOL_MIN_DISPID						3500
#define VCMIDLTOOL_MAX_DISPID						3899

// MIDL Tool, General
#define MIDLGENERAL_MIN_DISPID						3500	// MIN/MAX used for property page ranges
#define MIDLGENERAL_MAX_DISPID						3529

#define VCMIDLID_ToolName							3500
#define VCMIDLID_AdditionalOptions					3501
#define VCMIDLID_PreprocessorDefinitions			3502
#define VCMIDLID_AdditionalIncludeDirectories		3503
#define VCMIDLID_IgnoreStandardIncludePath			3504
#define VCMIDLID_MkTypLibCompatible					3505
#define VCMIDLID_WarningLevel						3506
#define VCMIDLID_WarnAsError						3507
#define VCMIDLID_SuppressStartupBanner				3508
#define VCMIDLID_DefaultCharType					3509
#define VCMIDLID_TargetEnvironment 					3510
#define VCMIDLID_GenerateStublessProxies			3511

// MIDL Tool, Output 
#define MIDLOUTPUT_MIN_DISPID						3530	// MIN/MAX used for property page ranges
#define MIDLOUTPUT_MAX_DISPID						3549

#define VCMIDLID_OutputDirectory					3530
#define VCMIDLID_HeaderFileName						3531
#define VCMIDLID_DLLDataFileName					3532
#define VCMIDLID_InterfaceIdentifierFileName		3533
#define VCMIDLID_ProxyFileName						3534
#define VCMIDLID_GenerateTypeLibrary				3535
#define VCMIDLID_TypeLibraryName					3536

// MIDL Tool, Advanced
#define MIDLADVANCED_MIN_DISPID						3550	// MIN/MAX used for property page ranges
#define MIDLADVANCED_MAX_DISPID						3589

#define VCMIDLID_EnableErrorChecks					3550
#define VCMIDLID_ErrorCheckAllocations				3551
#define VCMIDLID_ErrorCheckBounds					3552
#define VCMIDLID_ErrorCheckEnumRange				3553
#define VCMIDLID_ErrorCheckRefPointers				3554
#define VCMIDLID_ErrorCheckStubData					3555
#define VCMIDLID_ValidateParameters					3556
#define VCMIDLID_StructMemberAlignment				3557
#define VCMIDLID_RedirectOutputAndErrors			3558
#define VCMIDLID_CPreprocessOptions					3559
#define VCMIDLID_UndefinePreprocessorDefinitions	3560

// MIDL Tool, Automation properties
#define VCMIDLID_ToolPath							3570
#define VCMIDLID_PropertyOption						3571
#define VCMIDLID_FullIncludePath					3572
#define VCMIDLID_VCProjectEngine					3573
#define VCMIDLID_CmdLineOptionsDirty				3574	// virtual prop; have we already dirtied the command line for this tool?
#define VCMIDLID_OutputsDirty						3575	// virtual prop; have we already dirtied the outputs for this tool?

// Resource Compiler Tool
// NOTE: If the range of dispids goes beyond VCRCTOOL_MAX_DISPID, keep it updated.
#define VCRCTOOL_MIN_DISPID							3900
#define VCRCTOOL_MAX_DISPID							3999

// Resource Compiler Tool, General
#define VCRCID_ToolName								3900
#define VCRCID_AdditionalOptions					3901
#define VCRCID_PreprocessorDefinitions				3902
#define VCRCID_Culture								3903
#define VCRCID_AdditionalIncludeDirectories			3904
#define VCRCID_IgnoreStandardIncludePath			3905
#define VCRCID_ShowProgress							3906
#define VCRCID_ResourceOutputFileName				3907

// Resource Compiler Tool, Automation
#define VCRCID_ToolPath								3920
#define VCRCID_PropertyOption						3921
#define VCRCID_FullIncludePath						3922
#define VCRCID_VCProjectEngine						3923
#define VCRCID_CmdLineOptionsDirty					3924	// virtual prop; have we already dirtied the command line for this tool?
#define VCRCID_OutputsDirty							3925	// virtual prop; have we already dirtied the outputs for this tool?

// BSCMake Tool
// NOTE: If the range of dispids goes beyond VCBSCMAKETOOL_MAX_DISPID, keep it updated.
#define VCBSCMAKETOOL_MIN_DISPID					4000
#define VCBSCMAKETOOL_MAX_DISPID					4099

// BSCMake Tool, General
#define VCBSCID_ToolName							4000
#define VCBSCID_AdditionalOptions					4001
#define VCBSCID_SuppressStartupBanner				4002
#define VCBSCID_OutputFile							4003
#define VCBSCID_RunBSCMakeTool						4004	// not in UI

// BSCMake Tool, Automation
#define VCBSCID_ToolPath							4020
#define VCBSCID_PropertyOption						4021
#define VCBSCID_VCProjectEngine						4022
#define VCBSCID_CmdLineOptionsDirty					4023	// virtual prop; have we already dirtied the command line for this tool?
#define VCBSCID_OutputsDirty						4024	// virtual prop; have we already dirtied the outputs for this tool?

// Custom Build Tool
// NOTE: If the range of dispids goes beyond VCCUSTOMBUILDTOOL_MAX_DISPID, keep it updated.
// Note 2: Assuming that the range of IDs we need to watch ranges between that for the command line and that for the inputs.
// If that changes, update the implementation code in CreateInstance.
#define VCCUSTOMBUILDTOOL_MIN_DISPID				4100
#define VCCUSTOMBUILDTOOL_MAX_DISPID				4199

// these are for the project configuration level
#define VCCUSTID_ToolName							4101
#define VCCUSTID_CommandLine						4102
#define VCCUSTID_Description						4103
#define VCCUSTID_Outputs							4104
#define VCCUSTID_AdditionalDependencies				4105
#define VCCUSTID_ToolPath							4106
#define VCCUSTID_CmdLineOptionsDirty				4107	// virtual prop; have we already dirtied the command line for this tool?
#define VCCUSTID_OutputsDirty						4108	// virtual prop; have we already dirtied the outputs for this tool?
#define VCCUSTID_FileLevelOffset					  30	// storage offset for file level props; no helpstring
#define VCCUSTID_VCProjectEngine					4109

// these are for the file configuration level
// NOTE: these are 'phantom' IDs, i.e., they exist for storage *only*.  
// They MUST remain at constant offset from the project configuration level IDs
// Use the IDs for the project configuration level for help, etc.

// this range is for checking if a dispid is a file level custom build step
// ONLY (kludge so that we can get info on these dispids out of the typelib)
// if the layout of the custom build dispids changes THESE MUST ALSO CHANGE
#define VCFILECUSTOMBUILDTOOL_MIN_DISPID			VCCUSTID_ToolName + VCCUSTID_FileLevelOffset
#define VCFILECUSTOMBUILDTOOL_MAX_DISPID			VCCUSTID_OutputsDirty + VCCUSTID_FileLevelOffset

#define VCFCSTID_ToolName							VCCUSTID_ToolName + VCCUSTID_FileLevelOffset
#define VCFCSTID_CommandLine						VCCUSTID_CommandLine + VCCUSTID_FileLevelOffset
#define VCFCSTID_Description						VCCUSTID_Description + VCCUSTID_FileLevelOffset
#define VCFCSTID_Outputs							VCCUSTID_Outputs + VCCUSTID_FileLevelOffset
#define VCFCSTID_AdditionalDependencies				VCCUSTID_AdditionalDependencies + VCCUSTID_FileLevelOffset
#define VCFCSTID_ToolPath							VCCUSTID_ToolPath + VCCUSTID_FileLevelOffset
#define VCFCSTID_CmdLineOptionsDirty				VCCUSTID_CmdLineOptionsDirty + VCCUSTID_FileLevelOffset
#define VCFCSTID_OutputsDirty						VCCUSTID_OutputsDirty + VCCUSTID_FileLevelOffset

// Note about IDs: an implementation assumption is being made that the low ID to watch is that for the command line
// and the high ID to watch is that for excluded from build.  If this changes, you will need to modify the base
// template implementation for the tool so that dirties happen correctly

// Pre-Build Event Tool
// NOTE: If the range of dispids goes beyond VCPREBLDEVENTTOOL_MAX_DISPID, keep it updated.
#define VCPREBLDEVENTTOOL_MIN_DISPID				4200
#define VCPREBLDEVENTTOOL_MAX_DISPID				4249

// Pre-Build Event Tool
#define PREBLDID_ToolName							4200
#define PREBLDID_CommandLine						4201
#define PREBLDID_Description						4202
#define PREBLDID_ExcludedFromBuild					4203
#define PREBLDID_ToolPath							4204
#define PREBLDID_VCProjectEngine					4205
#define PREBLDID_CmdLineOptionsDirty				4206	// virtual prop; have we already dirtied the command line for this tool?

// Pre-Link Event Tool
// NOTE: If the range of dispids goes beyond VCPRELINKEVENTTOOL_MAX_DISPID, keep it updated.
#define VCPRELINKEVENTTOOL_MIN_DISPID				4250
#define VCPRELINKEVENTTOOL_MAX_DISPID				4299

// Pre-Link Event Tool
#define PRELINKID_ToolName							4250
#define PRELINKID_CommandLine						4251
#define PRELINKID_Description						4252
#define PRELINKID_ExcludedFromBuild					4253
#define PRELINKID_ToolPath							4254
#define PRELINKID_VCProjectEngine					4255
#define PRELINKID_CmdLineOptionsDirty				4256	// virtual prop; have we already dirtied the command line for this tool?

// Post-Build Event Tool
// NOTE: If the range of dispids goes beyond VCPOSTBLDEVENTTOOL_MAX_DISPID, keep it updated.
#define VCPOSTBLDEVENTTOOL_MIN_DISPID				4300
#define VCPOSTBLDEVENTTOOL_MAX_DISPID				4349

// Pre-Build Event Tool
#define POSTBLDID_ToolName							4300
#define POSTBLDID_CommandLine						4301
#define POSTBLDID_Description						4302
#define POSTBLDID_ExcludedFromBuild					4303
#define POSTBLDID_ToolPath							4304
#define POSTBLDID_VCProjectEngine					4305
#define POSTBLDID_CmdLineOptionsDirty				4306	// virtual prop; have we already dirtied the command line for this tool?

// NMake Tool
// NOTE: If the range of dispids goes beyond VCNMAKETOOL_MAX_DISPID, keep it updated.
#define VCNMAKETOOL_MIN_DISPID						4400
#define VCNMAKETOOL_MAX_DISPID						4499

// these are for the project configuration level
#define VCNMAKEID_ToolName							4401
#define VCNMAKEID_BuildCommandLine					4402
#define VCNMAKEID_ReBuildCommandLine				4403
#define VCNMAKEID_CleanCommandLine					4404
#define VCNMAKEID_Output							4405
#define VCNMAKEID_ToolPath							4406
#define VCNMAKEID_VCProjectEngine					4407
#define VCNMAKEID_CmdLineOptionsDirty				4408	// virtual prop; have we already dirtied the command line for this tool?
#define VCNMAKEID_OutputsDirty						4409	// virtual prop; have we already dirtied the outputs for this tool?


// Web Service Proxy Generator Tool
// NOTE: If the range of dispids goes beyond VCWEBTOOL_MAX_DISPID, keep it updated.
#define VCWEBTOOL_MIN_DISPID						4500
#define VCWEBTOOL_MAX_DISPID						4599	
#define VCWEBID_ToolName							4501
#define VCWEBID_Output								4502
#define VCWEBID_SuppressStartupBanner				4503
#define VCWEBID_GeneratedProxyLanguage				4504
#define VCWEBID_AdditionalOptions					4505
#define VCWEBID_URL									4506
#define VCWEBID_ToolPath							4507
#define VCWEBID_PropertyOption						4508
#define VCWEBID_VCProjectEngine						4509
#define VCWEBID_CmdLineOptionsDirty					4517	// virtual prop; have we already dirtied the command line for this tool?
#define VCWEBID_OutputsDirty						4518	// virtual prop; have we already dirtied the outputs for this tool?

// Web Deployment Tool
// NOTE: If the range of dispids goes beyond VCDPLYTOOL_MAX_DISPID, keep it updated.
#define VCDPLYTOOL_MIN_DISPID						4600
#define VCDPLYTOOL_MAX_DISPID						4699
#define VCDPLYID_ToolName							4600	
#define VCDPLYID_ExcludedFromBuild					4601	
#define VCDPLYID_RelativePath						4602	
#define VCDPLYID_AdditionalFiles					4603	
#define VCDPLYID_UnloadBeforeCopy					4604	
#define VCDPLYID_RegisterOutput						4605	
#define VCDPLYID_VirtualDirectoryName				4606	
#define VCDPLYID_ApplicationMappings				4607	
#define VCDPLYID_ApplicationProtection				4608	
#define VCDPLYID_ToolPath							4609	
#define VCDPLYID_PropertyOption						4610	
#define VCDPLYID_VCProjectEngine					4611	
#define VCDPLYID_CmdLineOptionsDirty				4612	// virtual prop; have we already dirtied the command line for this tool?	

// Xbox Deployment Tool
// NOTE: If the range of dispids goes beyond XBOXVCDPLYTOOL_MAX_DISPID, keep it updated.
#define XBOXVCDPLYTOOL_MIN_DISPID					4700
#define XBOXVCDPLYTOOL_MAX_DISPID					4799
#define XBOXDPLYID_ToolName							4700	
#define XBOXDPLYID_ExcludedFromBuild				4701	
#define XBOXDPLYID_RemotePath						4702	
#define XBOXDPLYID_AdditionalFiles					4703
// Gap...	
#define XBOXDPLYID_ToolPath							4709	
#define XBOXDPLYID_PropertyOption					4710	
#define XBOXDPLYID_VCProjectEngine					4711	
#define XBOXDPLYID_CmdLineOptionsDirty				4712	// virtual prop; have we already dirtied the command line for this tool?	

// Xbox Image Tool
// NOTE: If the range of dispids goes beyond XBOXVCDPLYTOOL_MAX_DISPID, keep it updated.
#define XBOXIMAGETOOL_MIN_DISPID					4800
#define XBOXIMAGETOOL_MAX_DISPID					4899

// Xbox Image Tool, General
#define XBOXIMAGEGENERAL_MIN_DISPID					4800	// MIN/MAX used for property page ranges
#define XBOXIMAGEGENERAL_MAX_DISPID					4824
#define XBOXIMAGEID_ToolName						4800	
#define XBOXIMAGEID_AdditionalOptions				4801	
#define XBOXIMAGEID_FileName						4802	
#define XBOXIMAGEID_XBEFlags						4803	
#define XBOXIMAGEID_StackSize						4804	
#define XBOXIMAGEID_IncludeDebugInfo				4805	
#define XBOXIMAGEID_LimitAvailableMemoryTo64MB		4806	

// Xbox Image Tool, Certificate
#define XBOXIMAGECERTIFICATE_MIN_DISPID				4825	// MIN/MAX used for property page ranges
#define XBOXIMAGECERTIFICATE_MAX_DISPID				4849
#define XBOXIMAGEID_TitleID							4825	
#define XBOXIMAGEID_XboxGameRegion					4826	
#define XBOXIMAGEID_XboxGameRating					4827	

// Xbox Image Tool, Title Info
#define XBOXIMAGETITLEINFO_MIN_DISPID				4850	// MIN/MAX used for property page ranges
#define XBOXIMAGETITLEINFO_MAX_DISPID				4874
#define XBOXIMAGEID_TitleInfo						4850	
#define XBOXIMAGEID_TitleName						4851	
#define XBOXIMAGEID_TitleImage						4852	
#define XBOXIMAGEID_SaveGameImage					4853	

// Xbox Image Tool, Automation properties
#define XBOXIMAGEID_ToolPath						4875
#define XBOXIMAGEID_PropertyOption					4899
#define XBOXIMAGEID_VCProjectEngine					4875
#define XBOXIMAGEID_CmdLineOptionsDirty				4876	// virtual prop; have we already dirtied the command line for this tool?
#define XBOXIMAGEID_OutputsDirty					4877	// virtual prop; have we already dirtied the outputs for this tool?
#define XBOXIMAGEID_ImageCommandLineGenerated		4878	// virtual prop; has this image command line ever been generated (this IDE session?)

// VC Build Platform Properties, File Node
#define VCFILENODID_Name							7510	
#define VCFILENODID_FullPath						7511
#define VCFILENODID_RelativePath					7512
#define VCFILENODID_DeploymentContent				7513
#define VCFILENODID_Object							7514
#define VCFILENODID_Extender						7515
#define VCFILENODID_ExtenderNames					7516
#define VCFILENODID_ExtenderCATID					7517

// VC Build Platform Properties, Folder Node
#define VCFOLDNODID_Name							7530	
#define VCFOLDNODID_Filter							7531
#define VCFOLDNODID_Object							7532
#define VCFOLDNODID_ParseFiles						7533
#define VCFOLDNODID_SourceControlFiles				7534
#define VCFOLDNODID_Extender						7535
#define VCFOLDNODID_ExtenderNames					7536
#define VCFOLDNODID_ExtenderCATID					7537
#define IDS_ADVANCEDCAT								7538

// VC Build Platform Properties, Folder Node
#define VCPRJNODID_Name								7550	
#define VCPRJNODID_ProjectFile						7551
#define VCPRJNODID_ProjectDependencies				7552
#define VCPRJNODID_Object							7553
#define VCPRJNODID_Extender							7554
#define VCPRJNODID_ExtenderNames					7555
#define VCPRJNODID_ExtenderCATID					7556

// DIANEME_TODO: dead ID placeholders -- starts at 8000

//VCPB Miscellaneous Tools 
//NOTE: if the range of dispids goes beyond VCPBINTERFACES_MAX_DISPID, keep it updated
#define VCPBINTERFACES_MIN_DISPID					12000
#define VCPBINTERFACES_MAX_DISPID					12200

//VCPB Collections
#define VCCOLLID__NewEnum							12000
#define VCCOLLID_Item								12001
#define VCCOLLID_Count								12002
#define VCCOLLID_VCProjectEngine					12003

//VCPB Platform
#define VCPLATFORM_MIN_DISPID						12010
#define VCPLATFORM_MAX_DISPID						12019

#define VCPLATID_ExecutableDirectories				12010
#define VCPLATID_IncludeDirectories					12011
#define VCPLATID_ReferenceDirectories				12012
#define VCPLATID_LibraryDirectories					12013
#define VCPLATID_SourceDirectories					12014
#define VCPLATID_Name								12015
#define VCPLATID_Tools								12016
#define VCPLATID_MatchName							12017
#define VCPLATID_VCProjectEngine					12018
#define VCPLATID_Evaluate							12019

//VCPB Debug Settings
#define VCDEBUG_MIN_DISPID							12020
#define VCDEBUG_MAX_DISPID							12039

#define VCDSID_Command								12020
#define VCDSID_WorkingDirectory						12021
#define VCDSID_CommandArguments						12022
#define VCDSID_DebuggerType							12023
#define VCDSID_Attach								12024
#define VCDSID_Remote								12025
#define	VCDSID_RemoteCommand						12026
#define	VCDSID_RemoteMachine						12027
#define VCDSID_HttpUrl								12028
#define VCDSID_PDBPath								12029
#define VCDSID_BaseCommand							12030		// this is a fake prop for storing the path to an exe project
#define VCDSID_SQLDebugging							12031
// Leave 12032-12039 for more debug settings

//VCPB Filter, see also VCProjectItem
#define VCFLTID_Filter								12041
#define VCFLTID_Name								12042
#define VCFLTID_CanonicalName                       12043
#define VCFLTID_UniqueIdentifier                    12044
#define VCFLTID_AddFilter							12045
#define VCFLTID_CanAddFilter						12046
#define VCFLTID_RemoveFilter                        12047
#define VCFLTID_AddFile								12048
#define VCFLTID_CanAddFile							12049
#define VCFLTID_RemoveFile							12050
#define VCFLTID_Delete								12051
#define VCFLTID_MatchName							12052
#define VCFLTID_Filters								12053
#define VCFLTID_Files								12054
#define VCFLTID_Items								12055
#define VCFLTID_ParseFiles							12056
#define VCFLTID_SourceControlFiles					12057
#define VCFLTID_AddWebRef							12058
#define VCFLTID_Move								12059
#define VCFLTID_CanMove								12060

//VCPB Debug Enums
#define	VCDSID_RemoteLocal							12070
#define	VCDSID_RemoteDCOM							12071
#define	VCDSID_RemoteTCPIP							12072
#define VCDSID_TypeNativeOnly						12073
#define VCDSID_TypeManagedOnly						12074
#define VCDSID_TypeMixed							12075
#define VCDSID_TypeAuto								12076
#define	VCDSID_Command_IExplore						12077
#define	VCDSID_Command_ATLTest						12078
#define	VCDSID_Command_RegSvr                       12079
#define VCDSID_Command_Browse                       12080
// Leave 12081-12099 for more debug enums


// TypeDef Enumerators
// NOTE: if the range of dispids goes beyond VCTDENUM_MAX_DISPID, keep it updated.
#define VCTDENUM_MIN_DISPID							13000
#define VCTDENUM_MAX_DISPID							14999

// C/C++ Compiler 
// typedef enum optimizeOption
#define VCTDENUM_optimizeDisabled					13000
#define VCTDENUM_optimizeMinSpace					13001
#define VCTDENUM_optimizeMaxSpeed					13002
#define VCTDENUM_optimizeFull						13003
#define VCTDENUM_optimizeCustom						13004

// typedef enum inlineExpansionOption
#define VCTDENUM_expandDisable						13010
#define VCTDENUM_expandOnlyInline					13011
#define VCTDENUM_expandAnySuitable					13012

// typedef enum favorSizeOrSpeedOption
#define VCTDENUM_favorNone							13015
#define VCTDENUM_favorSpeed							13016
#define VCTDENUM_favorSize							13017

// typedef enum preprocessOption
#define VCTDENUM_preprocessNo						13020
#define VCTDENUM_preprocessYes						13021
#define VCTDENUM_preprocessNoLineNumbers			13022

// typedef enum stringPoolOption
#define VCTDENUM_stringPoolNone						13025
#define VCTDENUM_stringPoolSet						13026
#define VCTDENUM_stringPoolReadOnly					13027

// typedef enum ptrMemberOption
#define VCTDENUM_ptrMemAny							13030
#define VCTDENUM_ptrMemGeneral						13031
#define VCTDENUM_ptrMemBestCase						13032
#define VCTDENUM_ptrMemMultiInherit					13033
#define VCTDENUM_ptrMemSingleInherit				13034

// typedef enum pchOption
#define VCTDENUM_pchNone							13040
#define VCTDENUM_pchCreateUsingSpecific				13041
#define VCTDENUM_pchGenerateAuto					13042
#define VCTDENUM_pchUseUsingSpecific				13043

// typedef enum asmListingOption
#define VCTDENUM_asmListingNone						13050
#define VCTDENUM_asmListingAssemblyOnly				13051
#define VCTDENUM_asmListingAsmMachineSrc			13052
#define VCTDENUM_asmListingAsmMachine				13053
#define VCTDENUM_asmListingAsmSrc					13054

// typedef enum debugOption
#define VCTDENUM_debugDisabled						13060
#define VCTDENUM_debugOldStyleInfo					13061
#define VCTDENUM_debugLineInfoOnly					13062
#define VCTDENUM_debugEnabled						13063
#define VCTDENUM_debugEditAndContinue				13064

// typedef enum browseInfoOption
#define VCTDENUM_brAllInfo							13070
#define VCTDENUM_brNoLocalSymbols					13071

// typedef enum CompileAsOptions
#define VCTDENUM_compileAsC							13075
#define VCTDENUM_compileAsCPlusPlus					13076

// typedef enum warningLevelOption
#define VCTDENUM_warningLevel_0						13080
#define VCTDENUM_warningLevel_1						13081
#define VCTDENUM_warningLevel_2						13082
#define VCTDENUM_warningLevel_3						13083
#define VCTDENUM_warningLevel_4						13084
#define VCTDENUM_warningLevel_All					13085

// typedef enum ProcessorOptimizeOption
#define VCTDENUM_procOptimizeBlended				13090
#define VCTDENUM_procOptimizePentium				13091
#define VCTDENUM_procOptimizePentiumProAndAbove		13092

// typedef enum callingConventionOption
#define VCTDENUM_callConventionCDecl				13095
#define VCTDENUM_callConventionFastCall				13096
#define VCTDENUM_callConventionStdCall				13097

// typedef enum compileAsManagedOptions
#define VCTDENUM_managedNone						13110
#define VCTDENUM_managedMetaData					13111
#define VCTDENUM_managedAssembly					13112

// typedef enum structMemberAlignOption
#define VCTDENUM_alignSingleByte					13115
#define VCTDENUM_alignTwoBytes						13116
#define VCTDENUM_alignFourBytes						13117
#define VCTDENUM_alignEightBytes					13118
#define VCTDENUM_alignSixteenBytes					13119

// typedef enum runtimeLibraryOption
#define VCTDENUM_rtMultiThreaded					13125
#define VCTDENUM_rtMultiThreadedDebug				13126
#define VCTDENUM_rtMultiThreadedDLL					13127
#define VCTDENUM_rtMultiThreadedDebugDLL			13128
#define VCTDENUM_rtSingleThreaded					13129
#define VCTDENUM_rtSingleThreadedDebug				13130

// typedef enum codeGenerationStyles
#define VCTDENUM_codeGenerationOptionNotSet			13135
#define VCTDENUM_codeGenerationManagedMSIL			13136
#define VCTDENUM_codeGenerationManagedNative		13137
#define VCTDENUM_codeGenerationUnmanagedNative		13138

// Linker  & Library Tools
// typedef enum subSystemOption
#define VCTDENUM_subSystemNotSet					13145
#define VCTDENUM_subSystemConsole					13146
#define VCTDENUM_subSystemWindows					13147

// typedef enum linkProgressOption
#define VCTDENUM_linkProgressNotSet					13150
#define VCTDENUM_linkProgressAll					13151
#define VCTDENUM_linkProgressLibs					13152

// typedef enum addressAwarenessType
#define VCTDENUM_addrAwareDefault					13155
#define VCTDENUM_addrAwareNoLarge					13156
#define VCTDENUM_addrAwareLarge						13157

// typedef optRefType
#define VCTDENUM_optNoReferences					13160
#define VCTDENUM_optReferences						13161

// typedef optWin98
#define VCTDENUM_optWin98No							13165
#define VCTDENUM_optWin98Yes						13166

// typedef optFoldingType
#define VCTDENUM_optNoFolding						13170
#define VCTDENUM_optFolding							13171

// typedef linkIncrementalType
#define VCTDENUM_linkIncrementalNo					13175
#define VCTDENUM_linkIncrementalYes					13176

// typedef termSvrAwarenessType
#define VCTDENUM_termSvrAwareNo						13180
#define VCTDENUM_termSvrAwareYes					13181

// typedef enum midlCharOption
#define VCTDENUM_midlCharUnsigned					13185
#define VCTDENUM_midlCharSigned						13186
#define VCTDENUM_midlCharAscii7						13187

// typedef enum midlStubStyleOption
#define VCTDENUM_midlStubNotSet						13190
#define VCTDENUM_midlStubMixedMode					13191
#define VCTDENUM_midlStubOldStyle					13192
#define VCTDENUM_midlStubStandard					13193
#define VCTDENUM_midlStubDeluxe						13194

// typedef enum midlErrorCheckOption
#define VCTDENUM_midlEnableCustom					13200
#define VCTDENUM_midlDisableAll						13201
#define VCTDENUM_midlEnableAll						13202

// typedef enum midlTargetEnvironment
#define VCTDENUM_midlTargetNotSet					13205
#define VCTDENUM_midlTargetWin32					13206
#define VCTDENUM_midlTargetWin64					13207

// typedef enum midlTypeLibFormat
#define VCTDENUM_midlTypeLibNone					13210
#define VCTDENUM_midlTypeLibNewFormat				13211
#define VCTDENUM_midlTypeLibOldFormat				13212

// typedef enum midlWarningLevelOption
#define VCTDENUM_midlWarningLevel_0					13215
#define VCTDENUM_midlWarningLevel_1					13216
#define VCTDENUM_midlWarningLevel_2					13217
#define VCTDENUM_midlWarningLevel_3					13218
#define VCTDENUM_midlWarningLevel_4					13219

// typedef enum midlStructMemberAlignOption
#define VCTDENUM_midlAlignNotSet					13225
#define VCTDENUM_midlAlignSingleByte				13226
#define VCTDENUM_midlAlignTwoBytes					13227
#define VCTDENUM_midlAlignFourBytes					13228
#define VCTDENUM_midlAlignEightBytes				13229

// Configurations
// typedef enum ConfigurationTypes
#define VCTDENUM_typeUnknown						13235
#define VCTDENUM_typeApplication					13236
#define VCTDENUM_typeDynamicLibrary					13237
#define VCTDENUM_typeStaticLibrary					13238
#define VCTDENUM_typeGeneric						13239

// typedef enum useOfMfc
#define VCTDENUM_useMfcStdWin						13245
#define VCTDENUM_useMfcStatic						13246
#define VCTDENUM_useMfcDynamic						13247

// typedef enum useOfATL
#define VCTDENUM_useATLNotSet						13250
#define VCTDENUM_useATLStatic						13251
#define VCTDENUM_useATLDynamic						13252

// typedef enum charSet
#define VCTDENUM_charSetNotSet						13255
#define VCTDENUM_charSetUnicode						13256
#define VCTDENUM_charSetMBCS						13257

// typedef toolSetType
#define VCTDENUM_toolSetUtility						13260
#define VCTDENUM_toolSetMakefile					13261
#define VCTDENUM_toolSetLinker						13262
#define VCTDENUM_toolSetLibrarian					13263
#define VCTDENUM_toolSetAll							13264

// Projects
// typedef enum ProjectitemTypes
#define VCTDENUM_typeFile							13270
#define VCTDENUM_typeFilter							13271
#define VCTDENUM_typeProject						13272

// Projects
// typedef enumBool 
#define VCTDENUM_No									13275
#define VCTDENUM_Yes								13276

// typedef eLogSectionTypes
#define VCTDENUM_eLogNoSection						13280
#define VCTDENUM_eLogNoNewLine						13281
#define VCTDENUM_eLogStartSection					13282
#define VCTDENUM_eLogHeader							13283
#define VCTDENUM_eLogEnvironment					13284
#define VCTDENUM_eLogCommand						13285
#define VCTDENUM_eLogOutput							13286
#define VCTDENUM_eLogResults						13287
#define VCTDENUM_eLogFooter							13288
#define VCTDENUM_eLogAll							13289

// typedef CMD
#define VCTDENUM_CMD_Canceled						13290
#define VCTDENUM_CMD_Complete						13291
#define VCTDENUM_CMD_Error							13292

// typedef genProxyLanguage
#define VCTDENUM_genProxyNative						13295
#define VCTDENUM_genProxyManaged					13296

// random BOOL properties that we need to have junk enums for for settings pages
#define VCTDENUM_Default								14500
#define VCTDENUM_None									14501
#define VCTDENUM_GlobalOptimizationsTrue				14502
#define VCTDENUM_EnableIntrinsicFunctionsTrue			14503
#define VCTDENUM_ImproveFloatingPointConsistencyTrue	14504
#define VCTDENUM_ImproveFloatingPointConsistencyFalse	14505
#define VCTDENUM_OmitFramePointersTrue					14506
#define VCTDENUM_EnableFiberSafeOptimizationsTrue		14507
#define VCTDENUM_UndefineAllPreprocessorDefinitionsTrue	14508
#define VCTDENUM_IgnoreStandardIncludePathTrue			14509
#define VCTDENUM_KeepCommentsTrue						14510
#define VCTDENUM_ShowIncludesTrue						14511
#define VCTDENUM_SmallerTypeCheckTrue					14512
#define VCTDENUM_BufferSecurityCheckTrue				14513
#define VCTDENUM_MinimalRebuildFalse					14514
#define VCTDENUM_MinimalRebuildTrue						14515
#define VCTDENUM_RTTITrue								14516
#define VCTDENUM_DisableLanguageExtensionsTrue			14519
#define VCTDENUM_DefaultCharIsUnsignedTrue				14520
#define VCTDENUM_TreatWChar_tAsBuiltInTypeTrue			14521
#define VCTDENUM_EnableFunctionLevelLinkingTrue			14522
#define VCTDENUM_UsePCHTrue								14523
#define VCTDENUM_ExpandAttributedSourceTrue				14524
#define VCTDENUM_WarnAsErrorTrue						14525
#define VCTDENUM_SuppressStartupBannerTrue				14526
#define VCTDENUM_SuppressStartupBannerUpTrue			14527
#define VCTDENUM_Detect64BitPortabilityProblemsTrue		14528
#define VCTDENUM_ForceConformanceInForLoopScopeTrue		14529
#define VCTDENUM_IgnoreAllDefaultLibrariesTrue			14530
#define VCTDENUM_ResourceOnlyDLLTrue					14531
#define VCTDENUM_SetChecksumTrue						14532
#define VCTDENUM_TurnOffAssemblyGenerationTrue			14533
#define VCTDENUM_SwapRunFromCDTrue						14534
#define VCTDENUM_SwapRunFromNetTrue						14535
#define VCTDENUM_GenerateDebugInformationTrue			14536
#define VCTDENUM_GenerateMapFileTrue					14537
#define VCTDENUM_MapExportsTrue							14538
#define VCTDENUM_MapLinesTrue							14539
#define VCTDENUM_SupportUnloadOfDelayLoadedDLLFalse		14540
#define VCTDENUM_SupportUnloadOfDelayLoadedDLLTrue		14541
#define VCTDENUM_IgnoreEmbeddedIDLTrue					14542
#define VCTDENUM_MkTyplibCompatibleTrue					14543
#define VCTDENUM_MidlIgnoreStandardIncludePathTrue		14544
#define VCTDENUM_ErrorCheckAllocationsTrue				14545
#define VCTDENUM_ErrorCheckBoundsTrue					14546
#define VCTDENUM_ErrorCheckEnumRangeTrue				14547
#define VCTDENUM_ErrorCheckRefPointersTrue				14548
#define VCTDENUM_ErrorCheckStubDataTrue					14549
#define VCTDENUM_ValidateParametersTrue					14550
#define VCTDENUM_ShowProgressTrue						14551
#define VCTDENUM_ExceptionHandlingTrue					14552
#define VCTDENUM_runtimeCheckStackFrame					14553
#define VCTDENUM_runtimeCheckUninitVariables			14554
#define VCTDENUM_runtimeCheckBasic						14555
#define VCTDENUM_StringPoolingTrue						14556
#define VCTDENUM_GenerateStublessProxiesTrue			14557
#define VCTDENUM_GenerateTypeLibraryFalse				14558
#define VCTDENUM_OptimizeForWindowsApplicationYes		14559
#define VCTDENUM_machineNotSet							14560
#define VCTDENUM_machineX86								14561
#define VCTDENUM_eAppProtectLow							14562
#define VCTDENUM_eAppProtectMedium						14563
#define VCTDENUM_eAppProtectHigh						14564

// XBox Image Game Rating enum
#define XBOXGAMERATINGENUM_eNone						14580
#define XBOXGAMERATINGENUM_eRP							14581
#define XBOXGAMERATINGENUM_eAO							14582
#define XBOXGAMERATINGENUM_eM							14583
#define XBOXGAMERATINGENUM_eT							14584
#define XBOXGAMERATINGENUM_eE							14585
#define XBOXGAMERATINGENUM_eKA							14586
#define XBOXGAMERATINGENUM_eEC							14587

// TypeLib Coclasses
//NOTE: if the range of dispids goes beyond VCPBLIBCLASS_MAX_DISPID, keep it updated
#define VCPBLIBCLASS_MIN_DISPID							15000
#define VCPBLIBCLASS_MAX_DISPID							15100

// VCPB General Classes
#define VCCollection_Class								15000
#define	VCProjectEngine_Class							15001
#define VCProject_Class									15002
#define VCFile_Class									15003
#define VCFilter_Class									15004
#define VCConfiguration_Class							15005
#define VCStyleSheet_Class								15006
#define VCDebugSettings_Class							15007
#define VCFileConfiguration_Class						15008
#define VCPropertyContainer_Class						15009
#define VCGeneralConfigSettings_Class					15010

// VCPB Resource Tool Classes
#define VCResourceCompilerBaseTool_Class				15020
#define RCGeneral_Class									15021

// VCPB Library Tool Classes
#define VCLibrarianBaseTool_Class						15030
#define LibGeneral_Class								15031
#define LibInputOutput_Class							15032

// VCPB Linker Tool Classes
#define VCLinkerBaseTool_Class							15040
#define LinkGeneral_Class								15041
#define LinkInput_Class									15042
#define LinkDebug_Class									15043
#define LinkSystem_Class								15044
#define LinkOptimization_Class							15045
#define LinkMIDL_Class									15046
#define LinkAdvanced_Class								15047

// VCPB Compiler Tool Classes
#define VCCLCompilerBaseTool_Class						15060
#define ClOptimization_Class							15061
#define ClPreprocessor_Class							15062
#define ClAdvanced_Class								15063
#define ClCodeGeneration_Class							15065
#define ClLanguage_Class								15066
#define ClPrecompiledHeaders_Class						15067
#define ClOutputFiles_Class								15068
#define ClBrowseInfo_Class								15070
#define ClGeneral_Class									15071

// VCPB MIDL Tool Classes
#define VCMidlBaseTool_Class							15090
#define MidlGeneral_Class								15091
#define MidlOutput_Class								15092
#define MidlAdvanced_Class								15093

// VCPB Build Tool Classes
#define VCBuildOutput_Class								15100
#define VCPreBuildEventBaseTool_Class					15101
#define VCPreLinkEventBaseTool_Class					15102
#define VCPostBuildEventBaseTool_Class					15103
#define VCCustomBuildBaseTool_Class						15104
#define VCCustomGeneral_Class							15105
#define VCPreBldGeneral_Class							15106
#define VCPreLinkGeneral_Class							15107
#define VCPostBldGeneral_Class							15108
#define VCBSCGeneral_Class								15109

#define VCNMakeGeneral_Class							15110

// PROJBLD Project Engine Classes
#define VCProjBuildEvents_Class							15120
#define VCFolderNodeProps_Class							15121
#define VCFileNodeProps_Class							15122
#define VCProjectNodeProps_Class						15123
#define BuildPackage_Class								15124

// XBox Deployment Classes
#define XboxDeploymentGeneral_Class						15140

// XBox Image Classes
#define XboxImageGeneral_Class							15150
#define XboxImageCertificate_Class						15151
#define XboxImageTitleInfo_Class						15152

#define VCID_Obsolete									15200
