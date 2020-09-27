#include "mainfile.h"

TCHAR *glbString1 = "Global string one";
TCHAR *glbString2 = "Global string two";


int main( int argc, char *argv[] )
{
TCHAR *locString1 = "Local string one";
TCHAR *locString2 = "Local string two";
TCHAR *locString3 = "Local String three";		// Days of the week
TCHAR locStrArray[10];

locStrArray[0] = 'O';
locStrArray[1] = 'L';
locStrArray[2] = 'Y';
locStrArray[3] = 'M';
locStrArray[4] = 'P';
locStrArray[5] = 'U';
locStrArray[6] = 'S';
locStrArray[7] = '\0';

file_two();

printf( "%s %s\n", glbString1, glbString2 );
printf( "%s %s\n", locString1, locString2 );
printf( "Is this local string 3?: %s\n", locString3 );
printf( "Global string in file2?: %s\n", glbDBNumbers );

return 0;
}
