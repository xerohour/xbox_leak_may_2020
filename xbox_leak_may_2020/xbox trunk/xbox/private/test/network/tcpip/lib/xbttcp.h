
//
// Global variables
//


//
// Export functions in the LIB file
//
INT xbtUCastPing(
    CHAR *TESTADDR,
    UINT *Offset,
    CHAR *mydatabuf,
    UINT datalength,
    CHAR *ResponseIP,
    CHAR *mypingmessage, 
    UINT mypingmessagelength
    );

INT xbtMCastPing(
    CHAR *TESTADDR, 
    UINT *Offset, 
    CHAR *mydatabuf, 
    UINT datalength, 
    CHAR *ResponseIP,  
    CHAR *mypingmessage, 
    UINT mypingmessagelength
    );
