#ifdef USE_VSCLSID_FOR_CLSIDS
// don't forget to change src\Env\misc\pkgs\solution\sources

//
// This file lists CLSIDs that change between builds
// Don't use slash-star comments anywhere in this file.
//



//
// Replacement for DEFINE_GUID that works with our generated guids
//

#if defined (INITGUID) || defined (INITGUIDS)
#undef VSDEFINE_GUID
#define VSDEFINE_GUID(name, val) EXTERN_C const GUID name  = { val }
#else
#define VSDEFINE_GUID(name, val)  EXTERN_C const GUID FAR name
#endif // INITGUID


#ifdef DEBUG 


// idl format
#define VSCLSID_SDMServer						/*idl*/81b6566a-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_MsMachineDebugManager 			/*idl*/81b6566b-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_EncMgr							/*idl*/81b6566c-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_WFCComponentManager				/*idl*/81b6566d-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_WFCValueEditor					/*idl*/81b6566e-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsUserData						/*idl*/81b6566f-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsCodeWindow					/*idl*/81b65670-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsSplitRoot						/*idl*/81b65671-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsDropdownBar					/*idl*/81b65672-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsTextBuffer					/*idl*/81b65673-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsTextLineStorage				/*idl*/81b65674-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsDebugTextBuffer				/*idl*/81b65675-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsTextManager					/*idl*/81b65676-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsTextView						/*idl*/81b65677-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsTextPackage					/*idl*/81b65678-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsUndoUnit						/*idl*/81b65679-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsMethodTipWindow				/*idl*/81b6567a-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsHiddenTextLayer				/*idl*/81b6567b-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_JavaPkgService					/*idl*/81b6567c-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsContextClass                  /*idl*/81b6567d-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_MDMCallbackServer               /*idl*/81b6567e-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_LECallbackServer                /*idl*/81b6567f-cdd8-11d2-992e-00c04f68fdaf
#define VSIID_IVsBuildStatusCallback            /*idl*/81b65680-cdd8-11d2-992e-00c04f68fdaf
#define VSIID_IPrivateSession                   /*idl*/81b65681-cdd8-11d2-992e-00c04f68fdaf
#define VSIID_IPrivateMachine                   /*idl*/81b65682-cdd8-11d2-992e-00c04f68fdaf
#define VSIID_ILECallback                       /*idl*/81b65683-cdd8-11d2-992e-00c04f68fdaf
#define VSIID_IMDMCallback                      /*idl*/81b65684-cdd8-11d2-992e-00c04f68fdaf




// DEFINE_GUID format			
#define VSCLSID_DBGProxy 						/*C*/0x81b65685, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_Debugger 						/*C*/0x81b65686, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_JavaExprEvaluator				/*C*/0x81b65687, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_JavaExpressionContext			/*C*/0x81b65688, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_JavaProperties					/*C*/0x81b65689, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_JavaExprEvaluatorVS7			/*C*/0x81b6568a, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_JavaLEDebugManager				/*C*/0x81b6568b, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSguidJavaEng							/*C*/0x81b6568c, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSguidScriptEng							/*C*/0x81b6568d, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSguidNativeEng							/*C*/0x81b6568e, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSguidSQLEng							/*C*/0x81b6568f, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSguidCOMPlusEng						/*C*/0x81b65690, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSguidNativeOnlyEng						/*C*/0x81b65691, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_NativeDebugEngine				/*C*/0x81b65692, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_SqlDebugEngine					/*C*/0x81b65693, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_COMPlusDebugEngine				/*C*/0x81b65694, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_MSDbgProxy						/*C*/0x81b65695, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_MSENV							/*c*/0x81b65696, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_VSDebugPackage					/*c*/0x81b65697, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_VsEnvironmentPackage            /*c*/0x81b65698, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_FreeThreadedNativeEngine        /*c*/0x81b65699, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_NativeMachineSupplier           /*c*/0x81b6569a, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_FreeThreadedNativeOnlyEngine    /*c*/0x81b6569b, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_VsSolutionBuilderProxy          /*c*/0x81b6569c, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_VS_SOLUTION_PACKAGE             /*c*/0x81b6569d, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}

#else // DEBUG

// idl format
#define VSCLSID_SDMServer						/*idl*/81b6569e-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_MsMachineDebugManager 			/*idl*/81b6569f-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_EncMgr							/*idl*/81b656a0-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VSDebugPackage					/*idl*/81b656a1-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_WFCComponentManager				/*idl*/81b656a2-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_WFCValueEditor					/*idl*/81b656a3-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsUserData						/*idl*/81b656a4-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsCodeWindow					/*idl*/81b656a5-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsSplitRoot						/*idl*/81b656a6-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsDropdownBar					/*idl*/81b656a7-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsTextBuffer					/*idl*/81b656a8-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsTextLineStorage				/*idl*/81b656a9-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsDebugTextBuffer				/*idl*/81b656aa-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsTextManager					/*idl*/81b656ab-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsTextView						/*idl*/81b656ac-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsTextPackage					/*idl*/81b656ad-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsUndoUnit						/*idl*/81b656ae-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsMethodTipWindow				/*idl*/81b656af-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsHiddenTextLayer				/*idl*/81b656b0-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_JavaPkgService					/*idl*/81b656b1-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_VsContextClass                  /*idl*/81b656b2-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_MDMCallbackServer               /*idl*/81b656b3-cdd8-11d2-992e-00c04f68fdaf
#define VSCLSID_LECallbackServer                /*idl*/81b656b4-cdd8-11d2-992e-00c04f68fdaf
#define VSIID_IVsBuildStatusCallback            /*idl*/81b656b5-cdd8-11d2-992e-00c04f68fdaf
#define VSIID_IPrivateSession                   /*idl*/81b656b6-cdd8-11d2-992e-00c04f68fdaf
#define VSIID_ILECallback                       /*idl*/81b656b7-cdd8-11d2-992e-00c04f68fdaf
#define VSIID_IMDMCallback                      /*idl*/81b656b8-cdd8-11d2-992e-00c04f68fdaf


// DEFINE_GUID format			
#define VSCLSID_DBGProxy 						/*C*/0x81b656b9, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_Debugger 						/*C*/0x81b656ba, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_JavaExprEvaluator				/*C*/0x81b656bb, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_JavaExpressionContext			/*C*/0x81b656bc, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_JavaProperties					/*C*/0x81b656bd, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_JavaExprEvaluatorVS7			/*C*/0x81b656be, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_JavaLEDebugManager				/*C*/0x81b656bf, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSguidJavaEng							/*C*/0x81b656c0, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSguidScriptEng							/*C*/0x81b656c1, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSguidNativeEng							/*C*/0x81b656c2, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSguidSQLEng							/*C*/0x81b656c3, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSguidCOMPlusEng						/*C*/0x81b656c4, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSguidNativeOnlyEng						/*C*/0x81b656c5, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_NativeDebugEngine				/*C*/0x81b656c6, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_SqlDebugEngine					/*C*/0x81b656c7, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_COMPlusDebugEngine				/*C*/0x81b656c8, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_MSDbgProxy						/*C*/0x81b656c9, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_MSENV							/*c*/0x81b656ca, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_VSDebugPackage					/*c*/0x81b656cb, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_VsEnvironmentPackage            /*c*/0x81b656cc, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_FreeThreadedNativeEngine        /*c*/0x81b656cd, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_NativeMachineSupplier           /*c*/0x81b656ce, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_FreeThreadedNativeOnlyEngine    /*c*/0x81b656cf, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_VsSolutionBuilderProxy          /*c*/0x81b656d0, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}
#define VSCLSID_VS_SOLUTION_PACKAGE             /*c*/0x81b656d1, 0xcdd8, 0x11d2,  {0x99, 0x2e, 0x0, 0xc0, 0x4f, 0x68, 0xfd, 0xaf}

#endif // DEBUG



#endif // USE_VSCLSID_FOR_CLSIDS
