{
	register byte c1,c2;
	
	c1=((C[0]>>1)&0x55)|(C[8]&0xAA);
	c2=(C[0]&0x55)|((C[8]<<1)&0xAA);
	zz=c1&3;
	zz2=c2&3;
	P[6]=VRAM[0x3f00+zz+cc];
	P[7]=VRAM[0x3f00+zz2+cc];
	c1>>=2;
	c2>>=2;
	zz=c1&3;
	zz2=c2&3;
	P[4]=VRAM[0x3f00+zz+cc];
	P[5]=VRAM[0x3f00+zz2+cc];
	c1>>=2;
	c2>>=2;
	zz=c1&3;
	zz2=c2&3;
	P[2]=VRAM[0x3f00+zz+cc];
	P[3]=VRAM[0x3f00+zz2+cc];
	c1>>=2;
	c1+=cc;
	c2>>=2;
	c2+=cc;
	P[0]=VRAM[0x3f00+c1];
	P[1]=VRAM[0x3f00+c2];
}
