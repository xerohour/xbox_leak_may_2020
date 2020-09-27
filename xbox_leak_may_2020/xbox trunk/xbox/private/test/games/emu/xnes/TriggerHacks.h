/*	This is a hack to reduce screen jumpiness with certain games */
void __inline TriggerIRQSync(void)
{
	
	if(!(_P&I_FLAG))
	{
		M.ICount=0;
		_ICounta+=7;
		M_PUSH(_PC.B.h);
		M_PUSH(_PC.B.l);
		M_PUSH(_P&~B_FLAG);
		_P&=~D_FLAG;
		_P|=I_FLAG;
#ifdef LSB_FIRST
		_PC_=*(word *)(Page[0xFFFE>>13]+0xFFFE);
#else
		_PC.B.l=Op6502(0xFFFE);_PC.B.h=Op6502(0xFFFF);
#endif
	}
}

void __inline TriggerIRQ(void)
{
	if(!(_P&I_FLAG))
	{
		_ICounta+=7;M_PUSH(_PC.B.h);M_PUSH(_PC.B.l);M_PUSH(_P&~B_FLAG);
		_P&=~D_FLAG;
		_P|=I_FLAG;
#ifdef LSB_FIRST
		_PC_=*(word *)(Page[0xFFFE>>13]+0xFFFE);
#else
		_PC.B.l=Op6502(0xFFFE);_PC.B.h=Op6502(0xFFFF);
#endif
	}
}
