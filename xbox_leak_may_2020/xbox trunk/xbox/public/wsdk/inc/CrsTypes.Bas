Attribute VB_Name = "CrsTypes"
'====================================================================
'                                                                     
'   Microsoft Site Server v3.00                                   
'                                                                     
'   Copyright (c) 1997 - 1999 Microsoft Corporation.  All rights reserved.   
'   THIS CODE AND INFORMATION IS PROVIDED 'AS IS' WITHOUT WARRANTY OF
'   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
'   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'   PARTICULAR PURPOSE.
'=====================================================================

' Module Name:
'
'    crstypes.bas
'
' Abstract:
'
'    This module defines the common structures and prototypes for the   
'    Content Deployment Service API (ProgId = CrsApi)
'
' Revision History:
'

' NOTE: Enums for CRS_PROJECT_CREATION, CRS_SERVER_TYPES, and CRS_SCHEDULE_ACTION
'   are built in to the CrsApi type library

'Enum CRS_PROJECT_CREATION
'   CREATE_NEW_PROJECT = &h1
'   OPEN_EXISTING_PROJECT = &h2
'End Enum

'Enum CRS_SERVER_TYPES
'   STAGER = &h1
'   ENDPOINT = &h2
'End Enum

'Enum CRS_SCHEDULE_ACTION
'   CRS_SCHEDULE_APPLY = &h1
'   CRS_SCHEDULE_REPLICATE = &h2
'End Enum

'
' Parameter maximums
'
Public Const MAX_PATH        = 260		' Max Directory Path Length
Public Const MAX_PROJECT     = 50		' Max Project Name Length
Public Const MAX_LOCATION    = 300
Public Const MAX_DESTINATION_BUFFER = &h1000	' Max Length of All Destinations
Public Const MAX_ROUTE_NAME  = 50		' Max Route Name Length
Public Const MAX_REPLID	     = 40		' Max Length of Replication ID GUID
Public Const MAX_PARM_STRING = 300		' Max Length of Parameter/Property Name


'
' Replication Flags
'
' Note: 'Or' the values together to use combinations, e.g. (RF_FASTMODE Or RF_FORCE_REPL)
'
Public Const RF_NO_DELETE	= &H1		' Turns off Delete Processing
Public Const RF_REPLICATE_ACLS	= &H2		' Replicates ACLs
Public Const RF_NO_DATA		= &H4		' Skips replicating Data, will only replicate directories.
Public Const RF_FORCE_REPL	= &H8		' Forces data to be replicated without checking first if files exist on destination
Public Const RF_PERMIT_TREE	= &H10		' Applies ACLs on the directory structure to the files beneath it
Public Const RF_IN_PROC		= &H20		' Causes the StartReplication API to run in process (note: This blocks!)
Public Const RF_INCREMENTAL	= &H40		' Runs replication in "Incremental" mode, meaning that only files which have been locally detected to have been modified since the last replication are checked and sent
Public Const RF_FASTMODE	= &H80		' Uses Unframed Protocol for enhanced performance over faster/secure links
Public Const RF_ON_DIR_CHANGE	= &H100		' Turns on monitor mode.  This will monitor a directory for changes and then replicate them to the destination
Public Const RF_AUTO_ROUTE	= &H200		' Uses information in the routing table to determine destination servers
Public Const RF_ON_NOTIFY	= &H400
Public Const RF_NO_CHAIN	= &H800
Public Const RF_EXCLUDE_ALL	= &H1000	' Exclude all subdirectories
Public Const RF_UNINSTALL	= &H2000	' Used for Component Deployment projects (project type = 'ACTIVE')
Public Const RF_ACLS_ONLY	= &H4000	' Only Replicate the ACLs

Public Const RF_DELETE		= &H10000	' Disables RF_NO_DELETE
Public Const RF_NO_REPLICATE_ACLS = &H20000	' Disables RF_REPLICATE_ACLS
Public Const RF_DATA		= &H40000	' Disables RF_NO_DATA
Public Const RF_NO_FORCE_REPL	= &H80000	' Disables RF_FORCE_REPL
Public Const RF_NO_PERMIT_TREE	= &H100000	' Disables RF_PERMIT_TREE
Public Const RF_NO_IN_PROC	= &H200000	' Disables RF_IN_PROC
Public Const RF_NO_INCREMENTAL	= &H400000	' Disables RF_INCREMENTAL
Public Const RF_NO_FASTMODE	= &H800000	' Disables RF_FASTMODE
Public Const RF_NO_ON_DIR_CHANGE = &H1000000	' Disables RF_ON_DIR_CHANGE
Public Const RF_NO_AUTO_ROUTE	= &H2000000	' Disables RF_AUTO_ROUTE
Public Const RF_NO_ON_NOTIFY	= &H4000000	' Disables RF_ON_NOTIFY
Public Const RF_CHAIN		= &H8000000	' Disables RF_ON_CHAIN
Public Const RF_NO_ACLS_ONLY	= &H40000000	' Disables RF_ACLS_ONLY


'
' Replication State Flags
'
Public Const REPL_STATE_EMPTY	   = 0		' This Replication Info structure is empty
Public Const REPL_STATE_STARTING   = 1		' The replication is a startup state
Public Const REPL_STATE_RUNNING    = 2		' The replication is currently running
Public Const REPL_STATE_COMPLETE   = 3		' The replication is complete
Public Const REPL_STATE_ABORTED    = 4		' The replication aborted due to an error
Public Const REPL_STATE_CANCELED   = 5		' The replication was canceled
Public Const REPL_STATE_RECEIVING  = 6		' The replication is receiving data
Public Const REPL_STATE_PENDING    = 7		' The replication is pending
Public Const REPL_STATE_PARTIAL    = 8		' The replication completed with errors


'
' AccessMask Flags
'
Public Const CRS_USER_ACCESS	= &H20019	' KEY_READ
Public Const CRS_ADMIN_ACCESS	= &HF003F	' KEY_ALL_ACCESS Or READ_CONTROL
