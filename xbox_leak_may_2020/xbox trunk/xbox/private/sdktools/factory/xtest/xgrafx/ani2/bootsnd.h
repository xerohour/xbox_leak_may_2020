struct	DSPpatch {
	unsigned short *Start;				// address of start of sample
	unsigned short	Length;				// length of sample
	unsigned short	LoopStart;			// bytes from beg of sound to loop point 
	unsigned short	LoopLength;			// bytes form loop start to loop end
	unsigned short	LoopEnable;			// loop sound? T or F
	LPDSENVELOPEDESC		lpAmpEnvelope;
	LPDSENVELOPEDESC		lpMultiEnvelope;
};