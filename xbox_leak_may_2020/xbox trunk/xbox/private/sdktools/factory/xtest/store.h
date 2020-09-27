// store.h

typedef const unsigned char * LPCBYTE;

class CStore
{
	LPBYTE m_buffer;
	LPBYTE m_next;
	int m_size;
	bool m_delete;

	void Put (const void* bp, int ln); 
	void Get (void *bp, int ln);
	LPCTSTR GetStringPtr (void);

public:
	CStore (int size); // create buffer of size
	CStore (LPCBYTE buffer, int size); // marshall existing buffer
	virtual ~CStore () {if (m_delete) delete m_buffer;}

	LPBYTE GetBuffer () {return m_buffer;}
	int GetLength () {return m_next - m_buffer;}

	// insertion
	CStore& operator<<(BYTE by) {Put (&by, sizeof(BYTE)); return *this;}
	CStore& operator<<(WORD w) {Put (&w, sizeof(WORD)); return *this;}
	CStore& operator<<(LONG l) {Put (&l, sizeof(LONG)); return *this;}
	CStore& operator<<(DWORD dw) {Put (&dw, sizeof(DWORD)); return *this;}
	CStore& operator<<(float f) {Put (&f, sizeof(float)); return *this;}
	CStore& operator<<(double d) {Put (&d, sizeof(double)); return *this;}
	CStore& operator<<(int i) {Put (&i, sizeof(int)); return *this;}
	CStore& operator<<(short w) {Put (&w, sizeof(short)); return *this;}
	CStore& operator<<(char ch) {Put (&ch, sizeof(char)); return *this;}
	CStore& operator<<(unsigned u) {Put (&u, sizeof(unsigned)); return *this;}
	CStore& operator<<(LPCTSTR& s) 
		{Put (s, (_tcslen(s) + 1)*sizeof(_TCHAR)); return *this;}

	// extraction
	CStore& operator>>(BYTE& by) {Get (&by, sizeof(BYTE)); return *this;}
	CStore& operator>>(WORD& w) {Get (&w, sizeof(WORD)); return *this;}
	CStore& operator>>(DWORD& dw) {Get (&dw, sizeof(DWORD)); return *this;}
	CStore& operator>>(LONG& l) {Get (&l, sizeof(LONG)); return *this;}
	CStore& operator>>(float& f) {Get (&f, sizeof(float)); return *this;}
	CStore& operator>>(double& d) {Get (&d, sizeof(double)); return *this;}
	CStore& operator>>(int& i) {Get (&i, sizeof(int)); return *this;}
	CStore& operator>>(short& w) {Get (&w, sizeof(short)); return *this;}
	CStore& operator>>(char& ch) {Get (&ch, sizeof(char)); return *this;}
	CStore& operator>>(unsigned& u) {Get (&u, sizeof(unsigned)); return *this;}

	// Use this version if you want the data copied to your own buffer
	// this actually shouldn't be allowed because there is no way 
	// to detect or prevent buffer overruns.
	CStore& operator>>(LPTSTR& s) 
		{Get ((void*)s, (_tcslen(s) + 1)*sizeof(_TCHAR)); return *this;}

	// Use this version if you just want a pointer to the data
	CStore& operator>>(LPCTSTR* s) {*s = GetStringPtr (); return *this;}
};
