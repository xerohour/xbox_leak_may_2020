
#ifndef _CSHAPESFVF_H_
#define _CSHAPESFVF_H_

#include "dxlegacy.h"

typedef struct 
{
    struct {
        union { 
            float tu; 
            float dvTU; 
        }; 
        union { 
            float tv; 
            float dvTV; 
        }; 
    } Pair[D3DDP_MAXTEXCOORD];
} TEXCOORDS, *LPTEXCOORDS;

class CShapesFVF : public CShapes
{
    public:
    
    // FVF strides

    D3DDRAWPRIMITIVESTRIDEDDATA m_StrideData;
    TEXCOORDS   *m_TexCoords;
    DWORD       m_FVFTypes;
    
    CShapesFVF();
    ~CShapesFVF();

    public:

    // Member functions for conversion to FVF format

    BOOL ConvertToFVF(DWORD dwFlags);
    void ScaleCoords(DWORD dwCoord, float uScale, float vScale);
    void OffsetCoords(DWORD dwCoord, float uOffset, float vOffset);
};

#endif
