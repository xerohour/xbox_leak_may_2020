// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      stdafx.h
// Contents:  Precompiled header file.  All commonly used header files should appear in here.
// Revisions: 31-May-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma warning(disable:4530)

#include <windows.h>
#include <process.h>
#include <assert.h>
#include <Shlobj.h>
#include <stdio.h>

#define NUM_XDATA_SECTORS 1715632

#include "xsum.h"
#include "filestm.h"
#include "Tape.h"
#include "dx2ml.h"
#include "xbefile.h"
#include "randm.h"

#include "CInitedObject.h"
#include "Resource.h"
#include "Util.h"
#include "CDlg.h"
#include "CFile.h"
#include "CFST.h"
#include "CFLD.h"
#include "CStep.h"
