//                                                                             //
// (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved. //
//                                                                             //
////////////////////////////////// Direct 3D ////////////////////////////////////
//                                                                             //
// Module: nvILHash.h                                                          //
//   hash function for the dynamically compiled inner loops                    //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
// History:                                                                    //
//       Paul van der Kouwe 12/13/99 - created.                                //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#ifndef _NVILHASH_H_
#define _NVILHASH_H_

/////////////////////////////////////////////////////////////////////////////////
// structures
//
class CILHashEntry
{
private:
    DWORD         m_dwRenderClass;  // the class for which this loop was built
    DWORD         m_dwOffset;
    CILHashEntry *m_pNext;

    // common parameters
    DWORD m_dwVertexType;
    union
    {
        // NV4 unique parameters
        struct
        {
            DWORD        m_dwFLUI;
            DWORD        m_dwTextureOrder;
        };
        // celsius unique parameters
        struct
        {
            DWORD        m_dwFlags;
            DWORD        m_dwSTILFlags;
            DWORD        m_dwVertexStride;
            VASOURCEINFO m_vaSourceData[NV_CAPS_MAX_STREAMS];  // sources of the data loaded into the 16 vertex attribute registers
            DWORD        m_dwStateFlags;
            DWORD        m_dwTexUnitToTexStageMapping[2];
            DWORD        m_dwTexCoordIndices;
            DWORD        m_dwTexTransformFlags[4];
        };
    };

public:
    inline DWORD           getOffset (void)   const { return m_dwOffset; }
    inline CILHashEntry*   getNext   (void)   const { return m_pNext;    }

    // NV4 constuctor
    inline CILHashEntry
    (
        DWORD         dwFLUI,
        DWORD         dwVertexType,
        DWORD         dwTextureOrder,
        DWORD         dwOffset,
        CILHashEntry* pNext
    )
    {
        m_dwRenderClass  = NVCLASS_FAMILY_DXTRI;
        m_dwFLUI         = dwFLUI;
        m_dwVertexType   = dwVertexType;
        m_dwTextureOrder = dwTextureOrder;

        m_dwOffset       = dwOffset;
        m_pNext          = pNext;
    }

    inline BOOL match (DWORD dwFLUI, DWORD dwVertexType, DWORD dwTextureOrder) const
    {
        return (m_dwRenderClass  == NVCLASS_FAMILY_DXTRI)
            && (m_dwFLUI         == dwFLUI)
            && (m_dwVertexType   == dwVertexType)
            && (m_dwTextureOrder == dwTextureOrder);
    }

    // celsius/kelvin constructor
    inline CILHashEntry
    (
        PNVD3DCONTEXT   pContext,
        DWORD           dwClass,
        DWORD           dwFlags,
        DWORD           dwOffset,
        CILHashEntry*   pNext
    )
    {
        m_dwRenderClass = dwClass;

#if (NVARCH >= 0x020)
        if (dwClass == NVCLASS_FAMILY_KELVIN) {
            m_dwFlags           = dwFlags;
            m_dwVertexStride    = pContext->pCurrentVShader->getStride();
            m_dwSTILFlags       = pContext->hwState.SuperTri.dwSTILFlags;
            m_dwStateFlags      = pContext->hwState.dwStateFlags & KELVIN_MASK_INNERLOOPUNIQUENESS;
            m_dwTexCoordIndices = pContext->hwState.dwTexCoordIndices;
            nvMemCopy (m_dwTexUnitToTexStageMapping, pContext->hwState.dwTexUnitToTexStageMapping, sizeof(m_dwTexUnitToTexStageMapping));
            nvMemCopy (m_vaSourceData, pContext->pCurrentVShader->m_vaSourceData, sizeof(m_vaSourceData));
        }
        else
#endif
#if (NVARCH >= 0x010)
        if (dwClass == NVCLASS_FAMILY_CELSIUS) {
            m_dwFlags           = dwFlags;
            m_dwSTILFlags       = pContext->hwState.SuperTri.dwSTILFlags;
            m_dwVertexStride    = pContext->pCurrentVShader->getStride();
            m_dwStateFlags      = pContext->hwState.dwStateFlags & CELSIUS_MASK_INNERLOOPUNIQUENESS;
            m_dwTexCoordIndices = pContext->hwState.dwTexCoordIndices;
            nvMemCopy (m_dwTexUnitToTexStageMapping,pContext->hwState.dwTexUnitToTexStageMapping,sizeof(m_dwTexUnitToTexStageMapping));
            nvMemCopy (m_vaSourceData, pContext->pCurrentVShader->m_vaSourceData, sizeof(m_vaSourceData));
            if (m_dwStateFlags & (CELSIUS_FLAG_TEXMATRIXSWFIX(0) | CELSIUS_FLAG_TEXMATRIXSWFIX(1))) {
                m_dwTexTransformFlags[0] = pContext->tssState[0].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS];
                m_dwTexTransformFlags[1] = pContext->tssState[1].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS];
                m_dwTexTransformFlags[2] = pContext->tssState[2].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS];
                m_dwTexTransformFlags[3] = pContext->tssState[3].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS];
            }
        }
        else
#endif  // NVARCH >= 0x010
        {
            DPF ("unknown class in CILHashEntry::CILHashEntry");
            dbgD3DError();
        }

        m_dwOffset       = dwOffset;
        m_pNext          = pNext;
    }

    // celsius / kelvin match function
    inline BOOL match
    (
        PNVD3DCONTEXT pContext,
        DWORD         dwClass,
        DWORD         dwFlags
    )
    const
    {
#if (NVARCH >= 0x020)
        if (m_dwRenderClass == NVCLASS_FAMILY_KELVIN) {
            return ((m_dwFlags           == dwFlags)
                 && (m_dwSTILFlags       == pContext->hwState.SuperTri.dwSTILFlags)
                 && (m_dwVertexStride    == pContext->pCurrentVShader->getStride())
                 && (m_dwStateFlags      == (pContext->hwState.dwStateFlags & KELVIN_MASK_INNERLOOPUNIQUENESS))
                 && (m_dwTexCoordIndices == pContext->hwState.dwTexCoordIndices)
                 && !memcmp(m_dwTexUnitToTexStageMapping, pContext->hwState.dwTexUnitToTexStageMapping, sizeof(m_dwTexUnitToTexStageMapping))
                 && !memcmp(m_vaSourceData, pContext->pCurrentVShader->m_vaSourceData, sizeof(m_vaSourceData)));
        }
        else
#endif
#if (NVARCH >= 0x010)
        if (m_dwRenderClass == NVCLASS_FAMILY_CELSIUS) {
#if 0       // SK - will re-enable this in R7 after some additional work
            // SK - todo: Simplify this further.
            if ((pContext->dwEarlyCopyStrategy) && (dwFlags & CELSIUS_ILMASK_LOOPTYPE) == CELSIUS_ILFLAG_SUPER_TRI_LIST) 
            {
                if (!(pContext->hwState.SuperTri.dwSTILFlags & NV_STILFLAG_STDEFVB)) 
                {
                    return (m_dwSTILFlags == pContext->hwState.SuperTri.dwSTILFlags);                
                }
                else
                {
                    return ((m_dwSTILFlags == pContext->hwState.SuperTri.dwSTILFlags) 
                            && (m_dwVertexStride == pContext->hwState.SuperTri.getSTLBStride()));                
                }
            }
            else 
#endif
            {
                return ((m_dwFlags           == dwFlags)
                     && (m_dwSTILFlags       == pContext->hwState.SuperTri.dwSTILFlags)
                     && (m_dwVertexStride    == pContext->pCurrentVShader->getStride())
                     && (m_dwStateFlags      == (pContext->hwState.dwStateFlags & CELSIUS_MASK_INNERLOOPUNIQUENESS))
                     && (m_dwTexCoordIndices == pContext->hwState.dwTexCoordIndices)
                     && !memcmp(m_dwTexUnitToTexStageMapping, pContext->hwState.dwTexUnitToTexStageMapping, sizeof(m_dwTexUnitToTexStageMapping))
                     && !memcmp(m_vaSourceData, pContext->pCurrentVShader->m_vaSourceData, sizeof(m_vaSourceData))
                     && (!(m_dwStateFlags & (CELSIUS_FLAG_TEXMATRIXSWFIX(0) | CELSIUS_FLAG_TEXMATRIXSWFIX(1)))
                         ||
                         ((m_dwTexTransformFlags[0] == pContext->tssState[0].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS]) &&
                          (m_dwTexTransformFlags[1] == pContext->tssState[1].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS]) &&
                          (m_dwTexTransformFlags[2] == pContext->tssState[2].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS]) &&
                          (m_dwTexTransformFlags[3] == pContext->tssState[3].dwValue[D3DTSS_TEXTURETRANSFORMFLAGS]))));
            }
        }
        else
#endif  // NVARCH >= 0x010
        {
            DPF ("unknown class in CILHashEntry::match");
            dbgD3DError();
            return (FALSE);
        }
    }

};

#endif // _NVILHASH_H_

