#include "mbctest.h"

TCHAR *glbHiragana = "あかさたなはまやらん";
TCHAR *glbKatakana = "アカサタナハマパヤン";


int main( int argc, char *argv[] )
{
TCHAR *locHiragana = "あいうえおしゃしゅしょじゃじゅじょっとヴぁヴぃヴヴぇヴぉ";
TCHAR *locKatakana = "アイウエオシャシュショジャジュジョットヴァヴィヴヴェフォ";
TCHAR *locKanji    = "日月火水木金土";		// Days of the week
TCHAR locSBKana[10];

/*
	Ｅｘａｍｐｌｅ  ｏｆ  ｓｉｎｇｌｅ  ｂｙｔｅ  ｋａｔａｋａｎａ （カタカナ）
	ｃｈａｒａｃｔｅｒｓ．
*/
locSBKana[0] = 'ｱ';
locSBKana[1] = 'ｲ';
locSBKana[2] = 'ｳ';
locSBKana[3] = 'ｴ';
locSBKana[4] = 'ｵ';
locSBKana[5] = '\0';

file_two();

_tprintf( "%s %s\n", glbHiragana, glbKatakana );
_tprintf( "Single byte kana: %s\n", locSBKana );
_tprintf( "Days of the week: %s\n", locKanji );
_tprintf( "DB Numbers      : %s\n", glbDBNumbers );

return 0;
}
