void FDSInit(void);
void FDSReset(void);
byte FDSRead(word A);
void FDSWrite(word A, byte V);
void FDSscanline(void);
void FDSRAMWrite(word A, byte V);
void FDSFix(int a);
void FDSControl(int what);

#define FDS_IDISK  1
#define FDS_EJECT  2
#define FDS_SELECT 3

void FDSStateLoad(FILE *fp);
void FDSStateSave(FILE *fp);
int FDSLoad(char *name, FILE *fp);
