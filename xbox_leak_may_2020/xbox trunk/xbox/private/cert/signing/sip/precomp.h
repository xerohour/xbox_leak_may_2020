#include <windows.h>
#include <wincrypt.h>
#include <mssip.h>
#include <xsum.h>
#include <filestm.h>
#include <xbefile.h>
#include <ncipher.h>
#include <time.h>

extern HINSTANCE g_hinstCrypt;

class CFunctestCrypt : public CSimpleCrypt
{
public:
    CFunctestCrypt(void) : CSimpleCrypt(rgbPublicKey, rgbPrivateKey) {}
private:
    static const BYTE rgbPrivateKey[];
    static const BYTE rgbPublicKey[];
};
