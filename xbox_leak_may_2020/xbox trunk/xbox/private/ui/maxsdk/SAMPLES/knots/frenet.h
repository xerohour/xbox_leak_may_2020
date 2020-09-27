/* 
 * Frenet frames
 */

typedef Point2 (*Curve2D)(float u, float t);
typedef Point3 (*Curve3D)(float t);

class FrenetFrame {
public:
		FrenetFrame(Curve3D spaceCurve, float t, float deltaT);

		Point3 Value()    { return mValue; }
		Point3 Tangent()  { return mTangent; }
		Point3 Normal()   { return mNormal; }
		Point3 BiNormal() { return mBiNormal; }

private:
		Point3 mValue;
		Point3 mTangent;
		Point3 mNormal;
		Point3 mBiNormal;
};

