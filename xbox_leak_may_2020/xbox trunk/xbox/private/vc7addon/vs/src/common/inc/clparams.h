//=----------------------------------------------------------------------------
// Microsoft Visual Studio
//
// Microsoft Confidential
// Copyright 1995-1999 Microsoft Corporation.  All Rights Reserved.
//
// File:	clparams.h
// Area:	Command Line
//
// Contents:	Useful definitions for working with Command Line parameters
//=----------------------------------------------------------------------------


/*****************************************************************************

  Format of Parameter Lists

  A parameter list is a string that consists of a series of parameter
  descriptors separated by spaces.  Each parameter descriptor is either '*' or
  a series of parameter types separated by '|'.  Each parameter type is (for
  now) a single character that corresponds to a type of argument that is valid
  for that parameter. When a parameter descriptor specifies two or more
  parameter types "ored" together with '|', this means that any of the
  specified types is valid for that parameter, and that autocompletion should
  present all the lists merged together.

  If a parameter descriptor is '*', this indicates zero or more occurances of
  the previous parameter are allowed, with the same autocompletion for each.
  Only the last parameter descriptor is allowed to be '*', and there must be
  at least one preceding parameter descriptor.

  The following are currently valid as parameter types:

    ~  No autocompletion for this parameter
    $  This parameter is the rest of the input line (no autocompletion)

    a  An alias
    c  The canonical name of a command
    p  A filename from a project in the current solution
    u  A URL

  The function CAutoCompletionManager::EnableACSources() is responsible for 
  interpreting parameter lists and driving the autocompletion process.

*****************************************************************************/


/*****************************************************************************

  Querying the Parameter List of a Command

  The parameter list of a command is queried by calling Exec with the LOWORD
  of nCmdexecopt set to OLECMDEXECOPT_SHOWHELP (instead of the more usual
  OLECMDEXECOPT_DODEFAULT), the HIWORD of nCmdexecopt set to
  VSCmdOptQueryParameterList, pvaIn set to NULL, and pvaOut pointing to an
  empty VARIANT ready to receive the result BSTR.  This should be done only
  for commands that are marked with the ALLOWPARAMS flags in the command
  table.

  If you are implementing a handler for a command that accepts parameters, you
  should find the ISQUERYPARAMETERLIST macro a convenient way to test whether
  the command's parameter list is being queried.

*****************************************************************************/

enum VSCustomCmdOptions
  {
  VSCmdOptQueryParameterList = 1
  };


#define ISQUERYPARAMETERLIST(pvaIn, pvaOut, dwCmdExecOpt)\
  (pvaIn == NULL && pvaOut != NULL && \
   LOWORD(dwCmdExecOpt) == OLECMDEXECOPT_SHOWHELP && \
   HIWORD(dwCmdExecOpt) == VSCmdOptQueryParameterList)
