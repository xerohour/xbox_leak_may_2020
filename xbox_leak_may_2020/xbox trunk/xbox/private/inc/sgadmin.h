// ---------------------------------------------------------------------------------------
// sgadmin.h
//
// Security Gateway Admin Library
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#ifndef __SGADMIN_H__
#define __SGADMIN_H__

NTSTATUS SgConfigure(PSTR pConfig);
NTSTATUS SgCmd(PWSTR pCmd, ULONG cbOut, PBYTE pbOut);

#endif
