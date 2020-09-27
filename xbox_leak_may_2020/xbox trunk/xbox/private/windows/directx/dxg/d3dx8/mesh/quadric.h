#ifndef QUADRIC_INCLUDED
#define QUADRIC_INCLUDED

// UNDONE documen e vs element, and rename

#if 0 && defined(__WIN32)
inline bool isnanf(float f) {
    return _isnan(f);
}
#else
// On SGI, adapted from <ieeefp.h>
inline bool isnanf(float f) {
    float f2=f;
    return (((*(unsigned *)&(f2) & 0x7f800000)==0x7f800000)&& \
            ((*(unsigned *)&(f2) & 0x007fffff)!=0x00000000) );
}
#endif

class CQuadric {

    double m_el[10];

public:

    CQuadric() {}
    CQuadric(const CQuadric &q);
    CQuadric(double a, double b, double c, double d);
    ~CQuadric() {}

    void Init();
    void Generate(double a, double b, double c, double d);
    inline void element(INT row, INT col, double el);
    inline void e(INT row, INT col, double el);

    inline float CalculateCost(D3DXVECTOR3 &vPos);
    void CalculatePosition(D3DXVECTOR3 &vNewPos, D3DXVECTOR3 &vValidPos);

    CQuadric& operator=(const CQuadric &q);
    CQuadric& operator+=(const CQuadric &q);

    inline double element(INT row, INT col) const;
    inline double e(INT row, INT col) const;
};

void CQuadric::element(INT row, INT col, double el)
{
    if(col <= row)
        m_el[(row * (row + 1)) / 2 + col] = el;
    else
        m_el[(col * (col + 1)) / 2 + row] = el;
}

double CQuadric::element(INT row, INT col) const
{
    return (col <= row) ? m_el[(row * (row + 1)) / 2 + col] : m_el[(col * (col + 1)) / 2 + row];
}

void CQuadric::e(INT row, INT col, double el)
{
    m_el[(col * (col + 1)) / 2 + row] = el;
}

double CQuadric::e(INT row, INT col) const
{
    return m_el[(col * (col + 1)) / 2 + row];
}

float CQuadric::CalculateCost(D3DXVECTOR3 &vPos)
{
	float fRet = (float)((e(0, 0) * vPos.x + e(0, 1) * vPos.y + e(0, 2) * vPos.z + e(0, 3)) * vPos.x +
           (e(0, 1) * vPos.x + e(1, 1) * vPos.y + e(1, 2) * vPos.z + e(1, 3)) * vPos.y +
           (e(0, 2) * vPos.x + e(1, 2) * vPos.y + e(2, 2) * vPos.z + e(2, 3)) * vPos.z +
           (e(0, 3) * vPos.x + e(1, 3) * vPos.y + e(2, 3) * vPos.z + e(3, 3)));

	//GXASSERT(fRet > -1.0e-4 && !isnanf(fRet));
	return fRet;
}

#endif