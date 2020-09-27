#ifndef EHUTIL_H
#define EHUTIL_H


class ThrowObject {
public:
	ThrowObject();
	ThrowObject( int i );
	ThrowObject( const ThrowObject& );
	~ThrowObject();

private:
	int m_i;
	int m_iCopy;
};

extern int counter;
extern int ctorCounter;
extern int dtorCounter;


#endif // EHUTIL_H