// String values representing registry key locations. Most are relative strings, and are expected to 
// be appended to root key strings from other locations, if not here. - mweddle

#define SZ_REGISTRATION       "Registration"
#define LSZ_REGISTRATION      L"Registration"
#define SZ_PRODUCTID          "ProductID"
#define LSZ_PRODUCTID         L"ProductID"
#define SZ_USERNAME           "UserName"
#define SZ_ORGANIZATION       "RegisteredOrganization"
#define SZ_REGLOC_LVCERT      "LVCertificate"
#define SZ_REGLOC_INSTALLDIR  "InstallDir"
#define LSZ_REGLOC_INSTALLDIR L"InstallDir"

// Although this is not distinctly a registry location, it probably should be, or else a better
// method of acquiring the application name should be used. This is intended to be a temporary measure.
#define SZ_VS_APPNAME         "Visual Studio"
