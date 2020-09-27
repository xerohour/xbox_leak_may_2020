//-----------------------------------------------------------------------------
// Xbox bug# 7775
// VC7 loop unroll optimization bug
//-----------------------------------------------------------------------------
#ifdef _XBOX
#include <xtl.h>
#else
#include <d3d8.h>
#include <d3dx8.h>
#endif

//-----------------------------------------------------------------------------
// Name: Tree
// Desc: Simple structure to hold data for rendering a tree
//-----------------------------------------------------------------------------
struct Tree
{
    D3DXVECTOR4 vScale;
    D3DXVECTOR4 vPosition;
    D3DXVECTOR4 vColor;
};


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define NUM_TREES 2300


// Simple function to define "hilliness" for terrain
inline FLOAT HeightField( FLOAT x, FLOAT y )
{
    return 3*(cosf(x/20+0.2f)*cosf(y/15-0.2f)+1.0f);
}

inline FLOAT GaussianRand( FLOAT min, FLOAT max, FLOAT width )
{
    FLOAT x = (FLOAT)(rand()-rand())/RAND_MAX;
    return (max-min)*expf( -width*x*x) + min;
}


//-----------------------------------------------------------------------------
// Name: class CXboxBug
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXboxBug
{
    Tree*              m_Trees;              // Array of tree info
    Tree**             m_pTreeArray;         // Array of tree info
public:
    HRESULT Render();

    CXboxBug();
};




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXboxBug xbApp;
    xbApp.Render();
#ifdef _XBOX
    XLaunchNewImage(0, 0);
#endif
}




//-----------------------------------------------------------------------------
// Name: CXboxBug()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CXboxBug::CXboxBug()
{
    // Initialize the tree data
    m_Trees      = new Tree[NUM_TREES];
    m_pTreeArray = new Tree*[NUM_TREES];

    DWORD side = (DWORD)sqrtf( NUM_TREES );

    for( DWORD i=0; i<NUM_TREES; i++ )
    {
        // Position the trees
        FLOAT x = 60.0f * ( (FLOAT)(i%side) - (FLOAT)(side/2) ) / (FLOAT)(side/2);
        FLOAT z = 60.0f * ( (FLOAT)(i/side) - (FLOAT)(side/2) ) / (FLOAT)(side/2);

        x += 60.0f * 0.1f * ((FLOAT)(rand()-rand())/RAND_MAX) / (FLOAT)(side/2);
        z += 60.0f * 0.1f * ((FLOAT)(rand()-rand())/RAND_MAX) / (FLOAT)(side/2);
        
        FLOAT y = HeightField( x, z );

        // Size the trees randomly
        FLOAT fWidth  = GaussianRand( 5.0f, 2.0f, 2.0f );
        FLOAT fHeight = GaussianRand( 7.0f, 4.0f, 2.0f );

        // Each tree is a random color between red and green
        FLOAT r = (1.0f-0.75f) + (0.75f*rand())/RAND_MAX;
        FLOAT g = (1.0f-0.75f) + (0.75f*rand())/RAND_MAX;
        FLOAT b = 0.0f;

        m_Trees[i].vPosition      = D3DXVECTOR4( x, y, z, 1.0f );
        m_Trees[i].vColor         = D3DXVECTOR4( r, g, b, 1.0f );
        m_Trees[i].vScale         = D3DXVECTOR4( fWidth, fHeight, 1.0f, 1.0f );

        m_pTreeArray[i] = &m_Trees[i];
    }

	// bug 7775 repro code
	{
		Tree*		pMaxXTree = m_Trees;
		Tree*		pTree     = m_Trees;

		for (int n=0; n<NUM_TREES; n++, pTree++)
		{
			if ((pTree->vPosition.x > pMaxXTree->vPosition.x))
			{
				pMaxXTree = pTree;
			}
		}

		pMaxXTree->vScale.y *= 10;
	}
	// end repro code
}


//-----------------------------------------------------------------------------
// Name: App_Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXboxBug::Render()
{
#ifdef UNICODE
	OutputDebugStringW(L"CXboxBug::Render");
#else
	OutputDebugString("CXboxBug::Render");
#endif
	return S_OK;
}

