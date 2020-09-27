//If you change the IDE version, the word-vbe test in the suites may fail if
// you don't do it right because it has a reference to the version 5 of the
// typelib.
#define VBIDE_LIB_VERSION_MAJ 5
#define VBIDE_LIB_VERSION_MIN 0
#define VBIDE_LIB_VERSION     5.0

#define RBY_TYPELIBMAJVER 7
#define RBY_TYPELIBMINVER 0
#define RBY_TYPELIBVERSION 7.0

#define DTE_LIB_VERSION_MAJ 7
#define DTE_LIB_VERSION_MIN 0

#ifndef _PREFIX_
#define DTE_LIB_VERSION     7.0
#else
#if !defined(DTE_LIB_VERSION)
#define DTE_LIB_VERSION     0x0070 
// To get edg to parse some 
// template instantiations where a WORD argument is being passed
// 7.0 as high as I can tell. 
#endif /* !defined(DTE_LIB_VERSION)  */
#endif /* _PREFIX_ */
