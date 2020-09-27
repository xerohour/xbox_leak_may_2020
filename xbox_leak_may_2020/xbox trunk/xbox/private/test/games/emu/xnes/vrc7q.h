#define u8 unsigned char
void OPL2_setreg (u8,u8);
void ClearOPL2 (void);
void VRC7_LoadInstrument(u8);
void VRC7_dosound (u8,u8);

u8 Instrument[15][8];
u8 Chan1x[6];
u8 Chan2x[6];
u8 Chan3x[6];
u8 InstTrans[6];
