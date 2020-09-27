#define VERSION 	    4
#define SERVER_READ_PIPE    _T("\\\\%s\\PIPE\\%sIN")   //Client Writes and Server Reads
#define SERVER_WRITE_PIPE   _T("\\\\%s\\PIPE\\%sOUT")  //Server Reads  and Client Writes

#define MAGICNUMBER     0x31109000

typedef struct
{
    DWORD    Size;
    DWORD    Version;
    char     ClientName[15];
    DWORD    LinesToSend;
    DWORD    Flag;
}   SESSION_STARTUPINFO;

typedef struct
{
    DWORD MagicNumber;      //New Remote
    DWORD Size;             //Size of structure
    DWORD FileSize;         //Num bytes sent
}   SESSION_STARTREPLY;

