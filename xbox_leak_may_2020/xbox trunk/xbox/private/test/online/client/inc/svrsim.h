/*----------------------------------------------------------------------------
 *  Copyright © 1997 Microsoft.  ALL RIGHTS RESERVED. 
 *----------------------------------------------------------------------------
 *  Date: 10/24/00
 *  Developer: Sean Wohlgemuth
 *----------------------------------------------------------------------------
 */

#define MAX_RESPONSE_BUFFER 2048

class svrsim{

public:
	//constructor
	svrsim();
	//destructor
	~svrsim();

public:
	//set response from buffer; rgchBuffer is data; dwcBuffer is number of buffer bytes to send
	//NOTE: rgchBuffer can contain \0 values
	HRESULT SetResponse(const char* rgchBuffer, DWORD dwcBuffer);
	//set response from a file
	HRESULT SetResponse(const char* pszFilename);
	//listen for incoming request and send response after first \r\n
	HRESULT Listen();
	//stop listen
	HRESULT Stop();

private:
	//response buffer
	char m_rgchResponse[MAX_RESPONSE_BUFFER];
	//buffer response size
	DWORD m_dwcResponse;
	//protection for response buffer
	CRITICAL_SECTION m_cs;
	//stop listening event
	HANDLE m_shutdown;
	//singleton
	bool m_fListening;
	//network initialized
	bool m_fNetInit;

private:
	//process an incoming socket
	HRESULT ProcessClient(SOCKET* ps);

};//endclass