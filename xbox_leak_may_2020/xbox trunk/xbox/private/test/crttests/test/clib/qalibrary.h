#if !defined(__QALIBRARY_H__)
#define __QALIBRARY_H__

//
// Master header file for the library.  Since the library can be used in both
// source only or DLL version mode this header encapsulates which is used based
// on macros that are defined when test is built.
//

#include "qalib.h"      // always included.  also includes thread support if
                        // needed.
#if !defined(QA_MULTI_FILE_TEST)
#include "qalib.cpp"    // contains guards for DLL use case (as do all the
                        // source files).
#endif 

//
// Include other components base on need.
//

// Command line manager class.
//
#if defined(QA_USE_CMD_LINE_MGR)
#include "qa_cmdlinemgr.h"
#if !defined(QA_MULTI_FILE_TEST)
#include "qa_cmdlinemgr.cpp"
#endif 
#endif 

// Stdio stream redirection class.
//
#if defined(QA_USE_REDIRECT_STD)
#include "qa_redirectstd.h"
#if !defined(QA_MULTI_FILE_TEST)
#include "qa_redirectstd.cpp"
#endif 
#endif 

// STL style algorithm functions for converting strings to upper and lower case.
//
#if defined(QA_USE_STRING_ALGO)
#include "qa_util.h"
#endif 

// Other utilities.
//
#include "qa_util2.h"

// simple vector class.
//
#if defined(QA_USE_VECTOR)
#include "qa_vect.h"
#endif 

// Random number generator class.
//
#if defined(QA_USE_RANDOM_NUMBER)
#include "qa_rand.h"
#endif 

// Performace timer class.
//
#if defined(QA_USE_PERF_TIMER)
#include "qa_perftimer.h"
#endif 

// String table.
//
#if defined(QA_USE_STRINGTABLE) && !defined(QA_MULTI_FILE_TEST)
#include "qa_str_table.cpp"
#endif 

// MapNetworkDrive()
//
#if defined(QA_USE_MAP_NETWORK_DRIVE)
#include "qa_mapdrive.h"
#if !defined(QA_MULTI_FILE_TEST)
#include "qa_mapdrive.cpp"
#endif 
#endif 

// Auto pointer class (CAutoPtr).
//
#if defined(QA_USE_AUTOPTR)
#include "qa_autoptr.h"
#endif 

#endif // __QALIBRARY_H__

