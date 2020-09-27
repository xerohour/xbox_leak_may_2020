//
//	qrand.cpp
//
///////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2001, Pipeworks Software Inc.
//				All rights reserved
#ifndef __QRAND__
#define __QRAND__

class QRand
{
private:
	
	int seed;

public:
	
	void Init(int initSeed=0x76543210) { seed = initSeed; }
	void	SetSeed(int newSeed)   { seed = newSeed;  }
	int		Rand(int scale);	
	int		Rand();
};


#endif  //__QRAND__