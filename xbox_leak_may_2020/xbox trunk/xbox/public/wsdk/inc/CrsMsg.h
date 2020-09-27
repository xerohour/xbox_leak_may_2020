//====================================================================
//                                                                     
//   Microsoft Site Server v3.00                                   
//                                                                     
//   Copyright (c) 1997 - 1999 Microsoft Corporation.  All rights reserved.   
//   THIS CODE AND INFORMATION IS PROVIDED 'AS IS' WITHOUT WARRANTY OF
//   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//   PARTICULAR PURPOSE.
//=====================================================================

//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: CRS_ERROR_FIRST
//
// MessageText:
//
//  Site Server Content Deployment service first error message.
//
#define CRS_ERROR_FIRST                  0x40003A98L

//
// MessageId: CRS_NET_SOURCE
//
// MessageText:
//
//  Source directory %2 for the project %1 is on a network drive.  Cannot deploy from network drives.
//
#define CRS_NET_SOURCE                   0xC0003A9CL

//
// MessageId: CRS_ERROR_ACCESS_DENIED
//
// MessageText:
//
//  Access denied.
//
#define CRS_ERROR_ACCESS_DENIED          0xC0003A9DL

//
// MessageId: CRS_FILE_CREATE_ERROR
//
// MessageText:
//
//  Unable to open the file %1.
//
#define CRS_FILE_CREATE_ERROR            0xC0003A9EL

//
// MessageId: CRS_SENDINET_ERROR
//
// MessageText:
//
//  Project was unable to send file %1.
//
#define CRS_SENDINET_ERROR               0xC0003A9FL

//
// MessageId: CRS_SAMEBOX_NET_TARGET
//
// MessageText:
//
//  Destination directory %2 for the project %1 is on a network drive.  Cannot deploy to network drives.
//
#define CRS_SAMEBOX_NET_TARGET           0xC0003AA0L

//
// MessageId: CRS_BAD_SOURCE
//
// MessageText:
//
//  Unable to access the project source %1.
//
#define CRS_BAD_SOURCE                   0xC0003AA1L

//
// MessageId: CRS_SENDINET_CONNECT
//
// MessageText:
//
//  Unable to connect to the remote server %1.
//
#define CRS_SENDINET_CONNECT             0xC0003AA2L

//
// MessageId: CRS_HASH_MISMATCH
//
// MessageText:
//
//  MD5 Hash did not match during a transmission.  This could indicate transmission failure or that the files transmitted at the source have been modified in transit.
//
#define CRS_HASH_MISMATCH                0xC0003AA3L

//
// MessageId: CRS_SERVER_ABORTED
//
// MessageText:
//
//  Remote server was restarted.  The project %1 is being restarted.
//
#define CRS_SERVER_ABORTED               0xC0003AA4L

//
// MessageId: CRS_REPLICATION_ABORTED
//
// MessageText:
//
//  Project %1 was aborted.
//
#define CRS_REPLICATION_ABORTED          0xC0003AA5L

//
// MessageId: CRS_SEQUENCE_ERROR
//
// MessageText:
//
//  An internal messaging sequence error occurred.
//
#define CRS_SEQUENCE_ERROR               0xC0003AA6L

//
// MessageId: CRS_SERVICE_STARTED
//
// MessageText:
//
//  Site Server Content Deployment service started successfully.
//
#define CRS_SERVICE_STARTED              0x40003AA7L

//
// MessageId: CRS_SERVICE_SHUTDOWN
//
// MessageText:
//
//  Site Server Content Deployment service shut down successfully.
//
#define CRS_SERVICE_SHUTDOWN             0x40003AA8L

//
// MessageId: CRS_ERROR_METABASE_ACCESS
//
// MessageText:
//
//  Could not open storage/stream %1.  Ensure the authentication account has access to this storage/stream.
//
#define CRS_ERROR_METABASE_ACCESS        0xC0003AAAL

//
// MessageId: CRS_AUTH_FAILURE
//
// MessageText:
//
//  Authorization failure against server %1.  Ensure the service has access to this server.
//
#define CRS_AUTH_FAILURE                 0xC0003AABL

//
// MessageId: CRS_REMOTE_AUTH_FAILURE
//
// MessageText:
//
//  Remote authorization failed to server %1.  Ensure the service has access to this server.
//
#define CRS_REMOTE_AUTH_FAILURE          0xC0003AACL

//
// MessageId: CRS_ERROR_NOT_IN_ORDER
//
// MessageText:
//
//  Message was received out of order.  Closing connection.
//
#define CRS_ERROR_NOT_IN_ORDER           0xC0003AADL

//
// MessageId: CRS_CONNECTION_RETRY_FAILED
//
// MessageText:
//
//  Connection to server %1 failed and could not be re-established.
//
#define CRS_CONNECTION_RETRY_FAILED      0xC0003AAEL

//
// MessageId: CRS_ERROR_REPLICATION_CANCELED
//
// MessageText:
//
//  Project %1 canceled at the request of the user.
//
#define CRS_ERROR_REPLICATION_CANCELED   0xC0003AAFL

//
// MessageId: CRS_ERROR_DEST_TOOBIG
//
// MessageText:
//
//  Could not add the specified destination(s).  The maximum number of destinations has been reached.
//
#define CRS_ERROR_DEST_TOOBIG            0xC0003AB0L

//
// MessageId: CRS_ERROR_NO_IP_MAPPING
//
// MessageText:
//
//  Cannot resolve IP address for server %1.  No virtual root created.
//
#define CRS_ERROR_NO_IP_MAPPING          0xC0003AB1L

//
// MessageId: CRS_ERROR_DISK_FULL
//
// MessageText:
//
//  Not enough space on the disk.
//
#define CRS_ERROR_DISK_FULL              0xC0003AB2L

//
// MessageId: CRS_ERROR_TOO_MANY_RUNNING
//
// MessageText:
//
//  Too many active projects. No more can be started until others complete.
//
#define CRS_ERROR_TOO_MANY_RUNNING       0xC0003AB3L

//
// MessageId: CRS_ERROR_WOULDBLOCK
//
// MessageText:
//
//  The network operation would block.
//
#define CRS_ERROR_WOULDBLOCK             0x00003ABBL

//
// MessageId: CRS_ERROR_NETDOWN
//
// MessageText:
//
//  The remote network is down.
//
#define CRS_ERROR_NETDOWN                0xC0003ACAL

//
// MessageId: CRS_ERROR_NETUNREACH
//
// MessageText:
//
//  The remote network is unreachable.
//
#define CRS_ERROR_NETUNREACH             0xC0003ACBL

//
// MessageId: CRS_ERROR_CONNABORTED
//
// MessageText:
//
//  The network connection was aborted at the network layer.
//
#define CRS_ERROR_CONNABORTED            0xC0003ACDL

//
// MessageId: CRS_ERROR_TIMEOUT
//
// MessageText:
//
//  The network session timed out on a send or receive operation.
//
#define CRS_ERROR_TIMEOUT                0xC0003AD4L

//
// MessageId: WINSOCK_WSAEWOULDBLOCK
//
// MessageText:
//
//  The network operation would block.
//
#define WINSOCK_WSAEWOULDBLOCK           0x00002733L

//
// MessageId: WINSOCK_WSAENETDOWN
//
// MessageText:
//
//  The remote network is down.
//
#define WINSOCK_WSAENETDOWN              0x00002742L

//
// MessageId: WINSOCK_WSAENETUNREACH
//
// MessageText:
//
//  The remote network is unreachable.
//
#define WINSOCK_WSAENETUNREACH           0x00002743L

//
// MessageId: WINSOCK_WSAECONNABORTED
//
// MessageText:
//
//  The network connection was aborted at the network layer.
//
#define WINSOCK_WSAECONNABORTED          0x00002745L

//
// MessageId: WINSOCK_WSAECONNRESET
//
// MessageText:
//
//  The network session was aborted.
//
#define WINSOCK_WSAECONNRESET            0x00002746L

//
// MessageId: WINSOCK_WSAETIMEDOUT
//
// MessageText:
//
//  The network session timed out on a send or receive operation.
//
#define WINSOCK_WSAETIMEDOUT             0x0000274CL

//
// MessageId: WINSOCK_WSAECONNREFUSED
//
// MessageText:
//
//  The remote server was not listening on the appropriate port. This generally indicates that the service was not running but the server is up.
//
#define WINSOCK_WSAECONNREFUSED          0x0000274DL

//
// MessageId: CRS_ERROR_NO_PROJECTS
//
// MessageText:
//
//  No projects defined.
//
#define CRS_ERROR_NO_PROJECTS            0xC0003AFCL

//
// MessageId: CRS_ERROR_PROJECT_NOT_FOUND
//
// MessageText:
//
//  Could not find project %1.
//
#define CRS_ERROR_PROJECT_NOT_FOUND      0xC0003AFDL

//
// MessageId: CRS_ERROR_INVALID_PARAMETER
//
// MessageText:
//
//  Invalid parameter.
//
#define CRS_ERROR_INVALID_PARAMETER      0xC0003AFEL

//
// MessageId: CRS_PROJECT_CREATED
//
// MessageText:
//
//  Created project %1.
//
#define CRS_PROJECT_CREATED              0xC0003AFFL

//
// MessageId: CRS_ERROR_AUTHENTICATION_FAILURE
//
// MessageText:
//
//  Authentication failure.  Ensure the account has proper access on local and remote servers.
//
#define CRS_ERROR_AUTHENTICATION_FAILURE 0xC0003B00L

//
// MessageId: CRS_ERROR_DELETE_PROJECT_DOWN_ROUTE
//
// MessageText:
//
//  Could not delete the route from the project on at least one server down the defined routes.  The project will have to be edited manually there.
//
#define CRS_ERROR_DELETE_PROJECT_DOWN_ROUTE 0x80003B01L

//
// MessageId: CRS_ERROR_SERVICE_NOT_RUNNING
//
// MessageText:
//
//  Server %1 is available, but the Site Server Content Deployment service is not running.
//
#define CRS_ERROR_SERVICE_NOT_RUNNING    0xC0003B02L

//
// MessageId: CRS_ERROR_HOST_DOWN
//
// MessageText:
//
//  Server %1 is unavailable.
//
#define CRS_ERROR_HOST_DOWN              0xC0003B03L

//
// MessageId: CRS_ERROR_BAD_NET_NAME
//
// MessageText:
//
//  Server %1 cannot be found.
//
#define CRS_ERROR_BAD_NET_NAME           0xC0003B04L

//
// MessageId: CRS_ERROR_ROUTE_NOT_FOUND
//
// MessageText:
//
//  Could not find route %1.  Ensure the route name is correct and that it exists on the target servers.
//
#define CRS_ERROR_ROUTE_NOT_FOUND        0xC0003B05L

//
// MessageId: CRS_ERROR_NO_ROUTES
//
// MessageText:
//
//  No routes defined.
//
#define CRS_ERROR_NO_ROUTES              0xC0003B06L

//
// MessageId: CRS_ERROR_CREATE_PROJECT_DOWN_ROUTE
//
// MessageText:
//
//  Could not create the project on at least one server down the defined routes.  The project will have to be created manually there.
//
#define CRS_ERROR_CREATE_PROJECT_DOWN_ROUTE 0x80003B07L

//
// MessageId: CRS_ERROR_SERVICE_PAUSED
//
// MessageText:
//
//  Site Server Content Deployment service on server %1 is paused and is not accepting new requests.  Retry project %2 later.
//
#define CRS_ERROR_SERVICE_PAUSED         0xC0003B08L

//
// MessageId: CRS_ERROR_SERVICE_NOT_PAUSED
//
// MessageText:
//
//  Could not restart service. Service is not paused.
//
#define CRS_ERROR_SERVICE_NOT_PAUSED     0xC0003B09L

//
// MessageId: CRS_ERROR_NAME_NOT_RESOLVED
//
// MessageText:
//
//  Could not resolve URL %2 for project %1.  Ensure URL and/or proxy settings are correct.
//
#define CRS_ERROR_NAME_NOT_RESOLVED      0xC0003B0AL

//
// MessageId: CRS_ERROR_CONNECTION_RESET
//
// MessageText:
//
//  Connection to server %1 has been reset.  This indicates problems with the remote service or network.
//
#define CRS_ERROR_CONNECTION_RESET       0xC0003B0BL

//
// MessageId: CRS_ERROR_REPLICATION_NOT_FOUND
//
// MessageText:
//
//  No entries.
//
#define CRS_ERROR_REPLICATION_NOT_FOUND  0xC0003B0CL

//
// MessageId: CRS_ERROR_SERVICE_ALREADY_RUNNING
//
// MessageText:
//
//  Could not start service.  The service has already been started.
//
#define CRS_ERROR_SERVICE_ALREADY_RUNNING 0xC0003B0DL

//
// MessageId: CRS_ERROR_PROJECT_ALREADY_RUNNING
//
// MessageText:
//
//  Failed to start project %1.  It is already running.
//
#define CRS_ERROR_PROJECT_ALREADY_RUNNING 0xC0003B0EL

//
// MessageId: CRS_ERROR_FILE_MOVE_ERROR
//
// MessageText:
//
//  Failed to move file %1 to %2.
//
#define CRS_ERROR_FILE_MOVE_ERROR        0xC0003B0FL

//
// MessageId: CRS_ERROR_PRODUCT_EXPIRED
//
// MessageText:
//
//  Thank you for your interest in Site Server Content Deployment.  This software has expired.  Please acquire the final retail version of this software.
//
#define CRS_ERROR_PRODUCT_EXPIRED        0xC0003B10L

//
// MessageId: CRS_ERROR_PROJECT_INCOMPLETE
//
// MessageText:
//
//  Project information for %1 is incomplete.
//
#define CRS_ERROR_PROJECT_INCOMPLETE     0xC0003B11L

//
// MessageId: CRS_ERROR_PARAMETER_NOT_FOUND
//
// MessageText:
//
//  Parameter %1 not found.
//
#define CRS_ERROR_PARAMETER_NOT_FOUND    0xC0003B12L

//
// MessageId: CRS_ERROR_REPLICATION_RUNNING
//
// MessageText:
//
//  Project %1 is running.  The operation could not be completed.
//
#define CRS_ERROR_REPLICATION_RUNNING    0xC0003B13L

//
// MessageId: CRS_ERROR_INVALID_PROJECT_NAME
//
// MessageText:
//
//  Project name %1 is invalid.  Project names cannot contain backslashes and cannot be empty.
//
#define CRS_ERROR_INVALID_PROJECT_NAME   0xC0003B14L

//
// MessageId: CRS_ERROR_INVALID_ROUTE_NAME
//
// MessageText:
//
//  Route name %1 is invalid.  Route names cannot contain backslashes or DBCS characters and cannot be empty.
//
#define CRS_ERROR_INVALID_ROUTE_NAME     0xC0003B15L

//
// MessageId: CRS_ERROR_INVALID_DWORD
//
// MessageText:
//
//  Parameter %1 must be a positive integer.
//
#define CRS_ERROR_INVALID_DWORD          0xC0003B16L

//
// MessageId: CRS_ERROR_NO_MORE_ITEMS
//
// MessageText:
//
//  No more items in the list.
//
#define CRS_ERROR_NO_MORE_ITEMS          0x80003B17L

//
// MessageId: CRS_ERROR_TOO_MANY_ARGS
//
// MessageText:
//
//  Too many arguments specified.
//
#define CRS_ERROR_TOO_MANY_ARGS          0xC0003B18L

//
// MessageId: CRS_MBS_INIT_ERROR
//
// MessageText:
//
//  Internal error trying to initialize for ACL replication.
//
#define CRS_MBS_INIT_ERROR               0xC0003B19L

//
// MessageId: CRS_ERROR_OPENING_FILE
//
// MessageText:
//
//  Error opening file %1 or file not found.
//
#define CRS_ERROR_OPENING_FILE           0xC0003B1BL

//
// MessageId: CRS_ERROR_FILE_FORMAT
//
// MessageText:
//
//  Invalid file format.
//
#define CRS_ERROR_FILE_FORMAT            0xC0003B1CL

//
// MessageId: CRS_ERROR_FILE_WRITE
//
// MessageText:
//
//  Error writing to file %1.
//
#define CRS_ERROR_FILE_WRITE             0xC0003B1DL

//
// MessageId: WINSOCK_WSAHOSTNAME
//
// MessageText:
//
//  Gethostname failed.
//
#define WINSOCK_WSAHOSTNAME              0xC0003B1EL

//
// MessageId: CRS_ERROR_NODE_EXISTS
//
// MessageText:
//
//  Node already exists in the topology map.
//
#define CRS_ERROR_NODE_EXISTS            0xC0003B1FL

//
// MessageId: CRS_ERROR_NODE_NOT_FOUND
//
// MessageText:
//
//  Node not found in topology map.
//
#define CRS_ERROR_NODE_NOT_FOUND         0xC0003B20L

//
// MessageId: CRS_ERROR_EDGE_EXISTS
//
// MessageText:
//
//  Edge already exists in topology map.
//
#define CRS_ERROR_EDGE_EXISTS            0x80003B21L

//
// MessageId: CRS_ERROR_ROOTNODE_NOT_FOUND
//
// MessageText:
//
//  Root node does not exist in topology map.
//
#define CRS_ERROR_ROOTNODE_NOT_FOUND     0xC0003B22L

//
// MessageId: CRS_ERROR_PATH_NOT_FOUND
//
// MessageText:
//
//  No path to from root to node in topology map.
//
#define CRS_ERROR_PATH_NOT_FOUND         0xC0003B23L

//
// MessageId: CRS_ERROR_EDGE_NOT_FOUND
//
// MessageText:
//
//  Edge does not exist in topology map.
//
#define CRS_ERROR_EDGE_NOT_FOUND         0xC0003B24L

//
// MessageId: CRS_ERROR_INVALID_CONTEXT
//
// MessageText:
//
//  Current context is invalid.  Project being reset.
//
#define CRS_ERROR_INVALID_CONTEXT        0xC0003B25L

//
// MessageId: CRS_ERROR_DOWNLEVEL_SERVER
//
// MessageText:
//
//  Operation is not supported on a down-level server.
//
#define CRS_ERROR_DOWNLEVEL_SERVER       0xC0003B26L

//
// MessageId: CRS_ERROR_NO_MAPPING
//
// MessageText:
//
//  URL does not map to any project definitions.
//
#define CRS_ERROR_NO_MAPPING             0xC0003B27L

//
// MessageId: CRS_ERROR_START_PENDING
//
// MessageText:
//
//  Start pending completion of the active project.
//
#define CRS_ERROR_START_PENDING          0xC0003B28L

//
// MessageId: CRS_ERROR_NO_EVENTSINKS
//
// MessageText:
//
//  No event sinks defined.
//
#define CRS_ERROR_NO_EVENTSINKS          0xC0003B29L

//
// MessageId: CRS_ERROR_EVENTSINK_NOT_FOUND
//
// MessageText:
//
//  Could not find event sink %1.
//
#define CRS_ERROR_EVENTSINK_NOT_FOUND    0xC0003B2AL

//
// MessageId: CRS_ERROR_WSAHOSTNOTFOUND
//
// MessageText:
//
//  Host not found.
//
#define CRS_ERROR_WSAHOSTNOTFOUND        0xC0003B2BL

//
// MessageId: CRS_ERROR_WSATRYAGAIN
//
// MessageText:
//
//  Host not found, or SERVERFAIL.  Try again.
//
#define CRS_ERROR_WSATRYAGAIN            0xC0003B2CL

//
// MessageId: CRS_ERROR_WSANORECOVERY
//
// MessageText:
//
//  Non-recoverable winsock error.
//
#define CRS_ERROR_WSANORECOVERY          0xC0003B2DL

//
// MessageId: CRS_ERROR_WSANODATA
//
// MessageText:
//
//  Invalid address.
//
#define CRS_ERROR_WSANODATA              0xC0003B2EL

//
// MessageId: CRS_EVENTSINK_INITIALIZED
//
// MessageText:
//
//  Event sink %1 was successfully initialized.
//
#define CRS_EVENTSINK_INITIALIZED        0x00003B2FL

//
// MessageId: CRS_ERROR_EVENTSINK_NOT_INITIALIZED
//
// MessageText:
//
//  Event sink %1 failed to initialize.
//
#define CRS_ERROR_EVENTSINK_NOT_INITIALIZED 0xC0003B30L

//
// MessageId: CRS_ERROR_NOTHING_TO_ROLLBACK
//
// MessageText:
//
//  Could not roll back project.  No rollback information found.
//
#define CRS_ERROR_NOTHING_TO_ROLLBACK    0xC0003B31L

//
// MessageId: CRS_ERROR_NO_DESTINATIONS
//
// MessageText:
//
//  Project could not be started because no destinations are defined.
//
#define CRS_ERROR_NO_DESTINATIONS        0xC0003B32L

//
// MessageId: CRS_ERROR_REAUTHENTICATE
//
// MessageText:
//
//  Re-authenticate using an acceptable protocol.
//
#define CRS_ERROR_REAUTHENTICATE         0xC0003B33L

//
// MessageId: CRS_ERROR_UNSUPPORTED
//
// MessageText:
//
//  Function not supported in this version to this server.
//
#define CRS_ERROR_UNSUPPORTED            0xC0003B34L

//
// MessageId: CRS_ERROR_USER_NOT_FOUND
//
// MessageText:
//
//  Specified user %1 could not be found.
//
#define CRS_ERROR_USER_NOT_FOUND         0xC0003B35L

//
// MessageId: CRS_ERROR_NO_TRANSACTIONS
//
// MessageText:
//
//  No transactions for the specified project.
//
#define CRS_ERROR_NO_TRANSACTIONS        0xC0003B36L

//
// MessageId: CRS_ERROR_NO_SERVERS
//
// MessageText:
//
//  No destination servers defined.
//
#define CRS_ERROR_NO_SERVERS             0xC0003B37L

//
// MessageId: CRS_ERROR_NO_SCHEDULES
//
// MessageText:
//
//  No schedules defined for this project.
//
#define CRS_ERROR_NO_SCHEDULES           0xC0003B38L

//
// MessageId: CRS_ERROR_NO_EMAIL
//
// MessageText:
//
//  Could not contact SMTP server %1 to send e-mail report for project %2.
//
#define CRS_ERROR_NO_EMAIL               0xC0003B39L

//
// MessageId: CRS_ERROR_WAIT_ABORT
//
// MessageText:
//
//  Waiting abort on project %1 after losing connection with source server.
//
#define CRS_ERROR_WAIT_ABORT             0xC0003B3AL

//
// MessageId: CRS_RESUME_REPLICATION
//
// MessageText:
//
//  Resuming project %1 to server %2.
//
#define CRS_RESUME_REPLICATION           0x40003B3BL

//
// MessageId: CRS_METABASE_NOT_FOUND
//
// MessageText:
//
//  Metabase is not responding.
//
#define CRS_METABASE_NOT_FOUND           0xC0003B3CL

//
// MessageId: CRS_SSE_VROOT_NOT_FOUND
//
// MessageText:
//
//  Site Server virtual directory not found.
//
#define CRS_SSE_VROOT_NOT_FOUND          0xC0003B3DL

//
// MessageId: CRS_WARNING_CONNECTION_FAILED
//
// MessageText:
//
//  Connection to server %1 for project %2 failed.  Will attempt to re-establish session.
//
#define CRS_WARNING_CONNECTION_FAILED    0x80003B3EL

//
// MessageId: CRS_ERROR_DROPPED_EMAIL_REPORT
//
// MessageText:
//
//  Dropped e-mail report for project %1.
//
#define CRS_ERROR_DROPPED_EMAIL_REPORT   0xC0003B3FL

//
// MessageId: CRS_ERROR_EMAIL_HOST_NOT_DEFINED
//
// MessageText:
//
//  No SMTP server defined.
//
#define CRS_ERROR_EMAIL_HOST_NOT_DEFINED 0xC0003B40L

//
// MessageId: CRS_ERROR_VROOT_CREATION
//
// MessageText:
//
//  Could not create the specified virtual directory.
//
#define CRS_ERROR_VROOT_CREATION         0x80003B41L

//
// MessageId: CRS_ERROR_UPLPAGE_CREATION
//
// MessageText:
//
//  Could not create the upload page.
//
#define CRS_ERROR_UPLPAGE_CREATION       0xC0003B42L

//
// MessageId: CRS_ERROR_CIRCULAR_ROUTE
//
// MessageText:
//
//  System has detected a circular route.  Project not created.
//
#define CRS_ERROR_CIRCULAR_ROUTE         0xC0003B43L

//
// MessageId: CRS_ERROR_TRANSACTION_APPLY_FAILED
//
// MessageText:
//
//  Transaction application for project %1 failed.  Check permissions on the destination directory and files and ensure that no files are locked open.
//
#define CRS_ERROR_TRANSACTION_APPLY_FAILED 0xC0003B44L

//
// MessageId: CRS_ERROR_DATABASE
//
// MessageText:
//
//  Error occurred with the database %1.  Error is: %2.
//
#define CRS_ERROR_DATABASE               0xC0003B45L

//
// MessageId: CRS_ERROR_SINK_CONNECT_FAILED
//
// MessageText:
//
//  Unable to connect to remote server %1 to forward events.
//
#define CRS_ERROR_SINK_CONNECT_FAILED    0xC0003B46L

//
// MessageId: CRS_ERROR_CANT_SEND_EVENTS_TO_DOWNLEVEL_SERVER
//
// MessageText:
//
//  Unable to send events to server %1.  This operation is not supported.
//
#define CRS_ERROR_CANT_SEND_EVENTS_TO_DOWNLEVEL_SERVER 0xC0003B47L

//
// MessageId: CRS_ERROR_FILE_CORRUPT
//
// MessageText:
//
//  File %1 is corrupt or unreadable.
//
#define CRS_ERROR_FILE_CORRUPT           0xC0003B48L

//
// MessageId: CRS_ERROR_FILE_BUSY
//
// MessageText:
//
//  File %1 is busy.
//
#define CRS_ERROR_FILE_BUSY              0xC0003B49L

//
// MessageId: CRS_ACL_ERROR_INVALID_ACE
//
// MessageText:
//
//  Could not set the ACE %1 on file %2.
//
#define CRS_ACL_ERROR_INVALID_ACE        0x80003B4AL

//
// MessageId: CRS_ACL_ERROR_ACL_FAILED
//
// MessageText:
//
//  Could not set the ACLS appropriately on file %1, setting default ACLs on this file.
//
#define CRS_ACL_ERROR_ACL_FAILED         0xC0003B4BL

//
// MessageId: CRS_ERROR_PROJECT_NOT_FOUND_ON_DESTINATION
//
// MessageText:
//
//  Could not find project %1 on server %2.
//
#define CRS_ERROR_PROJECT_NOT_FOUND_ON_DESTINATION 0xC0003B4CL

//
// MessageId: CRS_ERROR_PROJECT_INCOMPLETE_ON_DESTINATION
//
// MessageText:
//
//  Project information for project %1 is incomplete on server %2.
//
#define CRS_ERROR_PROJECT_INCOMPLETE_ON_DESTINATION 0xC0003B4DL

//
// MessageId: CRS_ERROR_SERVICE_DOWN_ON_REMOTE_SERVER
//
// MessageText:
//
//  Remote server available, but the Site Server Content Deployment service is not running.
//
#define CRS_ERROR_SERVICE_DOWN_ON_REMOTE_SERVER 0xC0003B4EL

//
// MessageId: CRS_ERROR_DISK_FULL_ON_DESTINATION
//
// MessageText:
//
//  Disk is full on the destination server %1.
//
#define CRS_ERROR_DISK_FULL_ON_DESTINATION 0xC0003B4FL

//
// MessageId: CRS_FILE_CREATE_ERROR_ON_DESTINATION
//
// MessageText:
//
//  Unable to open a file on the destination server %1.  Check permissions on the project directory and files.
//
#define CRS_FILE_CREATE_ERROR_ON_DESTINATION 0xC0003B50L

//
// MessageId: CRS_ERROR_REMOTE_SERVER_NOT_FOUND
//
// MessageText:
//
//  Remote server could not be found.
//
#define CRS_ERROR_REMOTE_SERVER_NOT_FOUND 0xC0003B51L

//
// MessageId: CRS_ERROR_REMOTE_APPLY_FAILED
//
// MessageText:
//
//  Unable to apply transactions for project %1 on server %2.
//
#define CRS_ERROR_REMOTE_APPLY_FAILED    0xC0003B52L

//
// MessageId: CRS_ERROR_INVALID_FLAG_COMBINATION
//
// MessageText:
//
//  Invalid flag combination.
//
#define CRS_ERROR_INVALID_FLAG_COMBINATION 0xC0003B53L

//
// MessageId: CRS_ERROR_RUNNING_SCRIPT
//
// MessageText:
//
//  Error running the script %1.  Ensure that the script exists and can be located in the path.
//
#define CRS_ERROR_RUNNING_SCRIPT         0xC0003B54L

//
// MessageId: CRS_ERROR_CANT_FORWARD_TO_LOCAL_SERVER
//
// MessageText:
//
//  Cannot forward events to the local server.
//
#define CRS_ERROR_CANT_FORWARD_TO_LOCAL_SERVER 0xC0003B55L

//
// MessageId: CRS_ERROR_SCHEDULE_NOT_FOUND
//
// MessageText:
//
//  Specified project schedule could not be found.
//
#define CRS_ERROR_SCHEDULE_NOT_FOUND     0xC0003B56L

//
// MessageId: CRS_ERROR_METABASE
//
// MessageText:
//
//  An error occurred accessing the metabase for the project %1.  Ensure that the web site exists and that the account has access to it.
//
#define CRS_ERROR_METABASE               0xC0003B57L

//
// MessageId: CRS_ERROR_SCHEDULER_FAILURE
//
// MessageText:
//
//  An error occurred with the task scheduler.
//
#define CRS_ERROR_SCHEDULER_FAILURE      0xC0003B58L

//
// MessageId: CRS_SAMEBOX_META_TARGET
//
// MessageText:
//
//  The destination %1 for metabase replication is invalid.
//
#define CRS_SAMEBOX_META_TARGET          0xC0003B59L

//
// MessageId: CRS_ERROR_DROPPED_EVENTS
//
// MessageText:
//
//  The event sink %1 has started dropping events.
//
#define CRS_ERROR_DROPPED_EVENTS         0xC0003B5AL

//
// MessageId: CRS_MSG_REPLICATION_FIRST
//
// MessageText:
//
//  Site Server Content Deployment first replication event message.
//
#define CRS_MSG_REPLICATION_FIRST        0x40003BC4L

//
// MessageId: CRS_MSG_STARTING_REPLICATION
//
// MessageText:
//
//  Project %1 starting to server %2.
//
#define CRS_MSG_STARTING_REPLICATION     0x40003BC5L

//
// MessageId: CRS_MSG_REPLICATION_COMPLETE
//
// MessageText:
//
//  Project %1 complete to server %2.
//
#define CRS_MSG_REPLICATION_COMPLETE     0x40003BC6L

//
// MessageId: CRS_MSG_STARTING_PULL_REPLICATION
//
// MessageText:
//
//  Retrieval project %1 starting from URL %2.
//
#define CRS_MSG_STARTING_PULL_REPLICATION 0x40003BC7L

//
// MessageId: CRS_MSG_PULL_REPLICATION_COMPLETE
//
// MessageText:
//
//  Retrieval project %1 complete from URL %2.
//
#define CRS_MSG_PULL_REPLICATION_COMPLETE 0x40003BC8L

//
// MessageId: CRS_MSG_STARTING_RECEIVE_REPLICATION
//
// MessageText:
//
//  Starting to receive project %1 from server %2.
//
#define CRS_MSG_STARTING_RECEIVE_REPLICATION 0x40003BC9L

//
// MessageId: CRS_MSG_REPLICATION_RECEIVE_COMPLETE
//
// MessageText:
//
//  Finished receiving project %1 from server %2.
//
#define CRS_MSG_REPLICATION_RECEIVE_COMPLETE 0x40003BCAL

//
// MessageId: CRS_MSG_STARTING_REPLICATION_SCAN
//
// MessageText:
//
//  Project %1 starting.
//
#define CRS_MSG_STARTING_REPLICATION_SCAN 0x40003BCBL

//
// MessageId: CRS_MSG_REPLICATION_LAST
//
// MessageText:
//
//  Site Server Content Deployment last replication event message.
//
#define CRS_MSG_REPLICATION_LAST         0x40003BF5L

//
// MessageId: CRS_MSG_FILE_FIRST
//
// MessageText:
//
//  Site Server Content Deployment first file event message.
//
#define CRS_MSG_FILE_FIRST               0x40003BF6L

//
// MessageId: CRS_MSG_SENDING_FILE
//
// MessageText:
//
//  Sending file %1.
//
#define CRS_MSG_SENDING_FILE             0x40003BF7L

//
// MessageId: CRS_MSG_SENT_FILE
//
// MessageText:
//
//  File %1 sent.
//
#define CRS_MSG_SENT_FILE                0x40003BF8L

//
// MessageId: CRS_MSG_RECEIVING_FILE
//
// MessageText:
//
//  Receiving file %1.
//
#define CRS_MSG_RECEIVING_FILE           0x40003BF9L

//
// MessageId: CRS_MSG_RECEIVED_FILE
//
// MessageText:
//
//  File %1 received.
//
#define CRS_MSG_RECEIVED_FILE            0x40003BFAL

//
// MessageId: CRS_MSG_SENT_DELETE_FILE
//
// MessageText:
//
//  Sent delete file request for file %1.
//
#define CRS_MSG_SENT_DELETE_FILE         0x40003BFBL

//
// MessageId: CRS_MSG_RECEIVED_DELETE_FILE
//
// MessageText:
//
//  File delete request received for file %1.
//
#define CRS_MSG_RECEIVED_DELETE_FILE     0x40003BFCL

//
// MessageId: CRS_MSG_RETRYING_SENDING_FILE
//
// MessageText:
//
//  Trying to resend file %1.
//
#define CRS_MSG_RETRYING_SENDING_FILE    0x40003C00L

//
// MessageId: CRS_MSG_FILE_BUSY
//
// MessageText:
//
//  File %1 is busy.  The service is waiting for the file to become available.
//
#define CRS_MSG_FILE_BUSY                0x40003C01L

//
// MessageId: CRS_MSG_SENDING_ACL
//
// MessageText:
//
//  Sending the ACL for the file %1.
//
#define CRS_MSG_SENDING_ACL              0x40003C02L

//
// MessageId: CRS_MSG_SENT_ACL
//
// MessageText:
//
//  Sent the ACL for the file %1.
//
#define CRS_MSG_SENT_ACL                 0x40003C03L

//
// MessageId: CRS_MSG_RECEIVING_ACL
//
// MessageText:
//
//  Receiving the ACL for the file %1.
//
#define CRS_MSG_RECEIVING_ACL            0x40003C04L

//
// MessageId: CRS_MSG_RECEIVED_ACL
//
// MessageText:
//
//  Received the ACL for the file %1.
//
#define CRS_MSG_RECEIVED_ACL             0x40003C05L

//
// MessageId: CRS_MSG_PULLED_FILE_HTTP
//
// MessageText:
//
//  File %1 retrieved over HTTP.
//
#define CRS_MSG_PULLED_FILE_HTTP         0x40003C06L

//
// MessageId: CRS_MSG_PULLED_FILE_FTP
//
// MessageText:
//
//  File %1 retrieved over FTP.
//
#define CRS_MSG_PULLED_FILE_FTP          0x40003C07L

//
// MessageId: CRS_MSG_FILE_COMMITTED
//
// MessageText:
//
//  File %1 committed to disk.
//
#define CRS_MSG_FILE_COMMITTED           0x40003C08L

//
// MessageId: CRS_MSG_FILE_DELETED
//
// MessageText:
//
//  File %1 deleted.
//
#define CRS_MSG_FILE_DELETED             0x40003C09L

//
// MessageId: CRS_MSG_SKIPPING_LOCKED_FILE
//
// MessageText:
//
//  File %1 skipped because it could not be opened.
//
#define CRS_MSG_SKIPPING_LOCKED_FILE     0x80003C0AL

//
// MessageId: CRS_FILE_MATCHED
//
// MessageText:
//
//  File %1 matched.
//
#define CRS_FILE_MATCHED                 0x40003C0BL

//
// MessageId: CRS_MSG_FILE_LAST
//
// MessageText:
//
//  Site Server Content Deployment last file event message.
//
#define CRS_MSG_FILE_LAST                0x40003C27L

//
// MessageId: CRS_MSG_ADMIN_FIRST
//
// MessageText:
//
//  Site Server Content Deployment first administration event message.
//
#define CRS_MSG_ADMIN_FIRST              0x40003C28L

//
// MessageId: CRS_MSG_PROJECT_CREATED
//
// MessageText:
//
//  Project %1 created.
//
#define CRS_MSG_PROJECT_CREATED          0x40003C29L

//
// MessageId: CRS_MSG_PROJECT_DELETED
//
// MessageText:
//
//  Project %1 deleted.
//
#define CRS_MSG_PROJECT_DELETED          0x40003C2AL

//
// MessageId: CRS_MSG_PROJECT_EDITED
//
// MessageText:
//
//  Project %1 edited.
//
#define CRS_MSG_PROJECT_EDITED           0x40003C2BL

//
// MessageId: CRS_MSG_ROUTE_CREATED
//
// MessageText:
//
//  Route %1 created.
//
#define CRS_MSG_ROUTE_CREATED            0x40003C2CL

//
// MessageId: CRS_MSG_ROUTE_DELETED
//
// MessageText:
//
//  Route %1 deleted.
//
#define CRS_MSG_ROUTE_DELETED            0x40003C2DL

//
// MessageId: CRS_MSG_ROUTE_EDITED
//
// MessageText:
//
//  Route %1 edited.
//
#define CRS_MSG_ROUTE_EDITED             0x40003C2EL

//
// MessageId: CRS_MSG_SERVER_EDITED
//
// MessageText:
//
//  Server settings modified.
//
#define CRS_MSG_SERVER_EDITED            0x40003C2FL

//
// MessageId: CRS_MSG_EVENTSINK_CREATED
//
// MessageText:
//
//  Event sink %1 was added.
//
#define CRS_MSG_EVENTSINK_CREATED        0x40003C30L

//
// MessageId: CRS_MSG_EVENTSINK_DELETED
//
// MessageText:
//
//  Event sink %1 was deleted.
//
#define CRS_MSG_EVENTSINK_DELETED        0x40003C31L

//
// MessageId: CRS_MSG_CLEARED_EVENTS
//
// MessageText:
//
//  Event store %1 was cleared.
//
#define CRS_MSG_CLEARED_EVENTS           0x40003C32L

//
// MessageId: CRS_MSG_CLEARED_ALL_EVENTS
//
// MessageText:
//
//  Each database event store on this server was cleared.
//
#define CRS_MSG_CLEARED_ALL_EVENTS       0x40003C33L

//
// MessageId: CRS_MSG_ADMIN_LAST
//
// MessageText:
//
//  Site Server Content Deployment last administration event message.
//
#define CRS_MSG_ADMIN_LAST               0x40003C5AL

//
// MessageId: CRS_MSG_SUMMARY_FIRST
//
// MessageText:
//
//  Site Server Content Deployment first summary event message.
//
#define CRS_MSG_SUMMARY_FIRST            0x40003C8CL

//
// MessageId: CRS_MSG_SUMMARY_RECEIVE
//
// MessageText:
//
//  Project summary information (files received).
//
#define CRS_MSG_SUMMARY_RECEIVE          0x40003C8DL

//
// MessageId: CRS_MSG_SUMMARY_SEND
//
// MessageText:
//
//  Project summary information (files sent).
//
#define CRS_MSG_SUMMARY_SEND             0x40003C8EL

//
// MessageId: CRS_MSG_SUMMARY_PULL
//
// MessageText:
//
//  Project summary information (files pulled).
//
#define CRS_MSG_SUMMARY_PULL             0x40003C8FL

//
// MessageId: CRS_MSG_SUMMARY_AUTOMATIC_RECEIVE
//
// MessageText:
//
//  Automatic project (receive) checkpoint summary information.
//
#define CRS_MSG_SUMMARY_AUTOMATIC_RECEIVE 0x40003C90L

//
// MessageId: CRS_MSG_SUMMARY_AUTOMATIC_SEND
//
// MessageText:
//
//  Automatic project (send) checkpoint summary information.
//
#define CRS_MSG_SUMMARY_AUTOMATIC_SEND   0x40003C91L

//
// MessageId: CRS_MSG_SUMMARY_RECEIVE_MATCHED
//
// MessageText:
//
//  Project summary information (files matched during receive).
//
#define CRS_MSG_SUMMARY_RECEIVE_MATCHED  0x40003C92L

//
// MessageId: CRS_MSG_SUMMARY_SEND_MATCHED
//
// MessageText:
//
//  Project summary information (files matched during send).
//
#define CRS_MSG_SUMMARY_SEND_MATCHED     0x40003C93L

//
// MessageId: CRS_MSG_SUMMARY_PULL_MATCHED
//
// MessageText:
//
//  Project summary information (files matched during pull).
//
#define CRS_MSG_SUMMARY_PULL_MATCHED     0x40003C94L

//
// MessageId: CRS_MSG_SUMMARY_RECEIVE_ERRORED
//
// MessageText:
//
//  Project summary information (files errored during receive).
//
#define CRS_MSG_SUMMARY_RECEIVE_ERRORED  0x40003C95L

//
// MessageId: CRS_MSG_SUMMARY_SEND_ERRORED
//
// MessageText:
//
//  Project summary information (files errored during send).
//
#define CRS_MSG_SUMMARY_SEND_ERRORED     0x40003C96L

//
// MessageId: CRS_MSG_SUMMARY_PULL_ERRORED
//
// MessageText:
//
//  Project summary information (files errored during pull).
//
#define CRS_MSG_SUMMARY_PULL_ERRORED     0x40003C97L

//
// MessageId: CRS_MSG_SUMMARY_LAST
//
// MessageText:
//
//  Site Server Content Deployment last summary event message.
//
#define CRS_MSG_SUMMARY_LAST             0x40003CBEL

//
// MessageId: CRS_CAB_ERROR_FIRST
//
// MessageText:
//
//  Remote installer first error message.
//
#define CRS_CAB_ERROR_FIRST              0xC0003CF0L

//
// MessageId: CRS_CAB_ERROR_CABINET_NOT_FOUND
//
// MessageText:
//
//  Cannot find the specified cabinet file.
//
#define CRS_CAB_ERROR_CABINET_NOT_FOUND  0xC0003CF1L

//
// MessageId: CRS_CAB_ERROR_NOT_A_CABINET
//
// MessageText:
//
//  Specified file is not a valid cabinet file.
//
#define CRS_CAB_ERROR_NOT_A_CABINET      0xC0003CF2L

//
// MessageId: CRS_CAB_ERROR_UNKNOWN_CABINET_VERSION
//
// MessageText:
//
//  Specified file is an unknown version of a cabinet file.
//
#define CRS_CAB_ERROR_UNKNOWN_CABINET_VERSION 0xC0003CF3L

//
// MessageId: CRS_CAB_ERROR_CORRUPT_CABINET
//
// MessageText:
//
//  Specified cabinet file is corrupt.
//
#define CRS_CAB_ERROR_CORRUPT_CABINET    0xC0003CF4L

//
// MessageId: CRS_CAB_ERROR_ALLOC_FAIL
//
// MessageText:
//
//  Out of memory.
//
#define CRS_CAB_ERROR_ALLOC_FAIL         0xC0003CF5L

//
// MessageId: CRS_CAB_ERROR_BAD_COMPR_TYPE
//
// MessageText:
//
//  Specified cabinet file uses an unknown compression type.
//
#define CRS_CAB_ERROR_BAD_COMPR_TYPE     0xC0003CF6L

//
// MessageId: CRS_CAB_ERROR_MDI_FAIL
//
// MessageText:
//
//  Compressed data in the specified cabinet file is corrupt.
//
#define CRS_CAB_ERROR_MDI_FAIL           0xC0003CF7L

//
// MessageId: CRS_CAB_ERROR_TARGET_FILE
//
// MessageText:
//
//  Could not save extracted files from the specified cabinet file.
//
#define CRS_CAB_ERROR_TARGET_FILE        0xC0003CF8L

//
// MessageId: CRS_CAB_ERROR_RESERVE_MISMATCH
//
// MessageText:
//
//  Specified cabinet file has a non-valid RESERVE size.
//
#define CRS_CAB_ERROR_RESERVE_MISMATCH   0xC0003CF9L

//
// MessageId: CRS_CAB_ERROR_WRONG_CABINET
//
// MessageText:
//
//  Specified cabinet file is not the right cabinet in the set.
//
#define CRS_CAB_ERROR_WRONG_CABINET      0xC0003CFAL

//
// MessageId: CRS_CAB_ERROR_USER_ABORT
//
// MessageText:
//
//  Could not extract the specified cabinet file - user aborted the operation.
//
#define CRS_CAB_ERROR_USER_ABORT         0xC0003CFBL

//
// MessageId: CRS_CAB_WARNING_CAB_NOT_SIGNED
//
// MessageText:
//
//  Specified cabinet file is not a signed cabinet file.
//
#define CRS_CAB_WARNING_CAB_NOT_SIGNED   0x80003CFCL

//
// MessageId: CRS_CAB_WARNING_SIGNATURE_NOT_VERIFIED
//
// MessageText:
//
//  Signature on the specified cabinet could not be verified.
//
#define CRS_CAB_WARNING_SIGNATURE_NOT_VERIFIED 0x80003CFDL

//
// MessageId: CRS_CAB_ERROR_INFFILE_NOT_FOUND
//
// MessageText:
//
//  INF file was not found inside the specified cabinet file.
//
#define CRS_CAB_ERROR_INFFILE_NOT_FOUND  0xC0003CFEL

//
// MessageId: CRS_INF_ERROR_ADVPACK_NOT_LOADED
//
// MessageText:
//
//  Could not load ADVPACK.DLL.
//
#define CRS_INF_ERROR_ADVPACK_NOT_LOADED 0xC0003CFFL

//
// MessageId: CRS_INF_ERROR_ADVPACK_WRONG_VERSION
//
// MessageText:
//
//  This version of ADVPACK.DLL is not supported.
//
#define CRS_INF_ERROR_ADVPACK_WRONG_VERSION 0xC0003D00L

//
// MessageId: CRS_INF_ERROR_INVALID_INF
//
// MessageText:
//
//  INF file found inside the specified cabinet file is not a valid INF file.
//
#define CRS_INF_ERROR_INVALID_INF        0xC0003D01L

//
// MessageId: CRS_INF_ERROR_NOTHING_TO_INSTALL
//
// MessageText:
//
//  Could not find any of the remote install sections inside the INF file.
//
#define CRS_INF_ERROR_NOTHING_TO_INSTALL 0xC0003D02L

//
// MessageId: CRS_INF_ERROR_NOTHING_TO_UNINSTALL
//
// MessageText:
//
//  Could not find any of the remote uninstall sections inside the INF file.
//
#define CRS_INF_ERROR_NOTHING_TO_UNINSTALL 0xC0003D03L

//
// MessageId: CRS_INF_ERROR_BUSY_FILES
//
// MessageText:
//
//  Files are busy.  Cannot install them unless allowed to do a REBOOT.
//
#define CRS_INF_ERROR_BUSY_FILES         0xC0003D04L

//
// MessageId: CRS_CAB_WARNING_MANUAL_REBOOT
//
// MessageText:
//
//  Setup completed successfully, but the server could not be rebooted automatically. Manual reboot is necessary.
//
#define CRS_CAB_WARNING_MANUAL_REBOOT    0x80003D05L

//
// MessageId: CRS_CAB_WARNING_REBOOT_NEEDED
//
// MessageText:
//
//  Setup completed successfully, but the files became busy during setup.  Manual reboot is necessary.
//
#define CRS_CAB_WARNING_REBOOT_NEEDED    0x80003D06L

//
// MessageId: CRS_INF_SUCCESS_REBOOT_IN_PROGRESS
//
// MessageText:
//
//  Setup completed successfully. The server is being rebooted.
//
#define CRS_INF_SUCCESS_REBOOT_IN_PROGRESS 0x00003D07L

//
// MessageId: CRS_INF_SUCCESS
//
// MessageText:
//
//  Setup completed successfully.
//
#define CRS_INF_SUCCESS                  0x00003D09L

//
// MessageId: CRS_INF_ERROR_BUSY_FILES_UNINSTALL
//
// MessageText:
//
//  Files are busy.  Cannot uninstall files unless allowed to reboot.
//
#define CRS_INF_ERROR_BUSY_FILES_UNINSTALL 0xC0003D0AL

//
// MessageId: CRS_CAB_WARNING_MANUAL_REBOOT_UNINSTALL
//
// MessageText:
//
//  Uninstall completed successfully, but the server could not be rebooted automatically. Manual reboot is necessary.
//
#define CRS_CAB_WARNING_MANUAL_REBOOT_UNINSTALL 0x80003D0BL

//
// MessageId: CRS_CAB_WARNING_REBOOT_NEEDED_UNINSTALL
//
// MessageText:
//
//  Uninstall completed successfully, but the files became busy during setup.  Manual reboot is necessary.
//
#define CRS_CAB_WARNING_REBOOT_NEEDED_UNINSTALL 0x80003D0CL

//
// MessageId: CRS_INF_SUCCESS_REBOOT_IN_PROGRESS_UNINSTALL
//
// MessageText:
//
//  Uninstall completed successfully. The server is being rebooted.
//
#define CRS_INF_SUCCESS_REBOOT_IN_PROGRESS_UNINSTALL 0x00003D0DL

//
// MessageId: CRS_INF_SUCCESS_UNINSTALL
//
// MessageText:
//
//  Uninstall completed successfully.
//
#define CRS_INF_SUCCESS_UNINSTALL        0x00003D0EL

//
// MessageId: CRS_CAB_ERROR_ADVPACK_FAILED
//
// MessageText:
//
//  Problem installing the CAB. Check CAB and INF files for validity. 
//
#define CRS_CAB_ERROR_ADVPACK_FAILED     0xC0003D0FL

//
// MessageId: CRS_CAB_ERROR_LOCKED_FILES
//
// MessageText:
//
//  At least one of the files inside the CAB is open for exclusive write/delete access.
//
#define CRS_CAB_ERROR_LOCKED_FILES       0xC0003D10L

//
// MessageId: CRS_CAB_ERROR_UNKNOWN
//
// MessageText:
//
//  An unknown error occurred while processing the specified cabinet file.
//
#define CRS_CAB_ERROR_UNKNOWN            0xC0003DEEL

//
// MessageId: CRS_CAB_ERROR_LAST
//
// MessageText:
//
//  Remote installer last error message.
//
#define CRS_CAB_ERROR_LAST               0x40003DEFL

//
// MessageId: CRS_ERROR_LAST
//
// MessageText:
//
//  Site Server Content Deployment service last error message.
//
#define CRS_ERROR_LAST                   0x40003E7FL

//
// MessageId: CRS_EVENT_FIRST
//
// MessageText:
//
//  Site Server Content Deployment Service first event message.
//
#define CRS_EVENT_FIRST                  0x400036B0L

//
// MessageId: CRS_EVENT_NET_SOURCE
//
// MessageText:
//
//  Source directory is on a network drive.  Cannot deploy from network drives.
//
#define CRS_EVENT_NET_SOURCE             0xC00036B4L

//
// MessageId: CRS_EVENT_ERROR_ACCESS_DENIED
//
// MessageText:
//
//  Access denied.
//
#define CRS_EVENT_ERROR_ACCESS_DENIED    0xC00036B5L

//
// MessageId: CRS_EVENT_FILE_CREATE_ERROR
//
// MessageText:
//
//  Unable to open the file.
//
#define CRS_EVENT_FILE_CREATE_ERROR      0xC00036B6L

//
// MessageId: CRS_EVENT_SENDINET_ERROR
//
// MessageText:
//
//  Project was unable to send the file.
//
#define CRS_EVENT_SENDINET_ERROR         0xC00036B7L

//
// MessageId: CRS_EVENT_SAMEBOX_NET_TARGET
//
// MessageText:
//
//  Destination directory is on a network drive.  Cannot deploy to network drives.
//
#define CRS_EVENT_SAMEBOX_NET_TARGET     0xC00036B8L

//
// MessageId: CRS_EVENT_BAD_SOURCE
//
// MessageText:
//
//  Unable to access the project source.
//
#define CRS_EVENT_BAD_SOURCE             0xC00036B9L

//
// MessageId: CRS_EVENT_SENDINET_CONNECT
//
// MessageText:
//
//  Unable to connect to the remote server.
//
#define CRS_EVENT_SENDINET_CONNECT       0xC00036BAL

//
// MessageId: CRS_EVENT_HASH_MISMATCH
//
// MessageText:
//
//  MD5 Hash did not match during a transmission.  This could indicate transmission failure or that the files transmitted at the source have been modified in transit.
//
#define CRS_EVENT_HASH_MISMATCH          0xC00036BBL

//
// MessageId: CRS_EVENT_SERVER_ABORTED
//
// MessageText:
//
//  Remote server was restarted.  The project is being restarted.
//
#define CRS_EVENT_SERVER_ABORTED         0xC00036BCL

//
// MessageId: CRS_EVENT_REPLICATION_ABORTED
//
// MessageText:
//
//  Project was aborted.
//
#define CRS_EVENT_REPLICATION_ABORTED    0xC00036BDL

//
// MessageId: CRS_EVENT_SEQUENCE_ERROR
//
// MessageText:
//
//  Internal messaging sequence error occurred.
//
#define CRS_EVENT_SEQUENCE_ERROR         0xC00036BEL

//
// MessageId: CRS_EVENT_SERVICE_STARTED
//
// MessageText:
//
//  Site Server Content Deployment service started successfully.
//
#define CRS_EVENT_SERVICE_STARTED        0x400036BFL

//
// MessageId: CRS_EVENT_SERVICE_SHUTDOWN
//
// MessageText:
//
//  Site Server Content Deployment service shut down successfully.
//
#define CRS_EVENT_SERVICE_SHUTDOWN       0x400036C0L

//
// MessageId: CRS_EVENT_METABASE_ACCESS
//
// MessageText:
//
//  Could not open given storage/stream.  Ensure the authentication account has access to this storage/stream.
//
#define CRS_EVENT_METABASE_ACCESS        0xC00036C2L

//
// MessageId: CRS_EVENT_AUTH_FAILURE
//
// MessageText:
//
//  Authorization failure against the destination server.  Ensure the service has access to this server.
//
#define CRS_EVENT_AUTH_FAILURE           0xC00036C3L

//
// MessageId: CRS_EVENT_REMOTE_AUTH_FAILURE
//
// MessageText:
//
//  Remote authorization failed to the destination server.  Ensure the service has access to this server.
//
#define CRS_EVENT_REMOTE_AUTH_FAILURE    0xC00036C4L

//
// MessageId: CRS_EVENT_ERROR_NOT_IN_ORDER
//
// MessageText:
//
//  Message was received out of order.  Closing connection.
//
#define CRS_EVENT_ERROR_NOT_IN_ORDER     0xC00036C5L

//
// MessageId: CRS_EVENT_CONNECTION_RETRY_FAILED
//
// MessageText:
//
//  Connection failed and could not be re-established.
//
#define CRS_EVENT_CONNECTION_RETRY_FAILED 0xC00036C6L

//
// MessageId: CRS_EVENT_REPLICATION_CANCELED
//
// MessageText:
//
//  Project canceled at the request of the user.
//
#define CRS_EVENT_REPLICATION_CANCELED   0xC00036C7L

//
// MessageId: CRS_EVENT_ERROR_DEST_TOOBIG
//
// MessageText:
//
//  Could not add the specified destination(s).  The maximum number of destinations has been reached.
//
#define CRS_EVENT_ERROR_DEST_TOOBIG      0xC00036C8L

//
// MessageId: CRS_EVENT_ERROR_NO_IP_MAPPING
//
// MessageText:
//
//  Cannot resolve IP address for the destination server.  No virtual directory created.
//
#define CRS_EVENT_ERROR_NO_IP_MAPPING    0xC00036C9L

//
// MessageId: CRS_EVENT_ERROR_DISK_FULL
//
// MessageText:
//
//  Not enough space on the disk.
//
#define CRS_EVENT_ERROR_DISK_FULL        0xC00036CAL

//
// MessageId: CRS_EVENT_ERROR_TOO_MANY_RUNNING
//
// MessageText:
//
//  Too many active projects. No more can be started until others complete.
//
#define CRS_EVENT_ERROR_TOO_MANY_RUNNING 0xC00036CBL

//
// MessageId: CRS_EVENT_ERROR_WOULDBLOCK
//
// MessageText:
//
//  The network operation would block.
//
#define CRS_EVENT_ERROR_WOULDBLOCK       0x000036D3L

//
// MessageId: CRS_EVENT_ERROR_NETDOWN
//
// MessageText:
//
//  The remote network is down.
//
#define CRS_EVENT_ERROR_NETDOWN          0xC00036E2L

//
// MessageId: CRS_EVENT_ERROR_NETUNREACH
//
// MessageText:
//
//  The remote network is unreachable.
//
#define CRS_EVENT_ERROR_NETUNREACH       0xC00036E3L

//
// MessageId: CRS_EVENT_ERROR_CONNABORTED
//
// MessageText:
//
//  The network connection was aborted at the network layer.
//
#define CRS_EVENT_ERROR_CONNABORTED      0xC00036E5L

//
// MessageId: CRS_EVENT_ERROR_TIMEOUT
//
// MessageText:
//
//  The network session timed out on a send or receive operation.
//
#define CRS_EVENT_ERROR_TIMEOUT          0xC00036ECL

//
// MessageId: CRS_EVENT_ERROR_NO_PROJECTS
//
// MessageText:
//
//  No projects defined.
//
#define CRS_EVENT_ERROR_NO_PROJECTS      0xC0003714L

//
// MessageId: CRS_EVENT_ERROR_PROJECT_NOT_FOUND
//
// MessageText:
//
//  Could not find the project.
//
#define CRS_EVENT_ERROR_PROJECT_NOT_FOUND 0xC0003715L

//
// MessageId: CRS_EVENT_ERROR_INVALID_PARAMETER
//
// MessageText:
//
//  Invalid parameter.
//
#define CRS_EVENT_ERROR_INVALID_PARAMETER 0xC0003716L

//
// MessageId: CRS_EVENT_ERROR_AUTHENTICATION_FAILURE
//
// MessageText:
//
//  Authentication failure. Ensure the account has proper access on local and remote servers.
//
#define CRS_EVENT_ERROR_AUTHENTICATION_FAILURE 0xC0003718L

//
// MessageId: CRS_EVENT_ERROR_DELETE_PROJECT_DOWN_ROUTE
//
// MessageText:
//
//  Could not delete the route from the project on at least one machine down the defined routes.  The project will have to be edited manually there.
//
#define CRS_EVENT_ERROR_DELETE_PROJECT_DOWN_ROUTE 0x80003719L

//
// MessageId: CRS_EVENT_ERROR_SERVICE_NOT_RUNNING
//
// MessageText:
//
//  Server is available, but the Site Server Content Deployment service is not running.
//
#define CRS_EVENT_ERROR_SERVICE_NOT_RUNNING 0xC000371AL

//
// MessageId: CRS_EVENT_ERROR_HOST_DOWN
//
// MessageText:
//
//  Server is unavailable.
//
#define CRS_EVENT_ERROR_HOST_DOWN        0xC000371BL

//
// MessageId: CRS_EVENT_ERROR_BAD_NET_NAME
//
// MessageText:
//
//  Server cannot be found.
//
#define CRS_EVENT_ERROR_BAD_NET_NAME     0xC000371CL

//
// MessageId: CRS_EVENT_ERROR_ROUTE_NOT_FOUND
//
// MessageText:
//
//  Could not find route.  Ensure the route name is correct and that it exists on the target servers.
//
#define CRS_EVENT_ERROR_ROUTE_NOT_FOUND  0xC000371DL

//
// MessageId: CRS_EVENT_ERROR_NO_ROUTES
//
// MessageText:
//
//  No routes defined.
//
#define CRS_EVENT_ERROR_NO_ROUTES        0xC000371EL

//
// MessageId: CRS_EVENT_ERROR_CREATE_PROJECT_DOWN_ROUTE
//
// MessageText:
//
//  Could not create the project on at least one server down the defined routes.  The project will have to be created manually there.
//
#define CRS_EVENT_ERROR_CREATE_PROJECT_DOWN_ROUTE 0x8000371FL

//
// MessageId: CRS_EVENT_ERROR_SERVICE_PAUSED
//
// MessageText:
//
//  Site Server Content Deployment service on the destination server is paused and is not accepting new requests.  Retry the project later.
//
#define CRS_EVENT_ERROR_SERVICE_PAUSED   0xC0003720L

//
// MessageId: CRS_EVENT_ERROR_SERVICE_NOT_PAUSED
//
// MessageText:
//
//  Could not restart service. Service is not paused.
//
#define CRS_EVENT_ERROR_SERVICE_NOT_PAUSED 0xC0003721L

//
// MessageId: CRS_EVENT_ERROR_NAME_NOT_RESOLVED
//
// MessageText:
//
//  Could not resolve the URL.  Ensure URL and/or proxy settings are correct.
//
#define CRS_EVENT_ERROR_NAME_NOT_RESOLVED 0xC0003722L

//
// MessageId: CRS_EVENT_ERROR_CONNECTION_RESET
//
// MessageText:
//
//  Connection to the destination server has been reset.  This indicates problems with the remote service or network.
//
#define CRS_EVENT_ERROR_CONNECTION_RESET 0xC0003723L

//
// MessageId: CRS_EVENT_ERROR_REPLICATION_NOT_FOUND
//
// MessageText:
//
//  No entries.
//
#define CRS_EVENT_ERROR_REPLICATION_NOT_FOUND 0xC0003724L

//
// MessageId: CRS_EVENT_ERROR_SERVICE_ALREADY_RUNNING
//
// MessageText:
//
//  Could not start service.  The service has already been started.
//
#define CRS_EVENT_ERROR_SERVICE_ALREADY_RUNNING 0xC0003725L

//
// MessageId: CRS_EVENT_ERROR_PROJECT_ALREADY_RUNNING
//
// MessageText:
//
//  Failed to start project.  It is already running.
//
#define CRS_EVENT_ERROR_PROJECT_ALREADY_RUNNING 0xC0003726L

//
// MessageId: CRS_EVENT_FILE_MOVE_ERROR
//
// MessageText:
//
//  Failed to move file.
//
#define CRS_EVENT_FILE_MOVE_ERROR        0xC0003727L

//
// MessageId: CRS_EVENT_PRODUCT_EXPIRED
//
// MessageText:
//
//  Thank you for your interest in Site Server Content Deployment.  This software has expired.  Please acquire the final retail version of this software.
//
#define CRS_EVENT_PRODUCT_EXPIRED        0xC0003728L

//
// MessageId: CRS_EVENT_ERROR_PROJECT_INCOMPLETE
//
// MessageText:
//
//  Project information is incomplete.
//
#define CRS_EVENT_ERROR_PROJECT_INCOMPLETE 0xC0003729L

//
// MessageId: CRS_EVENT_ERROR_PARAMETER_NOT_FOUND
//
// MessageText:
//
//  Parameter not found.
//
#define CRS_EVENT_ERROR_PARAMETER_NOT_FOUND 0xC000372AL

//
// MessageId: CRS_EVENT_ERROR_REPLICATION_RUNNING
//
// MessageText:
//
//  Project is running.  The operation could not be completed.
//
#define CRS_EVENT_ERROR_REPLICATION_RUNNING 0xC000372BL

//
// MessageId: CRS_EVENT_ERROR_INVALID_PROJECT_NAME
//
// MessageText:
//
//  Project name is invalid.  Project names cannot contain backslashes and cannot be empty.
//
#define CRS_EVENT_ERROR_INVALID_PROJECT_NAME 0xC000372CL

//
// MessageId: CRS_EVENT_ERROR_INVALID_ROUTE_NAME
//
// MessageText:
//
//  Route name is invalid.  Route names cannot contain backslashes or DBCS characters and cannot be empty.
//
#define CRS_EVENT_ERROR_INVALID_ROUTE_NAME 0xC000372DL

//
// MessageId: CRS_EVENT_ERROR_INVALID_DWORD
//
// MessageText:
//
//  Parameter must be a positive integer.
//
#define CRS_EVENT_ERROR_INVALID_DWORD    0xC000372EL

//
// MessageId: CRS_EVENT_ERROR_NO_MORE_ITEMS
//
// MessageText:
//
//  No more items are in the list.
//
#define CRS_EVENT_ERROR_NO_MORE_ITEMS    0x8000372FL

//
// MessageId: CRS_EVENT_ERROR_TOO_MANY_ARGS
//
// MessageText:
//
//  Too many arguments specified.
//
#define CRS_EVENT_ERROR_TOO_MANY_ARGS    0xC0003730L

//
// MessageId: CRS_EVENT_MBS_INIT_ERROR
//
// MessageText:
//
//  Internal error trying to initialize for ACL replication.
//
#define CRS_EVENT_MBS_INIT_ERROR         0xC0003731L

//
// MessageId: CRS_EVENT_ERROR_OPENING_FILE
//
// MessageText:
//
//  Error opening file or file not found.
//
#define CRS_EVENT_ERROR_OPENING_FILE     0xC0003733L

//
// MessageId: CRS_EVENT_ERROR_FILE_FORMAT
//
// MessageText:
//
//  Invalid file format.
//
#define CRS_EVENT_ERROR_FILE_FORMAT      0xC0003734L

//
// MessageId: CRS_EVENT_ERROR_FILE_WRITE
//
// MessageText:
//
//  Error writing to file.
//
#define CRS_EVENT_ERROR_FILE_WRITE       0xC0003735L

//
// MessageId: CRS_EVENT_WINSOCK_WSAHOSTNAME
//
// MessageText:
//
//  Gethostname failed.
//
#define CRS_EVENT_WINSOCK_WSAHOSTNAME    0xC0003736L

//
// MessageId: CRS_EVENT_ERROR_INVALID_CONTEXT
//
// MessageText:
//
//  Current context is invalid.  Project being reset.
//
#define CRS_EVENT_ERROR_INVALID_CONTEXT  0xC000373DL

//
// MessageId: CRS_EVENT_ERROR_DOWNLEVEL_SERVER
//
// MessageText:
//
//  Operation is not supported on a down-level server.
//
#define CRS_EVENT_ERROR_DOWNLEVEL_SERVER 0xC000373EL

//
// MessageId: CRS_EVENT_ERROR_NO_MAPPING
//
// MessageText:
//
//  URL does not map to any project definitions.
//
#define CRS_EVENT_ERROR_NO_MAPPING       0xC000373FL

//
// MessageId: CRS_EVENT_ERROR_START_PENDING
//
// MessageText:
//
//  Start pending completion of the active project.
//
#define CRS_EVENT_ERROR_START_PENDING    0xC0003740L

//
// MessageId: CRS_EVENT_ERROR_NO_EVENTSINKS
//
// MessageText:
//
//  No event sinks defined.
//
#define CRS_EVENT_ERROR_NO_EVENTSINKS    0xC0003741L

//
// MessageId: CRS_EVENT_ERROR_EVENTSINK_NOT_FOUND
//
// MessageText:
//
//  Could not find event sink.
//
#define CRS_EVENT_ERROR_EVENTSINK_NOT_FOUND 0xC0003742L

//
// MessageId: CRS_EVENT_ERROR_WSAHOSTNOTFOUND
//
// MessageText:
//
//  Host not found.
//
#define CRS_EVENT_ERROR_WSAHOSTNOTFOUND  0xC0003743L

//
// MessageId: CRS_EVENT_ERROR_WSATRYAGAIN
//
// MessageText:
//
//  Host not found, or SERVERFAIL.  Try again.
//
#define CRS_EVENT_ERROR_WSATRYAGAIN      0xC0003744L

//
// MessageId: CRS_EVENT_ERROR_WSANORECOVERY
//
// MessageText:
//
//  Non-recoverable winsock error.
//
#define CRS_EVENT_ERROR_WSANORECOVERY    0xC0003745L

//
// MessageId: CRS_EVENT_ERROR_WSANODATA
//
// MessageText:
//
//  Invalid address.
//
#define CRS_EVENT_ERROR_WSANODATA        0xC0003746L

//
// MessageId: CRS_EVENT_EVENTSINK_INITIALIZED
//
// MessageText:
//
//  Event sink specified by the data field was successfully initialized.
//
#define CRS_EVENT_EVENTSINK_INITIALIZED  0x00003747L

//
// MessageId: CRS_EVENT_ERROR_EVENTSINK_NOT_INITIALIZED
//
// MessageText:
//
//  Event sink specified by the data field failed to initialize.
//
#define CRS_EVENT_ERROR_EVENTSINK_NOT_INITIALIZED 0xC0003748L

//
// MessageId: CRS_EVENT_ERROR_NOTHING_TO_ROLLBACK
//
// MessageText:
//
//  Could not roll back project.  No rollback information found.
//
#define CRS_EVENT_ERROR_NOTHING_TO_ROLLBACK 0xC0003749L

//
// MessageId: CRS_EVENT_ERROR_NO_DESTINATIONS
//
// MessageText:
//
//  Project could not be started because no destinations are defined.
//
#define CRS_EVENT_ERROR_NO_DESTINATIONS  0xC000374AL

//
// MessageId: CRS_EVENT_ERROR_REAUTHENTICATE
//
// MessageText:
//
//  Re-authenticate using an acceptable protocol.
//
#define CRS_EVENT_ERROR_REAUTHENTICATE   0xC000374BL

//
// MessageId: CRS_EVENT_ERROR_UNSUPPORTED
//
// MessageText:
//
//  Function not supported in this version to this server.
//
#define CRS_EVENT_ERROR_UNSUPPORTED      0xC000374CL

//
// MessageId: CRS_EVENT_ERROR_USER_NOT_FOUND
//
// MessageText:
//
//  Specified user could not be found.
//
#define CRS_EVENT_ERROR_USER_NOT_FOUND   0xC000374DL

//
// MessageId: CRS_EVENT_ERROR_NO_TRANSACTIONS
//
// MessageText:
//
//  No transactions for the specified project.
//
#define CRS_EVENT_ERROR_NO_TRANSACTIONS  0xC000374EL

//
// MessageId: CRS_EVENT_ERROR_NO_SERVERS
//
// MessageText:
//
//  No destination servers defined.
//
#define CRS_EVENT_ERROR_NO_SERVERS       0xC000374FL

//
// MessageId: CRS_EVENT_ERROR_NO_SCHEDULES
//
// MessageText:
//
//  No schedules are defined for this project.
//
#define CRS_EVENT_ERROR_NO_SCHEDULES     0xC0003750L

//
// MessageId: CRS_EVENT_ERROR_NO_EMAIL
//
// MessageText:
//
//  Could not contact SMTP server to send e-mail report.
//
#define CRS_EVENT_ERROR_NO_EMAIL         0xC0003751L

//
// MessageId: CRS_EVENT_ERROR_WAIT_ABORT
//
// MessageText:
//
//  Waiting to abort after losing connection with source server.
//
#define CRS_EVENT_ERROR_WAIT_ABORT       0xC0003752L

//
// MessageId: CRS_EVENT_RESUME_REPLICATION
//
// MessageText:
//
//  Resuming project.
//
#define CRS_EVENT_RESUME_REPLICATION     0x40003753L

//
// MessageId: CRS_EVENT_METABASE_NOT_FOUND
//
// MessageText:
//
//  Metabase is not responding.
//
#define CRS_EVENT_METABASE_NOT_FOUND     0xC0003754L

//
// MessageId: CRS_EVENT_SSE_VROOT_NOT_FOUND
//
// MessageText:
//
//  Site Server virtual directory not found.
//
#define CRS_EVENT_SSE_VROOT_NOT_FOUND    0xC0003755L

//
// MessageId: CRS_EVENT_WARNING_CONNECTION_FAILED
//
// MessageText:
//
//  Connection failed.  Will attempt to re-establish session.
//
#define CRS_EVENT_WARNING_CONNECTION_FAILED 0x80003756L

//
// MessageId: CRS_EVENT_ERROR_DROPPED_EMAIL_REPORT
//
// MessageText:
//
//  Dropped e-mail report.
//
#define CRS_EVENT_ERROR_DROPPED_EMAIL_REPORT 0xC0003757L

//
// MessageId: CRS_EVENT_ERROR_EMAIL_HOST_NOT_DEFINED
//
// MessageText:
//
//  No SMTP server defined.
//
#define CRS_EVENT_ERROR_EMAIL_HOST_NOT_DEFINED 0xC0003758L

//
// MessageId: CRS_EVENT_VROOT_CREATION
//
// MessageText:
//
//  Could not create the specified virtual directory.
//
#define CRS_EVENT_VROOT_CREATION         0x80003759L

//
// MessageId: CRS_EVENT_UPLPAGE_CREATION
//
// MessageText:
//
//  Could not create the upload page.
//
#define CRS_EVENT_UPLPAGE_CREATION       0xC000375AL

//
// MessageId: CRS_EVENT_ERROR_CIRCULAR_ROUTE
//
// MessageText:
//
//  System has detected a circular route.  Project not created.
//
#define CRS_EVENT_ERROR_CIRCULAR_ROUTE   0xC000375BL

//
// MessageId: CRS_EVENT_ERROR_TRANSACTION_APPLY_FAILED
//
// MessageText:
//
//  Transaction application failed.  Check permissions on the destination directory and files and ensure that no files are locked open.
//
#define CRS_EVENT_ERROR_TRANSACTION_APPLY_FAILED 0xC000375CL

//
// MessageId: CRS_EVENT_ERROR_DATABASE
//
// MessageText:
//
//  Database error occurred.
//
#define CRS_EVENT_ERROR_DATABASE         0xC000375DL

//
// MessageId: CRS_EVENT_ERROR_SINK_CONNECT_FAILED
//
// MessageText:
//
//  Unable to connect to remote server to forward events.
//
#define CRS_EVENT_ERROR_SINK_CONNECT_FAILED 0xC000375EL

//
// MessageId: CRS_EVENT_ERROR_CANT_SEND_EVENTS_TO_DOWNLEVEL_SERVER
//
// MessageText:
//
//  Unable to send events to the destination server.  This operation is not supported.
//
#define CRS_EVENT_ERROR_CANT_SEND_EVENTS_TO_DOWNLEVEL_SERVER 0xC000375FL

//
// MessageId: CRS_EVENT_ERROR_FILE_CORRUPT
//
// MessageText:
//
//  File is corrupt or unreadable.
//
#define CRS_EVENT_ERROR_FILE_CORRUPT     0xC0003760L

//
// MessageId: CRS_EVENT_ERROR_FILE_BUSY
//
// MessageText:
//
//  File is busy.
//
#define CRS_EVENT_ERROR_FILE_BUSY        0xC0003761L

//
// MessageId: CRS_EVENT_ACL_ERROR_INVALID_ACE
//
// MessageText:
//
//  Could not set the ACE on this file.
//
#define CRS_EVENT_ACL_ERROR_INVALID_ACE  0x80003762L

//
// MessageId: CRS_EVENT_ACL_ERROR_ACL_FAILED
//
// MessageText:
//
//  Could not set the ACLs appropriately on this file, setting default ACLs on this file.
//
#define CRS_EVENT_ACL_ERROR_ACL_FAILED   0xC0003763L

//
// MessageId: CRS_EVENT_ERROR_PROJECT_NOT_FOUND_ON_DESTINATION
//
// MessageText:
//
//  Could not find project on the destination server.
//
#define CRS_EVENT_ERROR_PROJECT_NOT_FOUND_ON_DESTINATION 0xC0003764L

//
// MessageId: CRS_EVENT_ERROR_PROJECT_INCOMPLETE_ON_DESTINATION
//
// MessageText:
//
//  Project information is incomplete on the destination server.
//
#define CRS_EVENT_ERROR_PROJECT_INCOMPLETE_ON_DESTINATION 0xC0003765L

//
// MessageId: CRS_EVENT_ERROR_SERVICE_DOWN_ON_REMOTE_SERVER
//
// MessageText:
//
//  Remote server is available, but the Site Server Content Deployment service is not running.
//
#define CRS_EVENT_ERROR_SERVICE_DOWN_ON_REMOTE_SERVER 0xC0003766L

//
// MessageId: CRS_EVENT_ERROR_DISK_FULL_ON_DESTINATION
//
// MessageText:
//
//  Disk is full on the destination server.
//
#define CRS_EVENT_ERROR_DISK_FULL_ON_DESTINATION 0xC0003767L

//
// MessageId: CRS_EVENT_FILE_CREATE_ERROR_ON_DESTINATION
//
// MessageText:
//
//  Unable to open a file on the destination server.  Check permissions on the project directory and files.
//
#define CRS_EVENT_FILE_CREATE_ERROR_ON_DESTINATION 0xC0003768L

//
// MessageId: CRS_EVENT_ERROR_REMOTE_SERVER_NOT_FOUND
//
// MessageText:
//
//  Remote server could not be found.
//
#define CRS_EVENT_ERROR_REMOTE_SERVER_NOT_FOUND 0xC0003769L

//
// MessageId: CRS_EVENT_ERROR_REMOTE_APPLY_FAILED
//
// MessageText:
//
//  Unable to apply transactions on the destination server.
//
#define CRS_EVENT_ERROR_REMOTE_APPLY_FAILED 0xC000376AL

//
// MessageId: CRS_EVENT_ERROR_INVALID_FLAG_COMBINATION
//
// MessageText:
//
//  Invalid flag combination.
//
#define CRS_EVENT_ERROR_INVALID_FLAG_COMBINATION 0xC000376BL

//
// MessageId: CRS_EVENT_ERROR_RUNNING_SCRIPT
//
// MessageText:
//
//  There was an error running the script.  Ensure that the script exists and can be located in the path.
//
#define CRS_EVENT_ERROR_RUNNING_SCRIPT   0xC000376CL

//
// MessageId: CRS_EVENT_ERROR_CANT_FORWARD_TO_LOCAL_SERVER
//
// MessageText:
//
//  Cannot forward events to the local server.
//
#define CRS_EVENT_ERROR_CANT_FORWARD_TO_LOCAL_SERVER 0xC000376DL

//
// MessageId: CRS_EVENT_ERROR_SCHEDULE_NOT_FOUND
//
// MessageText:
//
//  Specified project schedule could not be found.
//
#define CRS_EVENT_ERROR_SCHEDULE_NOT_FOUND 0xC000376EL

//
// MessageId: CRS_EVENT_ERROR_METABASE
//
// MessageText:
//
//  An error occurred accessing the metabase.  Ensure that the web site exists and that the account has access to it.
//
#define CRS_EVENT_ERROR_METABASE         0xC000376FL

//
// MessageId: CRS_EVENT_ERROR_SCHEDULER_FAILURE
//
// MessageText:
//
//  An error occurred with the task scheduler.
//
#define CRS_EVENT_ERROR_SCHEDULER_FAILURE 0xC0003770L

//
// MessageId: CRS_EVENT_SAMEBOX_META_TARGET
//
// MessageText:
//
//  A destination for metabase replication is invalid.
//
#define CRS_EVENT_SAMEBOX_META_TARGET    0xC0003771L

//
// MessageId: CRS_EVENT_ERROR_DROPPED_EVENTS
//
// MessageText:
//
//  The event sink specified by the data field has started dropping events.
//
#define CRS_EVENT_ERROR_DROPPED_EVENTS   0xC0003772L

//
// MessageId: CRS_EVENT_REPLICATION_FIRST
//
// MessageText:
//
//  Site Server Content Deployment first replication event message.
//
#define CRS_EVENT_REPLICATION_FIRST      0x400037DCL

//
// MessageId: CRS_EVENT_STARTING_REPLICATION
//
// MessageText:
//
//  Project starting to the destination server.
//
#define CRS_EVENT_STARTING_REPLICATION   0x400037DDL

//
// MessageId: CRS_EVENT_REPLICATION_COMPLETE
//
// MessageText:
//
//  Project complete to the destination server.
//
#define CRS_EVENT_REPLICATION_COMPLETE   0x400037DEL

//
// MessageId: CRS_EVENT_STARTING_PULL_REPLICATION
//
// MessageText:
//
//  Retrieval project starting.
//
#define CRS_EVENT_STARTING_PULL_REPLICATION 0x400037DFL

//
// MessageId: CRS_EVENT_PULL_REPLICATION_COMPLETE
//
// MessageText:
//
//  Retrieval project complete.
//
#define CRS_EVENT_PULL_REPLICATION_COMPLETE 0x400037E0L

//
// MessageId: CRS_EVENT_STARTING_RECEIVE_REPLICATION
//
// MessageText:
//
//  Starting to receive project.
//
#define CRS_EVENT_STARTING_RECEIVE_REPLICATION 0x400037E1L

//
// MessageId: CRS_EVENT_REPLICATION_RECEIVE_COMPLETE
//
// MessageText:
//
//  Finished receiving project.
//
#define CRS_EVENT_REPLICATION_RECEIVE_COMPLETE 0x400037E2L

//
// MessageId: CRS_EVENT_STARTING_REPLICATION_SCAN
//
// MessageText:
//
//  Project starting.
//
#define CRS_EVENT_STARTING_REPLICATION_SCAN 0x400037E3L

//
// MessageId: CRS_EVENT_REPLICATION_LAST
//
// MessageText:
//
//  Site Server Content Deployment last replication event message.
//
#define CRS_EVENT_REPLICATION_LAST       0x4000380DL

//
// MessageId: CRS_EVENT_FILE_FIRST
//
// MessageText:
//
//  Site Server Content Deployment first file event message.
//
#define CRS_EVENT_FILE_FIRST             0x4000380EL

//
// MessageId: CRS_EVENT_SENDING_FILE
//
// MessageText:
//
//  Sending file.
//
#define CRS_EVENT_SENDING_FILE           0x4000380FL

//
// MessageId: CRS_EVENT_SENT_FILE
//
// MessageText:
//
//  File sent.
//
#define CRS_EVENT_SENT_FILE              0x40003810L

//
// MessageId: CRS_EVENT_RECEIVING_FILE
//
// MessageText:
//
//  Receiving file.
//
#define CRS_EVENT_RECEIVING_FILE         0x40003811L

//
// MessageId: CRS_EVENT_RECEIVED_FILE
//
// MessageText:
//
//  File received.
//
#define CRS_EVENT_RECEIVED_FILE          0x40003812L

//
// MessageId: CRS_EVENT_SENT_DELETE_FILE
//
// MessageText:
//
//  Sent delete file request.
//
#define CRS_EVENT_SENT_DELETE_FILE       0x40003813L

//
// MessageId: CRS_EVENT_RECEIVED_DELETE_FILE
//
// MessageText:
//
//  File delete request received.
//
#define CRS_EVENT_RECEIVED_DELETE_FILE   0x40003814L

//
// MessageId: CRS_EVENT_RETRYING_SENDING_FILE
//
// MessageText:
//
//  Retrying to send file.
//
#define CRS_EVENT_RETRYING_SENDING_FILE  0x40003818L

//
// MessageId: CRS_EVENT_FILE_BUSY
//
// MessageText:
//
//  File is busy.  The service is waiting for the file to become available.
//
#define CRS_EVENT_FILE_BUSY              0x40003819L

//
// MessageId: CRS_EVENT_SENDING_ACL
//
// MessageText:
//
//  Sending the ACL for this file.
//
#define CRS_EVENT_SENDING_ACL            0x4000381AL

//
// MessageId: CRS_EVENT_SENT_ACL
//
// MessageText:
//
//  Sent the ACL for this file.
//
#define CRS_EVENT_SENT_ACL               0x4000381BL

//
// MessageId: CRS_EVENT_RECEIVING_ACL
//
// MessageText:
//
//  Receiving the ACL for this file.
//
#define CRS_EVENT_RECEIVING_ACL          0x4000381CL

//
// MessageId: CRS_EVENT_RECEIVED_ACL
//
// MessageText:
//
//  Received the ACL for this file.
//
#define CRS_EVENT_RECEIVED_ACL           0x4000381DL

//
// MessageId: CRS_EVENT_PULLED_FILE_HTTP
//
// MessageText:
//
//  File retrieved over HTTP.
//
#define CRS_EVENT_PULLED_FILE_HTTP       0x4000381EL

//
// MessageId: CRS_EVENT_PULLED_FILE_FTP
//
// MessageText:
//
//  File retrieved over FTP.
//
#define CRS_EVENT_PULLED_FILE_FTP        0x4000381FL

//
// MessageId: CRS_EVENT_FILE_COMMITTED
//
// MessageText:
//
//  File committed to disk.
//
#define CRS_EVENT_FILE_COMMITTED         0x40003820L

//
// MessageId: CRS_EVENT_FILE_DELETED
//
// MessageText:
//
//  File deleted.
//
#define CRS_EVENT_FILE_DELETED           0x40003821L

//
// MessageId: CRS_EVENT_SKIPPING_LOCKED_FILE
//
// MessageText:
//
//  File skipped because it could not be opened.
//
#define CRS_EVENT_SKIPPING_LOCKED_FILE   0x80003822L

//
// MessageId: CRS_EVENT_FILE_MATCHED
//
// MessageText:
//
//  File matched.
//
#define CRS_EVENT_FILE_MATCHED           0x40003823L

//
// MessageId: CRS_EVENT_FILE_LAST
//
// MessageText:
//
//  Site Server Content Deployment last file event message.
//
#define CRS_EVENT_FILE_LAST              0x4000383FL

//
// MessageId: CRS_EVENT_ADMIN_FIRST
//
// MessageText:
//
//  Site Server Content Deployment first administration event message.
//
#define CRS_EVENT_ADMIN_FIRST            0x40003840L

//
// MessageId: CRS_EVENT_PROJECT_CREATED
//
// MessageText:
//
//  Project created.
//
#define CRS_EVENT_PROJECT_CREATED        0x40003841L

//
// MessageId: CRS_EVENT_PROJECT_DELETED
//
// MessageText:
//
//  Project deleted.
//
#define CRS_EVENT_PROJECT_DELETED        0x40003842L

//
// MessageId: CRS_EVENT_PROJECT_EDITED
//
// MessageText:
//
//  Project edited.
//
#define CRS_EVENT_PROJECT_EDITED         0x40003843L

//
// MessageId: CRS_EVENT_ROUTE_CREATED
//
// MessageText:
//
//  Route created.
//
#define CRS_EVENT_ROUTE_CREATED          0x40003844L

//
// MessageId: CRS_EVENT_ROUTE_DELETED
//
// MessageText:
//
//  Route deleted.
//
#define CRS_EVENT_ROUTE_DELETED          0x40003845L

//
// MessageId: CRS_EVENT_ROUTE_EDITED
//
// MessageText:
//
//  Route edited.
//
#define CRS_EVENT_ROUTE_EDITED           0x40003846L

//
// MessageId: CRS_EVENT_SERVER_EDITED
//
// MessageText:
//
//  Server settings modified.
//
#define CRS_EVENT_SERVER_EDITED          0x40003847L

//
// MessageId: CRS_EVENT_EVENTSINK_CREATED
//
// MessageText:
//
//  Event sink specified by the data field was added.
//
#define CRS_EVENT_EVENTSINK_CREATED      0x40003848L

//
// MessageId: CRS_EVENT_EVENTSINK_DELETED
//
// MessageText:
//
//  Event sink specified by the data field was deleted.
//
#define CRS_EVENT_EVENTSINK_DELETED      0x40003849L

//
// MessageId: CRS_EVENT_CLEARED_EVENTS
//
// MessageText:
//
//  Event store specified by the data field was cleared.
//
#define CRS_EVENT_CLEARED_EVENTS         0x4000384AL

//
// MessageId: CRS_EVENT_CLEARED_ALL_EVENTS
//
// MessageText:
//
//  Each database event store on this server was cleared.
//
#define CRS_EVENT_CLEARED_ALL_EVENTS     0x4000384BL

//
// MessageId: CRS_EVENT_ADMIN_LAST
//
// MessageText:
//
//  Site Server Content Deployment last administration event message.
//
#define CRS_EVENT_ADMIN_LAST             0x40003872L

//
// MessageId: CRS_EVENT_SUMMARY_FIRST
//
// MessageText:
//
//  Site Server Content Deployment first summary event message.
//
#define CRS_EVENT_SUMMARY_FIRST          0x400038A4L

//
// MessageId: CRS_EVENT_SUMMARY_RECEIVE
//
// MessageText:
//
//  Project summary information (files received).
//
#define CRS_EVENT_SUMMARY_RECEIVE        0x400038A5L

//
// MessageId: CRS_EVENT_SUMMARY_SEND
//
// MessageText:
//
//  Project summary information (files sent).
//
#define CRS_EVENT_SUMMARY_SEND           0x400038A6L

//
// MessageId: CRS_EVENT_SUMMARY_PULL
//
// MessageText:
//
//  Project summary information (files pulled).
//
#define CRS_EVENT_SUMMARY_PULL           0x400038A7L

//
// MessageId: CRS_EVENT_SUMMARY_AUTOMATIC_RECEIVE
//
// MessageText:
//
//  Automatic project (receive) checkpoint summary information.
//
#define CRS_EVENT_SUMMARY_AUTOMATIC_RECEIVE 0x400038A8L

//
// MessageId: CRS_EVENT_SUMMARY_AUTOMATIC_SEND
//
// MessageText:
//
//  Automatic project (send) checkpoint summary information.
//
#define CRS_EVENT_SUMMARY_AUTOMATIC_SEND 0x400038A9L

//
// MessageId: CRS_EVENT_SUMMARY_RECEIVE_MATCHED
//
// MessageText:
//
//  Project summary information (files matched during receive).
//
#define CRS_EVENT_SUMMARY_RECEIVE_MATCHED 0x400038AAL

//
// MessageId: CRS_EVENT_SUMMARY_SEND_MATCHED
//
// MessageText:
//
//  Project summary information (files matched during send).
//
#define CRS_EVENT_SUMMARY_SEND_MATCHED   0x400038ABL

//
// MessageId: CRS_EVENT_SUMMARY_PULL_MATCHED
//
// MessageText:
//
//  Project summary information (files matched during pull).
//
#define CRS_EVENT_SUMMARY_PULL_MATCHED   0x400038ACL

//
// MessageId: CRS_EVENT_SUMMARY_RECEIVE_ERRORED
//
// MessageText:
//
//  Project summary information (files errored during receive).
//
#define CRS_EVENT_SUMMARY_RECEIVE_ERRORED 0x400038ADL

//
// MessageId: CRS_EVENT_SUMMARY_SEND_ERRORED
//
// MessageText:
//
//  Project summary information (files errored during send).
//
#define CRS_EVENT_SUMMARY_SEND_ERRORED   0x400038AEL

//
// MessageId: CRS_EVENT_SUMMARY_PULL_ERRORED
//
// MessageText:
//
//  Project summary information (files errored during pull).
//
#define CRS_EVENT_SUMMARY_PULL_ERRORED   0x400038AFL

//
// MessageId: CRS_EVENT_SUMMARY_LAST
//
// MessageText:
//
//  Site Server Content Deployment last summary event message.
//
#define CRS_EVENT_SUMMARY_LAST           0x400038D6L

//
// MessageId: CRS_EVENT_CAB_ERROR_FIRST
//
// MessageText:
//
//  Remote installer first error message.
//
#define CRS_EVENT_CAB_ERROR_FIRST        0xC0003908L

//
// MessageId: CRS_EVENT_CAB_ERROR_CABINET_NOT_FOUND
//
// MessageText:
//
//  Cannot find the specified cabinet file.
//
#define CRS_EVENT_CAB_ERROR_CABINET_NOT_FOUND 0xC0003909L

//
// MessageId: CRS_EVENT_CAB_ERROR_NOT_A_CABINET
//
// MessageText:
//
//  Specified file is not a valid cabinet file.
//
#define CRS_EVENT_CAB_ERROR_NOT_A_CABINET 0xC000390AL

//
// MessageId: CRS_EVENT_CAB_ERROR_UNKNOWN_CABINET_VERSION
//
// MessageText:
//
//  Specified file is an unknown version of a cabinet file.
//
#define CRS_EVENT_CAB_ERROR_UNKNOWN_CABINET_VERSION 0xC000390BL

//
// MessageId: CRS_EVENT_CAB_ERROR_CORRUPT_CABINET
//
// MessageText:
//
//  Specified cabinet file is corrupt.
//
#define CRS_EVENT_CAB_ERROR_CORRUPT_CABINET 0xC000390CL

//
// MessageId: CRS_EVENT_CAB_ERROR_ALLOC_FAIL
//
// MessageText:
//
//  Out of memory.
//
#define CRS_EVENT_CAB_ERROR_ALLOC_FAIL   0xC000390DL

//
// MessageId: CRS_EVENT_CAB_ERROR_BAD_COMPR_TYPE
//
// MessageText:
//
//  Specified cabinet file uses an unknown compression type.
//
#define CRS_EVENT_CAB_ERROR_BAD_COMPR_TYPE 0xC000390EL

//
// MessageId: CRS_EVENT_CAB_ERROR_MDI_FAIL
//
// MessageText:
//
//  Compressed data in the specified cabinet file is corrupt.
//
#define CRS_EVENT_CAB_ERROR_MDI_FAIL     0xC000390FL

//
// MessageId: CRS_EVENT_CAB_ERROR_TARGET_FILE
//
// MessageText:
//
//  Could not save extracted files from the specified cabinet file.
//
#define CRS_EVENT_CAB_ERROR_TARGET_FILE  0xC0003910L

//
// MessageId: CRS_EVENT_CAB_ERROR_RESERVE_MISMATCH
//
// MessageText:
//
//  Specified cabinet file has a non-valid RESERVE size.
//
#define CRS_EVENT_CAB_ERROR_RESERVE_MISMATCH 0xC0003911L

//
// MessageId: CRS_EVENT_CAB_ERROR_WRONG_CABINET
//
// MessageText:
//
//  Specified cabinet file is not the right cabinet in the set.
//
#define CRS_EVENT_CAB_ERROR_WRONG_CABINET 0xC0003912L

//
// MessageId: CRS_EVENT_CAB_ERROR_USER_ABORT
//
// MessageText:
//
//  Could not extract the specified cabinet file. User aborted the operation.
//
#define CRS_EVENT_CAB_ERROR_USER_ABORT   0xC0003913L

//
// MessageId: CRS_EVENT_CAB_WARNING_CAB_NOT_SIGNED
//
// MessageText:
//
//  Specified cabinet file is not a signed cabinet file.
//
#define CRS_EVENT_CAB_WARNING_CAB_NOT_SIGNED 0x80003914L

//
// MessageId: CRS_EVENT_CAB_WARNING_SIGNATURE_NOT_VERIFIED
//
// MessageText:
//
//  Signature on the specified cabinet could not be verified.
//
#define CRS_EVENT_CAB_WARNING_SIGNATURE_NOT_VERIFIED 0x80003915L

//
// MessageId: CRS_EVENT_CAB_ERROR_INFFILE_NOT_FOUND
//
// MessageText:
//
//  INF file was not found inside the specified cabinet file.
//
#define CRS_EVENT_CAB_ERROR_INFFILE_NOT_FOUND 0xC0003916L

//
// MessageId: CRS_EVENT_INF_ERROR_ADVPACK_NOT_LOADED
//
// MessageText:
//
//  Could not load ADVPACK.DLL.
//
#define CRS_EVENT_INF_ERROR_ADVPACK_NOT_LOADED 0xC0003917L

//
// MessageId: CRS_EVENT_INF_ERROR_ADVPACK_WRONG_VERSION
//
// MessageText:
//
//  This version of ADVPACK.DLL is not supported.
//
#define CRS_EVENT_INF_ERROR_ADVPACK_WRONG_VERSION 0xC0003918L

//
// MessageId: CRS_EVENT_INF_ERROR_INVALID_INF
//
// MessageText:
//
//  INF file found inside the specified cabinet file is not a valid INF file.
//
#define CRS_EVENT_INF_ERROR_INVALID_INF  0xC0003919L

//
// MessageId: CRS_EVENT_INF_ERROR_NOTHING_TO_INSTALL
//
// MessageText:
//
//  Could not find any of the remote install sections inside the INF file.
//
#define CRS_EVENT_INF_ERROR_NOTHING_TO_INSTALL 0xC000391AL

//
// MessageId: CRS_EVENT_INF_ERROR_NOTHING_TO_UNINSTALL
//
// MessageText:
//
//  Could not find any of the remote uninstall sections inside the INF file.
//
#define CRS_EVENT_INF_ERROR_NOTHING_TO_UNINSTALL 0xC000391BL

//
// MessageId: CRS_EVENT_INF_ERROR_BUSY_FILES
//
// MessageText:
//
//  Files are busy.  Cannot install them unless allowed to reboot.
//
#define CRS_EVENT_INF_ERROR_BUSY_FILES   0xC000391CL

//
// MessageId: CRS_EVENT_CAB_WARNING_MANUAL_REBOOT
//
// MessageText:
//
//  Setup completed successfully. The server could not be rebooted automatically. Manual reboot is necessary.
//
#define CRS_EVENT_CAB_WARNING_MANUAL_REBOOT 0x8000391DL

//
// MessageId: CRS_EVENT_CAB_WARNING_REBOOT_NEEDED
//
// MessageText:
//
//  Setup completed successfully. The files became busy during setup.  Manual reboot is necessary.
//
#define CRS_EVENT_CAB_WARNING_REBOOT_NEEDED 0x8000391EL

//
// MessageId: CRS_EVENT_INF_SUCCESS_REBOOT_IN_PROGRESS
//
// MessageText:
//
//  Setup completed successfully. The server is being rebooted.
//
#define CRS_EVENT_INF_SUCCESS_REBOOT_IN_PROGRESS 0x0000391FL

//
// MessageId: CRS_EVENT_INF_SUCCESS
//
// MessageText:
//
//  Setup completed successfully.
//
#define CRS_EVENT_INF_SUCCESS            0x00003921L

//
// MessageId: CRS_EVENT_INF_ERROR_BUSY_FILES_UNINSTALL
//
// MessageText:
//
//  Files are busy.  Cannot uninstall them unless allowed to reboot.
//
#define CRS_EVENT_INF_ERROR_BUSY_FILES_UNINSTALL 0xC0003922L

//
// MessageId: CRS_EVENT_CAB_WARNING_MANUAL_REBOOT_UNINSTALL
//
// MessageText:
//
//  Uninstall completed successfully. The server could not be rebooted automatically. Manual reboot is necessary.
//
#define CRS_EVENT_CAB_WARNING_MANUAL_REBOOT_UNINSTALL 0x80003923L

//
// MessageId: CRS_EVENT_CAB_WARNING_REBOOT_NEEDED_UNINSTALL
//
// MessageText:
//
//  Uninstall completed successfully. The files became busy during setup.  Manual reboot is necessary.
//
#define CRS_EVENT_CAB_WARNING_REBOOT_NEEDED_UNINSTALL 0x80003924L

//
// MessageId: CRS_EVENT_INF_SUCCESS_REBOOT_IN_PROGRESS_UNINSTALL
//
// MessageText:
//
//  Uninstall completed successfully. The server is being rebooted.
//
#define CRS_EVENT_INF_SUCCESS_REBOOT_IN_PROGRESS_UNINSTALL 0x00003925L

//
// MessageId: CRS_EVENT_INF_SUCCESS_UNINSTALL
//
// MessageText:
//
//  Uninstall completed successfully.
//
#define CRS_EVENT_INF_SUCCESS_UNINSTALL  0x00003926L

//
// MessageId: CRS_EVENT_CAB_ERROR_ADVPACK_FAILED
//
// MessageText:
//
//  Problem installing the CAB. Check CAB and INF files for validity. 
//
#define CRS_EVENT_CAB_ERROR_ADVPACK_FAILED 0xC0003927L

//
// MessageId: CRS_EVENT_CAB_ERROR_LOCKED_FILES
//
// MessageText:
//
//  At least one of the files inside the CAB is open for exclusive write/delete access.
//
#define CRS_EVENT_CAB_ERROR_LOCKED_FILES 0xC0003928L

//
// MessageId: CRS_EVENT_CAB_ERROR_UNKNOWN
//
// MessageText:
//
//  Unknown error occurred while processing the specified cabinet file.
//
#define CRS_EVENT_CAB_ERROR_UNKNOWN      0xC0003A06L

//
// MessageId: CRS_EVENT_CAB_ERROR_LAST
//
// MessageText:
//
//  Remote installer last event message.
//
#define CRS_EVENT_CAB_ERROR_LAST         0x40003A07L

//
// MessageId: CRS_EVENT_LAST
//
// MessageText:
//
//  Site Server Content Deployment last event message.
//
#define CRS_EVENT_LAST                   0x40003A97L

