#ifndef EH01_H
#define EH01_H

class ThrowObject {
public:
	ThrowObject();
	ThrowObject( int i );
	ThrowObject( const ThrowObject& );
	~ThrowObject();

private:
	int m_i;
};

extern int counter;

void Case1();
void Case2();
void Case3();
void Case4();
void Case5();
void Case6();
void Case7();

#endif // EH01_H