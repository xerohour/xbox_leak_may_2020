/******************************************************
** polyobject.h
** 
** A basic polygon object capable of rendering itself
**
** 03/09/2001
** by James N. Helm
******************************************************/

#ifndef _POLYOBJECT_H_
#define _POLYOBJECT_H_

class CPolyObject
{
public:
    // Constructors and Destructors
    CPolyObject();
    ~CPolyObject();

    // Public Properties
    
    // Public Methods
    void Initialize( IDirect3DDevice8* pD3DDevice );    // Setup our Vertex Buffer and Poly Points
    void Render( IDirect3DDevice8* pD3DDevice );        // Render the poly to the backbuffer

private:
    // Private Properties
    IDirect3DVertexBuffer8* m_pVertexBuffer;            // Vertex buffer for our Polygon

    // Private Methods
    void CleanUp();                                     // Clean up any memory we have allocated
};

#endif // _POLYOBJECT_H_