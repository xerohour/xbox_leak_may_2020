#include "d3dapi.hpp"

LINKME(14);

#pragma data_seg(".d3dapi$test032") 


//BEGINTEST(testUmaTextureStuff) 
//{
//
//}
//ENDTEST()

//BEGINTEST(testUmaVertexBuffer) 
//{
//
//}
//ENDTEST()

/*
TODO!!!
BEGINTEST(testUmaPalette)
{
}
ENDTEST()
*/

BEGINTEST(testAllocateAndFreeMemory) 
{
	void* b;
	int i;
	xStartVariation(hlog, "Sizes loop");
	{
		for(i = 0; i <= 25; i++) {
			xLog(hlog, XLL_INFO, "1<<i: %d, (1<<i)-1: %d", (1 << i), (1 << i)-1);

			b = D3D_AllocContiguousMemory((1 << i), 1);
			CHECKRESULT(b != NULL);
			if(WASGOODRESULT()) {
				D3D_FreeContiguousMemory(b);
			}

			if(i) {
				b = D3D_AllocContiguousMemory((1 << i) - 1, 1);
				CHECKRESULT(b != NULL);
				if(WASGOODRESULT()) {
					D3D_FreeContiguousMemory(b);
				}
			}
		}
	}
	xEndVariation(hlog);

}
ENDTEST()

BEGINTEST(testCopyMemory) 
{
	char* buf[2];
	int i, j;

	xStartVariation(hlog, "");
	for(i = 1; i != (1 << 24) / 5 + 1; i += i / 5 + 1) { //loop through a bunch of sizes
		if (i > 28000000) i = (1 << 24);		//when we're done, get a nice big power of 2 also.


		xLog(hlog, XLL_INFO, "Size: %d", i);
		buf[0] = (char*)D3D_AllocContiguousMemory(i, 1);
		buf[1] = (char*)D3D_AllocContiguousMemory(i, 1);
		
		if(!buf[0]) {
			xLog(hlog, XLL_FAIL, "malloc(%d) failed!", i);
			break;
		}
		if(!buf[1]) {
			D3D_FreeContiguousMemory(buf[0]);
			xLog(hlog, XLL_FAIL, "second malloc(%d) failed!", i);
			break;
		}

		for(j = 0; j < i; j++) {
			buf[0][j] = (char)j;
			buf[1][j] = (char)~j;
		}

		D3D_CopyContiguousMemory(buf[0], buf[1], i);

		for(j = 0; j < i; j++) {
			if(buf[0][j] != buf[1][j]) {
				xLog(hlog, XLL_FAIL, "copy didn't work right??? Source byte %d = %d, dest = %d", i, buf[0][j], buf[1][j]);
			}
		}

		D3D_FreeContiguousMemory(buf[0]);
		D3D_FreeContiguousMemory(buf[1]);
		if(i == (1 << 24)) break;
	}
}
ENDTEST()

BEGINTEST(testCopyToVideo)
{
	char* buf;
	int i, j;
	for(i = 1; i != (1 << 24) / 5 + 1; i += i / 5 + 1) { //loop through a bunch of sizes
		if (i > 28000000) i = (1 << 24);		//when we're done, get a nice big power of 2 also.


		xLog(hlog, XLL_INFO, "Size: %d", i);
		buf = (char*)D3D_AllocContiguousMemory(i, 1);
		
		if(!buf) {
			xLog(hlog, XLL_FAIL, "malloc(%d) failed!", i);
			break;
		}

		for(j = 0; j < i; j++) {
			buf[j] = (char)j;
		}

		D3D_CopyContiguousMemoryToVideo(buf);
		//all this test can really do is just make sure that it didn't crash...

		D3D_FreeContiguousMemory(buf);
		if(i == (1 << 24)) break;
	}
} 
ENDTEST()

BEGINTEST(testRegister)
{
	D3DTexture* t;
	D3DTexture* n;

	char* d;

	//g_pd3dd8->CreateTexture(
}
ENDTEST()


#pragma data_seg()