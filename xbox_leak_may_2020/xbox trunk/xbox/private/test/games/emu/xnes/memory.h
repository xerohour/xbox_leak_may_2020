/*              Various macros for faster memory stuff (at least that's the idea) */
#define dwmemset(d,c,n) {int x; uint32 gorf=(c)|((c)<<8)|((c)<<16)|((c)<<24); for(x=n-4;x>=0;x-=4) *(uint32 *)&(d)[x]=gorf;}
