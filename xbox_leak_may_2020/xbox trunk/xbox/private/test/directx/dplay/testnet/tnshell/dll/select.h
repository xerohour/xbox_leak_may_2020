#ifndef __TNSHELL_SELECT__
#define __TNSHELL_SELECT__
//#pragma message("Defining __TNSHELL_SELECT__")






//==================================================================================
// External Prototypes
//==================================================================================
HRESULT SelectLoadModuleAndInfo(char* szTestNetRootPath, PMODULEDATA pModuleData);

HRESULT SelectDoSelectModule(HINSTANCE hInstance);





#else //__TNSHELL_SELECT__
//#pragma message("__TNSHELL_SELECT__ already included!")
#endif //__TNSHELL_SELECT__
