#include "ehutil.h"


ThrowObject::ThrowObject() {
	m_i = 0; m_iCopy = 0;
}


ThrowObject::ThrowObject( int i ) {
	m_i = i; m_iCopy = 0;
}


ThrowObject::ThrowObject( const ThrowObject& src) {
	++ctorCounter;
	m_i = src.m_i; m_iCopy = src.m_iCopy + 1;

	//	Decrement m_i if non-zero and throw exception if m_i 
	//	decrements to 0.
//	if( m_i > 0 ) {
//		m_i--;
//		if( m_i == 0 ) {
//			throw m_i;
//		}
//	}
}


ThrowObject::~ThrowObject() {
	++dtorCounter;
}

