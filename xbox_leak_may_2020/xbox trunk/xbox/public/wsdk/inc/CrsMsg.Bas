Attribute VB_Name = "CrsMsg"
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

'
'  Values are 32 bit values layed out as follows:
'
'   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
'   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
'  +---+-+-+-----------------------+-------------------------------+
'  |Sev|C|R|     Facility          |               Code            |
'  +---+-+-+-----------------------+-------------------------------+
'
'  where
'
'      Sev - is the severity code
'
'          00 - Success
'          01 - Informational
'          10 - Warning
'          11 - Error
'
'      C - is the Customer code flag
'
'      R - is a reserved bit
'
'      Facility - is the facility code
'
'      Code - is the facility's status code
'
'
' Define the facility codes
'


'
' Define the severity codes
'


'
' MessageId: CRS_ERROR_FIRST
'
' MessageText:
'
'  Site Server Content Deployment service first error message.
'
Public Const CRS_ERROR_FIRST                  = &H40003A98

'
' MessageId: CRS_NET_SOURCE
'
' MessageText:
'
'  Source directory %2 for the project %1 is on a network drive.  Cannot deploy from network drives.
'
Public Const CRS_NET_SOURCE                   = &HC0003A9C

'
' MessageId: CRS_ERROR_ACCESS_DENIED
'
' MessageText:
'
'  Access denied.
'
Public Const CRS_ERROR_ACCESS_DENIED          = &HC0003A9D

'
' MessageId: CRS_FILE_CREATE_ERROR
'
' MessageText:
'
'  Unable to open the file %1.
'
Public Const CRS_FILE_CREATE_ERROR            = &HC0003A9E

'
' MessageId: CRS_SENDINET_ERROR
'
' MessageText:
'
'  Project was unable to send file %1.
'
Public Const CRS_SENDINET_ERROR               = &HC0003A9F

'
' MessageId: CRS_SAMEBOX_NET_TARGET
'
' MessageText:
'
'  Destination directory %2 for the project %1 is on a network drive.  Cannot deploy to network drives.
'
Public Const CRS_SAMEBOX_NET_TARGET           = &HC0003AA0

'
' MessageId: CRS_BAD_SOURCE
'
' MessageText:
'
'  Unable to access the project source %1.
'
Public Const CRS_BAD_SOURCE                   = &HC0003AA1

'
' MessageId: CRS_SENDINET_CONNECT
'
' MessageText:
'
'  Unable to connect to the remote server %1.
'
Public Const CRS_SENDINET_CONNECT             = &HC0003AA2

'
' MessageId: CRS_HASH_MISMATCH
'
' MessageText:
'
'  MD5 Hash did not match during a transmission.  This could indicate transmission failure or that the files transmitted at the source have been modified in transit.
'
Public Const CRS_HASH_MISMATCH                = &HC0003AA3

'
' MessageId: CRS_SERVER_ABORTED
'
' MessageText:
'
'  Remote server was restarted.  The project %1 is being restarted.
'
Public Const CRS_SERVER_ABORTED               = &HC0003AA4

'
' MessageId: CRS_REPLICATION_ABORTED
'
' MessageText:
'
'  Project %1 was aborted.
'
Public Const CRS_REPLICATION_ABORTED          = &HC0003AA5

'
' MessageId: CRS_SEQUENCE_ERROR
'
' MessageText:
'
'  An internal messaging sequence error occurred.
'
Public Const CRS_SEQUENCE_ERROR               = &HC0003AA6

'
' MessageId: CRS_SERVICE_STARTED
'
' MessageText:
'
'  Site Server Content Deployment service started successfully.
'
Public Const CRS_SERVICE_STARTED              = &H40003AA7

'
' MessageId: CRS_SERVICE_SHUTDOWN
'
' MessageText:
'
'  Site Server Content Deployment service shut down successfully.
'
Public Const CRS_SERVICE_SHUTDOWN             = &H40003AA8

'
' MessageId: CRS_ERROR_METABASE_ACCESS
'
' MessageText:
'
'  Could not open storage/stream %1.  Ensure the authentication account has access to this storage/stream.
'
Public Const CRS_ERROR_METABASE_ACCESS        = &HC0003AAA

'
' MessageId: CRS_AUTH_FAILURE
'
' MessageText:
'
'  Authorization failure against server %1.  Ensure the service has access to this server.
'
Public Const CRS_AUTH_FAILURE                 = &HC0003AAB

'
' MessageId: CRS_REMOTE_AUTH_FAILURE
'
' MessageText:
'
'  Remote authorization failed to server %1.  Ensure the service has access to this server.
'
Public Const CRS_REMOTE_AUTH_FAILURE          = &HC0003AAC

'
' MessageId: CRS_ERROR_NOT_IN_ORDER
'
' MessageText:
'
'  Message was received out of order.  Closing connection.
'
Public Const CRS_ERROR_NOT_IN_ORDER           = &HC0003AAD

'
' MessageId: CRS_CONNECTION_RETRY_FAILED
'
' MessageText:
'
'  Connection to server %1 failed and could not be re-established.
'
Public Const CRS_CONNECTION_RETRY_FAILED      = &HC0003AAE

'
' MessageId: CRS_ERROR_REPLICATION_CANCELED
'
' MessageText:
'
'  Project %1 canceled at the request of the user.
'
Public Const CRS_ERROR_REPLICATION_CANCELED   = &HC0003AAF

'
' MessageId: CRS_ERROR_DEST_TOOBIG
'
' MessageText:
'
'  Could not add the specified destination(s).  The maximum number of destinations has been reached.
'
Public Const CRS_ERROR_DEST_TOOBIG            = &HC0003AB0

'
' MessageId: CRS_ERROR_NO_IP_MAPPING
'
' MessageText:
'
'  Cannot resolve IP address for server %1.  No virtual root created.
'
Public Const CRS_ERROR_NO_IP_MAPPING          = &HC0003AB1

'
' MessageId: CRS_ERROR_DISK_FULL
'
' MessageText:
'
'  Not enough space on the disk.
'
Public Const CRS_ERROR_DISK_FULL              = &HC0003AB2

'
' MessageId: CRS_ERROR_TOO_MANY_RUNNING
'
' MessageText:
'
'  Too many active projects. No more can be started until others complete.
'
Public Const CRS_ERROR_TOO_MANY_RUNNING       = &HC0003AB3

'
' MessageId: CRS_ERROR_WOULDBLOCK
'
' MessageText:
'
'  The network operation would block.
'
Public Const CRS_ERROR_WOULDBLOCK             = &H00003ABB

'
' MessageId: CRS_ERROR_NETDOWN
'
' MessageText:
'
'  The remote network is down.
'
Public Const CRS_ERROR_NETDOWN                = &HC0003ACA

'
' MessageId: CRS_ERROR_NETUNREACH
'
' MessageText:
'
'  The remote network is unreachable.
'
Public Const CRS_ERROR_NETUNREACH             = &HC0003ACB

'
' MessageId: CRS_ERROR_CONNABORTED
'
' MessageText:
'
'  The network connection was aborted at the network layer.
'
Public Const CRS_ERROR_CONNABORTED            = &HC0003ACD

'
' MessageId: CRS_ERROR_TIMEOUT
'
' MessageText:
'
'  The network session timed out on a send or receive operation.
'
Public Const CRS_ERROR_TIMEOUT                = &HC0003AD4

'
' MessageId: WINSOCK_WSAEWOULDBLOCK
'
' MessageText:
'
'  The network operation would block.
'
Public Const WINSOCK_WSAEWOULDBLOCK           = &H00002733

'
' MessageId: WINSOCK_WSAENETDOWN
'
' MessageText:
'
'  The remote network is down.
'
Public Const WINSOCK_WSAENETDOWN              = &H00002742

'
' MessageId: WINSOCK_WSAENETUNREACH
'
' MessageText:
'
'  The remote network is unreachable.
'
Public Const WINSOCK_WSAENETUNREACH           = &H00002743

'
' MessageId: WINSOCK_WSAECONNABORTED
'
' MessageText:
'
'  The network connection was aborted at the network layer.
'
Public Const WINSOCK_WSAECONNABORTED          = &H00002745

'
' MessageId: WINSOCK_WSAECONNRESET
'
' MessageText:
'
'  The network session was aborted.
'
Public Const WINSOCK_WSAECONNRESET            = &H00002746

'
' MessageId: WINSOCK_WSAETIMEDOUT
'
' MessageText:
'
'  The network session timed out on a send or receive operation.
'
Public Const WINSOCK_WSAETIMEDOUT             = &H0000274C

'
' MessageId: WINSOCK_WSAECONNREFUSED
'
' MessageText:
'
'  The remote server was not listening on the appropriate port. This generally indicates that the service was not running but the server is up.
'
Public Const WINSOCK_WSAECONNREFUSED          = &H0000274D

'
' MessageId: CRS_ERROR_NO_PROJECTS
'
' MessageText:
'
'  No projects defined.
'
Public Const CRS_ERROR_NO_PROJECTS            = &HC0003AFC

'
' MessageId: CRS_ERROR_PROJECT_NOT_FOUND
'
' MessageText:
'
'  Could not find project %1.
'
Public Const CRS_ERROR_PROJECT_NOT_FOUND      = &HC0003AFD

'
' MessageId: CRS_ERROR_INVALID_PARAMETER
'
' MessageText:
'
'  Invalid parameter.
'
Public Const CRS_ERROR_INVALID_PARAMETER      = &HC0003AFE

'
' MessageId: CRS_PROJECT_CREATED
'
' MessageText:
'
'  Created project %1.
'
Public Const CRS_PROJECT_CREATED              = &HC0003AFF

'
' MessageId: CRS_ERROR_AUTHENTICATION_FAILURE
'
' MessageText:
'
'  Authentication failure.  Ensure the account has proper access on local and remote servers.
'
Public Const CRS_ERROR_AUTHENTICATION_FAILURE = &HC0003B00

'
' MessageId: CRS_ERROR_DELETE_PROJECT_DOWN_ROUTE
'
' MessageText:
'
'  Could not delete the route from the project on at least one server down the defined routes.  The project will have to be edited manually there.
'
Public Const CRS_ERROR_DELETE_PROJECT_DOWN_ROUTE = &H80003B01

'
' MessageId: CRS_ERROR_SERVICE_NOT_RUNNING
'
' MessageText:
'
'  Server %1 is available, but the Site Server Content Deployment service is not running.
'
Public Const CRS_ERROR_SERVICE_NOT_RUNNING    = &HC0003B02

'
' MessageId: CRS_ERROR_HOST_DOWN
'
' MessageText:
'
'  Server %1 is unavailable.
'
Public Const CRS_ERROR_HOST_DOWN              = &HC0003B03

'
' MessageId: CRS_ERROR_BAD_NET_NAME
'
' MessageText:
'
'  Server %1 cannot be found.
'
Public Const CRS_ERROR_BAD_NET_NAME           = &HC0003B04

'
' MessageId: CRS_ERROR_ROUTE_NOT_FOUND
'
' MessageText:
'
'  Could not find route %1.  Ensure the route name is correct and that it exists on the target servers.
'
Public Const CRS_ERROR_ROUTE_NOT_FOUND        = &HC0003B05

'
' MessageId: CRS_ERROR_NO_ROUTES
'
' MessageText:
'
'  No routes defined.
'
Public Const CRS_ERROR_NO_ROUTES              = &HC0003B06

'
' MessageId: CRS_ERROR_CREATE_PROJECT_DOWN_ROUTE
'
' MessageText:
'
'  Could not create the project on at least one server down the defined routes.  The project will have to be created manually there.
'
Public Const CRS_ERROR_CREATE_PROJECT_DOWN_ROUTE = &H80003B07

'
' MessageId: CRS_ERROR_SERVICE_PAUSED
'
' MessageText:
'
'  Site Server Content Deployment service on server %1 is paused and is not accepting new requests.  Retry project %2 later.
'
Public Const CRS_ERROR_SERVICE_PAUSED         = &HC0003B08

'
' MessageId: CRS_ERROR_SERVICE_NOT_PAUSED
'
' MessageText:
'
'  Could not restart service. Service is not paused.
'
Public Const CRS_ERROR_SERVICE_NOT_PAUSED     = &HC0003B09

'
' MessageId: CRS_ERROR_NAME_NOT_RESOLVED
'
' MessageText:
'
'  Could not resolve URL %2 for project %1.  Ensure URL and/or proxy settings are correct.
'
Public Const CRS_ERROR_NAME_NOT_RESOLVED      = &HC0003B0A

'
' MessageId: CRS_ERROR_CONNECTION_RESET
'
' MessageText:
'
'  Connection to server %1 has been reset.  This indicates problems with the remote service or network.
'
Public Const CRS_ERROR_CONNECTION_RESET       = &HC0003B0B

'
' MessageId: CRS_ERROR_REPLICATION_NOT_FOUND
'
' MessageText:
'
'  No entries.
'
Public Const CRS_ERROR_REPLICATION_NOT_FOUND  = &HC0003B0C

'
' MessageId: CRS_ERROR_SERVICE_ALREADY_RUNNING
'
' MessageText:
'
'  Could not start service.  The service has already been started.
'
Public Const CRS_ERROR_SERVICE_ALREADY_RUNNING = &HC0003B0D

'
' MessageId: CRS_ERROR_PROJECT_ALREADY_RUNNING
'
' MessageText:
'
'  Failed to start project %1.  It is already running.
'
Public Const CRS_ERROR_PROJECT_ALREADY_RUNNING = &HC0003B0E

'
' MessageId: CRS_ERROR_FILE_MOVE_ERROR
'
' MessageText:
'
'  Failed to move file %1 to %2.
'
Public Const CRS_ERROR_FILE_MOVE_ERROR        = &HC0003B0F

'
' MessageId: CRS_ERROR_PRODUCT_EXPIRED
'
' MessageText:
'
'  Thank you for your interest in Site Server Content Deployment.  This software has expired.  Please acquire the final retail version of this software.
'
Public Const CRS_ERROR_PRODUCT_EXPIRED        = &HC0003B10

'
' MessageId: CRS_ERROR_PROJECT_INCOMPLETE
'
' MessageText:
'
'  Project information for %1 is incomplete.
'
Public Const CRS_ERROR_PROJECT_INCOMPLETE     = &HC0003B11

'
' MessageId: CRS_ERROR_PARAMETER_NOT_FOUND
'
' MessageText:
'
'  Parameter %1 not found.
'
Public Const CRS_ERROR_PARAMETER_NOT_FOUND    = &HC0003B12

'
' MessageId: CRS_ERROR_REPLICATION_RUNNING
'
' MessageText:
'
'  Project %1 is running.  The operation could not be completed.
'
Public Const CRS_ERROR_REPLICATION_RUNNING    = &HC0003B13

'
' MessageId: CRS_ERROR_INVALID_PROJECT_NAME
'
' MessageText:
'
'  Project name %1 is invalid.  Project names cannot contain backslashes and cannot be empty.
'
Public Const CRS_ERROR_INVALID_PROJECT_NAME   = &HC0003B14

'
' MessageId: CRS_ERROR_INVALID_ROUTE_NAME
'
' MessageText:
'
'  Route name %1 is invalid.  Route names cannot contain backslashes or DBCS characters and cannot be empty.
'
Public Const CRS_ERROR_INVALID_ROUTE_NAME     = &HC0003B15

'
' MessageId: CRS_ERROR_INVALID_DWORD
'
' MessageText:
'
'  Parameter %1 must be a positive integer.
'
Public Const CRS_ERROR_INVALID_DWORD          = &HC0003B16

'
' MessageId: CRS_ERROR_NO_MORE_ITEMS
'
' MessageText:
'
'  No more items in the list.
'
Public Const CRS_ERROR_NO_MORE_ITEMS          = &H80003B17

'
' MessageId: CRS_ERROR_TOO_MANY_ARGS
'
' MessageText:
'
'  Too many arguments specified.
'
Public Const CRS_ERROR_TOO_MANY_ARGS          = &HC0003B18

'
' MessageId: CRS_MBS_INIT_ERROR
'
' MessageText:
'
'  Internal error trying to initialize for ACL replication.
'
Public Const CRS_MBS_INIT_ERROR               = &HC0003B19

'
' MessageId: CRS_ERROR_OPENING_FILE
'
' MessageText:
'
'  Error opening file %1 or file not found.
'
Public Const CRS_ERROR_OPENING_FILE           = &HC0003B1B

'
' MessageId: CRS_ERROR_FILE_FORMAT
'
' MessageText:
'
'  Invalid file format.
'
Public Const CRS_ERROR_FILE_FORMAT            = &HC0003B1C

'
' MessageId: CRS_ERROR_FILE_WRITE
'
' MessageText:
'
'  Error writing to file %1.
'
Public Const CRS_ERROR_FILE_WRITE             = &HC0003B1D

'
' MessageId: WINSOCK_WSAHOSTNAME
'
' MessageText:
'
'  Gethostname failed.
'
Public Const WINSOCK_WSAHOSTNAME              = &HC0003B1E

'
' MessageId: CRS_ERROR_NODE_EXISTS
'
' MessageText:
'
'  Node already exists in the topology map.
'
Public Const CRS_ERROR_NODE_EXISTS            = &HC0003B1F

'
' MessageId: CRS_ERROR_NODE_NOT_FOUND
'
' MessageText:
'
'  Node not found in topology map.
'
Public Const CRS_ERROR_NODE_NOT_FOUND         = &HC0003B20

'
' MessageId: CRS_ERROR_EDGE_EXISTS
'
' MessageText:
'
'  Edge already exists in topology map.
'
Public Const CRS_ERROR_EDGE_EXISTS            = &H80003B21

'
' MessageId: CRS_ERROR_ROOTNODE_NOT_FOUND
'
' MessageText:
'
'  Root node does not exist in topology map.
'
Public Const CRS_ERROR_ROOTNODE_NOT_FOUND     = &HC0003B22

'
' MessageId: CRS_ERROR_PATH_NOT_FOUND
'
' MessageText:
'
'  No path to from root to node in topology map.
'
Public Const CRS_ERROR_PATH_NOT_FOUND         = &HC0003B23

'
' MessageId: CRS_ERROR_EDGE_NOT_FOUND
'
' MessageText:
'
'  Edge does not exist in topology map.
'
Public Const CRS_ERROR_EDGE_NOT_FOUND         = &HC0003B24

'
' MessageId: CRS_ERROR_INVALID_CONTEXT
'
' MessageText:
'
'  Current context is invalid.  Project being reset.
'
Public Const CRS_ERROR_INVALID_CONTEXT        = &HC0003B25

'
' MessageId: CRS_ERROR_DOWNLEVEL_SERVER
'
' MessageText:
'
'  Operation is not supported on a down-level server.
'
Public Const CRS_ERROR_DOWNLEVEL_SERVER       = &HC0003B26

'
' MessageId: CRS_ERROR_NO_MAPPING
'
' MessageText:
'
'  URL does not map to any project definitions.
'
Public Const CRS_ERROR_NO_MAPPING             = &HC0003B27

'
' MessageId: CRS_ERROR_START_PENDING
'
' MessageText:
'
'  Start pending completion of the active project.
'
Public Const CRS_ERROR_START_PENDING          = &HC0003B28

'
' MessageId: CRS_ERROR_NO_EVENTSINKS
'
' MessageText:
'
'  No event sinks defined.
'
Public Const CRS_ERROR_NO_EVENTSINKS          = &HC0003B29

'
' MessageId: CRS_ERROR_EVENTSINK_NOT_FOUND
'
' MessageText:
'
'  Could not find event sink %1.
'
Public Const CRS_ERROR_EVENTSINK_NOT_FOUND    = &HC0003B2A

'
' MessageId: CRS_ERROR_WSAHOSTNOTFOUND
'
' MessageText:
'
'  Host not found.
'
Public Const CRS_ERROR_WSAHOSTNOTFOUND        = &HC0003B2B

'
' MessageId: CRS_ERROR_WSATRYAGAIN
'
' MessageText:
'
'  Host not found, or SERVERFAIL.  Try again.
'
Public Const CRS_ERROR_WSATRYAGAIN            = &HC0003B2C

'
' MessageId: CRS_ERROR_WSANORECOVERY
'
' MessageText:
'
'  Non-recoverable winsock error.
'
Public Const CRS_ERROR_WSANORECOVERY          = &HC0003B2D

'
' MessageId: CRS_ERROR_WSANODATA
'
' MessageText:
'
'  Invalid address.
'
Public Const CRS_ERROR_WSANODATA              = &HC0003B2E

'
' MessageId: CRS_EVENTSINK_INITIALIZED
'
' MessageText:
'
'  Event sink %1 was successfully initialized.
'
Public Const CRS_EVENTSINK_INITIALIZED        = &H00003B2F

'
' MessageId: CRS_ERROR_EVENTSINK_NOT_INITIALIZED
'
' MessageText:
'
'  Event sink %1 failed to initialize.
'
Public Const CRS_ERROR_EVENTSINK_NOT_INITIALIZED = &HC0003B30

'
' MessageId: CRS_ERROR_NOTHING_TO_ROLLBACK
'
' MessageText:
'
'  Could not roll back project.  No rollback information found.
'
Public Const CRS_ERROR_NOTHING_TO_ROLLBACK    = &HC0003B31

'
' MessageId: CRS_ERROR_NO_DESTINATIONS
'
' MessageText:
'
'  Project could not be started because no destinations are defined.
'
Public Const CRS_ERROR_NO_DESTINATIONS        = &HC0003B32

'
' MessageId: CRS_ERROR_REAUTHENTICATE
'
' MessageText:
'
'  Re-authenticate using an acceptable protocol.
'
Public Const CRS_ERROR_REAUTHENTICATE         = &HC0003B33

'
' MessageId: CRS_ERROR_UNSUPPORTED
'
' MessageText:
'
'  Function not supported in this version to this server.
'
Public Const CRS_ERROR_UNSUPPORTED            = &HC0003B34

'
' MessageId: CRS_ERROR_USER_NOT_FOUND
'
' MessageText:
'
'  Specified user %1 could not be found.
'
Public Const CRS_ERROR_USER_NOT_FOUND         = &HC0003B35

'
' MessageId: CRS_ERROR_NO_TRANSACTIONS
'
' MessageText:
'
'  No transactions for the specified project.
'
Public Const CRS_ERROR_NO_TRANSACTIONS        = &HC0003B36

'
' MessageId: CRS_ERROR_NO_SERVERS
'
' MessageText:
'
'  No destination servers defined.
'
Public Const CRS_ERROR_NO_SERVERS             = &HC0003B37

'
' MessageId: CRS_ERROR_NO_SCHEDULES
'
' MessageText:
'
'  No schedules defined for this project.
'
Public Const CRS_ERROR_NO_SCHEDULES           = &HC0003B38

'
' MessageId: CRS_ERROR_NO_EMAIL
'
' MessageText:
'
'  Could not contact SMTP server %1 to send e-mail report for project %2.
'
Public Const CRS_ERROR_NO_EMAIL               = &HC0003B39

'
' MessageId: CRS_ERROR_WAIT_ABORT
'
' MessageText:
'
'  Waiting abort on project %1 after losing connection with source server.
'
Public Const CRS_ERROR_WAIT_ABORT             = &HC0003B3A

'
' MessageId: CRS_RESUME_REPLICATION
'
' MessageText:
'
'  Resuming project %1 to server %2.
'
Public Const CRS_RESUME_REPLICATION           = &H40003B3B

'
' MessageId: CRS_METABASE_NOT_FOUND
'
' MessageText:
'
'  Metabase is not responding.
'
Public Const CRS_METABASE_NOT_FOUND           = &HC0003B3C

'
' MessageId: CRS_SSE_VROOT_NOT_FOUND
'
' MessageText:
'
'  Site Server virtual directory not found.
'
Public Const CRS_SSE_VROOT_NOT_FOUND          = &HC0003B3D

'
' MessageId: CRS_WARNING_CONNECTION_FAILED
'
' MessageText:
'
'  Connection to server %1 for project %2 failed.  Will attempt to re-establish session.
'
Public Const CRS_WARNING_CONNECTION_FAILED    = &H80003B3E

'
' MessageId: CRS_ERROR_DROPPED_EMAIL_REPORT
'
' MessageText:
'
'  Dropped e-mail report for project %1.
'
Public Const CRS_ERROR_DROPPED_EMAIL_REPORT   = &HC0003B3F

'
' MessageId: CRS_ERROR_EMAIL_HOST_NOT_DEFINED
'
' MessageText:
'
'  No SMTP server defined.
'
Public Const CRS_ERROR_EMAIL_HOST_NOT_DEFINED = &HC0003B40

'
' MessageId: CRS_ERROR_VROOT_CREATION
'
' MessageText:
'
'  Could not create the specified virtual directory.
'
Public Const CRS_ERROR_VROOT_CREATION         = &H80003B41

'
' MessageId: CRS_ERROR_UPLPAGE_CREATION
'
' MessageText:
'
'  Could not create the upload page.
'
Public Const CRS_ERROR_UPLPAGE_CREATION       = &HC0003B42

'
' MessageId: CRS_ERROR_CIRCULAR_ROUTE
'
' MessageText:
'
'  System has detected a circular route.  Project not created.
'
Public Const CRS_ERROR_CIRCULAR_ROUTE         = &HC0003B43

'
' MessageId: CRS_ERROR_TRANSACTION_APPLY_FAILED
'
' MessageText:
'
'  Transaction application for project %1 failed.  Check permissions on the destination directory and files and ensure that no files are locked open.
'
Public Const CRS_ERROR_TRANSACTION_APPLY_FAILED = &HC0003B44

'
' MessageId: CRS_ERROR_DATABASE
'
' MessageText:
'
'  Error occurred with the database %1.  Error is: %2.
'
Public Const CRS_ERROR_DATABASE               = &HC0003B45

'
' MessageId: CRS_ERROR_SINK_CONNECT_FAILED
'
' MessageText:
'
'  Unable to connect to remote server %1 to forward events.
'
Public Const CRS_ERROR_SINK_CONNECT_FAILED    = &HC0003B46

'
' MessageId: CRS_ERROR_CANT_SEND_EVENTS_TO_DOWNLEVEL_SERVER
'
' MessageText:
'
'  Unable to send events to server %1.  This operation is not supported.
'
Public Const CRS_ERROR_CANT_SEND_EVENTS_TO_DOWNLEVEL_SERVER = &HC0003B47

'
' MessageId: CRS_ERROR_FILE_CORRUPT
'
' MessageText:
'
'  File %1 is corrupt or unreadable.
'
Public Const CRS_ERROR_FILE_CORRUPT           = &HC0003B48

'
' MessageId: CRS_ERROR_FILE_BUSY
'
' MessageText:
'
'  File %1 is busy.
'
Public Const CRS_ERROR_FILE_BUSY              = &HC0003B49

'
' MessageId: CRS_ACL_ERROR_INVALID_ACE
'
' MessageText:
'
'  Could not set the ACE %1 on file %2.
'
Public Const CRS_ACL_ERROR_INVALID_ACE        = &H80003B4A

'
' MessageId: CRS_ACL_ERROR_ACL_FAILED
'
' MessageText:
'
'  Could not set the ACLS appropriately on file %1, setting default ACLs on this file.
'
Public Const CRS_ACL_ERROR_ACL_FAILED         = &HC0003B4B

'
' MessageId: CRS_ERROR_PROJECT_NOT_FOUND_ON_DESTINATION
'
' MessageText:
'
'  Could not find project %1 on server %2.
'
Public Const CRS_ERROR_PROJECT_NOT_FOUND_ON_DESTINATION = &HC0003B4C

'
' MessageId: CRS_ERROR_PROJECT_INCOMPLETE_ON_DESTINATION
'
' MessageText:
'
'  Project information for project %1 is incomplete on server %2.
'
Public Const CRS_ERROR_PROJECT_INCOMPLETE_ON_DESTINATION = &HC0003B4D

'
' MessageId: CRS_ERROR_SERVICE_DOWN_ON_REMOTE_SERVER
'
' MessageText:
'
'  Remote server available, but the Site Server Content Deployment service is not running.
'
Public Const CRS_ERROR_SERVICE_DOWN_ON_REMOTE_SERVER = &HC0003B4E

'
' MessageId: CRS_ERROR_DISK_FULL_ON_DESTINATION
'
' MessageText:
'
'  Disk is full on the destination server %1.
'
Public Const CRS_ERROR_DISK_FULL_ON_DESTINATION = &HC0003B4F

'
' MessageId: CRS_FILE_CREATE_ERROR_ON_DESTINATION
'
' MessageText:
'
'  Unable to open a file on the destination server %1.  Check permissions on the project directory and files.
'
Public Const CRS_FILE_CREATE_ERROR_ON_DESTINATION = &HC0003B50

'
' MessageId: CRS_ERROR_REMOTE_SERVER_NOT_FOUND
'
' MessageText:
'
'  Remote server could not be found.
'
Public Const CRS_ERROR_REMOTE_SERVER_NOT_FOUND = &HC0003B51

'
' MessageId: CRS_ERROR_REMOTE_APPLY_FAILED
'
' MessageText:
'
'  Unable to apply transactions for project %1 on server %2.
'
Public Const CRS_ERROR_REMOTE_APPLY_FAILED    = &HC0003B52

'
' MessageId: CRS_ERROR_INVALID_FLAG_COMBINATION
'
' MessageText:
'
'  Invalid flag combination.
'
Public Const CRS_ERROR_INVALID_FLAG_COMBINATION = &HC0003B53

'
' MessageId: CRS_ERROR_RUNNING_SCRIPT
'
' MessageText:
'
'  Error running the script %1.  Ensure that the script exists and can be located in the path.
'
Public Const CRS_ERROR_RUNNING_SCRIPT         = &HC0003B54

'
' MessageId: CRS_ERROR_CANT_FORWARD_TO_LOCAL_SERVER
'
' MessageText:
'
'  Cannot forward events to the local server.
'
Public Const CRS_ERROR_CANT_FORWARD_TO_LOCAL_SERVER = &HC0003B55

'
' MessageId: CRS_ERROR_SCHEDULE_NOT_FOUND
'
' MessageText:
'
'  Specified project schedule could not be found.
'
Public Const CRS_ERROR_SCHEDULE_NOT_FOUND     = &HC0003B56

'
' MessageId: CRS_ERROR_METABASE
'
' MessageText:
'
'  An error occurred accessing the metabase for the project %1.  Ensure that the web site exists and that the account has access to it.
'
Public Const CRS_ERROR_METABASE               = &HC0003B57

'
' MessageId: CRS_ERROR_SCHEDULER_FAILURE
'
' MessageText:
'
'  An error occurred with the task scheduler.
'
Public Const CRS_ERROR_SCHEDULER_FAILURE      = &HC0003B58

'
' MessageId: CRS_SAMEBOX_META_TARGET
'
' MessageText:
'
'  The destination %1 for metabase replication is invalid.
'
Public Const CRS_SAMEBOX_META_TARGET          = &HC0003B59

'
' MessageId: CRS_ERROR_DROPPED_EVENTS
'
' MessageText:
'
'  The event sink %1 has started dropping events.
'
Public Const CRS_ERROR_DROPPED_EVENTS         = &HC0003B5A

'
' MessageId: CRS_MSG_REPLICATION_FIRST
'
' MessageText:
'
'  Site Server Content Deployment first replication event message.
'
Public Const CRS_MSG_REPLICATION_FIRST        = &H40003BC4

'
' MessageId: CRS_MSG_STARTING_REPLICATION
'
' MessageText:
'
'  Project %1 starting to server %2.
'
Public Const CRS_MSG_STARTING_REPLICATION     = &H40003BC5

'
' MessageId: CRS_MSG_REPLICATION_COMPLETE
'
' MessageText:
'
'  Project %1 complete to server %2.
'
Public Const CRS_MSG_REPLICATION_COMPLETE     = &H40003BC6

'
' MessageId: CRS_MSG_STARTING_PULL_REPLICATION
'
' MessageText:
'
'  Retrieval project %1 starting from URL %2.
'
Public Const CRS_MSG_STARTING_PULL_REPLICATION = &H40003BC7

'
' MessageId: CRS_MSG_PULL_REPLICATION_COMPLETE
'
' MessageText:
'
'  Retrieval project %1 complete from URL %2.
'
Public Const CRS_MSG_PULL_REPLICATION_COMPLETE = &H40003BC8

'
' MessageId: CRS_MSG_STARTING_RECEIVE_REPLICATION
'
' MessageText:
'
'  Starting to receive project %1 from server %2.
'
Public Const CRS_MSG_STARTING_RECEIVE_REPLICATION = &H40003BC9

'
' MessageId: CRS_MSG_REPLICATION_RECEIVE_COMPLETE
'
' MessageText:
'
'  Finished receiving project %1 from server %2.
'
Public Const CRS_MSG_REPLICATION_RECEIVE_COMPLETE = &H40003BCA

'
' MessageId: CRS_MSG_STARTING_REPLICATION_SCAN
'
' MessageText:
'
'  Project %1 starting.
'
Public Const CRS_MSG_STARTING_REPLICATION_SCAN = &H40003BCB

'
' MessageId: CRS_MSG_REPLICATION_LAST
'
' MessageText:
'
'  Site Server Content Deployment last replication event message.
'
Public Const CRS_MSG_REPLICATION_LAST         = &H40003BF5

'
' MessageId: CRS_MSG_FILE_FIRST
'
' MessageText:
'
'  Site Server Content Deployment first file event message.
'
Public Const CRS_MSG_FILE_FIRST               = &H40003BF6

'
' MessageId: CRS_MSG_SENDING_FILE
'
' MessageText:
'
'  Sending file %1.
'
Public Const CRS_MSG_SENDING_FILE             = &H40003BF7

'
' MessageId: CRS_MSG_SENT_FILE
'
' MessageText:
'
'  File %1 sent.
'
Public Const CRS_MSG_SENT_FILE                = &H40003BF8

'
' MessageId: CRS_MSG_RECEIVING_FILE
'
' MessageText:
'
'  Receiving file %1.
'
Public Const CRS_MSG_RECEIVING_FILE           = &H40003BF9

'
' MessageId: CRS_MSG_RECEIVED_FILE
'
' MessageText:
'
'  File %1 received.
'
Public Const CRS_MSG_RECEIVED_FILE            = &H40003BFA

'
' MessageId: CRS_MSG_SENT_DELETE_FILE
'
' MessageText:
'
'  Sent delete file request for file %1.
'
Public Const CRS_MSG_SENT_DELETE_FILE         = &H40003BFB

'
' MessageId: CRS_MSG_RECEIVED_DELETE_FILE
'
' MessageText:
'
'  File delete request received for file %1.
'
Public Const CRS_MSG_RECEIVED_DELETE_FILE     = &H40003BFC

'
' MessageId: CRS_MSG_RETRYING_SENDING_FILE
'
' MessageText:
'
'  Trying to resend file %1.
'
Public Const CRS_MSG_RETRYING_SENDING_FILE    = &H40003C00

'
' MessageId: CRS_MSG_FILE_BUSY
'
' MessageText:
'
'  File %1 is busy.  The service is waiting for the file to become available.
'
Public Const CRS_MSG_FILE_BUSY                = &H40003C01

'
' MessageId: CRS_MSG_SENDING_ACL
'
' MessageText:
'
'  Sending the ACL for the file %1.
'
Public Const CRS_MSG_SENDING_ACL              = &H40003C02

'
' MessageId: CRS_MSG_SENT_ACL
'
' MessageText:
'
'  Sent the ACL for the file %1.
'
Public Const CRS_MSG_SENT_ACL                 = &H40003C03

'
' MessageId: CRS_MSG_RECEIVING_ACL
'
' MessageText:
'
'  Receiving the ACL for the file %1.
'
Public Const CRS_MSG_RECEIVING_ACL            = &H40003C04

'
' MessageId: CRS_MSG_RECEIVED_ACL
'
' MessageText:
'
'  Received the ACL for the file %1.
'
Public Const CRS_MSG_RECEIVED_ACL             = &H40003C05

'
' MessageId: CRS_MSG_PULLED_FILE_HTTP
'
' MessageText:
'
'  File %1 retrieved over HTTP.
'
Public Const CRS_MSG_PULLED_FILE_HTTP         = &H40003C06

'
' MessageId: CRS_MSG_PULLED_FILE_FTP
'
' MessageText:
'
'  File %1 retrieved over FTP.
'
Public Const CRS_MSG_PULLED_FILE_FTP          = &H40003C07

'
' MessageId: CRS_MSG_FILE_COMMITTED
'
' MessageText:
'
'  File %1 committed to disk.
'
Public Const CRS_MSG_FILE_COMMITTED           = &H40003C08

'
' MessageId: CRS_MSG_FILE_DELETED
'
' MessageText:
'
'  File %1 deleted.
'
Public Const CRS_MSG_FILE_DELETED             = &H40003C09

'
' MessageId: CRS_MSG_SKIPPING_LOCKED_FILE
'
' MessageText:
'
'  File %1 skipped because it could not be opened.
'
Public Const CRS_MSG_SKIPPING_LOCKED_FILE     = &H80003C0A

'
' MessageId: CRS_FILE_MATCHED
'
' MessageText:
'
'  File %1 matched.
'
Public Const CRS_FILE_MATCHED                 = &H40003C0B

'
' MessageId: CRS_MSG_FILE_LAST
'
' MessageText:
'
'  Site Server Content Deployment last file event message.
'
Public Const CRS_MSG_FILE_LAST                = &H40003C27

'
' MessageId: CRS_MSG_ADMIN_FIRST
'
' MessageText:
'
'  Site Server Content Deployment first administration event message.
'
Public Const CRS_MSG_ADMIN_FIRST              = &H40003C28

'
' MessageId: CRS_MSG_PROJECT_CREATED
'
' MessageText:
'
'  Project %1 created.
'
Public Const CRS_MSG_PROJECT_CREATED          = &H40003C29

'
' MessageId: CRS_MSG_PROJECT_DELETED
'
' MessageText:
'
'  Project %1 deleted.
'
Public Const CRS_MSG_PROJECT_DELETED          = &H40003C2A

'
' MessageId: CRS_MSG_PROJECT_EDITED
'
' MessageText:
'
'  Project %1 edited.
'
Public Const CRS_MSG_PROJECT_EDITED           = &H40003C2B

'
' MessageId: CRS_MSG_ROUTE_CREATED
'
' MessageText:
'
'  Route %1 created.
'
Public Const CRS_MSG_ROUTE_CREATED            = &H40003C2C

'
' MessageId: CRS_MSG_ROUTE_DELETED
'
' MessageText:
'
'  Route %1 deleted.
'
Public Const CRS_MSG_ROUTE_DELETED            = &H40003C2D

'
' MessageId: CRS_MSG_ROUTE_EDITED
'
' MessageText:
'
'  Route %1 edited.
'
Public Const CRS_MSG_ROUTE_EDITED             = &H40003C2E

'
' MessageId: CRS_MSG_SERVER_EDITED
'
' MessageText:
'
'  Server settings modified.
'
Public Const CRS_MSG_SERVER_EDITED            = &H40003C2F

'
' MessageId: CRS_MSG_EVENTSINK_CREATED
'
' MessageText:
'
'  Event sink %1 was added.
'
Public Const CRS_MSG_EVENTSINK_CREATED        = &H40003C30

'
' MessageId: CRS_MSG_EVENTSINK_DELETED
'
' MessageText:
'
'  Event sink %1 was deleted.
'
Public Const CRS_MSG_EVENTSINK_DELETED        = &H40003C31

'
' MessageId: CRS_MSG_CLEARED_EVENTS
'
' MessageText:
'
'  Event store %1 was cleared.
'
Public Const CRS_MSG_CLEARED_EVENTS           = &H40003C32

'
' MessageId: CRS_MSG_CLEARED_ALL_EVENTS
'
' MessageText:
'
'  Each database event store on this server was cleared.
'
Public Const CRS_MSG_CLEARED_ALL_EVENTS       = &H40003C33

'
' MessageId: CRS_MSG_ADMIN_LAST
'
' MessageText:
'
'  Site Server Content Deployment last administration event message.
'
Public Const CRS_MSG_ADMIN_LAST               = &H40003C5A

'
' MessageId: CRS_MSG_SUMMARY_FIRST
'
' MessageText:
'
'  Site Server Content Deployment first summary event message.
'
Public Const CRS_MSG_SUMMARY_FIRST            = &H40003C8C

'
' MessageId: CRS_MSG_SUMMARY_RECEIVE
'
' MessageText:
'
'  Project summary information (files received).
'
Public Const CRS_MSG_SUMMARY_RECEIVE          = &H40003C8D

'
' MessageId: CRS_MSG_SUMMARY_SEND
'
' MessageText:
'
'  Project summary information (files sent).
'
Public Const CRS_MSG_SUMMARY_SEND             = &H40003C8E

'
' MessageId: CRS_MSG_SUMMARY_PULL
'
' MessageText:
'
'  Project summary information (files pulled).
'
Public Const CRS_MSG_SUMMARY_PULL             = &H40003C8F

'
' MessageId: CRS_MSG_SUMMARY_AUTOMATIC_RECEIVE
'
' MessageText:
'
'  Automatic project (receive) checkpoint summary information.
'
Public Const CRS_MSG_SUMMARY_AUTOMATIC_RECEIVE = &H40003C90

'
' MessageId: CRS_MSG_SUMMARY_AUTOMATIC_SEND
'
' MessageText:
'
'  Automatic project (send) checkpoint summary information.
'
Public Const CRS_MSG_SUMMARY_AUTOMATIC_SEND   = &H40003C91

'
' MessageId: CRS_MSG_SUMMARY_RECEIVE_MATCHED
'
' MessageText:
'
'  Project summary information (files matched during receive).
'
Public Const CRS_MSG_SUMMARY_RECEIVE_MATCHED  = &H40003C92

'
' MessageId: CRS_MSG_SUMMARY_SEND_MATCHED
'
' MessageText:
'
'  Project summary information (files matched during send).
'
Public Const CRS_MSG_SUMMARY_SEND_MATCHED     = &H40003C93

'
' MessageId: CRS_MSG_SUMMARY_PULL_MATCHED
'
' MessageText:
'
'  Project summary information (files matched during pull).
'
Public Const CRS_MSG_SUMMARY_PULL_MATCHED     = &H40003C94

'
' MessageId: CRS_MSG_SUMMARY_RECEIVE_ERRORED
'
' MessageText:
'
'  Project summary information (files errored during receive).
'
Public Const CRS_MSG_SUMMARY_RECEIVE_ERRORED  = &H40003C95

'
' MessageId: CRS_MSG_SUMMARY_SEND_ERRORED
'
' MessageText:
'
'  Project summary information (files errored during send).
'
Public Const CRS_MSG_SUMMARY_SEND_ERRORED     = &H40003C96

'
' MessageId: CRS_MSG_SUMMARY_PULL_ERRORED
'
' MessageText:
'
'  Project summary information (files errored during pull).
'
Public Const CRS_MSG_SUMMARY_PULL_ERRORED     = &H40003C97

'
' MessageId: CRS_MSG_SUMMARY_LAST
'
' MessageText:
'
'  Site Server Content Deployment last summary event message.
'
Public Const CRS_MSG_SUMMARY_LAST             = &H40003CBE

'
' MessageId: CRS_CAB_ERROR_FIRST
'
' MessageText:
'
'  Remote installer first error message.
'
Public Const CRS_CAB_ERROR_FIRST              = &HC0003CF0

'
' MessageId: CRS_CAB_ERROR_CABINET_NOT_FOUND
'
' MessageText:
'
'  Cannot find the specified cabinet file.
'
Public Const CRS_CAB_ERROR_CABINET_NOT_FOUND  = &HC0003CF1

'
' MessageId: CRS_CAB_ERROR_NOT_A_CABINET
'
' MessageText:
'
'  Specified file is not a valid cabinet file.
'
Public Const CRS_CAB_ERROR_NOT_A_CABINET      = &HC0003CF2

'
' MessageId: CRS_CAB_ERROR_UNKNOWN_CABINET_VERSION
'
' MessageText:
'
'  Specified file is an unknown version of a cabinet file.
'
Public Const CRS_CAB_ERROR_UNKNOWN_CABINET_VERSION = &HC0003CF3

'
' MessageId: CRS_CAB_ERROR_CORRUPT_CABINET
'
' MessageText:
'
'  Specified cabinet file is corrupt.
'
Public Const CRS_CAB_ERROR_CORRUPT_CABINET    = &HC0003CF4

'
' MessageId: CRS_CAB_ERROR_ALLOC_FAIL
'
' MessageText:
'
'  Out of memory.
'
Public Const CRS_CAB_ERROR_ALLOC_FAIL         = &HC0003CF5

'
' MessageId: CRS_CAB_ERROR_BAD_COMPR_TYPE
'
' MessageText:
'
'  Specified cabinet file uses an unknown compression type.
'
Public Const CRS_CAB_ERROR_BAD_COMPR_TYPE     = &HC0003CF6

'
' MessageId: CRS_CAB_ERROR_MDI_FAIL
'
' MessageText:
'
'  Compressed data in the specified cabinet file is corrupt.
'
Public Const CRS_CAB_ERROR_MDI_FAIL           = &HC0003CF7

'
' MessageId: CRS_CAB_ERROR_TARGET_FILE
'
' MessageText:
'
'  Could not save extracted files from the specified cabinet file.
'
Public Const CRS_CAB_ERROR_TARGET_FILE        = &HC0003CF8

'
' MessageId: CRS_CAB_ERROR_RESERVE_MISMATCH
'
' MessageText:
'
'  Specified cabinet file has a non-valid RESERVE size.
'
Public Const CRS_CAB_ERROR_RESERVE_MISMATCH   = &HC0003CF9

'
' MessageId: CRS_CAB_ERROR_WRONG_CABINET
'
' MessageText:
'
'  Specified cabinet file is not the right cabinet in the set.
'
Public Const CRS_CAB_ERROR_WRONG_CABINET      = &HC0003CFA

'
' MessageId: CRS_CAB_ERROR_USER_ABORT
'
' MessageText:
'
'  Could not extract the specified cabinet file - user aborted the operation.
'
Public Const CRS_CAB_ERROR_USER_ABORT         = &HC0003CFB

'
' MessageId: CRS_CAB_WARNING_CAB_NOT_SIGNED
'
' MessageText:
'
'  Specified cabinet file is not a signed cabinet file.
'
Public Const CRS_CAB_WARNING_CAB_NOT_SIGNED   = &H80003CFC

'
' MessageId: CRS_CAB_WARNING_SIGNATURE_NOT_VERIFIED
'
' MessageText:
'
'  Signature on the specified cabinet could not be verified.
'
Public Const CRS_CAB_WARNING_SIGNATURE_NOT_VERIFIED = &H80003CFD

'
' MessageId: CRS_CAB_ERROR_INFFILE_NOT_FOUND
'
' MessageText:
'
'  INF file was not found inside the specified cabinet file.
'
Public Const CRS_CAB_ERROR_INFFILE_NOT_FOUND  = &HC0003CFE

'
' MessageId: CRS_INF_ERROR_ADVPACK_NOT_LOADED
'
' MessageText:
'
'  Could not load ADVPACK.DLL.
'
Public Const CRS_INF_ERROR_ADVPACK_NOT_LOADED = &HC0003CFF

'
' MessageId: CRS_INF_ERROR_ADVPACK_WRONG_VERSION
'
' MessageText:
'
'  This version of ADVPACK.DLL is not supported.
'
Public Const CRS_INF_ERROR_ADVPACK_WRONG_VERSION = &HC0003D00

'
' MessageId: CRS_INF_ERROR_INVALID_INF
'
' MessageText:
'
'  INF file found inside the specified cabinet file is not a valid INF file.
'
Public Const CRS_INF_ERROR_INVALID_INF        = &HC0003D01

'
' MessageId: CRS_INF_ERROR_NOTHING_TO_INSTALL
'
' MessageText:
'
'  Could not find any of the remote install sections inside the INF file.
'
Public Const CRS_INF_ERROR_NOTHING_TO_INSTALL = &HC0003D02

'
' MessageId: CRS_INF_ERROR_NOTHING_TO_UNINSTALL
'
' MessageText:
'
'  Could not find any of the remote uninstall sections inside the INF file.
'
Public Const CRS_INF_ERROR_NOTHING_TO_UNINSTALL = &HC0003D03

'
' MessageId: CRS_INF_ERROR_BUSY_FILES
'
' MessageText:
'
'  Files are busy.  Cannot install them unless allowed to do a REBOOT.
'
Public Const CRS_INF_ERROR_BUSY_FILES         = &HC0003D04

'
' MessageId: CRS_CAB_WARNING_MANUAL_REBOOT
'
' MessageText:
'
'  Setup completed successfully, but the server could not be rebooted automatically. Manual reboot is necessary.
'
Public Const CRS_CAB_WARNING_MANUAL_REBOOT    = &H80003D05

'
' MessageId: CRS_CAB_WARNING_REBOOT_NEEDED
'
' MessageText:
'
'  Setup completed successfully, but the files became busy during setup.  Manual reboot is necessary.
'
Public Const CRS_CAB_WARNING_REBOOT_NEEDED    = &H80003D06

'
' MessageId: CRS_INF_SUCCESS_REBOOT_IN_PROGRESS
'
' MessageText:
'
'  Setup completed successfully. The server is being rebooted.
'
Public Const CRS_INF_SUCCESS_REBOOT_IN_PROGRESS = &H00003D07

'
' MessageId: CRS_INF_SUCCESS
'
' MessageText:
'
'  Setup completed successfully.
'
Public Const CRS_INF_SUCCESS                  = &H00003D09

'
' MessageId: CRS_INF_ERROR_BUSY_FILES_UNINSTALL
'
' MessageText:
'
'  Files are busy.  Cannot uninstall files unless allowed to reboot.
'
Public Const CRS_INF_ERROR_BUSY_FILES_UNINSTALL = &HC0003D0A

'
' MessageId: CRS_CAB_WARNING_MANUAL_REBOOT_UNINSTALL
'
' MessageText:
'
'  Uninstall completed successfully, but the server could not be rebooted automatically. Manual reboot is necessary.
'
Public Const CRS_CAB_WARNING_MANUAL_REBOOT_UNINSTALL = &H80003D0B

'
' MessageId: CRS_CAB_WARNING_REBOOT_NEEDED_UNINSTALL
'
' MessageText:
'
'  Uninstall completed successfully, but the files became busy during setup.  Manual reboot is necessary.
'
Public Const CRS_CAB_WARNING_REBOOT_NEEDED_UNINSTALL = &H80003D0C

'
' MessageId: CRS_INF_SUCCESS_REBOOT_IN_PROGRESS_UNINSTALL
'
' MessageText:
'
'  Uninstall completed successfully. The server is being rebooted.
'
Public Const CRS_INF_SUCCESS_REBOOT_IN_PROGRESS_UNINSTALL = &H00003D0D

'
' MessageId: CRS_INF_SUCCESS_UNINSTALL
'
' MessageText:
'
'  Uninstall completed successfully.
'
Public Const CRS_INF_SUCCESS_UNINSTALL        = &H00003D0E

'
' MessageId: CRS_CAB_ERROR_ADVPACK_FAILED
'
' MessageText:
'
'  Problem installing the CAB. Check CAB and INF files for validity. 
'
Public Const CRS_CAB_ERROR_ADVPACK_FAILED     = &HC0003D0F

'
' MessageId: CRS_CAB_ERROR_LOCKED_FILES
'
' MessageText:
'
'  At least one of the files inside the CAB is open for exclusive write/delete access.
'
Public Const CRS_CAB_ERROR_LOCKED_FILES       = &HC0003D10

'
' MessageId: CRS_CAB_ERROR_UNKNOWN
'
' MessageText:
'
'  An unknown error occurred while processing the specified cabinet file.
'
Public Const CRS_CAB_ERROR_UNKNOWN            = &HC0003DEE

'
' MessageId: CRS_CAB_ERROR_LAST
'
' MessageText:
'
'  Remote installer last error message.
'
Public Const CRS_CAB_ERROR_LAST               = &H40003DEF

'
' MessageId: CRS_ERROR_LAST
'
' MessageText:
'
'  Site Server Content Deployment service last error message.
'
Public Const CRS_ERROR_LAST                   = &H40003E7F

'
' MessageId: CRS_EVENT_FIRST
'
' MessageText:
'
'  Site Server Content Deployment Service first event message.
'
Public Const CRS_EVENT_FIRST                  = &H400036B0

'
' MessageId: CRS_EVENT_NET_SOURCE
'
' MessageText:
'
'  Source directory is on a network drive.  Cannot deploy from network drives.
'
Public Const CRS_EVENT_NET_SOURCE             = &HC00036B4

'
' MessageId: CRS_EVENT_ERROR_ACCESS_DENIED
'
' MessageText:
'
'  Access denied.
'
Public Const CRS_EVENT_ERROR_ACCESS_DENIED    = &HC00036B5

'
' MessageId: CRS_EVENT_FILE_CREATE_ERROR
'
' MessageText:
'
'  Unable to open the file.
'
Public Const CRS_EVENT_FILE_CREATE_ERROR      = &HC00036B6

'
' MessageId: CRS_EVENT_SENDINET_ERROR
'
' MessageText:
'
'  Project was unable to send the file.
'
Public Const CRS_EVENT_SENDINET_ERROR         = &HC00036B7

'
' MessageId: CRS_EVENT_SAMEBOX_NET_TARGET
'
' MessageText:
'
'  Destination directory is on a network drive.  Cannot deploy to network drives.
'
Public Const CRS_EVENT_SAMEBOX_NET_TARGET     = &HC00036B8

'
' MessageId: CRS_EVENT_BAD_SOURCE
'
' MessageText:
'
'  Unable to access the project source.
'
Public Const CRS_EVENT_BAD_SOURCE             = &HC00036B9

'
' MessageId: CRS_EVENT_SENDINET_CONNECT
'
' MessageText:
'
'  Unable to connect to the remote server.
'
Public Const CRS_EVENT_SENDINET_CONNECT       = &HC00036BA

'
' MessageId: CRS_EVENT_HASH_MISMATCH
'
' MessageText:
'
'  MD5 Hash did not match during a transmission.  This could indicate transmission failure or that the files transmitted at the source have been modified in transit.
'
Public Const CRS_EVENT_HASH_MISMATCH          = &HC00036BB

'
' MessageId: CRS_EVENT_SERVER_ABORTED
'
' MessageText:
'
'  Remote server was restarted.  The project is being restarted.
'
Public Const CRS_EVENT_SERVER_ABORTED         = &HC00036BC

'
' MessageId: CRS_EVENT_REPLICATION_ABORTED
'
' MessageText:
'
'  Project was aborted.
'
Public Const CRS_EVENT_REPLICATION_ABORTED    = &HC00036BD

'
' MessageId: CRS_EVENT_SEQUENCE_ERROR
'
' MessageText:
'
'  Internal messaging sequence error occurred.
'
Public Const CRS_EVENT_SEQUENCE_ERROR         = &HC00036BE

'
' MessageId: CRS_EVENT_SERVICE_STARTED
'
' MessageText:
'
'  Site Server Content Deployment service started successfully.
'
Public Const CRS_EVENT_SERVICE_STARTED        = &H400036BF

'
' MessageId: CRS_EVENT_SERVICE_SHUTDOWN
'
' MessageText:
'
'  Site Server Content Deployment service shut down successfully.
'
Public Const CRS_EVENT_SERVICE_SHUTDOWN       = &H400036C0

'
' MessageId: CRS_EVENT_METABASE_ACCESS
'
' MessageText:
'
'  Could not open given storage/stream.  Ensure the authentication account has access to this storage/stream.
'
Public Const CRS_EVENT_METABASE_ACCESS        = &HC00036C2

'
' MessageId: CRS_EVENT_AUTH_FAILURE
'
' MessageText:
'
'  Authorization failure against the destination server.  Ensure the service has access to this server.
'
Public Const CRS_EVENT_AUTH_FAILURE           = &HC00036C3

'
' MessageId: CRS_EVENT_REMOTE_AUTH_FAILURE
'
' MessageText:
'
'  Remote authorization failed to the destination server.  Ensure the service has access to this server.
'
Public Const CRS_EVENT_REMOTE_AUTH_FAILURE    = &HC00036C4

'
' MessageId: CRS_EVENT_ERROR_NOT_IN_ORDER
'
' MessageText:
'
'  Message was received out of order.  Closing connection.
'
Public Const CRS_EVENT_ERROR_NOT_IN_ORDER     = &HC00036C5

'
' MessageId: CRS_EVENT_CONNECTION_RETRY_FAILED
'
' MessageText:
'
'  Connection failed and could not be re-established.
'
Public Const CRS_EVENT_CONNECTION_RETRY_FAILED = &HC00036C6

'
' MessageId: CRS_EVENT_REPLICATION_CANCELED
'
' MessageText:
'
'  Project canceled at the request of the user.
'
Public Const CRS_EVENT_REPLICATION_CANCELED   = &HC00036C7

'
' MessageId: CRS_EVENT_ERROR_DEST_TOOBIG
'
' MessageText:
'
'  Could not add the specified destination(s).  The maximum number of destinations has been reached.
'
Public Const CRS_EVENT_ERROR_DEST_TOOBIG      = &HC00036C8

'
' MessageId: CRS_EVENT_ERROR_NO_IP_MAPPING
'
' MessageText:
'
'  Cannot resolve IP address for the destination server.  No virtual directory created.
'
Public Const CRS_EVENT_ERROR_NO_IP_MAPPING    = &HC00036C9

'
' MessageId: CRS_EVENT_ERROR_DISK_FULL
'
' MessageText:
'
'  Not enough space on the disk.
'
Public Const CRS_EVENT_ERROR_DISK_FULL        = &HC00036CA

'
' MessageId: CRS_EVENT_ERROR_TOO_MANY_RUNNING
'
' MessageText:
'
'  Too many active projects. No more can be started until others complete.
'
Public Const CRS_EVENT_ERROR_TOO_MANY_RUNNING = &HC00036CB

'
' MessageId: CRS_EVENT_ERROR_WOULDBLOCK
'
' MessageText:
'
'  The network operation would block.
'
Public Const CRS_EVENT_ERROR_WOULDBLOCK       = &H000036D3

'
' MessageId: CRS_EVENT_ERROR_NETDOWN
'
' MessageText:
'
'  The remote network is down.
'
Public Const CRS_EVENT_ERROR_NETDOWN          = &HC00036E2

'
' MessageId: CRS_EVENT_ERROR_NETUNREACH
'
' MessageText:
'
'  The remote network is unreachable.
'
Public Const CRS_EVENT_ERROR_NETUNREACH       = &HC00036E3

'
' MessageId: CRS_EVENT_ERROR_CONNABORTED
'
' MessageText:
'
'  The network connection was aborted at the network layer.
'
Public Const CRS_EVENT_ERROR_CONNABORTED      = &HC00036E5

'
' MessageId: CRS_EVENT_ERROR_TIMEOUT
'
' MessageText:
'
'  The network session timed out on a send or receive operation.
'
Public Const CRS_EVENT_ERROR_TIMEOUT          = &HC00036EC

'
' MessageId: CRS_EVENT_ERROR_NO_PROJECTS
'
' MessageText:
'
'  No projects defined.
'
Public Const CRS_EVENT_ERROR_NO_PROJECTS      = &HC0003714

'
' MessageId: CRS_EVENT_ERROR_PROJECT_NOT_FOUND
'
' MessageText:
'
'  Could not find the project.
'
Public Const CRS_EVENT_ERROR_PROJECT_NOT_FOUND = &HC0003715

'
' MessageId: CRS_EVENT_ERROR_INVALID_PARAMETER
'
' MessageText:
'
'  Invalid parameter.
'
Public Const CRS_EVENT_ERROR_INVALID_PARAMETER = &HC0003716

'
' MessageId: CRS_EVENT_ERROR_AUTHENTICATION_FAILURE
'
' MessageText:
'
'  Authentication failure. Ensure the account has proper access on local and remote servers.
'
Public Const CRS_EVENT_ERROR_AUTHENTICATION_FAILURE = &HC0003718

'
' MessageId: CRS_EVENT_ERROR_DELETE_PROJECT_DOWN_ROUTE
'
' MessageText:
'
'  Could not delete the route from the project on at least one machine down the defined routes.  The project will have to be edited manually there.
'
Public Const CRS_EVENT_ERROR_DELETE_PROJECT_DOWN_ROUTE = &H80003719

'
' MessageId: CRS_EVENT_ERROR_SERVICE_NOT_RUNNING
'
' MessageText:
'
'  Server is available, but the Site Server Content Deployment service is not running.
'
Public Const CRS_EVENT_ERROR_SERVICE_NOT_RUNNING = &HC000371A

'
' MessageId: CRS_EVENT_ERROR_HOST_DOWN
'
' MessageText:
'
'  Server is unavailable.
'
Public Const CRS_EVENT_ERROR_HOST_DOWN        = &HC000371B

'
' MessageId: CRS_EVENT_ERROR_BAD_NET_NAME
'
' MessageText:
'
'  Server cannot be found.
'
Public Const CRS_EVENT_ERROR_BAD_NET_NAME     = &HC000371C

'
' MessageId: CRS_EVENT_ERROR_ROUTE_NOT_FOUND
'
' MessageText:
'
'  Could not find route.  Ensure the route name is correct and that it exists on the target servers.
'
Public Const CRS_EVENT_ERROR_ROUTE_NOT_FOUND  = &HC000371D

'
' MessageId: CRS_EVENT_ERROR_NO_ROUTES
'
' MessageText:
'
'  No routes defined.
'
Public Const CRS_EVENT_ERROR_NO_ROUTES        = &HC000371E

'
' MessageId: CRS_EVENT_ERROR_CREATE_PROJECT_DOWN_ROUTE
'
' MessageText:
'
'  Could not create the project on at least one server down the defined routes.  The project will have to be created manually there.
'
Public Const CRS_EVENT_ERROR_CREATE_PROJECT_DOWN_ROUTE = &H8000371F

'
' MessageId: CRS_EVENT_ERROR_SERVICE_PAUSED
'
' MessageText:
'
'  Site Server Content Deployment service on the destination server is paused and is not accepting new requests.  Retry the project later.
'
Public Const CRS_EVENT_ERROR_SERVICE_PAUSED   = &HC0003720

'
' MessageId: CRS_EVENT_ERROR_SERVICE_NOT_PAUSED
'
' MessageText:
'
'  Could not restart service. Service is not paused.
'
Public Const CRS_EVENT_ERROR_SERVICE_NOT_PAUSED = &HC0003721

'
' MessageId: CRS_EVENT_ERROR_NAME_NOT_RESOLVED
'
' MessageText:
'
'  Could not resolve the URL.  Ensure URL and/or proxy settings are correct.
'
Public Const CRS_EVENT_ERROR_NAME_NOT_RESOLVED = &HC0003722

'
' MessageId: CRS_EVENT_ERROR_CONNECTION_RESET
'
' MessageText:
'
'  Connection to the destination server has been reset.  This indicates problems with the remote service or network.
'
Public Const CRS_EVENT_ERROR_CONNECTION_RESET = &HC0003723

'
' MessageId: CRS_EVENT_ERROR_REPLICATION_NOT_FOUND
'
' MessageText:
'
'  No entries.
'
Public Const CRS_EVENT_ERROR_REPLICATION_NOT_FOUND = &HC0003724

'
' MessageId: CRS_EVENT_ERROR_SERVICE_ALREADY_RUNNING
'
' MessageText:
'
'  Could not start service.  The service has already been started.
'
Public Const CRS_EVENT_ERROR_SERVICE_ALREADY_RUNNING = &HC0003725

'
' MessageId: CRS_EVENT_ERROR_PROJECT_ALREADY_RUNNING
'
' MessageText:
'
'  Failed to start project.  It is already running.
'
Public Const CRS_EVENT_ERROR_PROJECT_ALREADY_RUNNING = &HC0003726

'
' MessageId: CRS_EVENT_FILE_MOVE_ERROR
'
' MessageText:
'
'  Failed to move file.
'
Public Const CRS_EVENT_FILE_MOVE_ERROR        = &HC0003727

'
' MessageId: CRS_EVENT_PRODUCT_EXPIRED
'
' MessageText:
'
'  Thank you for your interest in Site Server Content Deployment.  This software has expired.  Please acquire the final retail version of this software.
'
Public Const CRS_EVENT_PRODUCT_EXPIRED        = &HC0003728

'
' MessageId: CRS_EVENT_ERROR_PROJECT_INCOMPLETE
'
' MessageText:
'
'  Project information is incomplete.
'
Public Const CRS_EVENT_ERROR_PROJECT_INCOMPLETE = &HC0003729

'
' MessageId: CRS_EVENT_ERROR_PARAMETER_NOT_FOUND
'
' MessageText:
'
'  Parameter not found.
'
Public Const CRS_EVENT_ERROR_PARAMETER_NOT_FOUND = &HC000372A

'
' MessageId: CRS_EVENT_ERROR_REPLICATION_RUNNING
'
' MessageText:
'
'  Project is running.  The operation could not be completed.
'
Public Const CRS_EVENT_ERROR_REPLICATION_RUNNING = &HC000372B

'
' MessageId: CRS_EVENT_ERROR_INVALID_PROJECT_NAME
'
' MessageText:
'
'  Project name is invalid.  Project names cannot contain backslashes and cannot be empty.
'
Public Const CRS_EVENT_ERROR_INVALID_PROJECT_NAME = &HC000372C

'
' MessageId: CRS_EVENT_ERROR_INVALID_ROUTE_NAME
'
' MessageText:
'
'  Route name is invalid.  Route names cannot contain backslashes or DBCS characters and cannot be empty.
'
Public Const CRS_EVENT_ERROR_INVALID_ROUTE_NAME = &HC000372D

'
' MessageId: CRS_EVENT_ERROR_INVALID_DWORD
'
' MessageText:
'
'  Parameter must be a positive integer.
'
Public Const CRS_EVENT_ERROR_INVALID_DWORD    = &HC000372E

'
' MessageId: CRS_EVENT_ERROR_NO_MORE_ITEMS
'
' MessageText:
'
'  No more items are in the list.
'
Public Const CRS_EVENT_ERROR_NO_MORE_ITEMS    = &H8000372F

'
' MessageId: CRS_EVENT_ERROR_TOO_MANY_ARGS
'
' MessageText:
'
'  Too many arguments specified.
'
Public Const CRS_EVENT_ERROR_TOO_MANY_ARGS    = &HC0003730

'
' MessageId: CRS_EVENT_MBS_INIT_ERROR
'
' MessageText:
'
'  Internal error trying to initialize for ACL replication.
'
Public Const CRS_EVENT_MBS_INIT_ERROR         = &HC0003731

'
' MessageId: CRS_EVENT_ERROR_OPENING_FILE
'
' MessageText:
'
'  Error opening file or file not found.
'
Public Const CRS_EVENT_ERROR_OPENING_FILE     = &HC0003733

'
' MessageId: CRS_EVENT_ERROR_FILE_FORMAT
'
' MessageText:
'
'  Invalid file format.
'
Public Const CRS_EVENT_ERROR_FILE_FORMAT      = &HC0003734

'
' MessageId: CRS_EVENT_ERROR_FILE_WRITE
'
' MessageText:
'
'  Error writing to file.
'
Public Const CRS_EVENT_ERROR_FILE_WRITE       = &HC0003735

'
' MessageId: CRS_EVENT_WINSOCK_WSAHOSTNAME
'
' MessageText:
'
'  Gethostname failed.
'
Public Const CRS_EVENT_WINSOCK_WSAHOSTNAME    = &HC0003736

'
' MessageId: CRS_EVENT_ERROR_INVALID_CONTEXT
'
' MessageText:
'
'  Current context is invalid.  Project being reset.
'
Public Const CRS_EVENT_ERROR_INVALID_CONTEXT  = &HC000373D

'
' MessageId: CRS_EVENT_ERROR_DOWNLEVEL_SERVER
'
' MessageText:
'
'  Operation is not supported on a down-level server.
'
Public Const CRS_EVENT_ERROR_DOWNLEVEL_SERVER = &HC000373E

'
' MessageId: CRS_EVENT_ERROR_NO_MAPPING
'
' MessageText:
'
'  URL does not map to any project definitions.
'
Public Const CRS_EVENT_ERROR_NO_MAPPING       = &HC000373F

'
' MessageId: CRS_EVENT_ERROR_START_PENDING
'
' MessageText:
'
'  Start pending completion of the active project.
'
Public Const CRS_EVENT_ERROR_START_PENDING    = &HC0003740

'
' MessageId: CRS_EVENT_ERROR_NO_EVENTSINKS
'
' MessageText:
'
'  No event sinks defined.
'
Public Const CRS_EVENT_ERROR_NO_EVENTSINKS    = &HC0003741

'
' MessageId: CRS_EVENT_ERROR_EVENTSINK_NOT_FOUND
'
' MessageText:
'
'  Could not find event sink.
'
Public Const CRS_EVENT_ERROR_EVENTSINK_NOT_FOUND = &HC0003742

'
' MessageId: CRS_EVENT_ERROR_WSAHOSTNOTFOUND
'
' MessageText:
'
'  Host not found.
'
Public Const CRS_EVENT_ERROR_WSAHOSTNOTFOUND  = &HC0003743

'
' MessageId: CRS_EVENT_ERROR_WSATRYAGAIN
'
' MessageText:
'
'  Host not found, or SERVERFAIL.  Try again.
'
Public Const CRS_EVENT_ERROR_WSATRYAGAIN      = &HC0003744

'
' MessageId: CRS_EVENT_ERROR_WSANORECOVERY
'
' MessageText:
'
'  Non-recoverable winsock error.
'
Public Const CRS_EVENT_ERROR_WSANORECOVERY    = &HC0003745

'
' MessageId: CRS_EVENT_ERROR_WSANODATA
'
' MessageText:
'
'  Invalid address.
'
Public Const CRS_EVENT_ERROR_WSANODATA        = &HC0003746

'
' MessageId: CRS_EVENT_EVENTSINK_INITIALIZED
'
' MessageText:
'
'  Event sink specified by the data field was successfully initialized.
'
Public Const CRS_EVENT_EVENTSINK_INITIALIZED  = &H00003747

'
' MessageId: CRS_EVENT_ERROR_EVENTSINK_NOT_INITIALIZED
'
' MessageText:
'
'  Event sink specified by the data field failed to initialize.
'
Public Const CRS_EVENT_ERROR_EVENTSINK_NOT_INITIALIZED = &HC0003748

'
' MessageId: CRS_EVENT_ERROR_NOTHING_TO_ROLLBACK
'
' MessageText:
'
'  Could not roll back project.  No rollback information found.
'
Public Const CRS_EVENT_ERROR_NOTHING_TO_ROLLBACK = &HC0003749

'
' MessageId: CRS_EVENT_ERROR_NO_DESTINATIONS
'
' MessageText:
'
'  Project could not be started because no destinations are defined.
'
Public Const CRS_EVENT_ERROR_NO_DESTINATIONS  = &HC000374A

'
' MessageId: CRS_EVENT_ERROR_REAUTHENTICATE
'
' MessageText:
'
'  Re-authenticate using an acceptable protocol.
'
Public Const CRS_EVENT_ERROR_REAUTHENTICATE   = &HC000374B

'
' MessageId: CRS_EVENT_ERROR_UNSUPPORTED
'
' MessageText:
'
'  Function not supported in this version to this server.
'
Public Const CRS_EVENT_ERROR_UNSUPPORTED      = &HC000374C

'
' MessageId: CRS_EVENT_ERROR_USER_NOT_FOUND
'
' MessageText:
'
'  Specified user could not be found.
'
Public Const CRS_EVENT_ERROR_USER_NOT_FOUND   = &HC000374D

'
' MessageId: CRS_EVENT_ERROR_NO_TRANSACTIONS
'
' MessageText:
'
'  No transactions for the specified project.
'
Public Const CRS_EVENT_ERROR_NO_TRANSACTIONS  = &HC000374E

'
' MessageId: CRS_EVENT_ERROR_NO_SERVERS
'
' MessageText:
'
'  No destination servers defined.
'
Public Const CRS_EVENT_ERROR_NO_SERVERS       = &HC000374F

'
' MessageId: CRS_EVENT_ERROR_NO_SCHEDULES
'
' MessageText:
'
'  No schedules are defined for this project.
'
Public Const CRS_EVENT_ERROR_NO_SCHEDULES     = &HC0003750

'
' MessageId: CRS_EVENT_ERROR_NO_EMAIL
'
' MessageText:
'
'  Could not contact SMTP server to send e-mail report.
'
Public Const CRS_EVENT_ERROR_NO_EMAIL         = &HC0003751

'
' MessageId: CRS_EVENT_ERROR_WAIT_ABORT
'
' MessageText:
'
'  Waiting to abort after losing connection with source server.
'
Public Const CRS_EVENT_ERROR_WAIT_ABORT       = &HC0003752

'
' MessageId: CRS_EVENT_RESUME_REPLICATION
'
' MessageText:
'
'  Resuming project.
'
Public Const CRS_EVENT_RESUME_REPLICATION     = &H40003753

'
' MessageId: CRS_EVENT_METABASE_NOT_FOUND
'
' MessageText:
'
'  Metabase is not responding.
'
Public Const CRS_EVENT_METABASE_NOT_FOUND     = &HC0003754

'
' MessageId: CRS_EVENT_SSE_VROOT_NOT_FOUND
'
' MessageText:
'
'  Site Server virtual directory not found.
'
Public Const CRS_EVENT_SSE_VROOT_NOT_FOUND    = &HC0003755

'
' MessageId: CRS_EVENT_WARNING_CONNECTION_FAILED
'
' MessageText:
'
'  Connection failed.  Will attempt to re-establish session.
'
Public Const CRS_EVENT_WARNING_CONNECTION_FAILED = &H80003756

'
' MessageId: CRS_EVENT_ERROR_DROPPED_EMAIL_REPORT
'
' MessageText:
'
'  Dropped e-mail report.
'
Public Const CRS_EVENT_ERROR_DROPPED_EMAIL_REPORT = &HC0003757

'
' MessageId: CRS_EVENT_ERROR_EMAIL_HOST_NOT_DEFINED
'
' MessageText:
'
'  No SMTP server defined.
'
Public Const CRS_EVENT_ERROR_EMAIL_HOST_NOT_DEFINED = &HC0003758

'
' MessageId: CRS_EVENT_VROOT_CREATION
'
' MessageText:
'
'  Could not create the specified virtual directory.
'
Public Const CRS_EVENT_VROOT_CREATION         = &H80003759

'
' MessageId: CRS_EVENT_UPLPAGE_CREATION
'
' MessageText:
'
'  Could not create the upload page.
'
Public Const CRS_EVENT_UPLPAGE_CREATION       = &HC000375A

'
' MessageId: CRS_EVENT_ERROR_CIRCULAR_ROUTE
'
' MessageText:
'
'  System has detected a circular route.  Project not created.
'
Public Const CRS_EVENT_ERROR_CIRCULAR_ROUTE   = &HC000375B

'
' MessageId: CRS_EVENT_ERROR_TRANSACTION_APPLY_FAILED
'
' MessageText:
'
'  Transaction application failed.  Check permissions on the destination directory and files and ensure that no files are locked open.
'
Public Const CRS_EVENT_ERROR_TRANSACTION_APPLY_FAILED = &HC000375C

'
' MessageId: CRS_EVENT_ERROR_DATABASE
'
' MessageText:
'
'  Database error occurred.
'
Public Const CRS_EVENT_ERROR_DATABASE         = &HC000375D

'
' MessageId: CRS_EVENT_ERROR_SINK_CONNECT_FAILED
'
' MessageText:
'
'  Unable to connect to remote server to forward events.
'
Public Const CRS_EVENT_ERROR_SINK_CONNECT_FAILED = &HC000375E

'
' MessageId: CRS_EVENT_ERROR_CANT_SEND_EVENTS_TO_DOWNLEVEL_SERVER
'
' MessageText:
'
'  Unable to send events to the destination server.  This operation is not supported.
'
Public Const CRS_EVENT_ERROR_CANT_SEND_EVENTS_TO_DOWNLEVEL_SERVER = &HC000375F

'
' MessageId: CRS_EVENT_ERROR_FILE_CORRUPT
'
' MessageText:
'
'  File is corrupt or unreadable.
'
Public Const CRS_EVENT_ERROR_FILE_CORRUPT     = &HC0003760

'
' MessageId: CRS_EVENT_ERROR_FILE_BUSY
'
' MessageText:
'
'  File is busy.
'
Public Const CRS_EVENT_ERROR_FILE_BUSY        = &HC0003761

'
' MessageId: CRS_EVENT_ACL_ERROR_INVALID_ACE
'
' MessageText:
'
'  Could not set the ACE on this file.
'
Public Const CRS_EVENT_ACL_ERROR_INVALID_ACE  = &H80003762

'
' MessageId: CRS_EVENT_ACL_ERROR_ACL_FAILED
'
' MessageText:
'
'  Could not set the ACLs appropriately on this file, setting default ACLs on this file.
'
Public Const CRS_EVENT_ACL_ERROR_ACL_FAILED   = &HC0003763

'
' MessageId: CRS_EVENT_ERROR_PROJECT_NOT_FOUND_ON_DESTINATION
'
' MessageText:
'
'  Could not find project on the destination server.
'
Public Const CRS_EVENT_ERROR_PROJECT_NOT_FOUND_ON_DESTINATION = &HC0003764

'
' MessageId: CRS_EVENT_ERROR_PROJECT_INCOMPLETE_ON_DESTINATION
'
' MessageText:
'
'  Project information is incomplete on the destination server.
'
Public Const CRS_EVENT_ERROR_PROJECT_INCOMPLETE_ON_DESTINATION = &HC0003765

'
' MessageId: CRS_EVENT_ERROR_SERVICE_DOWN_ON_REMOTE_SERVER
'
' MessageText:
'
'  Remote server is available, but the Site Server Content Deployment service is not running.
'
Public Const CRS_EVENT_ERROR_SERVICE_DOWN_ON_REMOTE_SERVER = &HC0003766

'
' MessageId: CRS_EVENT_ERROR_DISK_FULL_ON_DESTINATION
'
' MessageText:
'
'  Disk is full on the destination server.
'
Public Const CRS_EVENT_ERROR_DISK_FULL_ON_DESTINATION = &HC0003767

'
' MessageId: CRS_EVENT_FILE_CREATE_ERROR_ON_DESTINATION
'
' MessageText:
'
'  Unable to open a file on the destination server.  Check permissions on the project directory and files.
'
Public Const CRS_EVENT_FILE_CREATE_ERROR_ON_DESTINATION = &HC0003768

'
' MessageId: CRS_EVENT_ERROR_REMOTE_SERVER_NOT_FOUND
'
' MessageText:
'
'  Remote server could not be found.
'
Public Const CRS_EVENT_ERROR_REMOTE_SERVER_NOT_FOUND = &HC0003769

'
' MessageId: CRS_EVENT_ERROR_REMOTE_APPLY_FAILED
'
' MessageText:
'
'  Unable to apply transactions on the destination server.
'
Public Const CRS_EVENT_ERROR_REMOTE_APPLY_FAILED = &HC000376A

'
' MessageId: CRS_EVENT_ERROR_INVALID_FLAG_COMBINATION
'
' MessageText:
'
'  Invalid flag combination.
'
Public Const CRS_EVENT_ERROR_INVALID_FLAG_COMBINATION = &HC000376B

'
' MessageId: CRS_EVENT_ERROR_RUNNING_SCRIPT
'
' MessageText:
'
'  There was an error running the script.  Ensure that the script exists and can be located in the path.
'
Public Const CRS_EVENT_ERROR_RUNNING_SCRIPT   = &HC000376C

'
' MessageId: CRS_EVENT_ERROR_CANT_FORWARD_TO_LOCAL_SERVER
'
' MessageText:
'
'  Cannot forward events to the local server.
'
Public Const CRS_EVENT_ERROR_CANT_FORWARD_TO_LOCAL_SERVER = &HC000376D

'
' MessageId: CRS_EVENT_ERROR_SCHEDULE_NOT_FOUND
'
' MessageText:
'
'  Specified project schedule could not be found.
'
Public Const CRS_EVENT_ERROR_SCHEDULE_NOT_FOUND = &HC000376E

'
' MessageId: CRS_EVENT_ERROR_METABASE
'
' MessageText:
'
'  An error occurred accessing the metabase.  Ensure that the web site exists and that the account has access to it.
'
Public Const CRS_EVENT_ERROR_METABASE         = &HC000376F

'
' MessageId: CRS_EVENT_ERROR_SCHEDULER_FAILURE
'
' MessageText:
'
'  An error occurred with the task scheduler.
'
Public Const CRS_EVENT_ERROR_SCHEDULER_FAILURE = &HC0003770

'
' MessageId: CRS_EVENT_SAMEBOX_META_TARGET
'
' MessageText:
'
'  A destination for metabase replication is invalid.
'
Public Const CRS_EVENT_SAMEBOX_META_TARGET    = &HC0003771

'
' MessageId: CRS_EVENT_ERROR_DROPPED_EVENTS
'
' MessageText:
'
'  The event sink specified by the data field has started dropping events.
'
Public Const CRS_EVENT_ERROR_DROPPED_EVENTS   = &HC0003772

'
' MessageId: CRS_EVENT_REPLICATION_FIRST
'
' MessageText:
'
'  Site Server Content Deployment first replication event message.
'
Public Const CRS_EVENT_REPLICATION_FIRST      = &H400037DC

'
' MessageId: CRS_EVENT_STARTING_REPLICATION
'
' MessageText:
'
'  Project starting to the destination server.
'
Public Const CRS_EVENT_STARTING_REPLICATION   = &H400037DD

'
' MessageId: CRS_EVENT_REPLICATION_COMPLETE
'
' MessageText:
'
'  Project complete to the destination server.
'
Public Const CRS_EVENT_REPLICATION_COMPLETE   = &H400037DE

'
' MessageId: CRS_EVENT_STARTING_PULL_REPLICATION
'
' MessageText:
'
'  Retrieval project starting.
'
Public Const CRS_EVENT_STARTING_PULL_REPLICATION = &H400037DF

'
' MessageId: CRS_EVENT_PULL_REPLICATION_COMPLETE
'
' MessageText:
'
'  Retrieval project complete.
'
Public Const CRS_EVENT_PULL_REPLICATION_COMPLETE = &H400037E0

'
' MessageId: CRS_EVENT_STARTING_RECEIVE_REPLICATION
'
' MessageText:
'
'  Starting to receive project.
'
Public Const CRS_EVENT_STARTING_RECEIVE_REPLICATION = &H400037E1

'
' MessageId: CRS_EVENT_REPLICATION_RECEIVE_COMPLETE
'
' MessageText:
'
'  Finished receiving project.
'
Public Const CRS_EVENT_REPLICATION_RECEIVE_COMPLETE = &H400037E2

'
' MessageId: CRS_EVENT_STARTING_REPLICATION_SCAN
'
' MessageText:
'
'  Project starting.
'
Public Const CRS_EVENT_STARTING_REPLICATION_SCAN = &H400037E3

'
' MessageId: CRS_EVENT_REPLICATION_LAST
'
' MessageText:
'
'  Site Server Content Deployment last replication event message.
'
Public Const CRS_EVENT_REPLICATION_LAST       = &H4000380D

'
' MessageId: CRS_EVENT_FILE_FIRST
'
' MessageText:
'
'  Site Server Content Deployment first file event message.
'
Public Const CRS_EVENT_FILE_FIRST             = &H4000380E

'
' MessageId: CRS_EVENT_SENDING_FILE
'
' MessageText:
'
'  Sending file.
'
Public Const CRS_EVENT_SENDING_FILE           = &H4000380F

'
' MessageId: CRS_EVENT_SENT_FILE
'
' MessageText:
'
'  File sent.
'
Public Const CRS_EVENT_SENT_FILE              = &H40003810

'
' MessageId: CRS_EVENT_RECEIVING_FILE
'
' MessageText:
'
'  Receiving file.
'
Public Const CRS_EVENT_RECEIVING_FILE         = &H40003811

'
' MessageId: CRS_EVENT_RECEIVED_FILE
'
' MessageText:
'
'  File received.
'
Public Const CRS_EVENT_RECEIVED_FILE          = &H40003812

'
' MessageId: CRS_EVENT_SENT_DELETE_FILE
'
' MessageText:
'
'  Sent delete file request.
'
Public Const CRS_EVENT_SENT_DELETE_FILE       = &H40003813

'
' MessageId: CRS_EVENT_RECEIVED_DELETE_FILE
'
' MessageText:
'
'  File delete request received.
'
Public Const CRS_EVENT_RECEIVED_DELETE_FILE   = &H40003814

'
' MessageId: CRS_EVENT_RETRYING_SENDING_FILE
'
' MessageText:
'
'  Retrying to send file.
'
Public Const CRS_EVENT_RETRYING_SENDING_FILE  = &H40003818

'
' MessageId: CRS_EVENT_FILE_BUSY
'
' MessageText:
'
'  File is busy.  The service is waiting for the file to become available.
'
Public Const CRS_EVENT_FILE_BUSY              = &H40003819

'
' MessageId: CRS_EVENT_SENDING_ACL
'
' MessageText:
'
'  Sending the ACL for this file.
'
Public Const CRS_EVENT_SENDING_ACL            = &H4000381A

'
' MessageId: CRS_EVENT_SENT_ACL
'
' MessageText:
'
'  Sent the ACL for this file.
'
Public Const CRS_EVENT_SENT_ACL               = &H4000381B

'
' MessageId: CRS_EVENT_RECEIVING_ACL
'
' MessageText:
'
'  Receiving the ACL for this file.
'
Public Const CRS_EVENT_RECEIVING_ACL          = &H4000381C

'
' MessageId: CRS_EVENT_RECEIVED_ACL
'
' MessageText:
'
'  Received the ACL for this file.
'
Public Const CRS_EVENT_RECEIVED_ACL           = &H4000381D

'
' MessageId: CRS_EVENT_PULLED_FILE_HTTP
'
' MessageText:
'
'  File retrieved over HTTP.
'
Public Const CRS_EVENT_PULLED_FILE_HTTP       = &H4000381E

'
' MessageId: CRS_EVENT_PULLED_FILE_FTP
'
' MessageText:
'
'  File retrieved over FTP.
'
Public Const CRS_EVENT_PULLED_FILE_FTP        = &H4000381F

'
' MessageId: CRS_EVENT_FILE_COMMITTED
'
' MessageText:
'
'  File committed to disk.
'
Public Const CRS_EVENT_FILE_COMMITTED         = &H40003820

'
' MessageId: CRS_EVENT_FILE_DELETED
'
' MessageText:
'
'  File deleted.
'
Public Const CRS_EVENT_FILE_DELETED           = &H40003821

'
' MessageId: CRS_EVENT_SKIPPING_LOCKED_FILE
'
' MessageText:
'
'  File skipped because it could not be opened.
'
Public Const CRS_EVENT_SKIPPING_LOCKED_FILE   = &H80003822

'
' MessageId: CRS_EVENT_FILE_MATCHED
'
' MessageText:
'
'  File matched.
'
Public Const CRS_EVENT_FILE_MATCHED           = &H40003823

'
' MessageId: CRS_EVENT_FILE_LAST
'
' MessageText:
'
'  Site Server Content Deployment last file event message.
'
Public Const CRS_EVENT_FILE_LAST              = &H4000383F

'
' MessageId: CRS_EVENT_ADMIN_FIRST
'
' MessageText:
'
'  Site Server Content Deployment first administration event message.
'
Public Const CRS_EVENT_ADMIN_FIRST            = &H40003840

'
' MessageId: CRS_EVENT_PROJECT_CREATED
'
' MessageText:
'
'  Project created.
'
Public Const CRS_EVENT_PROJECT_CREATED        = &H40003841

'
' MessageId: CRS_EVENT_PROJECT_DELETED
'
' MessageText:
'
'  Project deleted.
'
Public Const CRS_EVENT_PROJECT_DELETED        = &H40003842

'
' MessageId: CRS_EVENT_PROJECT_EDITED
'
' MessageText:
'
'  Project edited.
'
Public Const CRS_EVENT_PROJECT_EDITED         = &H40003843

'
' MessageId: CRS_EVENT_ROUTE_CREATED
'
' MessageText:
'
'  Route created.
'
Public Const CRS_EVENT_ROUTE_CREATED          = &H40003844

'
' MessageId: CRS_EVENT_ROUTE_DELETED
'
' MessageText:
'
'  Route deleted.
'
Public Const CRS_EVENT_ROUTE_DELETED          = &H40003845

'
' MessageId: CRS_EVENT_ROUTE_EDITED
'
' MessageText:
'
'  Route edited.
'
Public Const CRS_EVENT_ROUTE_EDITED           = &H40003846

'
' MessageId: CRS_EVENT_SERVER_EDITED
'
' MessageText:
'
'  Server settings modified.
'
Public Const CRS_EVENT_SERVER_EDITED          = &H40003847

'
' MessageId: CRS_EVENT_EVENTSINK_CREATED
'
' MessageText:
'
'  Event sink specified by the data field was added.
'
Public Const CRS_EVENT_EVENTSINK_CREATED      = &H40003848

'
' MessageId: CRS_EVENT_EVENTSINK_DELETED
'
' MessageText:
'
'  Event sink specified by the data field was deleted.
'
Public Const CRS_EVENT_EVENTSINK_DELETED      = &H40003849

'
' MessageId: CRS_EVENT_CLEARED_EVENTS
'
' MessageText:
'
'  Event store specified by the data field was cleared.
'
Public Const CRS_EVENT_CLEARED_EVENTS         = &H4000384A

'
' MessageId: CRS_EVENT_CLEARED_ALL_EVENTS
'
' MessageText:
'
'  Each database event store on this server was cleared.
'
Public Const CRS_EVENT_CLEARED_ALL_EVENTS     = &H4000384B

'
' MessageId: CRS_EVENT_ADMIN_LAST
'
' MessageText:
'
'  Site Server Content Deployment last administration event message.
'
Public Const CRS_EVENT_ADMIN_LAST             = &H40003872

'
' MessageId: CRS_EVENT_SUMMARY_FIRST
'
' MessageText:
'
'  Site Server Content Deployment first summary event message.
'
Public Const CRS_EVENT_SUMMARY_FIRST          = &H400038A4

'
' MessageId: CRS_EVENT_SUMMARY_RECEIVE
'
' MessageText:
'
'  Project summary information (files received).
'
Public Const CRS_EVENT_SUMMARY_RECEIVE        = &H400038A5

'
' MessageId: CRS_EVENT_SUMMARY_SEND
'
' MessageText:
'
'  Project summary information (files sent).
'
Public Const CRS_EVENT_SUMMARY_SEND           = &H400038A6

'
' MessageId: CRS_EVENT_SUMMARY_PULL
'
' MessageText:
'
'  Project summary information (files pulled).
'
Public Const CRS_EVENT_SUMMARY_PULL           = &H400038A7

'
' MessageId: CRS_EVENT_SUMMARY_AUTOMATIC_RECEIVE
'
' MessageText:
'
'  Automatic project (receive) checkpoint summary information.
'
Public Const CRS_EVENT_SUMMARY_AUTOMATIC_RECEIVE = &H400038A8

'
' MessageId: CRS_EVENT_SUMMARY_AUTOMATIC_SEND
'
' MessageText:
'
'  Automatic project (send) checkpoint summary information.
'
Public Const CRS_EVENT_SUMMARY_AUTOMATIC_SEND = &H400038A9

'
' MessageId: CRS_EVENT_SUMMARY_RECEIVE_MATCHED
'
' MessageText:
'
'  Project summary information (files matched during receive).
'
Public Const CRS_EVENT_SUMMARY_RECEIVE_MATCHED = &H400038AA

'
' MessageId: CRS_EVENT_SUMMARY_SEND_MATCHED
'
' MessageText:
'
'  Project summary information (files matched during send).
'
Public Const CRS_EVENT_SUMMARY_SEND_MATCHED   = &H400038AB

'
' MessageId: CRS_EVENT_SUMMARY_PULL_MATCHED
'
' MessageText:
'
'  Project summary information (files matched during pull).
'
Public Const CRS_EVENT_SUMMARY_PULL_MATCHED   = &H400038AC

'
' MessageId: CRS_EVENT_SUMMARY_RECEIVE_ERRORED
'
' MessageText:
'
'  Project summary information (files errored during receive).
'
Public Const CRS_EVENT_SUMMARY_RECEIVE_ERRORED = &H400038AD

'
' MessageId: CRS_EVENT_SUMMARY_SEND_ERRORED
'
' MessageText:
'
'  Project summary information (files errored during send).
'
Public Const CRS_EVENT_SUMMARY_SEND_ERRORED   = &H400038AE

'
' MessageId: CRS_EVENT_SUMMARY_PULL_ERRORED
'
' MessageText:
'
'  Project summary information (files errored during pull).
'
Public Const CRS_EVENT_SUMMARY_PULL_ERRORED   = &H400038AF

'
' MessageId: CRS_EVENT_SUMMARY_LAST
'
' MessageText:
'
'  Site Server Content Deployment last summary event message.
'
Public Const CRS_EVENT_SUMMARY_LAST           = &H400038D6

'
' MessageId: CRS_EVENT_CAB_ERROR_FIRST
'
' MessageText:
'
'  Remote installer first error message.
'
Public Const CRS_EVENT_CAB_ERROR_FIRST        = &HC0003908

'
' MessageId: CRS_EVENT_CAB_ERROR_CABINET_NOT_FOUND
'
' MessageText:
'
'  Cannot find the specified cabinet file.
'
Public Const CRS_EVENT_CAB_ERROR_CABINET_NOT_FOUND = &HC0003909

'
' MessageId: CRS_EVENT_CAB_ERROR_NOT_A_CABINET
'
' MessageText:
'
'  Specified file is not a valid cabinet file.
'
Public Const CRS_EVENT_CAB_ERROR_NOT_A_CABINET = &HC000390A

'
' MessageId: CRS_EVENT_CAB_ERROR_UNKNOWN_CABINET_VERSION
'
' MessageText:
'
'  Specified file is an unknown version of a cabinet file.
'
Public Const CRS_EVENT_CAB_ERROR_UNKNOWN_CABINET_VERSION = &HC000390B

'
' MessageId: CRS_EVENT_CAB_ERROR_CORRUPT_CABINET
'
' MessageText:
'
'  Specified cabinet file is corrupt.
'
Public Const CRS_EVENT_CAB_ERROR_CORRUPT_CABINET = &HC000390C

'
' MessageId: CRS_EVENT_CAB_ERROR_ALLOC_FAIL
'
' MessageText:
'
'  Out of memory.
'
Public Const CRS_EVENT_CAB_ERROR_ALLOC_FAIL   = &HC000390D

'
' MessageId: CRS_EVENT_CAB_ERROR_BAD_COMPR_TYPE
'
' MessageText:
'
'  Specified cabinet file uses an unknown compression type.
'
Public Const CRS_EVENT_CAB_ERROR_BAD_COMPR_TYPE = &HC000390E

'
' MessageId: CRS_EVENT_CAB_ERROR_MDI_FAIL
'
' MessageText:
'
'  Compressed data in the specified cabinet file is corrupt.
'
Public Const CRS_EVENT_CAB_ERROR_MDI_FAIL     = &HC000390F

'
' MessageId: CRS_EVENT_CAB_ERROR_TARGET_FILE
'
' MessageText:
'
'  Could not save extracted files from the specified cabinet file.
'
Public Const CRS_EVENT_CAB_ERROR_TARGET_FILE  = &HC0003910

'
' MessageId: CRS_EVENT_CAB_ERROR_RESERVE_MISMATCH
'
' MessageText:
'
'  Specified cabinet file has a non-valid RESERVE size.
'
Public Const CRS_EVENT_CAB_ERROR_RESERVE_MISMATCH = &HC0003911

'
' MessageId: CRS_EVENT_CAB_ERROR_WRONG_CABINET
'
' MessageText:
'
'  Specified cabinet file is not the right cabinet in the set.
'
Public Const CRS_EVENT_CAB_ERROR_WRONG_CABINET = &HC0003912

'
' MessageId: CRS_EVENT_CAB_ERROR_USER_ABORT
'
' MessageText:
'
'  Could not extract the specified cabinet file. User aborted the operation.
'
Public Const CRS_EVENT_CAB_ERROR_USER_ABORT   = &HC0003913

'
' MessageId: CRS_EVENT_CAB_WARNING_CAB_NOT_SIGNED
'
' MessageText:
'
'  Specified cabinet file is not a signed cabinet file.
'
Public Const CRS_EVENT_CAB_WARNING_CAB_NOT_SIGNED = &H80003914

'
' MessageId: CRS_EVENT_CAB_WARNING_SIGNATURE_NOT_VERIFIED
'
' MessageText:
'
'  Signature on the specified cabinet could not be verified.
'
Public Const CRS_EVENT_CAB_WARNING_SIGNATURE_NOT_VERIFIED = &H80003915

'
' MessageId: CRS_EVENT_CAB_ERROR_INFFILE_NOT_FOUND
'
' MessageText:
'
'  INF file was not found inside the specified cabinet file.
'
Public Const CRS_EVENT_CAB_ERROR_INFFILE_NOT_FOUND = &HC0003916

'
' MessageId: CRS_EVENT_INF_ERROR_ADVPACK_NOT_LOADED
'
' MessageText:
'
'  Could not load ADVPACK.DLL.
'
Public Const CRS_EVENT_INF_ERROR_ADVPACK_NOT_LOADED = &HC0003917

'
' MessageId: CRS_EVENT_INF_ERROR_ADVPACK_WRONG_VERSION
'
' MessageText:
'
'  This version of ADVPACK.DLL is not supported.
'
Public Const CRS_EVENT_INF_ERROR_ADVPACK_WRONG_VERSION = &HC0003918

'
' MessageId: CRS_EVENT_INF_ERROR_INVALID_INF
'
' MessageText:
'
'  INF file found inside the specified cabinet file is not a valid INF file.
'
Public Const CRS_EVENT_INF_ERROR_INVALID_INF  = &HC0003919

'
' MessageId: CRS_EVENT_INF_ERROR_NOTHING_TO_INSTALL
'
' MessageText:
'
'  Could not find any of the remote install sections inside the INF file.
'
Public Const CRS_EVENT_INF_ERROR_NOTHING_TO_INSTALL = &HC000391A

'
' MessageId: CRS_EVENT_INF_ERROR_NOTHING_TO_UNINSTALL
'
' MessageText:
'
'  Could not find any of the remote uninstall sections inside the INF file.
'
Public Const CRS_EVENT_INF_ERROR_NOTHING_TO_UNINSTALL = &HC000391B

'
' MessageId: CRS_EVENT_INF_ERROR_BUSY_FILES
'
' MessageText:
'
'  Files are busy.  Cannot install them unless allowed to reboot.
'
Public Const CRS_EVENT_INF_ERROR_BUSY_FILES   = &HC000391C

'
' MessageId: CRS_EVENT_CAB_WARNING_MANUAL_REBOOT
'
' MessageText:
'
'  Setup completed successfully. The server could not be rebooted automatically. Manual reboot is necessary.
'
Public Const CRS_EVENT_CAB_WARNING_MANUAL_REBOOT = &H8000391D

'
' MessageId: CRS_EVENT_CAB_WARNING_REBOOT_NEEDED
'
' MessageText:
'
'  Setup completed successfully. The files became busy during setup.  Manual reboot is necessary.
'
Public Const CRS_EVENT_CAB_WARNING_REBOOT_NEEDED = &H8000391E

'
' MessageId: CRS_EVENT_INF_SUCCESS_REBOOT_IN_PROGRESS
'
' MessageText:
'
'  Setup completed successfully. The server is being rebooted.
'
Public Const CRS_EVENT_INF_SUCCESS_REBOOT_IN_PROGRESS = &H0000391F

'
' MessageId: CRS_EVENT_INF_SUCCESS
'
' MessageText:
'
'  Setup completed successfully.
'
Public Const CRS_EVENT_INF_SUCCESS            = &H00003921

'
' MessageId: CRS_EVENT_INF_ERROR_BUSY_FILES_UNINSTALL
'
' MessageText:
'
'  Files are busy.  Cannot uninstall them unless allowed to reboot.
'
Public Const CRS_EVENT_INF_ERROR_BUSY_FILES_UNINSTALL = &HC0003922

'
' MessageId: CRS_EVENT_CAB_WARNING_MANUAL_REBOOT_UNINSTALL
'
' MessageText:
'
'  Uninstall completed successfully. The server could not be rebooted automatically. Manual reboot is necessary.
'
Public Const CRS_EVENT_CAB_WARNING_MANUAL_REBOOT_UNINSTALL = &H80003923

'
' MessageId: CRS_EVENT_CAB_WARNING_REBOOT_NEEDED_UNINSTALL
'
' MessageText:
'
'  Uninstall completed successfully. The files became busy during setup.  Manual reboot is necessary.
'
Public Const CRS_EVENT_CAB_WARNING_REBOOT_NEEDED_UNINSTALL = &H80003924

'
' MessageId: CRS_EVENT_INF_SUCCESS_REBOOT_IN_PROGRESS_UNINSTALL
'
' MessageText:
'
'  Uninstall completed successfully. The server is being rebooted.
'
Public Const CRS_EVENT_INF_SUCCESS_REBOOT_IN_PROGRESS_UNINSTALL = &H00003925

'
' MessageId: CRS_EVENT_INF_SUCCESS_UNINSTALL
'
' MessageText:
'
'  Uninstall completed successfully.
'
Public Const CRS_EVENT_INF_SUCCESS_UNINSTALL  = &H00003926

'
' MessageId: CRS_EVENT_CAB_ERROR_ADVPACK_FAILED
'
' MessageText:
'
'  Problem installing the CAB. Check CAB and INF files for validity. 
'
Public Const CRS_EVENT_CAB_ERROR_ADVPACK_FAILED = &HC0003927

'
' MessageId: CRS_EVENT_CAB_ERROR_LOCKED_FILES
'
' MessageText:
'
'  At least one of the files inside the CAB is open for exclusive write/delete access.
'
Public Const CRS_EVENT_CAB_ERROR_LOCKED_FILES = &HC0003928

'
' MessageId: CRS_EVENT_CAB_ERROR_UNKNOWN
'
' MessageText:
'
'  Unknown error occurred while processing the specified cabinet file.
'
Public Const CRS_EVENT_CAB_ERROR_UNKNOWN      = &HC0003A06

'
' MessageId: CRS_EVENT_CAB_ERROR_LAST
'
' MessageText:
'
'  Remote installer last event message.
'
Public Const CRS_EVENT_CAB_ERROR_LAST         = &H40003A07

'
' MessageId: CRS_EVENT_LAST
'
' MessageText:
'
'  Site Server Content Deployment last event message.
'
Public Const CRS_EVENT_LAST                   = &H40003A97

