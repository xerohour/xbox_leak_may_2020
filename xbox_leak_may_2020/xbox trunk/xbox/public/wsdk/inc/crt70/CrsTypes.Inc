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
'    crstypes.inc
'
' Abstract:
'
'    This module defines the common structures and prototypes for the   
'    Content Deployment Service API (ProgId = CrsApi)
'
' Revision History:
'


' CRS_PROJECT_CREATION
Const CREATE_NEW_PROJECT = &h1
Const OPEN_EXISTING_PROJECT = &h2

' CRS_SERVER_TYPES
Const STAGER = &h1
Const ENDPOINT = &h2

' CRS_SCHEDULE_ACTION
Const CRS_SCHEDULE_APPLY = &h1
Const CRS_SCHEDULE_REPLICATE = &h2

'
' Parameter maximums
'
Const MAX_PATH        = 260		' Max Directory Path Length
Const MAX_PROJECT     = 50		' Max Project Name Length
Const MAX_LOCATION    = 300
Const MAX_DESTINATION_BUFFER = &h1000	' Max Length of All Destinations
Const MAX_ROUTE_NAME  = 50		' Max Route Name Length
Const MAX_REPLID	     = 40		' Max Length of Replication ID GUID
Const MAX_PARM_STRING = 300		' Max Length of Parameter/Property Name


'
' Replication Flags
'
' Note: 'Or' the values together to use combinations, e.g. (RF_FASTMODE Or RF_FORCE_REPL)
'
Const RF_NO_DELETE	= &H1		' Turns off Delete Processing
Const RF_REPLICATE_ACLS	= &H2		' Replicates ACLs
Const RF_NO_DATA		= &H4		' Skips replicating Data, will only replicate directories.
Const RF_FORCE_REPL	= &H8		' Forces data to be replicated without checking first if files exist on destination
Const RF_PERMIT_TREE	= &H10		' Applies ACLs on the directory structure to the files beneath it
Const RF_IN_PROC		= &H20		' Causes the StartReplication API to run in process (note: This blocks!)
Const RF_INCREMENTAL	= &H40		' Runs replication in "Incremental" mode, meaning that only files which have been locally detected to have been modified since the last replication are checked and sent
Const RF_FASTMODE	= &H80		' Uses Unframed Protocol for enhanced performance over faster/secure links
Const RF_ON_DIR_CHANGE	= &H100		' Turns on monitor mode.  This will monitor a directory for changes and then replicate them to the destination
Const RF_AUTO_ROUTE	= &H200		' Uses information in the routing table to determine destination servers
Const RF_ON_NOTIFY	= &H400
Const RF_NO_CHAIN	= &H800
Const RF_EXCLUDE_ALL	= &H1000	' Exclude all subdirectories
Const RF_UNINSTALL	= &H2000		' Used for Component Deployment projects (project type = 'ACTIVE')
Const RF_ACLS_ONLY	= &H4000	' Only Replicate the ACLs

Const RF_DELETE		= &H10000	' Disables RF_NO_DELETE
Const RF_NO_REPLICATE_ACLS = &H20000	' Disables RF_REPLICATE_ACLS
Const RF_DATA		= &H40000	' Disables RF_NO_DATA
Const RF_NO_FORCE_REPL	= &H80000	' Disables RF_FORCE_REPL
Const RF_NO_PERMIT_TREE	= &H100000	' Disables RF_PERMIT_TREE
Const RF_NO_IN_PROC	= &H200000	' Disables RF_IN_PROC
Const RF_NO_INCREMENTAL	= &H400000	' Disables RF_INCREMENTAL
Const RF_NO_FASTMODE	= &H800000	' Disables RF_FASTMODE
Const RF_NO_ON_DIR_CHANGE = &H1000000	' Disables RF_ON_DIR_CHANGE
Const RF_NO_AUTO_ROUTE	= &H2000000	' Disables RF_AUTO_ROUTE
Const RF_NO_ON_NOTIFY	= &H4000000	' Disables RF_ON_NOTIFY
Const RF_CHAIN		= &H8000000	' Disables RF_ON_CHAIN
Const RF_NO_ACLS_ONLY	= &H40000000	' Disables RF_ACLS_ONLY


'
' Replication State Flags
'
Const REPL_STATE_EMPTY	    = 0		' This Replication Info structure is empty
Const REPL_STATE_STARTING   = 1		' The replication is a startup state
Const REPL_STATE_RUNNING    = 2		' The replication is currently running
Const REPL_STATE_COMPLETE   = 3		' The replication is complete
Const REPL_STATE_ABORTED    = 4		' The replication aborted due to an error
Const REPL_STATE_CANCELED   = 5		' The replication was canceled
Const REPL_STATE_RECEIVING  = 6		' The replication is receiving data
Const REPL_STATE_PENDING    = 7		' The replication is pending
Const REPL_STATE_PARTIAL    = 8		' The replication completed with errors


'
' AccessMask Flags
'
Const CRS_USER_ACCESS	= &H20019	' KEY_READ
Const CRS_ADMIN_ACCESS	= &HF003F	' KEY_ALL_ACCESS Or READ_CONTROL
