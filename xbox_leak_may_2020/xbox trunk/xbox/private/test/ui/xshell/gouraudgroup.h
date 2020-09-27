#ifndef _GOURAUDGROUP_H_
#define _GOURAUDGROUP_H_

#include "gouraudobject.h"

class GouraudGroup
{

public:
    // Constructors and Destructors
    GouraudGroup( void );
	~GouraudGroup( void );

    // Public Properties
	DWORD		    m_dwTotalObjects;
	GouraudObject*  m_pObjects;

    // Public Methods
	void render( IDirect3DDevice8* pD3DDevice );
	void renderStrip( IDirect3DDevice8* pD3DDevice );
	void loadMDL( char* pszFilename, IDirect3DDevice8* pD3DDevice );
	void loadStrippedMDL( char* pszFilename, IDirect3DDevice8* pD3DDevice );
};

#endif // _GOURAUDGROUP_H_