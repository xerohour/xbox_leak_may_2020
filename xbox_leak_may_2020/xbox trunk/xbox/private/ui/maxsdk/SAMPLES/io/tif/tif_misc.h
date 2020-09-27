//
// Tom's miscellaneous TIF header file
//
// Deals with missing parts of the TIF library
//

extern void TIFFError(char *module, char *fmt, ...);
extern void TIFFWarning(char *module, char *fmt, ...);
extern void TIFFFreeDirectory(TIFF *tif);
extern void MissingRequired(TIFF *tif, char *tagname);
extern void TIFFGetField1(TIFFDirectory *td, u_short tag, va_list ap);
extern void TIFFSwabShort(unsigned short *wp);
extern void TIFFSwabLong(unsigned long *lp);
extern void TIFFSwabArrayOfShort(unsigned short *wp, int n);
extern void TIFFSwabArrayOfLong(unsigned long *lp, int n);
extern TIFFDefaultDirectory(TIFF *tif);
