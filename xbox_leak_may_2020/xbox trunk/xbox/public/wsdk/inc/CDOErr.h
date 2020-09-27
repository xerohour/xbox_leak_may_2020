





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
// MessageId: CDO_E_FAIL
//
// MessageText:
//
//  AnBrad memorial error
//
#define CDO_E_FAIL                       0x80040200L

//
// MessageId: CDO_E_UNCAUGHT_EXCEPTION
//
// MessageText:
//
//  Exception %1 was generated at address %2
//
#define CDO_E_UNCAUGHT_EXCEPTION         0x80040201L

//
// MessageId: CDO_E_NOT_BOUND
//
// MessageText:
//
//  The object is not bound to a data source
//
#define CDO_E_NOT_BOUND                  0x80040202L

//
// MessageId: CDO_E_UNSUPPORTED_DATASOURCE
//
// MessageText:
//
//  The datasource is not supported by this object
//
#define CDO_E_UNSUPPORTED_DATASOURCE     0x80040203L

//
// MessageId: CDO_E_INVALID_PROPERTYNAME
//
// MessageText:
//
//  Invalid property name
//
#define CDO_E_INVALID_PROPERTYNAME       0x80040204L

//
// MessageId: CDO_E_CUSTOMPROP_UNSUPPORTED
//
// MessageText:
//
//  Custom property is not supported by this object class
//
#define CDO_E_CUSTOMPROP_UNSUPPORTED     0x80040205L

//
// MessageId: CDO_E_NOTINITIALIZED
//
// MessageText:
//
//  The object is not initialized
//
#define CDO_E_NOTINITIALIZED             0x80040206L

//
// MessageId: CDO_E_INVALID_ENCODING_TYPE
//
// MessageText:
//
//  Invalid encoding type
//
#define CDO_E_INVALID_ENCODING_TYPE      0x80040207L

//
// MessageId: CDO_E_NO_SUPPORT_FOR_OBJECTS
//
// MessageText:
//
//  No support to store objects.
//
#define CDO_E_NO_SUPPORT_FOR_OBJECTS     0x80040208L

//
// MessageId: CDO_E_PROP_NOT_AVAILABLE
//
// MessageText:
//
//  This property is not available.
//
#define CDO_E_PROP_NOT_AVAILABLE         0x80040209L

//
// MessageId: CDO_E_NO_DEFAULT_DROP_DIR
//
// MessageText:
//
//  There is no default drop directory available.
//
#define CDO_E_NO_DEFAULT_DROP_DIR        0x8004020AL

//
// MessageId: CDO_E_SKIP_ALL_SINKS
//
// MessageText:
//
//  Skip all remaining sinks for this event.
//
#define CDO_E_SKIP_ALL_SINKS             0x8004020BL

//
// MessageId: CDO_E_SMTP_SERVER_REQUIRED
//
// MessageText:
//
//  SMTP server name is required.
//
#define CDO_E_SMTP_SERVER_REQUIRED       0x8004020CL

//
// MessageId: CDO_E_NNTP_SERVER_REQUIRED
//
// MessageText:
//
//  NNTP server name is required.
//
#define CDO_E_NNTP_SERVER_REQUIRED       0x8004020DL

//
// MessageId: CDO_E_RECIPIENT_MISSING
//
// MessageText:
//
//  A required recipient is missing.
//
#define CDO_E_RECIPIENT_MISSING          0x8004020EL

//
// MessageId: CDO_E_SENDER_REJECTED
//
// MessageText:
//
//  The sender address was rejected.
//
#define CDO_E_SENDER_REJECTED            0x8004020FL

//
// MessageId: CDO_E_RECIPIENTS_REJECTED
//
// MessageText:
//
//  The recipients addresses were rejected.
//
#define CDO_E_RECIPIENTS_REJECTED        0x80040210L

//
// MessageId: CDO_E_SOCKET_WRITE_ERROR
//
// MessageText:
//
//  The transport reported a socket write error.
//
#define CDO_E_SOCKET_WRITE_ERROR         0x80040211L

//
// MessageId: CDO_E_NNTP_POST_FAILED
//
// MessageText:
//
//  The transport reported a post failed error.
//
#define CDO_E_NNTP_POST_FAILED           0x80040212L

//
// MessageId: CDO_E_CONNECTION_DROPPED
//
// MessageText:
//
//  The transport lost the connecion to the server.
//
#define CDO_E_CONNECTION_DROPPED         0x80040213L

//
// MessageId: CDO_E_FAILED_TO_CONNECT
//
// MessageText:
//
//  The transport failed to connect to the server.
//
#define CDO_E_FAILED_TO_CONNECT          0x80040214L

//
// MessageId: CDO_E_INVALID_POST
//
// MessageText:
//
//  Both the subject field and the newsgroup field must be present.
//
#define CDO_E_INVALID_POST               0x80040215L

//
// MessageId: CDO_E_INVALID_CONTENT_TYPE
//
// MessageText:
//
//  The content type cannot be added to the message.
//
#define CDO_E_INVALID_CONTENT_TYPE       0x80040216L




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
// MessageId: XX_MIME_E_REG_CREATE_KEY
//
// MessageText:
//
//  XX_MIME_E_REG_CREATE_KEY
//
#define XX_MIME_E_REG_CREATE_KEY         0x800CCE01L

//
// MessageId: XX_MIME_E_REG_QUERY_INFO
//
// MessageText:
//
//  XX_MIME_E_REG_QUERY_INFO
//
#define XX_MIME_E_REG_QUERY_INFO         0x800CCE02L

//
// MessageId: XX_MIME_E_INVALID_ENCTYPE
//
// MessageText:
//
//  XX_MIME_E_INVALID_ENCTYPE
//
#define XX_MIME_E_INVALID_ENCTYPE        0x800CCE03L

//
// MessageId: XX_MIME_E_BOUNDARY_MISMATCH
//
// MessageText:
//
//  XX_MIME_E_BOUNDARY_MISMATCH
//
#define XX_MIME_E_BOUNDARY_MISMATCH      0x800CCE04L

//
// MessageId: XX_MIME_E_NOT_FOUND
//
// MessageText:
//
//  XX_MIME_E_NOT_FOUND
//
#define XX_MIME_E_NOT_FOUND              0x800CCE05L


//
// MessageId: XX_MIME_E_BUFFER_TOO_SMALL
//
// MessageText:
//
//  XX_MIME_E_BUFFER_TOO_SMALL
//
#define XX_MIME_E_BUFFER_TOO_SMALL       0x800CCE06L

//
// MessageId: XX_MIME_E_INVALID_ITEM_FLAGS
//
// MessageText:
//
//  XX_MIME_E_INVALID_ITEM_FLAGS
//
#define XX_MIME_E_INVALID_ITEM_FLAGS     0x800CCE07L

//
// MessageId: XX_MIME_E_ONE_LINE_ITEM
//
// MessageText:
//
//  XX_MIME_E_ONE_LINE_ITEM
//
#define XX_MIME_E_ONE_LINE_ITEM          0x800CCE08L

//
// MessageId: XX_MIME_E_INVALID_HANDLE
//
// MessageText:
//
//  XX_MIME_E_INVALID_HANDLE
//
#define XX_MIME_E_INVALID_HANDLE         0x800CCE09L

//
// MessageId: XX_MIME_E_CHARSET_TRANSLATE
//
// MessageText:
//
//  XX_MIME_E_CHARSET_TRANSLATE
//
#define XX_MIME_E_CHARSET_TRANSLATE      0x800CCE0AL

//
// MessageId: XX_MIME_E_NOT_INITIALIZED
//
// MessageText:
//
//  XX_MIME_E_NOT_INITIALIZED
//
#define XX_MIME_E_NOT_INITIALIZED        0x800CCE0BL

//
// MessageId: XX_MIME_E_NO_MORE_ROWS
//
// MessageText:
//
//  XX_MIME_E_NO_MORE_ROWS
//
#define XX_MIME_E_NO_MORE_ROWS           0x800CCE0CL

//
// MessageId: XX_MIME_E_ALREADY_BOUND
//
// MessageText:
//
//  XX_MIME_E_ALREADY_BOUND
//
#define XX_MIME_E_ALREADY_BOUND          0x800CCE0DL

//
// MessageId: XX_MIME_E_CANT_RESET_ROOT
//
// MessageText:
//
//  XX_MIME_E_CANT_RESET_ROOT
//
#define XX_MIME_E_CANT_RESET_ROOT        0x800CCE0EL

//
// MessageId: XX_MIME_E_INSERT_NOT_ALLOWED
//
// MessageText:
//
//  XX_MIME_E_INSERT_NOT_ALLOWED
//
#define XX_MIME_E_INSERT_NOT_ALLOWED     0x800CCE0FL

//
// MessageId: XX_MIME_E_BAD_BODY_LOCATION
//
// MessageText:
//
//  XX_MIME_E_BAD_BODY_LOCATION
//
#define XX_MIME_E_BAD_BODY_LOCATION      0x800CCE10L

//
// MessageId: XX_MIME_E_NOT_MULTIPART
//
// MessageText:
//
//  XX_MIME_E_NOT_MULTIPART
//
#define XX_MIME_E_NOT_MULTIPART          0x800CCE11L

//
// MessageId: XX_MIME_E_NO_MULTIPART_BOUNDARY
//
// MessageText:
//
//  XX_MIME_E_NO_MULTIPART_BOUNDARY
//
#define XX_MIME_E_NO_MULTIPART_BOUNDARY  0x800CCE12L

//
// MessageId: XX_MIME_E_CONVERT_NOT_NEEDED
//
// MessageText:
//
//  XX_MIME_E_CONVERT_NOT_NEEDED
//
#define XX_MIME_E_CONVERT_NOT_NEEDED     0x800CCE13L

//
// MessageId: XX_MIME_E_CANT_MOVE_BODY
//
// MessageText:
//
//  XX_MIME_E_CANT_MOVE_BODY
//
#define XX_MIME_E_CANT_MOVE_BODY         0x800CCE14L

//
// MessageId: XX_MIME_E_UNKNOWN_BODYTREE_VERSION
//
// MessageText:
//
//  XX_MIME_E_UNKNOWN_BODYTREE_VERSION
//
#define XX_MIME_E_UNKNOWN_BODYTREE_VERSION 0x800CCE15L

//
// MessageId: XX_MIME_E_NOTHING_TO_SAVE
//
// MessageText:
//
//  XX_MIME_E_NOTHING_TO_SAVE
//
#define XX_MIME_E_NOTHING_TO_SAVE        0x800CCE16L

//
// MessageId: XX_MIME_E_NEED_SAVE_MESSAGE
//
// MessageText:
//
//  XX_MIME_E_NEED_SAVE_MESSAGE
//
#define XX_MIME_E_NEED_SAVE_MESSAGE      0x800CCE17L

//
// MessageId: XX_MIME_E_NOTHING_TO_REVERT
//
// MessageText:
//
//  XX_MIME_E_NOTHING_TO_REVERT
//
#define XX_MIME_E_NOTHING_TO_REVERT      0x800CCE18L

//
// MessageId: XX_MIME_E_MSG_SIZE_DIFF
//
// MessageText:
//
//  XX_MIME_E_MSG_SIZE_DIFF
//
#define XX_MIME_E_MSG_SIZE_DIFF          0x800CCE19L

//
// MessageId: XX_MIME_E_CANT_RESET_PARENT
//
// MessageText:
//
//  XX_MIME_E_CANT_RESET_PARENT
//
#define XX_MIME_E_CANT_RESET_PARENT      0x800CCE1AL

//
// MessageId: XX_MIME_E_CORRUPT_CACHE_TREE
//
// MessageText:
//
//  XX_MIME_E_CORRUPT_CACHE_TREE
//
#define XX_MIME_E_CORRUPT_CACHE_TREE     0x800CCE1BL

//
// MessageId: XX_MIME_E_BODYTREE_OUT_OF_SYNC
//
// MessageText:
//
//  XX_MIME_E_BODYTREE_OUT_OF_SYNC
//
#define XX_MIME_E_BODYTREE_OUT_OF_SYNC   0x800CCE1CL

//
// MessageId: XX_MIME_E_INVALID_ENCODINGTYPE
//
// MessageText:
//
//  XX_MIME_E_INVALID_ENCODINGTYPE
//
#define XX_MIME_E_INVALID_ENCODINGTYPE   0x800CCE1DL

//
// MessageId: XX_MIME_E_MULTIPART_NO_DATA
//
// MessageText:
//
//  XX_MIME_E_MULTIPART_NO_DATA
//
#define XX_MIME_E_MULTIPART_NO_DATA      0x800CCE1EL

//
// MessageId: XX_MIME_E_INVALID_OPTION_VALUE
//
// MessageText:
//
//  XX_MIME_E_INVALID_OPTION_VALUE
//
#define XX_MIME_E_INVALID_OPTION_VALUE   0x800CCE1FL

//
// MessageId: XX_MIME_E_INVALID_OPTION_ID
//
// MessageText:
//
//  XX_MIME_E_INVALID_OPTION_ID
//
#define XX_MIME_E_INVALID_OPTION_ID      0x800CCE20L

//
// MessageId: XX_MIME_E_INVALID_HEADER_NAME
//
// MessageText:
//
//  XX_MIME_E_INVALID_HEADER_NAME
//
#define XX_MIME_E_INVALID_HEADER_NAME    0x800CCE21L

//
// MessageId: XX_MIME_E_NOT_BOUND
//
// MessageText:
//
//  XX_MIME_E_NOT_BOUND
//
#define XX_MIME_E_NOT_BOUND              0x800CCE22L

//
// MessageId: XX_MIME_E_MAX_SIZE_TOO_SMALL
//
// MessageText:
//
//  XX_MIME_E_MAX_SIZE_TOO_SMALL
//
#define XX_MIME_E_MAX_SIZE_TOO_SMALL     0x800CCE23L

//
// MessageId: XX_MIME_E_MULTIPART_HAS_CHILDREN
//
// MessageText:
//
//  XX_MIME_E_MULTIPART_HAS_CHILDREN
//
#define XX_MIME_E_MULTIPART_HAS_CHILDREN 0x800CCE25L

//
// MessageId: XX_MIME_E_INVALID_PROP_FLAGS
//
// MessageText:
//
//  XX_MIME_E_INVALID_PROP_FLAGS
//
#define XX_MIME_E_INVALID_PROP_FLAGS     0x800CCE26L

//
// MessageId: XX_MIME_E_INVALID_ADDRESS_TYPE
//
// MessageText:
//
//  XX_MIME_E_INVALID_ADDRESS_TYPE
//
#define XX_MIME_E_INVALID_ADDRESS_TYPE   0x800CCE27L

//
// MessageId: XX_MIME_E_INVALID_OBJECT_IID
//
// MessageText:
//
//  XX_MIME_E_INVALID_OBJECT_IID
//
#define XX_MIME_E_INVALID_OBJECT_IID     0x800CCE28L

//
// MessageId: XX_MIME_E_MLANG_DLL_NOT_FOUND
//
// MessageText:
//
//  XX_MIME_E_MLANG_DLL_NOT_FOUND
//
#define XX_MIME_E_MLANG_DLL_NOT_FOUND    0x800CCE29L

//
// MessageId: XX_MIME_E_ROOT_NOT_EMPTY
//
// MessageText:
//
//  XX_MIME_E_ROOT_NOT_EMPTY
//
#define XX_MIME_E_ROOT_NOT_EMPTY         0x800CCE2AL

//
// MessageId: XX_MIME_E_MLANG_BAD_DLL
//
// MessageText:
//
//  XX_MIME_E_MLANG_BAD_DLL
//
#define XX_MIME_E_MLANG_BAD_DLL          0x800CCE2BL

//
// MessageId: XX_MIME_E_REG_OPEN_KEY
//
// MessageText:
//
//  XX_MIME_E_REG_OPEN_KEY
//
#define XX_MIME_E_REG_OPEN_KEY           0x800CCE2CL

//
// MessageId: XX_MIME_E_INVALID_INET_DATE
//
// MessageText:
//
//  XX_MIME_E_INVALID_INET_DATE
//
#define XX_MIME_E_INVALID_INET_DATE      0x800CCE2DL

//
// MessageId: XX_MIME_E_INVALID_BODYTYPE
//
// MessageText:
//
//  XX_MIME_E_INVALID_BODYTYPE
//
#define XX_MIME_E_INVALID_BODYTYPE       0x800CCE2EL

//
// MessageId: XX_MIME_E_INVALID_DELETE_TYPE
//
// MessageText:
//
//  XX_MIME_E_INVALID_DELETE_TYPE
//
#define XX_MIME_E_INVALID_DELETE_TYPE    0x800CCE2FL

//
// MessageId: XX_MIME_E_OPTION_HAS_NO_VALUE
//
// MessageText:
//
//  XX_MIME_E_OPTION_HAS_NO_VALUE
//
#define XX_MIME_E_OPTION_HAS_NO_VALUE    0x800CCE30L

//
// MessageId: XX_MIME_E_INVALID_CHARSET_TYPE
//
// MessageText:
//
//  XX_MIME_E_INVALID_CHARSET_TYPE
//
#define XX_MIME_E_INVALID_CHARSET_TYPE   0x800CCE31L

//
// MessageId: XX_MIME_E_VARTYPE_NO_CONVERT
//
// MessageText:
//
//  XX_MIME_E_VARTYPE_NO_CONVERT
//
#define XX_MIME_E_VARTYPE_NO_CONVERT     0x800CCE32L

//
// MessageId: XX_MIME_E_INVALID_VARTYPE
//
// MessageText:
//
//  XX_MIME_E_INVALID_VARTYPE
//
#define XX_MIME_E_INVALID_VARTYPE        0x800CCE33L

//
// MessageId: XX_MIME_E_NO_MORE_ADDRESS_TYPES
//
// MessageText:
//
//  XX_MIME_E_NO_MORE_ADDRESS_TYPES
//
#define XX_MIME_E_NO_MORE_ADDRESS_TYPES  0x800CCE34L

//
// MessageId: XX_MIME_E_INVALID_ENCODING_TYPE
//
// MessageText:
//
//  XX_MIME_E_INVALID_ENCODING_TYPE
//
#define XX_MIME_E_INVALID_ENCODING_TYPE  0x800CCE35L

//
// MessageId: XX_MIME_S_ILLEGAL_LINES_FOUND
//
// MessageText:
//
//  XX_MIME_S_ILLEGAL_LINES_FOUND
//
#define XX_MIME_S_ILLEGAL_LINES_FOUND    0x800CCE36L

//
// MessageId: XX_MIME_S_MIME_VERSION
//
// MessageText:
//
//  XX_MIME_S_MIME_VERSION
//
#define XX_MIME_S_MIME_VERSION           0x800CCE37L

//
// MessageId: XX_MIME_E_INVALID_TEXT_TYPE
//
// MessageText:
//
//  XX_MIME_E_INVALID_TEXT_TYPE
//
#define XX_MIME_E_INVALID_TEXT_TYPE      0x800CCE38L

//
// MessageId: XX_MIME_E_READ_ONLY
//
// MessageText:
//
//  XX_MIME_E_READ_ONLY
//
#define XX_MIME_E_READ_ONLY              0x800CCE39L

//
// MessageId: XX_MIME_S_INVALID_MESSAGE
//
// MessageText:
//
//  XX_MIME_S_INVALID_MESSAGE
//
#define XX_MIME_S_INVALID_MESSAGE        0x800CCE3AL

//
// MessageId: XX_MIME_S_CHARSET_CONFLICT
//
// MessageText:
//
//  XX_MIME_S_CHARSET_CONFLICT
//
#define XX_MIME_S_CHARSET_CONFLICT       0x800CCE3BL

//
// MessageId: XX_MIME_E_UNSUPPORTED_VARTYPE
//
// MessageText:
//
//  XX_MIME_E_UNSUPPORTED_VARTYPE
//
#define XX_MIME_E_UNSUPPORTED_VARTYPE    0x800CCE3CL

//
// MessageId: XX_MIME_S_NO_CHARSET_CONVERT
//
// MessageText:
//
//  XX_MIME_S_NO_CHARSET_CONVERT
//
#define XX_MIME_S_NO_CHARSET_CONVERT     0x800CCE3DL

//
// MessageId: XX_MIME_E_URL_NOTFOUND
//
// MessageText:
//
//  XX_MIME_E_URL_NOTFOUND
//
#define XX_MIME_E_URL_NOTFOUND           0x800CCE3EL

//
// MessageId: XX_MIME_E_SECURITY_NOTINIT
//
// MessageText:
//
//  XX_MIME_E_SECURITY_NOTINIT
//
#define XX_MIME_E_SECURITY_NOTINIT       0x800CCEA0L

//
// MessageId: XX_MIME_E_SECURITY_LOADCRYPT32
//
// MessageText:
//
//  XX_MIME_E_SECURITY_LOADCRYPT32
//
#define XX_MIME_E_SECURITY_LOADCRYPT32   0x800CCEA1L

//
// MessageId: XX_MIME_E_SECURITY_BADPROCADDR
//
// MessageText:
//
//  XX_MIME_E_SECURITY_BADPROCADDR
//
#define XX_MIME_E_SECURITY_BADPROCADDR   0x800CCEA2L

//
// MessageId: XX_MIME_E_SECURITY_NODEFAULT
//
// MessageText:
//
//  XX_MIME_E_SECURITY_NODEFAULT
//
#define XX_MIME_E_SECURITY_NODEFAULT     0x800CCEB0L

//
// MessageId: XX_MIME_E_SECURITY_NOOP
//
// MessageText:
//
//  XX_MIME_E_SECURITY_NOOP
//
#define XX_MIME_E_SECURITY_NOOP          0x800CCEB1L

//
// MessageId: XX_MIME_S_SECURITY_NONE
//
// MessageText:
//
//  XX_MIME_S_SECURITY_NONE
//
#define XX_MIME_S_SECURITY_NONE          0x800CCEB2L

//
// MessageId: XX_MIME_S_SECURITY_ERROROCCURED
//
// MessageText:
//
//  XX_MIME_S_SECURITY_ERROROCCURED
//
#define XX_MIME_S_SECURITY_ERROROCCURED  0x800CCEB3L

//
// MessageId: XX_MIME_E_SECURITY_USERCHOICE
//
// MessageText:
//
//  XX_MIME_E_SECURITY_USERCHOICE
//
#define XX_MIME_E_SECURITY_USERCHOICE    0x800CCEB4L

//
// MessageId: XX_MIME_E_SECURITY_UNKMSGTYPE
//
// MessageText:
//
//  XX_MIME_E_SECURITY_UNKMSGTYPE
//
#define XX_MIME_E_SECURITY_UNKMSGTYPE    0x800CCEB5L

//
// MessageId: XX_MIME_E_SECURITY_BADMESSAGE
//
// MessageText:
//
//  XX_MIME_E_SECURITY_BADMESSAGE
//
#define XX_MIME_E_SECURITY_BADMESSAGE    0x800CCEB6L

//
// MessageId: XX_MIME_E_SECURITY_BADCONTENT
//
// MessageText:
//
//  XX_MIME_E_SECURITY_BADCONTENT
//
#define XX_MIME_E_SECURITY_BADCONTENT    0x800CCEB7L

//
// MessageId: XX_MIME_E_SECURITY_BADSECURETYPE
//
// MessageText:
//
//  XX_MIME_E_SECURITY_BADSECURETYPE
//
#define XX_MIME_E_SECURITY_BADSECURETYPE 0x800CCEB8L

//
// MessageId: XX_MIME_E_SECURITY_CLASSNOTSUPPORTED
//
// MessageText:
//
//  XX_MIME_E_SECURITY_CLASSNOTSUPPORTED
//
#define XX_MIME_E_SECURITY_CLASSNOTSUPPORTED 0x800CCEB9L

//
// MessageId: XX_MIME_S_SECURITY_RECURSEONLY
//
// MessageText:
//
//  XX_MIME_S_SECURITY_RECURSEONLY
//
#define XX_MIME_S_SECURITY_RECURSEONLY   0x800CCEBAL

//
// MessageId: XX_MIME_E_SECURITY_BADSTORE
//
// MessageText:
//
//  XX_MIME_E_SECURITY_BADSTORE
//
#define XX_MIME_E_SECURITY_BADSTORE      0x800CCED0L

//
// MessageId: XX_MIME_E_SECURITY_NOCERT
//
// MessageText:
//
//  XX_MIME_E_SECURITY_NOCERT
//
#define XX_MIME_E_SECURITY_NOCERT        0x800CCED1L

//
// MessageId: XX_MIME_E_SECURITY_CERTERROR
//
// MessageText:
//
//  XX_MIME_E_SECURITY_CERTERROR
//
#define XX_MIME_E_SECURITY_CERTERROR     0x800CCED2L

//
// MessageId: XX_MIME_S_SECURITY_NODEFCERT
//
// MessageText:
//
//  XX_MIME_S_SECURITY_NODEFCERT
//
#define XX_MIME_S_SECURITY_NODEFCERT     0x800CCED3L

//
// MessageId: XX_MIME_E_SECURITY_BADSIGNATURE
//
// MessageText:
//
//  XX_MIME_E_SECURITY_BADSIGNATURE
//
#define XX_MIME_E_SECURITY_BADSIGNATURE  0x800CCEE0L

//
// MessageId: XX_MIME_E_SECURITY_MULTSIGNERS
//
// MessageText:
//
//  XX_MIME_E_SECURITY_MULTSIGNERS
//
#define XX_MIME_E_SECURITY_MULTSIGNERS   0x800CCEE1L

//
// MessageId: XX_MIME_E_SECURITY_NOSIGNINGCERT
//
// MessageText:
//
//  XX_MIME_E_SECURITY_NOSIGNINGCERT
//
#define XX_MIME_E_SECURITY_NOSIGNINGCERT 0x800CCEE2L

//
// MessageId: XX_MIME_E_SECURITY_CANTDECRYPT
//
// MessageText:
//
//  XX_MIME_E_SECURITY_CANTDECRYPT
//
#define XX_MIME_E_SECURITY_CANTDECRYPT   0x800CCEF0L

//
// MessageId: XX_MIME_E_SECURITY_ENCRYPTNOSENDERCERT
//
// MessageText:
//
//  XX_MIME_E_SECURITY_ENCRYPTNOSENDERCERT
//
#define XX_MIME_E_SECURITY_ENCRYPTNOSENDERCERT 0x800CCEF1L

//
// MessageId: XX_MIME_S_SECURITY_NOSENDERCERT
//
// MessageText:
//
//  XX_MIME_S_SECURITY_NOSENDERCERT
//
#define XX_MIME_S_SECURITY_NOSENDERCERT  0x800CCEF2L

