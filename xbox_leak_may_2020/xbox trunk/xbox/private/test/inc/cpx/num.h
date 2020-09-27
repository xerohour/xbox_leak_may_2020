#ifndef __num_h_
#define __num_h_

//first 2 bits determine alignment
#define numALIGN_LEFT		0x00000000
#define numALIGN_RIGHT		0x00000001
#define numALIGN_CENTER		0x00000002
#define numDRAW_VERCIAL		0x00000003

//sinwave rippling
#define numRIPPLE			0x00000004

void numDrawNumbers(int num, float x, float y, float sizex, float sizey, DWORD col, DWORD flags);
void numRipple();

#endif