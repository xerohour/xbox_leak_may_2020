/*//////////////////////////////////////////////////////////////////////////////
//
// File: bezier.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// @@BEGIN_MSINTERNAL
//
// History:
// -@-          (craigp)    - created 
// -@- 08/26/99 (mikemarr)  - started comment history
//                          - replace gxbasetype.h with gxmathcore.h
// -@- 09/23/99 (mikemarr)  - changed <> to "" on #includes
//
// @@END_MSINTERNAL
//
//////////////////////////////////////////////////////////////////////////////*/
#include "pchmesh.h"
#include "quadric.h"

#include "float.h"

CQuadric::CQuadric(const CQuadric &q)
{
    memcpy(m_el, q.m_el, sizeof(m_el));
}

CQuadric::CQuadric(double a, double b, double c, double d)
{
    Generate(a, b, c, d);
}

void CQuadric::Init()
{
    for(INT i = 0; i < 10; m_el[i++] = 0.0);
}

void CQuadric::Generate(double a, double b, double c, double d)
{
    m_el[0] = a * a;
    m_el[1] = a * b;
    m_el[2] = b * b;
    m_el[3] = a * c;
    m_el[4] = b * c;
    m_el[5] = c * c;
    m_el[6] = a * d;
    m_el[7] = b * d;
    m_el[8] = c * d;
    m_el[9] = d * d;

#ifdef _DEBUG
    for (int i = 0; i < 10; i++)
    {
        GXASSERT(!_isnan(m_el[i]));
    }
#endif
}

CQuadric& CQuadric::operator=(const CQuadric &q)
{
    memcpy(m_el, q.m_el, sizeof(m_el));
    return *this;
}

CQuadric& CQuadric::operator+=(const CQuadric &q)
{
    m_el[0] += q.m_el[0];
    m_el[1] += q.m_el[1];
    m_el[2] += q.m_el[2];
    m_el[3] += q.m_el[3];
    m_el[4] += q.m_el[4];
    m_el[5] += q.m_el[5];
    m_el[6] += q.m_el[6];
    m_el[7] += q.m_el[7];
    m_el[8] += q.m_el[8];
    m_el[9] += q.m_el[9];
    return *this;
}

void CQuadric::CalculatePosition(D3DXVECTOR3 &vNewPos, D3DXVECTOR3 &vValidPos)
{
    double idDenom = (e(0, 0) * e(1, 2) * e(1, 2) -
                       e(0, 0) * e(1, 1) * e(2, 2) +
                       e(0, 1) * e(0, 1) * e(2, 2) -
                       2.0 * e(0, 1) * e(1, 2) * e(0, 2) +
                       e(1, 1) * e(0, 2) * e(0, 2));

    if (idDenom == 0)
    {
        vNewPos = vValidPos;
    }
    else
    {
        double id = 1.0 / idDenom;


        vNewPos.x = float((e(0, 1) * e(1, 2) * e(2, 3) -
                        e(0, 1) * e(1, 3) * e(2, 2) -
                        e(1, 1) * e(0, 2) * e(2, 3) +
                        e(1, 1) * e(0, 3) * e(2, 2) +
                        e(1, 2) * e(0, 2) * e(1, 3) -
                        e(0, 3) * e(1, 2) * e(1, 2)) * id);

        vNewPos.y = float((e(0, 0) * e(1, 3) * e(2, 2) -
                        e(0, 0) * e(1, 2) * e(2, 3) +
                        e(0, 1) * e(0, 2) * e(2, 3) -
                        e(0, 1) * e(0, 3) * e(2, 2) -
                        e(0, 2) * e(0, 2) * e(1, 3) +
                        e(0, 2) * e(0, 3) * e(1, 2)) * id);


        vNewPos.z = float((e(0, 0) * e(1, 1) * e(2, 3) -
                        e(0, 1) * e(0, 1) * e(2, 3) -
                        e(0, 0) * e(1, 3) * e(1, 2) +
                        e(0, 1) * e(0, 3) * e(1, 2) + 
                        e(0, 2) * e(0, 1) * e(1, 3) -
                        e(0, 2) * e(0, 3) * e(1, 1)) * id);
    }
}

