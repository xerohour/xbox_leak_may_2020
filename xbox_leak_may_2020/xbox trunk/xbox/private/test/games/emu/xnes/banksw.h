#pragma warning( disable : 4244 )

void VROM_BANK1(uint32 A,uint32 V) 
{
	if(VROM_size)
	{
		V&=vmask1;
		VPAL[(A)>>10]=0;
		CHRBankList[(A)>>10]=V;
		VPage[(A)>>10]=&VROM[(V)<<10]-(A);
	}
}

void VRAM_BANK1(word A, byte V)
{
	V&=7;
	VPAL[(A)>>10]=1;
	CHRBankList[(A)>>10]=V;
	VPage[(A)>>10]=&VRAM[V<<10]-(A);
}

void VROM_BANK2(uint32 A,uint32 V) 
{
	if(VROM_size)
	{
		V&=vmask2;
		VPage[(A)>>10]=VPage[((A)>>10)+1]=&VROM[(V)<<11]-(A);
		CHRBankList[(A)>>10]=(V<<1);
		CHRBankList[((A)>>10)+1]=(V<<1)+1;
	}
}
void VROM_BANK4(uint32 A, uint32 V)
{
	if(VROM_size)
	{
		V&=vmask4;
		VPage[(A)>>10]=VPage[((A)>>10)+1]= 
			VPage[((A)>>10)+2]=VPage[((A)>>10)+3]=&VROM[(V)<<12]-(A);
		CHRBankList[(A)>>10]=((V)<<2);
		CHRBankList[((A)>>10)+1]=((V)<<2)+1;
		CHRBankList[((A)>>10)+2]=((V)<<2)+2;
		CHRBankList[((A)>>10)+3]=((V)<<2)+3;        
	}
}

void VROM_BANK8(uint32 V)
{
	if(VROM_size)
	{
		V&=vmask;
		VPage[0]=VPage[1]=VPage[2]=VPage[3]=
			VPage[4]=VPage[5]=VPage[6]=VPage[7]=&VROM[V<<13];
		CHRBankList[0]=(V<<3);
		CHRBankList[1]=(V<<3)+1;
		CHRBankList[2]=(V<<3)+2;
		CHRBankList[3]=(V<<3)+3;
		CHRBankList[4]=(V<<3)+4;
		CHRBankList[5]=(V<<3)+5;
		CHRBankList[6]=(V<<3)+6;
		CHRBankList[7]=(V<<3)+7;
	}
}

void ROM_BANK8(unsigned short A, byte V)
{
	V&=pmask8;
	Page[A>>13]=(&ROM[V<<13])-A;
	
	if(A>=0x8000)
	{
		PRGBankList[((A-0x8000)>>13)]=V;
		if(modcon!=0xFF && (genie&2))
		{
			int x;
			for(x=0;x<3;x++)
			{
				if(!(((modcon>>(4+x))&1)))
				{
					if((genieaddr[x]&0xE000) != A) continue;   
					if(((modcon>>(1+x))&1))
					{
						if(Page[genieaddr[x]>>13][genieaddr[x]]==geniech[x])
						{
							Page[genieaddr[x]>>13][genieaddr[x]]=genieval[x];
						}
					}
					else
					{
						Page[genieaddr[x]>>13][genieaddr[x]]=genieval[x];
					}
				}
			}
		}
	}
	else
	{
		PRGBankListLow[((A)>>13)]=V;
	}
}

void ROM_BANK16(unsigned short A, byte V)
{
	V&=pmask16;
	Page[(A)>>13]=Page[((A)>>13)+1]=&ROM[V<<14]-(A);
	if(A>=0x8000)
	{
		PRGBankList[(((A)-0x8000)>>13)]=(V)<<1;
		PRGBankList[(((A)-0x8000)>>13)+1]=((V)<<1)+1;
		Page[(A)>>13]=Page[((A)>>13)+1]=&ROM[V<<14]-(A);
		
		if(modcon!=0xFF && (genie&2))
		{
			int x;
			for(x=0;x<3;x++)
			{
				if(!(((modcon>>(4+x))&1)))
				{
					if((genieaddr[x]&0xC000) != A) continue;
					if(((modcon>>(1+x))&1))
					{
						if(Page[genieaddr[x]>>13][genieaddr[x]]==geniech[x])
						{
							Page[genieaddr[x]>>13][genieaddr[x]]=genieval[x];
						}
					}
					else     
					{
						Page[genieaddr[x]>>13][genieaddr[x]]=genieval[x];
					}
				}
			}
		}
	}
	else
	{
		PRGBankListLow[(((A))>>13)]=(V)<<1;
		PRGBankListLow[(((A))>>13)+1]=((V)<<1)+1;
	}
	
}





void ROM_BANK32(unsigned short A, byte V)
{
	V&=pmask32;
	
	PRGBankList[(((A)-0x8000)>>13)]=(V)<<2;
	PRGBankList[(((A)-0x8000)>>13)+1]=((V)<<2)+1;
	PRGBankList[(((A)-0x8000)>>13)+2]=((V)<<2)+2;
	PRGBankList[(((A)-0x8000)>>13)+3]=((V)<<2)+3;
	Page[(A)>>13]=Page[((A)>>13)+1]=Page[((A)>>13)+2]=Page[((A)>>13)+3]=&ROM[V<<15]-(A);
	
	if(modcon!=0xFF && (genie&2))
	{
		int x;
		for(x=0;x<3;x++)
		{
			if(!(((modcon>>(4+x))&1)))
			{
				if(((modcon>>(1+x))&1))
				{
					if(Page[genieaddr[x]>>13][genieaddr[x]]==geniech[x])
						Page[genieaddr[x]>>13][genieaddr[x]]=genieval[x];
				}
				else
					Page[genieaddr[x]>>13][genieaddr[x]]=genieval[x];
			}
		}
	}
}

