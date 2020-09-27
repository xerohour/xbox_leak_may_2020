//{{NO_DEPENDENCIES}}
// compiler option control IDs

// 'General' Category IDs
#define OPTNCPLR_IDDP_PROJ_COMPILER	18443

// 'Custom Options' Category IDs
#define OPTNCPLR_IDDP_COMPILER_CUSTOM	18433
#define OPTNCPLR_IDC_OPTEXT		0x4920	// Za, Ze
#define OPTNCPLR_IDC_OPTINCLINK	0x4921	// Gy
#define OPTNCPLR_IDC_STRPOOL		0x4922	// Gf
#define OPTNCPLR_IDC_OPTWARN		0x4923	// W0, W1, ...
#define OPTNCPLR_IDC_WARNASERR	0x4924	// WX
#define OPTNCPLR_IDC_CPLRNOLOGO	0x4925	// nologo
#define OPTNCPLR_IDC_ENABLE_GI	0x4926	// Gi
#define OPTNCPLR_IDC_ENABLE_MR	0x4927	// Gm
#define OPTNCPLR_IDC_TXT_WARN	0x4930

// 'Custom Options (C++)' Category IDs
#define OPTNCPLR_IDDP_COMPILER_CUSTOMCPP	18434
#define OPTNCPLR_IDC_PTRMBR1		0x4940	// (representation method)
#define OPTNCPLR_IDC_PTRMBR2		0x4941	// (general-purpose representation)
#define OPTNCPLR_IDC_PTRGEN		0x4942	// pseudo flag (not user selectable)
#define OPTNCPLR_IDC_VTORDISP	0x4943
/* temporarily reverse the logic for PDC (BamBam) 'cos the compiler default will be /GX- for this
** release .. ie. we need [X] Enable EH in the options dialog
#define OPTNCPLR_IDC_DISABLE_EH	0x4944
*/
#define OPTNCPLR_IDC_ENABLE_EH	0x4944
#define OPTNCPLR_IDC_ENABLE_RTTI	0x4945
#define OPTNCPLR_IDC_TXT_PTRMBR1	0x4950
#define OPTNCPLR_IDC_TXT_PTRMBR2	0x4951
#define OPTNCPLR_IDC_TXT_PTRGRP	0x4952

// 'Debug and Runtime Options' Category IDs
#define OPTNCPLR_IDDP_COMPILER_DEBUG	18435
#define OPTNCPLR_IDC_DEBUG		0x4960	// Zd, Z7, Zi
#define OPTNCPLR_IDC_TXT_DEBUG	0x4970

// 'Listing Files' Category IDs
#define OPTNCPLR_IDDP_COMPILER_LISTFILE	18436
#define OPTNCPLR_IDC_LISTBSC		0x4980	// FR
#define OPTNCPLR_IDC_LISTLOCAL	0x4981	// Fr
#define OPTNCPLR_IDC_LISTBSC_NME	0x4983	// FR<file>
#define OPTNCPLR_IDC_LISTASM		0x4984	// FA, FAc, FAs, FASc
#define OPTNCPLR_IDC_LISTASM_NME	0x4985	// Fa<file>
#define OPTNCPLR_IDC_TXT_LISTASM	0x498A	// 
#define OPTNCPLR_IDC_TXT_LISTASM1 0x498B
#define OPTNCPLR_IDC_TXT_LISTBSC 0x498C

// 'Optimizations' Category IDs
#define OPTNCPLR_IDDP_COMPILER_OPTIMIZE	18437
#define OPTNCPLR_IDC_OPTIMIZE	0x4990	// O? etc. for Native Code
#define OPTNCPLR_IDC_INLINECTRL	0x4991	// Ob
#define OPTNCPLR_IDC_OPTIMIZE2	0x4992	// O2, /O1, /O? enabled
#define OPTNCPLR_IDC_TXT_OPTIMIZE 0x49A0
#define OPTNCPLR_IDC_TXT_INLINE	0x49A1

// 'Pre-compiled Headers' Category IDs
#define OPTNCPLR_IDDP_COMPILER_PCH		18438
#define OPTNCPLR_IDC_NOPCH		0x49B0
#define OPTNCPLR_IDC_AUTOPCH		0x49B1	// YX
#define OPTNCPLR_IDC_CREATEPCH	0x49B2	// Yc
#define OPTNCPLR_IDC_USEPCH		0x49B3	// Yu
#define OPTNCPLR_IDC_TXT_AUTOPCH 0x49B4
#define OPTNCPLR_IDC_AUTOPCHNAME	0x49B5	// YX<file>
#define OPTNCPLR_IDC_TXT_CREATE	0x49B6
#define OPTNCPLR_IDC_CREATENAME	0x49B7	// Yc<file>
#define OPTNCPLR_IDC_TXT_USE		0x49B8
#define OPTNCPLR_IDC_USENAME		0x49B9	// Yu<file>

// 'Preprocessor' Category IDs
#define OPTNCPLR_IDDP_COMPILER_PREPROCESS	18439
#define OPTNCPLR_IDC_MACROS		0x49D0	// D<macro>
#define OPTNCPLR_IDC_UNDEFS		0x49D1	// U<macro>
#define OPTNCPLR_IDC_UNDEFINE	0x49D2	// u
#define OPTNCPLR_IDC_IGNINC		0x49D3	// X
#define OPTNCPLR_IDC_INCLUDE		0x49D4	// I<path>
#define OPTNCPLR_IDC_TXT_MACROS	0x49E0
#define OPTNCPLR_IDC_TXT_UNDEFS	0x49E1
#define OPTNCPLR_IDC_TXT_INCS	0x49E2
