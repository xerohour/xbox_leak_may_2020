//////////////////////////////////////////////////////////////////////
// Matrix.h: interface for the CMatrix class.
//////////////////////////////////////////////////////////////////////

#define PI ((float)3.14159265358979)

class CMatrix  
{
public:
	CMatrix();
	virtual ~CMatrix();

	void Clear();
	void Identity();
	void RotatePitch(float Deg);
	void RotateYaw(float Deg);
	void RotateRoll(float Deg);
	void Translate(D3DVECTOR Position);

	D3DMATRIX m_Matrix;
};
