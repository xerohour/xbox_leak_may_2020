/*----------------------------------------------------------------------------
 *  Copyright © 1997 Microsoft.  ALL RIGHTS RESERVED. 
 *----------------------------------------------------------------------------
 *  Date: 10/24/00
 *  Developer: Sean Wohlgemuth
 *----------------------------------------------------------------------------
 */

/*	BuildFilesOfSize - Create a number of files who's names relate to their filesize

	hStart - file size in bytes to create
	dwCount - number of files to create increasing 1 byte per file
	pszPath - path for destination file (example: "T:\\" or T:\\somedir\\")

	Files created are 8.3 and follow the form: XXXXXXXX.YYY
		where YYY is the number of 1024 blocks in this file and
		where XXXXXXXX is the number of remaining bytes excluding 1024 blocks in this file
		XXXXXXXX and YYY are represented in uppercase HEX form with padded 0's.
*/
HRESULT BuildFilesOfSize(unsigned __int64 hStart, DWORD dwCount, char* pszPath);