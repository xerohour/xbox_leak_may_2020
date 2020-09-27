#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Runner.h"


CMathClass g_Math;

// TODO: Need a way to expose class properties...

START_NODE_FUN(CMathClass, CNodeClass)
	NODE_FUN_NN(abs)
	NODE_FUN_NN(acos)
	NODE_FUN_NN(asin)
	NODE_FUN_NN(atan)
	NODE_FUN_NNN(atan2)
	NODE_FUN_NN(ceil)
	NODE_FUN_NN(cos)
	NODE_FUN_NN(exp)
	NODE_FUN_NN(floor)
	NODE_FUN_NN(log)
	NODE_FUN_NNN(max)
	NODE_FUN_NNN(min)
	NODE_FUN_NNN(pow)
	NODE_FUN_NV(random)
	NODE_FUN_NN(round)
	NODE_FUN_NN(sin)
	NODE_FUN_NN(sqrt)
	NODE_FUN_NN(tan)
END_NODE_FUN()


CMathClass::CMathClass() : CNodeClass(_T("Math"), 0, NULL, NULL, NULL)
{
	m_E = 2.7182818284590452354f;
	m_LN2 = 0.69314718055994530942f;
	m_LN10 = 2.30258509299404568402f;
	m_LOG2E = 1.442f;
	m_LOG10E = 0.434f;
	m_PI = 3.14159265358979323846f;
	m_SQRT1_2 = 0.70710678118654752440f;
	m_SQRT2 = 1.41421356237309504880f;
}

void CMathClass::AddRef()
{
	// not reference counted!
}

void CMathClass::Release()
{
	// not reference counted!
}

float CMathClass::abs(float number)
{
	return ::fabsf(number);
}

float CMathClass::acos(float number)
{
	return ::acosf(number);
}

float CMathClass::asin(float number)
{
	return ::asinf(number);
}

float CMathClass::atan(float number)
{
	return ::atanf(number);
}

float CMathClass::atan2(float y, float x)
{
	return ::atan2f(x, y);
}

float CMathClass::ceil(float number)
{
	return ::ceilf(number);
}

float CMathClass::cos(float number)
{
	return ::cosf(number);
}

float CMathClass::exp(float number)
{
	return ::expf(number);
}

float CMathClass::floor(float number)
{
	return ::floorf(number);
}

float CMathClass::log(float number)
{
	return ::logf(number);
}

float CMathClass::max(float number1, float number2)
{
	if (number1 > number2)
		return number1;
	else
		return number2;
}

float CMathClass::min(float number1, float number2)
{
	if (number1 < number2)
		return number1;
	else
		return number2;
}

float CMathClass::pow(float base, float exponent)
{
	return ::powf(base, exponent);
}

float CMathClass::random()
{
	return (float)rand() / (float)RAND_MAX;
}

float CMathClass::round(float number)
{
	return (float)((int)(number + 0.5f));
}

float CMathClass::sin(float number)
{
	return ::sinf(number);
}

float CMathClass::sqrt(float number)
{
	return ::sqrtf(number);
}

float CMathClass::tan(float number)
{
	return ::tanf(number);
}

