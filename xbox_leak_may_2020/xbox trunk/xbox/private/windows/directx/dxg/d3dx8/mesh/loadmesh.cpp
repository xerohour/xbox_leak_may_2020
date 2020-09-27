/*//////////////////////////////////////////////////////////////////////////////
//
// File: loadmesh.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// @@BEGIN_MSINTERNAL
//
// History:
// -@-          (craigp)    - created
// -@- 08/06/99 (mikemarr)  - prepend GX to all Luciform functions
//                          - started comment history
// -@- 08/26/99 (mikemarr)  - replace gxbasetype.h with gxmathcore.h
// -@- 09/23/99 (mikemarr)  - changed <> to "" in #includes
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/

#include "pchmesh.h"

#include "loadutil.h"

#if 0
/*
 * Template routine to handle dynamic allocation, based on Heap* Win32 APIs.
 *
 *  T **ppBase: base of array.
 *  const T& obj: object to add to end of array.
 *  UINT *pcNum: number of objects in array.
 *  UINT *pcMax: max. number of objects in array.
 *
 * ppBase, pcNum and pcMax point to values that usually get modified
 * by the call. The only case when they are not updated is if there
 * is insufficient memory for the initial allocation (if *pBase is NULL)
 * or a second allocation if *pcNum==*pcMax.
 *
 * Return value: TRUE if allocation was successful
 *               FALSE if there was insufficient memory
 *      FALSE means nothing was added to the array, but nothing
 *        was lost either. No weird realloc semantics allowed!
 */
template<class T>
BOOL
AddToDynamicArray( T **ppBase, const T& obj, UINT *pcNum, UINT *pcMax )
{
    T *pBase = *ppBase;
    if ( ! pBase )
    {
        HeapValidate( GetProcessHeap(), 0, NULL );
        pBase = new T[2];
        *pcNum = 0;
        *pcMax = 2;
    }
    if ( *pcNum == *pcMax )
    {
        UINT cNewMax = *pcMax*2;
        T *newarr = new T[cNewMax];
        if ( ! newarr )
            return FALSE;
        for (UINT i = 0; i < *pcNum; i++)
            newarr[i] = pBase[i];
        *pcMax = cNewMax;
        delete []pBase;
        pBase = newarr;
    }
    pBase[ (*pcNum)++ ] = obj;
    *ppBase = pBase;
    return TRUE;
}


class GXColorAttributeBundle : public IGXAttributeBundle
{
public:

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
    {
        return E_FAIL;
    };

    virtual ULONG STDMETHODCALLTYPE AddRef( void) { return 1; }

    virtual ULONG STDMETHODCALLTYPE Release( void) { return 1; };

    // punch attribute set to device
    virtual HRESULT SetAttributesToDevice( LPDIRECT3DDEVICE8 pD3DDevice )
    {
        return pD3DDevice->SetMaterial(&m_mtrlColor);
    }

    // user defined data
    virtual HRESULT SetUserData( PVOID pvUserData ) { return E_NOTIMPL; };
    virtual HRESULT GetUserData( PVOID *ppvUserData ) { return E_NOTIMPL; };

    // user defined sort index for preferred order to draw attribute bundle sets in
    virtual HRESULT SetSortIndex( DWORD dwSortIndex ) { return S_OK; };
    virtual HRESULT GetSortIndex( PDWORD pdwSortIndex ) { *pdwSortIndex = 0; return S_OK;};

    GXColorAttributeBundle(D3DXCOLOR &color)
    {
        ZeroMemory( &m_mtrlColor, sizeof(D3DMATERIAL8) );
        m_mtrlColor.dcvDiffuse.r = m_mtrlColor.dcvAmbient.r = color.r;
        m_mtrlColor.dcvDiffuse.g = m_mtrlColor.dcvAmbient.g = color.g;
        m_mtrlColor.dcvDiffuse.b = m_mtrlColor.dcvAmbient.b = color.b;
        m_mtrlColor.dcvDiffuse.a = m_mtrlColor.dcvAmbient.a = 1.0f;
        m_mtrlColor.dcvEmissive.r = 0;
        m_mtrlColor.dcvEmissive.g = 0;
        m_mtrlColor.dcvEmissive.b = 0;
    }

private:
    D3DMATERIAL8 m_mtrlColor;
};
#endif

// identifier was truncated to '255' characters in the browser information
#pragma warning(disable: 4786)

const char *x_szSeparators = " \n\r\t";
const char *x_szVertex = "Vertex";
const UINT x_cchVertex = 6;
const char *x_szCorner = "Corner";
const UINT x_cchCorner = 6;
const char *x_szFace = "Face";
const UINT x_cchFace = 4;
const char *x_szOpenParen = "(";
const char *x_szCloseParen = ")";
const char *x_szOptionalArgStart = "{";
const char *x_szOptionalArgEnd = "}";
const char *x_szRGB = "rgb";
const char *x_szMatId = "matid";
const char *x_szNormal = "normal";
const char *x_szUv = "uv";
const char *x_szEquals = "=";

HRESULT
CFileContext::GetCharHelper(char *pchBuffer, bool &bEOF)
{
    HRESULT hr = S_OK;
    unsigned long cchRead;

    GXASSERT(m_ichBuffer == m_cchBuffer);

    if (m_bEndOfFile)
    {
        bEOF = true;
        goto e_Exit;
    }

    hr = m_pstream->Read(m_szBuffer, x_cbBufferSize, &cchRead);
    if (FAILED(hr) || (cchRead == 0))
    {
        if ((hr == S_FALSE) || (cchRead == 0))
        {
            m_bEndOfFile = true;
            bEOF = true;
            hr = S_OK;
        }

        goto e_Exit;
    }

    m_cchBuffer = cchRead;
    m_ichBuffer = 0;

    hr = GetChar(pchBuffer, bEOF);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    return hr;
}

HRESULT
CFileContext::GetLine(char *szBuffer, int cchBuffer, bool &bEOF)
{
    int ichBuffer;
    HRESULT hr = S_OK;
    bool bTemp;
    char chTemp;

    ichBuffer = 0;
    bEOF = false;

    hr = GetChar(&szBuffer[ichBuffer], bEOF);
        if (FAILED(hr) || bEOF)
    {
        // this is both an acceptable EOF case, and an error case, but no way to tell the user
        goto e_Exit;
    }

    while ((szBuffer[ichBuffer] != '\n') && (ichBuffer < cchBuffer - 2))
    {
        ichBuffer += 1;

        hr = GetChar(&szBuffer[ichBuffer], bTemp);
            if (FAILED(hr) || bTemp)
        {
            szBuffer[ichBuffer+1] = '\0';
            goto e_Exit;
        }

        if (szBuffer[ichBuffer] == '\r')
            ichBuffer -= 1;
    }


    szBuffer[ichBuffer+1] = '\0';

    // if we hit the limit of the string without hitting the end of the line
    //    then skip the rest of the line
    if ((ichBuffer == cchBuffer - 2) && (szBuffer[ichBuffer] != '\n'))
    {
        do
        {
            hr = GetChar(&chTemp, bTemp);
            if (FAILED(hr) || bTemp)
                goto e_Exit;
        }
        while (chTemp != '\n');
    }

e_Exit:
    return hr;
}



const UINT x_cOptionalArgsMax = 4;

class CParseOptionalArgs
{
private:
    char *m_rgszArgs[x_cOptionalArgsMax];
    UINT m_rgcchArgs[x_cOptionalArgsMax];
    char *m_rgszValues[x_cOptionalArgsMax];
    UINT m_rgcchValues[x_cOptionalArgsMax];

    UINT m_cOptionalArgs;

public:
    CParseOptionalArgs()
        :m_cOptionalArgs(0) { }

    UINT COptionalArgs() const { return m_cOptionalArgs; }

    HRESULT Parse(char *szOptionalArgs);
    bool BCompareArg(UINT iOptionalArg, const char *szPossibleArg);

    HRESULT GetPoint2(UINT iOptionalArg, D3DXVECTOR2 *puvPoint);
    HRESULT GetVector3(UINT iOptionalArg, D3DXVECTOR3 *pvVector);
    HRESULT GetCoVector3(UINT iOptionalArg, D3DXVECTOR3 *pvVector);

    HRESULT GetColorRGB(UINT iOptionalArg, D3DXCOLOR *pcolor);
    HRESULT GetDWORD(UINT iOptionalArg, DWORD *piArg);
};

char *
EatWhiteSpace(char *szBuf)
{
    while ((*szBuf == ' ') || (*szBuf == '\t'))
    {
        szBuf++;
    }

    return szBuf;
}

bool NotZero(D3DXVECTOR3 &vNormal)
{
    return (vNormal.x != 0.0f) || (vNormal.y != 0.0f) || (vNormal.z != 0.0f);
}

char *
EatNonWhiteSpace(char *szBuf, char chTerminator)
{
    while ((*szBuf != '\0') && (*szBuf != ' ') && (*szBuf != '\t') && (*szBuf != chTerminator))
    {
        szBuf++;
    }

    return szBuf;
}

char *
EatUntilTerminator(char *szBuf, char chTerminator)
{
    while ((*szBuf != '\0') && (*szBuf != chTerminator))
    {
        szBuf++;
    }

    return szBuf;
}

HRESULT
CParseOptionalArgs::Parse(char *szOptionalArgs)
{
    HRESULT hr = S_OK;
    char *szCur = szOptionalArgs;
    UINT iOptionalArg;

    // find the start of the string
    szCur = EatWhiteSpace(szCur);

    if (*szCur != '{')
    {
        hr = E_FAIL;
        goto e_Exit;
    }
    szCur++;

    while (szCur != '\0')
    {
        szCur = EatWhiteSpace(szCur);

        // if at the end of the string, then exit
        if (*szCur == '\0')
            break;

        // found the first argument
        iOptionalArg = m_cOptionalArgs;
        m_cOptionalArgs += 1;

        m_rgszArgs[iOptionalArg] = szCur;


        // now figure out how long it is
        szCur = EatNonWhiteSpace(szCur, '=');
        m_rgcchArgs[iOptionalArg] = (DWORD)(szCur - m_rgszArgs[iOptionalArg]);

        szCur = EatWhiteSpace(szCur);

        // if there is a value, find it, else go to next arg
        if (*szCur == '=')
        {
            szCur++; // skip the '='

            szCur = EatWhiteSpace(szCur);

            // had better be an argument if there was an '='
            if (*szCur == '\0')
            {
                hr = E_FAIL;
                goto e_Exit;
            }

            m_rgszValues[iOptionalArg] = szCur;

            if (*szCur == '(')
            {
                szCur = EatUntilTerminator(szCur, ')');
                if (*szCur == '\0')
                {
                    hr = E_FAIL;
                    goto e_Exit;
                }
                szCur++;  // move beyond the ')'

                m_rgcchValues[iOptionalArg] = (DWORD)(szCur - m_rgszValues[iOptionalArg]);
            }
            else
            {
                // NOTE: hitting end of string here is completely acceptable
                //   and pass in ' ' as the terminator because there is no special terminator
                szCur = EatNonWhiteSpace(szCur, ' ');

                m_rgcchValues[iOptionalArg] = (DWORD)(szCur - m_rgszValues[iOptionalArg]);
            }

        }
        else
        {
            m_rgszValues[iOptionalArg] = NULL;
            m_rgcchValues[iOptionalArg] = 0;
        }
    }

e_Exit:
    return hr;
}

bool
CParseOptionalArgs::BCompareArg(UINT iOptionalArg, const char *szPossibleArg)
{
    GXASSERT(iOptionalArg < m_cOptionalArgs);
    return strncmp(szPossibleArg, m_rgszArgs[iOptionalArg], m_rgcchArgs[iOptionalArg]) == 0;
}

HRESULT
CParseOptionalArgs::GetDWORD(UINT iOptionalArg, DWORD *piArg)
{
    GXASSERT(iOptionalArg < m_cOptionalArgs);

    *piArg = atoi(m_rgszValues[iOptionalArg]);

    return S_OK;
}

HRESULT
CParseOptionalArgs::GetColorRGB(UINT iOptionalArg, D3DXCOLOR *pcolor)
{
    GXASSERT(iOptionalArg < m_cOptionalArgs);

    HRESULT hr = S_OK;
    char *szCur;

    szCur = m_rgszValues[iOptionalArg];

    GXASSERT(*szCur == '(');
    szCur++;

    // convert the three numbers into a color
    pcolor->r = (float)strtod(szCur, &szCur);
    pcolor->g = (float)strtod(szCur, &szCur);
    pcolor->b = (float)strtod(szCur, &szCur);
    pcolor->a = 0.0f;

    return hr;
}

HRESULT
CParseOptionalArgs::GetPoint2(UINT iOptionalArg, D3DXVECTOR2 *puvPoint)
{
    GXASSERT(iOptionalArg < m_cOptionalArgs);

    HRESULT hr = S_OK;
    char *szCur;

    szCur = m_rgszValues[iOptionalArg];

    GXASSERT(*szCur == '(');
    szCur++;

    // find and convert the first number
    puvPoint->x = (float)strtod(szCur, &szCur);
    puvPoint->y = (float)strtod(szCur, &szCur);

    return hr;
}

HRESULT
CParseOptionalArgs::GetVector3(UINT iOptionalArg, D3DXVECTOR3 *pvVector)
{
    GXASSERT(iOptionalArg < m_cOptionalArgs);

    HRESULT hr = S_OK;
    char *szCur;

    szCur = m_rgszValues[iOptionalArg];

    GXASSERT(*szCur == '(');
    szCur++;

    // find and convert the first number
    pvVector->x = (float)strtod(szCur, &szCur);
    pvVector->y = (float)strtod(szCur, &szCur);
    pvVector->z = (float)strtod(szCur, &szCur);

    return hr;
}

HRESULT
CParseOptionalArgs::GetCoVector3(UINT iOptionalArg, D3DXVECTOR3 *pvVector)
{
    HRESULT hr;

    D3DXVECTOR3 vTemp;
    hr = GetVector3(iOptionalArg, &vTemp);
    if (FAILED(hr))
        return hr;

    *pvVector = vTemp;

    return S_OK;
}


HRESULT
LoadCounts(CFileContext *pfc, UINT *pcVertices, UINT *pcFaces, UINT *pcCorners)
{
    HRESULT hr = S_OK;
    char rgchBuf[20];
    char *szHeader;
    UINT cFaces;
    UINT cVertices;
    UINT cCorners;
    char *szValue;
    UINT cNewValue;
    bool bEOF;

    GXASSERT((pcVertices != NULL) && (pcFaces != NULL) && (pcCorners != NULL));

    // initialize the counters
    *pcVertices = UINT_MAX;
    *pcFaces = UINT_MAX;
    *pcCorners = UINT_MAX;


    cFaces = 0;
    cVertices = 0;
    cCorners = 0;

    // get the counts of the faces, vertices, etc
    while ( 1 )
    {
        hr = pfc->GetLine(rgchBuf, sizeof(rgchBuf), bEOF);
        if (FAILED(hr))
        {
            goto e_Exit;
        }

        if (bEOF)
        {
            break;  // EOF, break out of loop
        }

        szHeader = strtok( rgchBuf, x_szSeparators );

        // if the token is NULL, go onto the next line
        if (szHeader != NULL)
        {
            // add the line to the proper count if it fits the profile
            if (strcmp(szHeader, x_szVertex) == 0)
            {
                szValue = strtok( NULL, x_szSeparators );
                if (szValue == NULL)
                {
                    hr = E_FAIL;
                    goto e_Exit;
                }

                cNewValue = atol(szValue);
                if (cVertices < cNewValue)
                    cVertices = cNewValue;
            }
            else if (strcmp(szHeader, x_szFace) == 0)
            {
                szValue = strtok( NULL, x_szSeparators );
                if (szValue == NULL)
                {
                    hr = E_FAIL;
                    goto e_Exit;
                }

                cNewValue = atol(szValue);
                if (cFaces < cNewValue)
                    cFaces = cNewValue;
            }
            else if (strcmp(szHeader, x_szCorner) == 0)
            {
                cCorners += 1;
            }

        }
    }

    *pcFaces = cFaces;
    *pcVertices = cVertices;
    *pcCorners = cCorners;

    // rewind
    hr = pfc->Rewind();
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    return hr;
}


HRESULT
LoadVertex(char *szLineBuf, SLoadVertex *rglvVertices, UINT cVertices, bool &bNormalFound)
{
    HRESULT hr = S_OK;
    char *szToken;
    char *szPointId;
    char *szPointX;
    char *szPointY;
    char *szPointZ;
    char *szHeader;
    UINT ulPointId;
    UINT iOptionalArg;


    // the header should always be present, checked before calling this function
    szHeader = strtok( szLineBuf, x_szSeparators );
    GXASSERT((szHeader != NULL) && strcmp(szHeader, x_szVertex) == 0);

    // get the number for the count
    szPointId = strtok( NULL, x_szSeparators );
    if (szPointId == NULL)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // load the various parts of the vertex from the strings
    szPointX = strtok( NULL, x_szSeparators );
    if (szPointX == NULL)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    szPointY = strtok( NULL, x_szSeparators );
    if (szPointY == NULL)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    szPointZ = strtok( NULL, x_szSeparators );
    if (szPointZ == NULL)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    ulPointId = atoi(szPointId);
    if ((ulPointId == 0) || (ulPointId > cVertices))
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // convert from one based to zero based
    ulPointId -= 1;

    // UNDONE - handle errors by not using atof
    rglvVertices[ulPointId].m_vPos = D3DXVECTOR3((float)atof(szPointX), (float)atof(szPointY), (float)atof(szPointZ));

    // look optional information
    szToken = strtok( NULL, "}" );
    if (szToken != NULL)
    {
        CParseOptionalArgs ParseArgs;

        // parse the optional information, into an wonder of all wonders, a readable form
        hr = ParseArgs.Parse(szToken);
        if (FAILED(hr))
        {
            goto e_Exit;
        }

        // loop over all the args, getting the info for the understood items
        for (iOptionalArg = 0; iOptionalArg < ParseArgs.COptionalArgs(); iOptionalArg++)
        {
            if (ParseArgs.BCompareArg(iOptionalArg, x_szNormal))
            {
                hr = ParseArgs.GetCoVector3(iOptionalArg, &rglvVertices[ulPointId].m_vNormal);
                if (FAILED(hr))
                    goto e_Exit;

                bNormalFound = NotZero(rglvVertices[ulPointId].m_vNormal);
            }
            else if (ParseArgs.BCompareArg(iOptionalArg, x_szUv))
            {
                hr = ParseArgs.GetPoint2(iOptionalArg, &rglvVertices[ulPointId].m_uvTex1);
                if (FAILED(hr))
                    goto e_Exit;
            }
        }
    }

e_Exit:
    return hr;
}

HRESULT
LoadFace(char *szLineBuf, UINT cFacesMax, SLoadedFace *rgFaces)
{
    HRESULT hr = S_OK;
    char *szToken;
    char *szFaceId;
    char *szPoint1;
    char *szPoint2;
    char *szPoint3;
    char *szHeader;
    UINT ulFaceId;
    UINT iOptionalArg;

    szHeader = strtok( szLineBuf, x_szSeparators );
    GXASSERT((szHeader != NULL) && strcmp(szHeader, x_szFace) == 0);


    // get the number for the count
    szFaceId = strtok( NULL, x_szSeparators );
    if (szFaceId == NULL)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    ulFaceId = atoi(szFaceId) - 1;

    // if the face id is too high, and/or the face is already in use (specified twice in file)
    if ((ulFaceId >= cFacesMax) || (rgFaces[ulFaceId].m_wIndices[0] != UNUSED32))
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    // load the various parts of the vertex from the strings
    szPoint1 = strtok( NULL, x_szSeparators );
    if (szPoint1 == NULL)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    szPoint2 = strtok( NULL, x_szSeparators );
    if (szPoint2 == NULL)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    szPoint3 = strtok( NULL, x_szSeparators );
    if (szPoint3 == NULL)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // UNDONE - handle errors by not using atoi
    // NOTE: reverse the ordering of the indices... otherwise CCW ordering becomes CW ordering
    //          when -z applied to change from right hand to left hand coordinate system
    rgFaces[ulFaceId] = SLoadedFace(atoi(szPoint1)-1, atoi(szPoint2)-1, atoi(szPoint3)-1);


    // look optional information
    szToken = strtok( NULL, "}\n" );
    if (szToken != NULL)
    {
        CParseOptionalArgs ParseArgs;

        // parse the optional information, into an wonder of all wonders, a readable form
        hr = ParseArgs.Parse(szToken);
        if (FAILED(hr))
        {
            goto e_Exit;
        }

        // loop over all the args, getting the info for the understood items
        for (iOptionalArg = 0; iOptionalArg < ParseArgs.COptionalArgs(); iOptionalArg++)
        {
            if (ParseArgs.BCompareArg(iOptionalArg, x_szMatId))
            {
                rgFaces[ulFaceId].m_bMaterialSpecified = true;

                hr = ParseArgs.GetDWORD(iOptionalArg, &rgFaces[ulFaceId].m_matid);
                if (FAILED(hr))
                    goto e_Exit;
            }
            else if (ParseArgs.BCompareArg(iOptionalArg, x_szRGB))
            {
                rgFaces[ulFaceId].m_bColorSpecified = true;

                hr = ParseArgs.GetColorRGB(iOptionalArg, &rgFaces[ulFaceId].m_colorFace);
                if (FAILED(hr))
                    goto e_Exit;
            }
        }
    }

e_Exit:

    return hr;
}

HRESULT
LoadCorner(char *szLineBuf, UINT iCorner, UINT cVertices, UINT cFaces, SCorner *rgCorners, bool &bNormalFound)
{
    HRESULT hr = S_OK;
    char *szToken;
    char *szFaceId;
    char *szPointId;
    char *szHeader;
    UINT ulFaceId;
    UINT ulPointId;
    UINT iOptionalArg;

    szHeader = strtok( szLineBuf, x_szSeparators );
    GXASSERT((szHeader != NULL) && strcmp(szHeader, x_szCorner) == 0);


    // get the number for the count
    szPointId = strtok( NULL, x_szSeparators );
    if (szPointId == NULL)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    szFaceId = strtok( NULL, x_szSeparators );
    if (szFaceId == NULL)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    ulFaceId = atoi(szFaceId) - 1;
    ulPointId = atoi(szPointId) - 1;

    if ((ulFaceId >= cFaces) || (ulPointId > cVertices))
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    rgCorners[iCorner].m_wFace = ulFaceId;
    rgCorners[iCorner].m_wPoint = ulPointId;


    // look optional information
    szToken = strtok( NULL, "}\n" );
    if (szToken != NULL)
    {
        CParseOptionalArgs ParseArgs;

        // parse the optional information, into an wonder of all wonders, a readable form
        hr = ParseArgs.Parse(szToken);
        if (FAILED(hr))
        {
            goto e_Exit;
        }

        // loop over all the args, getting the info for the understood items
        for (iOptionalArg = 0; iOptionalArg < ParseArgs.COptionalArgs(); iOptionalArg++)
        {

            if (ParseArgs.BCompareArg(iOptionalArg, x_szNormal))
            {
                rgCorners[iCorner].m_bNormalSpecified = true;

                hr = ParseArgs.GetCoVector3(iOptionalArg, &rgCorners[iCorner].m_vNormal);
                if (FAILED(hr))
                    goto e_Exit;

                bNormalFound = NotZero(rgCorners[iCorner].m_vNormal);
            }
            else if (ParseArgs.BCompareArg(iOptionalArg, x_szUv))
            {
                rgCorners[iCorner].m_bUvSpecified = true;

                hr = ParseArgs.GetPoint2(iOptionalArg, &rgCorners[iCorner].m_uvTex1);
                if (FAILED(hr))
                    goto e_Exit;
            }
        }
    }

e_Exit:

    return hr;
}

HRESULT WINAPI
LoadMeshFromM(IStream *pstream, DWORD options, DWORD fvf,
              LPDIRECT3DDEVICE8 pD3DDevice,
              LPD3DXMESH *ppMesh, LPD3DXBUFFER *ppbufAdjacency)
{
    HRESULT hr = S_OK;
    UINT cVertices;
    UINT cCorners;
    UINT cFaces;
    UINT iVert;
    bool bNormalFound = false;
    UINT iFace;

    SLoadedFace        *rglfFaces = NULL;
    SLoadVertex *rglvLoaded = NULL;
    SCorner *rgCorners = NULL;

    bool bEOF;
    UINT iCorner = 0;
    CFileContext fc(pstream);

    char rgchBuf[256];

#if 0 // color attribute bundle testing
    //IGXAttributeBundle *pattrColor;
    //IGXAttributeBundle **rgBundles = NULL;
    UINT cBundles = 0;
    UINT cBundlesMax = 0;
    D3DXCOLOR *rgColors = NULL;
    UINT cColors = 0;
    UINT cColorsMax = 0;
    UINT iColor;
    const D3DXCOLOR colorGrey(0.5f, 0.5f, 0.5f, 0.0f);
#endif

    if ((ppMesh == NULL) || (pstream == NULL))
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    hr = LoadCounts(&fc, &cVertices, &cFaces, &cCorners);
    if (FAILED(hr))
        goto e_Exit;

    if ((cVertices == 0) || (cFaces == 0))
    {
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    // allocate memory to load data into

    hr = InitVertices(rglvLoaded, cVertices);
    if (FAILED(hr))
        goto e_Exit;

    hr = InitFaces(rglfFaces, cFaces);
    if (FAILED(hr))
        goto e_Exit;

    hr = InitCorners(rgCorners, cCorners);
    if (FAILED(hr))
        goto e_Exit;

    // initialize the colors of the vertices to a reasonable default
    for (iVert = 0; iVert < cVertices; iVert++)
    {
        rglvLoaded[iVert].m_color = D3DXCOLOR(0.9f, 0.6f, 0.4f, 0.0f);
    }

    while ( 1 )
    {
        hr = fc.GetLine(rgchBuf, sizeof(rgchBuf), bEOF);
        if (FAILED(hr))
        {
            goto e_Exit;
        }

        if (bEOF)
        {
            break;  // EOF, break out of loop
        }

        // load the various face and vertices, ignore unrecognized lines

        if (strncmp(rgchBuf, x_szVertex, x_cchVertex) == 0)
        {
            hr = LoadVertex(rgchBuf, rglvLoaded, cVertices, bNormalFound);
            if (FAILED(hr))
                goto e_Exit;
        }
        else if (strncmp(rgchBuf, x_szFace, x_cchFace) == 0)
        {
            hr = LoadFace(rgchBuf, cFaces, rglfFaces);
            if (FAILED(hr))
                goto e_Exit;
        }
        else if (strncmp(rgchBuf, x_szCorner, x_cchCorner) == 0)
        {
            hr = LoadCorner(rgchBuf, iCorner, cVertices, cFaces, rgCorners, bNormalFound);
            if (FAILED(hr))
                goto e_Exit;

            iCorner++;
        }
    }

    GXASSERT(iCorner == cCorners);

    if (!bNormalFound)
    {
        for (iFace = 0; iFace < cFaces; iFace++)
        {
            if (rglfFaces[iFace].m_wIndices[0] != UNUSED32)
            {
                rglfFaces[iFace].m_bSmoothingGroupSpecified = true;
                rglfFaces[iFace].m_iSmoothingGroup = 1;
            }
        }
    }

    // test materials by generating materials for colors

#if 0
    for (iFace = 0; iFace < cFaces; iFace++)
    {
        if (rglfFaces[iFace].m_bColorSpecified)
        {
            for (iColor = 0; iColor < cColors; iColor++)
            {
                if (rgColors[iColor] == rglfFaces[iFace].m_colorFace)
                    break;
            }

            if (iColor == cColors)
            {
                if ( ! AddToDynamicArray( &rgColors, rglfFaces[iFace].m_colorFace, &cColors, &cColorsMax ) )
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
            }

#if 0
            if (iColor < cColors)
            {
                pattrColor = rgBundles[iColor];
            }
            else
            {
                if ( ! AddToDynamicArray( &rgColors, rglfFaces[iFace].m_colorFace, &cColors, &cColorsMax ) )
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }

                pattrColor = new GXColorAttributeBundle(rglfFaces[iFace].m_colorFace);
                if (pattrColor == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }

                if ( ! AddToDynamicArray( &rgBundles, pattrColor, &cBundles, &cBundlesMax ) )
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
            }
#endif

            //rglfFaces[iFace].m_pattr = pattrColor;
            rglfFaces[iFace].m_attr = iColor;
            rglfFaces[iFace].m_bAttributeSpecified = true;

            rglfFaces[iFace].m_colorFace = colorGrey;
            //rglfFaces[iFace].m_bColorSpecified = false;
        }
    }
#endif


    hr = SetMesh(rglfFaces, cFaces, rglvLoaded, cVertices, rgCorners, cCorners, NULL, FALSE,
                        options, fvf, pD3DDevice, ppMesh, ppbufAdjacency);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:

    delete []rglfFaces;
    delete []rglvLoaded;
    delete []rgCorners;

    return hr;
}
