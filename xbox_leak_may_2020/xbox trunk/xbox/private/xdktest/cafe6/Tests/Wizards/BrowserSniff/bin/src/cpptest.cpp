//
//	cpptest.cpp
//

#include "browse.h"
#include "cpptest.h"

static void Bfoos( B& b );

int MI::m_si = 0;

A::~A() {
}

void A::vfoo1() {
}

void A::vfoo2() {
}

B::~B() {
}

void B::vfoo2() {
}

void B::vfoo3() {
}

void MI::vfoo2() {
}

void cpptest() {
	MI mi;
	Afoos( mi );
	Bfoos( mi );
}

void Afoos( A& a ) {
	a.vfoo1();
	a.vfoo2();
}

static void Bfoos( B& b ) {
	b.vfoo2();
	b.vfoo3();
}

FOOTYPE MI::StaticFoo( MI a, MI b ) {
	m_si++;
	int ai = a.InlineFoo();
	int bi = b.InlineFoo();
	return FOOMAX(ai,bi);
}

// Add another definition for m_i to spice up ambiguity testing
static int m_i = 0;

void cpp_refto_mi() { int i = m_i; }