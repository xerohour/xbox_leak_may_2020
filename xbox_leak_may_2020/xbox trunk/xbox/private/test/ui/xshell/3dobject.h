#ifndef _3DOBJECT_H_
#define _3DOBJECT_H_

class Face
{
public:	
	WORD        point[3];
	D3DXVECTOR3 normal;
};

class Quaternion
{
public:
	float w;
	float x;
	float y;
	float z;
};

class C3DObject
{
public:
    // Constructors and Destructors
    C3DObject(void);
	~C3DObject(void);

    // Public Properties
	D3DXMATRIX   m_WorkMatrix;
	D3DXMATRIX   m_ObjectMatrix;
	D3DXVECTOR4  m_TranslateVector;

    // Public Methods
	void translate( float x, float y, float z );
	void rotateX( float x );
	void rotateY( float y );
	void rotateZ( float z );
	void spinX( float x );
	void spinY( float y );
	void spinZ( float z );
	void localRotateX( float x );
	void localRotateY( float y );
	void localRotateZ( float z );
};

#endif // _3DOBJECT_H_