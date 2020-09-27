class CBundler
{
public:
	CBundler();
	bool Init();
    void ErrorMsg( const char * strFmt, ... );
    HRESULT WriteData( void * pbBuff, DWORD cb );
    HRESULT PadToAlignment( DWORD dwAlign );

	HANDLE m_hFile;
    LPDIRECT3D8 m_pd3d;
    LPDIRECT3DDEVICE8 m_pd3ddev;
};
