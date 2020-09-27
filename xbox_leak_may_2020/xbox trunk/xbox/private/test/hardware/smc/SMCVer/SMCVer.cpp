

// 
// prints out the SMC version
//
// 




#include <xtl.h>
#include <dxconio.h>





extern "C"
    {
    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalReadSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN ReadWordValue,
        OUT ULONG *DataValue
        );

    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalWriteSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN WriteWordValue,
        IN ULONG DataValue
        );
    }

DWORD WriteSMC(unsigned char addr, unsigned char value)
    {
    return HalWriteSMBusValue(0x20, addr, FALSE, value);
    }

DWORD ReadSMC(unsigned char addr)
    {
    DWORD value = 0xCCCCCCCC;
    DWORD status = HalReadSMBusValue(0x21, addr, FALSE, &value);
    return value;
    }

void DebugPrint(char* format, ...)
    {
    va_list args;
    va_start(args, format);

    char szBuffer[1024];

    wsprintf(szBuffer, format, args);
    OutputDebugStringA(szBuffer);

    va_end(args);
    }






void main()
{
	
	char ver[4];


	xCreateConsole( NULL );
	xSetBackgroundImage( NULL );
	xSetFont( 15.0, 18.0, NULL );

	ver[0] = (char)ReadSMC( 0x01 );
	ver[1] = (char)ReadSMC( 0x01 );
	ver[2] = (char)ReadSMC( 0x01 );
	ver[3] = 0;

	DebugPrint( "SMC FW Version: %s\n", ver );
	printf( "   SMC FW Version: %s\n", ver );

	Sleep( INFINITE );
}








