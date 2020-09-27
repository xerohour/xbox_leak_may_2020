

























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
// MessageId: CDO_E_UNCAUGHT_EXCEPTION
//
// MessageText:
//
//  Exception %1 was generated at address %2
//
#define CDO_E_UNCAUGHT_EXCEPTION         0x80040201L


//
// MessageId: CDO_E_NOT_OPENED
//
// MessageText:
//
//  No data source has been opened for the object.
//
#define CDO_E_NOT_OPENED                 0x80040202L

//
// MessageId: CDO_E_UNSUPPORTED_DATASOURCE
//
// MessageText:
//
//  The object does not support this type of data source.
//
#define CDO_E_UNSUPPORTED_DATASOURCE     0x80040203L

//
// MessageId: CDO_E_INVALID_PROPERTYNAME
//
// MessageText:
//
//  The object does not support the requested property name or namespace.
//
#define CDO_E_INVALID_PROPERTYNAME       0x80040204L

//
// MessageId: CDO_E_PROP_UNSUPPORTED
//
// MessageText:
//
//  The object does not support the requested property.
//
#define CDO_E_PROP_UNSUPPORTED           0x80040205L


//
// MessageId: CDO_E_INACTIVE
//
// MessageText:
//
//  The object is not active. It may have been deleted or it may not have been opened.
//
#define CDO_E_INACTIVE                   0x80040206L

//
// MessageId: CDO_E_NO_SUPPORT_FOR_OBJECTS
//
// MessageText:
//
//  The object does not support storing persistent state information for objects.
//
#define CDO_E_NO_SUPPORT_FOR_OBJECTS     0x80040207L

//
// MessageId: CDO_E_NOT_AVAILABLE
//
// MessageText:
//
//  The requested property or feature, while supported, is not available at this time or in this context.
//
#define CDO_E_NOT_AVAILABLE              0x80040208L

//
// MessageId: CDO_E_NO_DEFAULT_DROP_DIR
//
// MessageText:
//
//  No default drop directory has been configured for this server.
//
#define CDO_E_NO_DEFAULT_DROP_DIR        0x80040209L

//
// MessageId: CDO_E_SMTP_SERVER_REQUIRED
//
// MessageText:
//
//  The SMTP server name is required, and was not found in the configuration source.
//
#define CDO_E_SMTP_SERVER_REQUIRED       0x8004020AL

//
// MessageId: CDO_E_NNTP_SERVER_REQUIRED
//
// MessageText:
//
//  The NNTP server name is required, and was not found in the configuration source.
//
#define CDO_E_NNTP_SERVER_REQUIRED       0x8004020BL

//
// MessageId: CDO_E_RECIPIENT_MISSING
//
// MessageText:
//
//  At least one recipient is required, but none were found.
//
#define CDO_E_RECIPIENT_MISSING          0x8004020CL

//
// MessageId: CDO_E_FROM_MISSING
//
// MessageText:
//
//  At least one of the From or Sender fields is required, and neither was found.
//
#define CDO_E_FROM_MISSING               0x8004020DL

//
// MessageId: CDO_E_SENDER_REJECTED
//
// MessageText:
//
//  The server rejected the sender address. The server response was: %1
//
#define CDO_E_SENDER_REJECTED            0x8004020EL

//
// MessageId: CDO_E_RECIPIENTS_REJECTED
//
// MessageText:
//
//  The server rejected one or more recipient addresses. The server response was: %1
//
#define CDO_E_RECIPIENTS_REJECTED        0x8004020FL

//
// MessageId: CDO_E_NNTP_POST_FAILED
//
// MessageText:
//
//  The message could not be posted to the NNTP server. The transport error code was %2. The server response was %1
//
#define CDO_E_NNTP_POST_FAILED           0x80040210L

//
// MessageId: CDO_E_SMTP_SEND_FAILED
//
// MessageText:
//
//  The message could not be sent to the SMTP server. The transport error code was %2. The server response was %1
//
#define CDO_E_SMTP_SEND_FAILED           0x80040211L

//
// MessageId: CDO_E_CONNECTION_DROPPED
//
// MessageText:
//
//  The transport lost its connection to the server.
//
#define CDO_E_CONNECTION_DROPPED         0x80040212L

//
// MessageId: CDO_E_FAILED_TO_CONNECT
//
// MessageText:
//
//  The transport failed to connect to the server.
//
#define CDO_E_FAILED_TO_CONNECT          0x80040213L

//
// MessageId: CDO_E_INVALID_POST
//
// MessageText:
//
//  The Subject, From, and Newsgroup fields are all required, and one or more was not found.
//
#define CDO_E_INVALID_POST               0x80040214L


//
// MessageId: CDO_E_AUTHENTICATION_FAILURE
//
// MessageText:
//
//  The server rejected the logon attempt due to authentication failure. The server response was: %1
//
#define CDO_E_AUTHENTICATION_FAILURE     0x80040215L

//
// MessageId: CDO_E_INVALID_CONTENT_TYPE
//
// MessageText:
//
//  The content type was not valid in this context. For example, the root of an MHTML message must be an HTML document.
//
#define CDO_E_INVALID_CONTENT_TYPE       0x80040216L

//
// MessageId: CDO_E_LOGON_FAILURE
//
// MessageText:
//
//  The transport was unable to log on to the server.
//
#define CDO_E_LOGON_FAILURE              0x80040217L

//
// MessageId: CDO_E_HTTP_NOT_FOUND
//
// MessageText:
//
//  The requested resource could not be found. The server response was: %1.
//
#define CDO_E_HTTP_NOT_FOUND             0x80040218L

//
// MessageId: CDO_E_HTTP_FORBIDDEN
//
// MessageText:
//
//  Access to the requested resource is denied. The server response was: %1.
//
#define CDO_E_HTTP_FORBIDDEN             0x80040219L

//
// MessageId: CDO_E_HTTP_FAILED
//
// MessageText:
//
//  The HTTP request failed.  The server response was: %1.
//
#define CDO_E_HTTP_FAILED                0x8004021AL

//
// MessageId: CDO_E_MULTIPART_NO_DATA
//
// MessageText:
//
//  This is a multipart body part. It has no content other than the body parts contained within it.
//
#define CDO_E_MULTIPART_NO_DATA          0x8004021BL


//
// MessageId: CDO_E_INVALID_ENCODING_FOR_MULTIPART
//
// MessageText:
//
//  Multipart body parts must be encoded as 7bit, 8bit, or binary.
//
#define CDO_E_INVALID_ENCODING_FOR_MULTIPART 0x8004021CL


//
// MessageId: CDO_E_PROP_NOT_FOUND
//
// MessageText:
//
//  The requested property was not found.
//
#define CDO_E_PROP_NOT_FOUND             0x8004021EL


//
// MessageId: CDO_E_INVALID_SEND_OPTION
//
// MessageText:
//
//  The "SendUsing" configuration value is invalid.
//
#define CDO_E_INVALID_SEND_OPTION        0x80040220L

//
// MessageId: CDO_E_INVALID_POST_OPTION
//
// MessageText:
//
//  The "PostUsing" configuration value is invalid.
//
#define CDO_E_INVALID_POST_OPTION        0x80040221L

//
// MessageId: CDO_E_NO_PICKUP_DIR
//
// MessageText:
//
//  The pickup directory path is required and was not specified. 
//
#define CDO_E_NO_PICKUP_DIR              0x80040222L

//
// MessageId: CDO_E_NOT_ALL_DELETED
//
// MessageText:
//
//  One or more messages could not be deleted.
//
#define CDO_E_NOT_ALL_DELETED            0x80040223L




//
// MessageId: CDO_E_PROP_READONLY
//
// MessageText:
//
//  The property is read-only.
//
#define CDO_E_PROP_READONLY              0x80040227L

//
// MessageId: CDO_E_PROP_CANNOT_DELETE
//
// MessageText:
//
//  The property cannot be deleted.
//
#define CDO_E_PROP_CANNOT_DELETE         0x80040228L

//
// MessageId: CDO_E_BAD_DATA
//
// MessageText:
//
//  Data  written to the object are inconsistent or invalid. 
//
#define CDO_E_BAD_DATA                   0x80040229L

//
// MessageId: CDO_E_PROP_NONHEADER
//
// MessageText:
//
//  The requested property is not in the mail header namespace.
//
#define CDO_E_PROP_NONHEADER             0x8004022AL

//
// MessageId: CDO_E_INVALID_CHARSET
//
// MessageText:
//
//  The requested character set is not installed on the computer.
//
#define CDO_E_INVALID_CHARSET            0x8004022BL

//
// MessageId: CDO_E_ADOSTREAM_NOT_BOUND
//
// MessageText:
//
//  The ADO stream has not been opened.
//
#define CDO_E_ADOSTREAM_NOT_BOUND        0x8004022CL

//
// MessageId: CDO_E_CONTENTPROPXML_NOT_FOUND
//
// MessageText:
//
//  The content properties are missing.
//
#define CDO_E_CONTENTPROPXML_NOT_FOUND   0x8004022DL

//
// MessageId: CDO_E_CONTENTPROPXML_WRONG_CHARSET
//
// MessageText:
//
//  Content properties XML must be encoded using UTF-8.
//
#define CDO_E_CONTENTPROPXML_WRONG_CHARSET 0x8004022EL

//
// MessageId: CDO_E_CONTENTPROPXML_PARSE_FAILED
//
// MessageText:
//
//  Failed to parse content properties XML.
//
#define CDO_E_CONTENTPROPXML_PARSE_FAILED 0x8004022FL

//
// MessageId: CDO_E_CONTENTPROPXML_CONVERT_FAILED
//
// MessageText:
//
//  Failed to convert a property from XML to a requested type.
//
#define CDO_E_CONTENTPROPXML_CONVERT_FAILED 0x80040230L

//
// MessageId: CDO_E_NO_DIRECTORIES_SPECIFIED
//
// MessageText:
//
//  No directories were specified for resolution.
//
#define CDO_E_NO_DIRECTORIES_SPECIFIED   0x80040231L

//
// MessageId: CDO_E_DIRECTORIES_UNREACHABLE
//
// MessageText:
//
//  Failed to resolve against one or more of the specified directories.
//
#define CDO_E_DIRECTORIES_UNREACHABLE    0x80040232L

//
// MessageId: CDO_E_BAD_SENDER
//
// MessageText:
//
//  Could not find the Sender's mailbox.
//
#define CDO_E_BAD_SENDER                 0x80040233L

//
// MessageId: CDO_E_SELF_BINDING
//
// MessageText:
//
//  Binding to self is not allowed.
//
#define CDO_E_SELF_BINDING               0x80040234L




//
// MessageId: CDO_E_ARGUMENT1
//
// MessageText:
//
//  The first argument is invalid
//
#define CDO_E_ARGUMENT1                  0x80044000L

//
// MessageId: CDO_E_ARGUMENT2
//
// MessageText:
//
//  The second argument is invalid
//
#define CDO_E_ARGUMENT2                  0x80044001L

//
// MessageId: CDO_E_ARGUMENT3
//
// MessageText:
//
//  The third argument is invalid
//
#define CDO_E_ARGUMENT3                  0x80044002L

//
// MessageId: CDO_E_ARGUMENT4
//
// MessageText:
//
//  The fourth argument is invalid
//
#define CDO_E_ARGUMENT4                  0x80044003L

//
// MessageId: CDO_E_ARGUMENT5
//
// MessageText:
//
//  The fifth argument is invalid
//
#define CDO_E_ARGUMENT5                  0x80044004L










//
// MessageId: CDO_E_NOT_FOUND
//
// MessageText:
//
//  The requested body part was not found in this message.
//
#define CDO_E_NOT_FOUND                  0x800CCE05L






//
// MessageId: CDO_E_INVALID_ENCODING_TYPE
//
// MessageText:
//
//  The content encoding type is invalid.
//
#define CDO_E_INVALID_ENCODING_TYPE      0x800CCE1DL






//
// MessageId: IDS_ORIGINAL_MESSAGE
//
// MessageText:
//
//  -----Original Message-----%0
//
#define IDS_ORIGINAL_MESSAGE             0x00011000L

//
// MessageId: IDS_FROM
//
// MessageText:
//
//  From:%0
//
#define IDS_FROM                         0x00011001L

//
// MessageId: IDS_SENT
//
// MessageText:
//
//  Sent:%0
//
#define IDS_SENT                         0x00011002L

//
// MessageId: IDS_POSTED_AT
//
// MessageText:
//
//  Posted At:%0
//
#define IDS_POSTED_AT                    0x00011003L

//
// MessageId: IDS_TO
//
// MessageText:
//
//  To:%0
//
#define IDS_TO                           0x00011004L

//
// MessageId: IDS_CC
//
// MessageText:
//
//  Cc:%0
//
#define IDS_CC                           0x00011005L

//
// MessageId: IDS_POSTED_TO
//
// MessageText:
//
//  Posted To:%0
//
#define IDS_POSTED_TO                    0x00011006L

//
// MessageId: IDS_CONVERSATION
//
// MessageText:
//
//  Conversation:%0
//
#define IDS_CONVERSATION                 0x00011007L

//
// MessageId: IDS_SUBJECT
//
// MessageText:
//
//  Subject:%0
//
#define IDS_SUBJECT                      0x00011008L

//
// MessageId: IDS_IMPORTANCE
//
// MessageText:
//
//  Importance:%0
//
#define IDS_IMPORTANCE                   0x00011009L

//
// MessageId: IDS_ON_BEHALF_OF
//
// MessageText:
//
//  on behalf of%0
//
#define IDS_ON_BEHALF_OF                 0x0001100AL

//
// MessageId: IDS_FW
//
// MessageText:
//
//  FW:%0
//
#define IDS_FW                           0x0001100BL

//
// MessageId: IDS_RE
//
// MessageText:
//
//  RE:%0
//
#define IDS_RE                           0x0001100CL

//
// MessageId: IDS_CODEPAGE
//
// MessageText:
//
//  1252%0
//
#define IDS_CODEPAGE                     0x0001100DL

#ifdef CDOSVR
//
// MessageId: IDS_CalendarFolder
//
// MessageText:
//
//  Calendar%0
//
#define IDS_CalendarFolder               0x0001100EL

//
// MessageId: IDS_ContactsFolder
//
// MessageText:
//
//  Contacts%0
//
#define IDS_ContactsFolder               0x0001100FL

//
// MessageId: IDS_DraftsFolder
//
// MessageText:
//
//  Drafts%0
//
#define IDS_DraftsFolder                 0x00011010L

//
// MessageId: IDS_JournalFolder
//
// MessageText:
//
//  Journal%0
//
#define IDS_JournalFolder                0x00011011L

//
// MessageId: IDS_NotesFolder
//
// MessageText:
//
//  Notes%0
//
#define IDS_NotesFolder                  0x00011012L

//
// MessageId: IDS_TasksFolder
//
// MessageText:
//
//  Tasks%0
//
#define IDS_TasksFolder                  0x00011013L

#endif





#ifdef CDOSVR
//
// MessageId: evtMethodCalled
//
// MessageText:
//
//  %1 Event on %2 called with Flags %3
//
#define evtMethodCalled                  0x00032000L

//
// MessageId: evtMethodReturning
//
// MessageText:
//
//  %1 Event Method is returning with HRESULT %2
//
#define evtMethodReturning               0x00032001L

//
// MessageId: evtIsAborting
//
// MessageText:
//
//  %1 Event Method is aborting, HRESULT %2
//
#define evtIsAborting                    0xC0032002L

//
// MessageId: evtExpansionInitialized
//
// MessageText:
//
//  Calendaring agent is initialized successfully.
//
#define evtExpansionInitialized          0x00032003L

//
// MessageId: evtExpansionUnInitialized
//
// MessageText:
//
//  Calendaring agent is stopping successfully.
//
#define evtExpansionUnInitialized        0x00032004L

//
// MessageId: evtExpansionInitializeFailed
//
// MessageText:
//
//  Calendaring agent failed to initialize with error %1.
//
#define evtExpansionInitializeFailed     0xC0032005L

//
// MessageId: evtExpansionRegisterFailed
//
// MessageText:
//
//  Calendaring recurring item expansion failed to register for notifications for MDB %1.
//
#define evtExpansionRegisterFailed       0xC0032006L

//
// MessageId: evtExpansionMessageSaveChangesFailed
//
// MessageText:
//
//  Calendaring agent failed in message save notification with error %1 on URL %2.
//
#define evtExpansionMessageSaveChangesFailed 0xC0032007L

//
// MessageId: evtExpansionMessageDeleteFailed
//
// MessageText:
//
//  Calendaring agent failed in message delete notification with error %1 on URL %2.
//
#define evtExpansionMessageDeleteFailed  0xC0032008L

//
// MessageId: evtExpansionFolderSaveChangesFailed
//
// MessageText:
//
//  Calendaring agent failed in folder save notification with error %1 on URL %2.
//
#define evtExpansionFolderSaveChangesFailed 0xC0032009L

//
// MessageId: evtExpansionTooManyInstancesPerDay
//
// MessageText:
//
//  Calendaring agent truncated expansion after %1 instances per day.
//
#define evtExpansionTooManyInstancesPerDay 0x8003200AL

//
// MessageId: evtMailboxCreateTotalFailure
//
// MessageText:
//
//  The mailbox creation callback failed to create any folders.
//
#define evtMailboxCreateTotalFailure     0xC003200BL

//
// MessageId: evtMailboxCreatePartialFailure
//
// MessageText:
//
//  The mailbox creation callback failed to create one or more folders.
//
#define evtMailboxCreatePartialFailure   0xC003200CL

//
// MessageId: evtUninitImplRestFailed
//
// MessageText:
//
//  Calendaring agent failed to uninitialize implied restriction with error %1 on URL %2.
//
#define evtUninitImplRestFailed          0xC003200DL

#endif
