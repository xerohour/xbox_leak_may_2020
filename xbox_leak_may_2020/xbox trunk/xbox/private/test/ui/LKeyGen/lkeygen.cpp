#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <xcrypt.h>

BOOL GetMacAddressFromUser( unsigned char* pszBuffer );
void ComputeSecretKey( BYTE* pbMacAddress, int iMacAddressSize, char* pszBuffer );
void Usage();   // Display usage for the program
BOOL ValidMACAddress( unsigned char* pszMacAddress );
void DisplaySecretCode( char* pszCode );
BOOL MACStringToHexString( unsigned char* pszMacAddress );

int __cdecl main( int argc, char* argv[] )
{
    unsigned char pszMacAddress[13];
    char pszSecretKey[5];
    BOOL bValidAddress = FALSE;

    ZeroMemory( pszMacAddress, 13 );
    ZeroMemory( pszSecretKey, 5 );

    if( argc > 2 )
    {
        Usage();

        return 0;
    }

    if( argc == 2 )
    {
        // First check to see if they are asking for Help or Usage
        if( ( strcmp( argv[1], "/?" ) == 0 ) || 
            ( strcmp( argv[1], "-?" ) == 0 ) ||
            ( strcmp( argv[1], "?" ) == 0 ) )
        {
            Usage();

            return 0;
        }

        // Check to see if the parameter is a valid MAC Address
        if( bValidAddress = ValidMACAddress( (unsigned char*)argv[1] ) )
        {
            strcpy( (char*)pszMacAddress, argv[1] );
        }
    }
    else
    //
    // Get the MAC Address from the user via the keyboard
    //
    {
        printf("Please enter the Title MAC Address: ");
        bValidAddress = GetMacAddressFromUser( pszMacAddress );
    }

    if( !bValidAddress )
    {
        printf( "Invalid MAC Address entered.\r\n" );

        return -1;
    }

    //
    // Convert our 12 byte string to a 6 byte Hex value string
    //
    if( !MACStringToHexString( pszMacAddress ) )
    {
        printf( "Failed to properly convert the MAC Address!!\r\n" );

        return -1;
    }

    //
    // Must be a valid MAC Address.
    //

    // Generate the key code
    ComputeSecretKey( (BYTE*)pszMacAddress, 6, pszSecretKey );

    // Display the Secret Code in a friendly manner
    DisplaySecretCode( pszSecretKey );

    return 0;
};


BOOL GetMacAddressFromUser( unsigned char* pszBuffer )
{
    BOOL bReturn = TRUE;
    BOOL bExit = FALSE;
    unsigned int x = 0;

    if( NULL == pszBuffer )
    {
        return FALSE;
    }

    while( !bExit ) // ( x < 12 )
    {
        int iChar = _getch();
        
        // Check for valid inputs
        if( ( iChar >= 48 && iChar <= 57 ) || ( iChar >= 65 && iChar <= 70 ) || ( iChar >= 97 && iChar <= 102 ) )
        //
        // Valid input
        //
        {
            if( x < 12 )
            {
                printf( "%c", iChar );
                pszBuffer[x++] = (UCHAR)iChar;
            }
        }
        else if( iChar == 8 )  // Check for back space key
        {
            if( x > 0 ) // Check to see if we have at least 1 character to backspace
            {
                printf( "%c %c", iChar, iChar );
                pszBuffer[--x] = '\0';
            }
        }
        else if( iChar == 13 )   // Check for ENTER key
        //
        // Check for return code
        //
        {
            bExit = TRUE;
            printf( "\r\n", iChar );

            if( x < 12 )
            {
                // Invalid input
                bReturn = FALSE;
            }
        }
    }

    return bReturn;
}


void DisplaySecretCode( char* pszCode )
{
    unsigned int x = 0;

    if( NULL == pszCode )
    {
        return;
    }

    printf( "\r\nEnter the following code:\r\n    " );
    for( x = 0; x < 4; x++ )
    {
        switch( pszCode[x] )
        {
        case 'A':
            printf( "A button" );
            break;
        case 'B':
            printf( "B button" );
            break;
        case 'Y':
            printf( "Y button" );
            break;
        case 'X':
            printf( "X button" );
            break;
        case 'L':
            printf( "Black button" );
            break;
        case 'W':
            printf( "White button" );
            break;
        case 'U':
            printf( "DPAD Up" );
            break;
        case 'D':
            printf( "DPAD Down" );
            break;
        case 'F':
            printf( "DPAD Left" );
            break;
        case 'R':
            printf( "DPAD Right" );
            break;
        case 'T':
            printf( "Left Thumb Button" );
            break;
        case 'I':
            printf( "Right Thumb Button" );
            break;
        case 'E':
            printf( "Left Trigger" );
            break;
        case 'O':
            printf( "Right Trigger" );
            break;
        }

        if( x < 3 )
            printf( " --> " );
        else
            printf( "\r\n" );
    }
}

// These should never change
#define SYMMETRIC_KEY_LEN 16
#define SECRET_RAND_KEY "\xAC\x21\x07\x1B\xB5\x94\xE8\x23\x50\x33\x4A\x71\x72\x7A\xD5\x91"
#define SECRET_RAND_KEY_LEN SYMMETRIC_KEY_LEN

// A   - A Button
// B   - B Button
// Y   - Y Button
// X   - X Button
// L   - bLack Button
// W   - White Button
// U   - DPAD Up
// D   - DPAD Down
// F   - DPAD leFt
// R   - DPAD Right
// T   - Left Thumb Button
// I   - rIght Thumb Position
// E   - lEft trigger button
// O   - right trigger buttOn
void ComputeSecretKey( BYTE* pbMacAddress, int iMacAddressSize, char* pszBuffer )
{

    BYTE Digest[XC_SERVICE_DIGEST_SIZE];
    WORD* pwDigestWORD = (WORD*)(&Digest[0]);
    int i = 4;
    
    // Don't use "A" or "X" button in the combination
    char* secretKeyMap = "BYLWUDFRTIEO";

    if( ( NULL == pszBuffer ) || ( NULL == pbMacAddress ) )
    {
        return;
    }
    
    
    XcHMAC( (LPBYTE)SECRET_RAND_KEY, SECRET_RAND_KEY_LEN,
            pbMacAddress, iMacAddressSize,
            NULL, 0,
            Digest );
    

    RtlZeroMemory( pszBuffer, 4 );
    while ( --i >= 0 )
    {
        pszBuffer[i] = secretKeyMap[ (*pwDigestWORD++) % 12 ];
    }
}


BOOL ValidMACAddress( unsigned char* pszMacAddress )
{
    unsigned int x = 0;

    if( NULL == pszMacAddress )
    {
        return FALSE;
    }

    // First check the length
    if( strlen( (char*)pszMacAddress ) != 12 )
    {
        return FALSE;
    }

    // Check the characters
    for( x = 0; x < 12; x++ )
    {
        if( ( pszMacAddress[x] < 48 ) || ( pszMacAddress[x] > 102 ) )
            return FALSE;

        if( ( pszMacAddress[x] > 57 ) && ( pszMacAddress[x] < 65 ) )
            return FALSE;

        if( ( pszMacAddress[x] > 70 ) && ( pszMacAddress[x] < 97 ) )
            return FALSE;
    }

    return TRUE;
}


BOOL MACStringToHexString( unsigned char* pszStringMacAddress )
{
    // unsigned char* pszTemp = pszStringMacAddress;  // Hack for VC6 Compiler

    int iTemp[6];
    ZeroMemory( iTemp, 6 * sizeof( iTemp[0] ) );

    if( sscanf( (char*)pszStringMacAddress, "%2x%2x%2x%2x%2x%2x", &iTemp[0], &iTemp[1], &iTemp[2], &iTemp[3], &iTemp[4], &iTemp[5] ) != 6)
    {
        printf( "Error in converting the MAC Address String to HEX Ints!\r\n" );

        return FALSE;
    }

    for( unsigned int x = 0; x < 6; x++ )
    {
        pszStringMacAddress[x] = (UCHAR)iTemp[x];
    }
    
    pszStringMacAddress[x] = '\0';

    return TRUE;
}


void Usage()
{
    printf( "Generates the secret key to display Xbox information from the XDK Launcher.\r\n" );
    printf( "\r\n" );
    printf( "LKEYGEN [Xbox Title MAC Address]\r\n" );
    printf( "\r\n" );
    printf( "  [Xbox Title MAC Address]\r\n" );
    printf( "               The debug MAC address of the Xbox for which the key should be generated.\r\n" );
    printf( "               This is an optional parameter.  If not specified, the program will prompt\r\n" );
    printf( "               for the MAC address.  The MAC address must be 12 digits long.\r\n" );
    printf( "\r\n" );
}