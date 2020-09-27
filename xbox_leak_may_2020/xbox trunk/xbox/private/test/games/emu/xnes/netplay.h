#ifdef NETWORK
int InitNetplay(void);
void NetplayUpdate(uint32 *JS);

uint8 netplay;
char netplayhost[256];
uint8 netskip;
#endif
