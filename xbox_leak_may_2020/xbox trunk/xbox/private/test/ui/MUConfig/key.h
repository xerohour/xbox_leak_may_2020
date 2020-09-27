#ifndef _KEY_H
#define _KEY_H

class CKey
{
public:
	int m_iXOrigin, m_iYOrigin;		// x,y coordinate of the key's upper left pixel
	int m_iWidth, m_iHeight;		// width and height of the key
	DWORD m_dwSelectColor;			// color to hilight the key with upon selection
	WCHAR* m_pwszResultChar;        // value attached to the key
	BOOL m_fRender;				    // should the key be rendered?

	// Constructors and Destructors
	CKey();
	~CKey();

	void define( int iX, int iY, int iW, int iH, DWORD dwColor );
	void defineText( WCHAR* pwszResult );
	void setRender( BOOL fValue );
	BOOL getRender( void );
};

#endif // _KEY_H