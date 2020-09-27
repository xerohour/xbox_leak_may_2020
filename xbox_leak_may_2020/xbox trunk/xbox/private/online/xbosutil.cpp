/*
 *  xbosutil.cpp
 *
 *  XBOS Utility APIs.  Copyright (c) Microsoft Corporation.
 *  
 *  Author: Ben Zotto (benzotto)
 *  Created: 12/10/01
 *
 *  Implements the APIs:   XOnlineOfferingPriceFormat()
 */		
#include "xonp.h"
#include "xonver.h"


// XONLINE_PRICE
//  This represents the price of an offer on the online service.  Within this struct
//  is enough information to format the price precisely.  That information is provided
//  by the server, taking into account both language and billing country.  Its members
//  are:
//              dwWholePart :       a DWORD giving the part of the price left of the
//                                  decimal point.
//              dwFractionalPart :  DWORD giving the decimal part, if applicable (see below)
//              bCurrencyFormat  :  a packed byte containing the following fields:
//                                      bits 4-7: currency (as in XONLINE_CURRENCY)            
//                                      bit    3: true if curr. marker precedes price
//                                      bit    2: true if there should be a space
//                                                between the marker and price
//                                      bit    1: true if there is a decimal (frac.) part
//                                      bit    0: true if decimal separator is a comma,
//                                                false if a period.
//              rgchISOCurrencyCode: the 3-character ISO code corresponding to this
//                                   currency.  should be used as the currency symbol
//                                   if the 4-bit currency from above is unrecognized.
//
/*
typedef struct {                    // DECLARED IN XONLINE.X.  REPRODUCED FOR REFERENCE.
    DWORD dwWholePart;
    DWORD dwFractionalPart;
    BYTE  bCurrencyFormat;
    WCHAR rgchISOCurrencyCode[3];
} XONLINE_PRICE;
*/

// XONLINE_CURRENCY
//  This enumeration represents the currencies that (this version of!) the client
//  knows about and can handle directly.  Enumeration should only handle 0-15
//  because it's stored in 4 bits.  This enumeration uses 0-13, leaving only two
//  "unknown" spots for future expansion...
//
typedef enum {
    AUD = 0,
    CAD, 
    CHF,
    DKK,
    EUR,
    GBP,
    JPY,
    KRW,
    NOK,
    NZD,
    SEK,
    USD,
    ZAR,
    USD_US,
    UNKNOWN_CURRENCY
} XONLINE_CURRENCY;

// XO_CURRENCY_*
//  These are bitflags to indicate availability of unicode extended currency
//  characters:
//      EUR = Euro
//      GBP = Pound sign
//      JPY = Yen
//      KRW = Korean Won
//
/* SHOWN HERE FOR REFERENCE.  DEFINED FOR REAL IN XONLINE.X
#define XO_CURRENCY_EUR     1
#define XO_CURRENCY_GBP     2
#define XO_CURRENCY_JPY     4
#define XO_CURRENCY_KRW     8
*/

// Internal manipulation macros for the currency formatting byte.  These
// macros, like the details of the byte's contents, are not explicitly
// exposed to title devs.
//
#define XONLINE_PRICE_FORMAT_MARKER_PRECEDES( x )      ( (x >> 3) & 1 )
#define XONLINE_PRICE_FORMAT_SPACE_PADDING( x )        ( (x >> 2) & 1 )
#define XONLINE_PRICE_FORMAT_DECIMAL_PART( x )         ( (x >> 1) & 1 )
#define XONLINE_PRICE_FORMAT_DECIMAL_SEPARATOR( x )    ( (x & 1) ? ',' : '.' )
#define XONLINE_PRICE_UNKNOWN_CURRENCY( x )            ( ((x >> 4) & 0x0F) >= UNKNOWN_CURRENCY ) 


// helper function prototype.
LPWSTR XOCopyCurrencyMarker(XONLINE_PRICE *Price, LPWSTR lpwstrMarker, DWORD *cbLength, DWORD dwExtendedCharsFilter);


// XOnlineOfferingPriceFormat
//
//  This synchronous API call will take an XONLINE_PRICE struture (freshly returned
//  from an OfferingGetDetails call) and produce a properly-formatted (unicode) 
//  currency string.  You will need to provide it with a pointer some pre-allocated 
//  space, and a pointer to the DWORD saying how big that space is.  
//  Finally, dwExtendedCharsFilter is a set of flags indicating which extended (unicode)
//  currency characters your font set includes/allows.  Set this by ORing in the
//  XO_CURRENCY_* flags. 
//
//  The function returns S_OK on success, and E_FAIL when confronted with too small a 
//  buffer.  If your call fails, simply recall with a big enough buffer-- the size
//  required will be placed into *cbLength.
//
HRESULT
CXo::XOnlineOfferingPriceFormat( XONLINE_PRICE *Price,
                            LPWSTR        lpwszFormattedPrice,
                            DWORD         *cbLength,
                            DWORD         dwExtendedCharsFilter
                          )
{
    XoEnter("XOnlineOfferingPriceFormat");
    XoCheck(cbLength != NULL);

    HRESULT hr;
    LPWSTR  lpwszPriceStart = lpwszFormattedPrice;
    DWORD   reqLen          = 0;
    
    DWORD   digits          = 0;
    DWORD   num;

    // calculate the required string size for the currency
    // need the symbol...
    XOCopyCurrencyMarker(Price, NULL, &reqLen, dwExtendedCharsFilter);
    // may need a space pad
    if( XONLINE_PRICE_UNKNOWN_CURRENCY(Price->bCurrencyFormat)  || 
        XONLINE_PRICE_FORMAT_SPACE_PADDING(Price->bCurrencyFormat) ) {        
        reqLen += 2;  // 1 space in unicode
    }
    // may need a 3-unicharacter decimal part... (eg: ".00")
    if( XONLINE_PRICE_FORMAT_DECIMAL_PART(Price->bCurrencyFormat) ) {
        reqLen += 6;   
    }
    // now... how many whole-part digits is this?
    num = Price->dwWholePart;
    if( num > 0 ) {
        for( digits = 1; ((num /= 10) > 0); digits++ );
    }
    reqLen += (2*digits);           // add in the digits (x2 for unicode)
    reqLen += 2;                    // ...and a null.

    // do we have enough string space for this?
    if( *cbLength < reqLen ) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto Exit;
    }

    Assert(lpwszFormattedPrice);
    
    // insert the currency symbol up front if we can
    if( XONLINE_PRICE_FORMAT_MARKER_PRECEDES(Price->bCurrencyFormat) ) {
        lpwszFormattedPrice = XOCopyCurrencyMarker( Price, 
                              lpwszFormattedPrice,                   
                              cbLength,
                              dwExtendedCharsFilter);
        
        // force an inserted space if we only know the ISO code, otherwise check
        if( XONLINE_PRICE_UNKNOWN_CURRENCY(Price->bCurrencyFormat)  || 
            XONLINE_PRICE_FORMAT_SPACE_PADDING(Price->bCurrencyFormat) ) {        
                *(lpwszFormattedPrice++) = L' ';
        }
    }

    // go ahead and splice in the actual prices 
    if( XONLINE_PRICE_FORMAT_DECIMAL_PART(Price->bCurrencyFormat) ) {
        swprintf( lpwszFormattedPrice, 
                  L"%d%c%.2d", 
                  Price->dwWholePart,
                  XONLINE_PRICE_FORMAT_DECIMAL_SEPARATOR(Price->bCurrencyFormat),
                  Price->dwFractionalPart
                  );
    } else {
        swprintf( lpwszFormattedPrice, 
                  L"%d", 
                  Price->dwWholePart 
                  );
    }
    lpwszFormattedPrice += wcslen(lpwszFormattedPrice);

    // insert the currency symbol now, if still necessary
    if( !XONLINE_PRICE_FORMAT_MARKER_PRECEDES(Price->bCurrencyFormat) ) {
        if( XONLINE_PRICE_UNKNOWN_CURRENCY(Price->bCurrencyFormat)  || 
            XONLINE_PRICE_FORMAT_SPACE_PADDING(Price->bCurrencyFormat) ) {        
                *(lpwszFormattedPrice++) = L' ';
        }
        
        lpwszFormattedPrice = XOCopyCurrencyMarker( Price,                             
                              lpwszFormattedPrice, 
                              cbLength,                        // this value will be incorrect, but OK
                              dwExtendedCharsFilter);
    
        // replace the NUL character
        *lpwszFormattedPrice = L'\0';
    }
        
    hr = S_OK;          // if we're here, we're fine.

Exit:
    *cbLength = reqLen;
    return(XoLeave(hr));
       
}
    
// XOCopyCurrencyMarker
//      an internal helper function to copy an appropriate currency symbol/marker into 
//      the provided string, which has length==*cbLength.  if cbLength is too small,
//      the required length will be written into the Length.
//
//
LPWSTR XOCopyCurrencyMarker(XONLINE_PRICE *Price, LPWSTR lpwstrMarker, DWORD *cbLength, DWORD dwExtendedCharsFilter)
{
    HRESULT hr              = E_FAIL;
    DWORD   reqLen          = 0;
    WCHAR   *marker			= NULL;
    BYTE    bCurrencyFormat = Price->bCurrencyFormat;
	DWORD   dwUnicodeChar   = 0;

    switch( (bCurrencyFormat >> 4) & 0x0F ) {
        case AUD:
            marker = L"A$";
            break;
        case CAD:
            marker = L"Can$";
            break;
        case CHF:
            marker = L"SFr";
            break;
        case DKK:
            marker = L"Dkr";
            break;
        case EUR:   
            if( dwExtendedCharsFilter & XO_CURRENCY_EUR ) {
				dwUnicodeChar = 0x20AC; // euro symbol
            } else {
                marker = L"EUR";
                Price->bCurrencyFormat |= 0x04;     // force a padding space
            }
            break;
        case GBP:
            if( dwExtendedCharsFilter & XO_CURRENCY_GBP ) {
                dwUnicodeChar = 0x00A3; // UK Pound symbol
            } else {
                marker = L"GBP";
                Price->bCurrencyFormat |= 0x04;     // force a padding space
            }
            break;
        case JPY:
            if( dwExtendedCharsFilter & XO_CURRENCY_JPY ) {
                dwUnicodeChar = 0x00A5; // Japansese Yen symbol
            } else {
                marker = L"JPY";
                Price->bCurrencyFormat |= 0x04;     // force a padding space
            }
            break;
        case KRW:
            if( dwExtendedCharsFilter & XO_CURRENCY_KRW ) {
                dwUnicodeChar = 0x20A9; // South Korean Won symbol
            } else {
                marker = L"KRW";
                Price->bCurrencyFormat |= 0x04;     // force a padding space
            }
            break;
        case NOK:
            marker = L"Nkr";
            break;
        case NZD:
            marker = L"NZ$";
            break;
        case SEK:
            marker = L"Skr";
            break;
        case USD:
            marker = L"US$";
            break;
        case ZAR:
            marker = L"R";
            break;
        case USD_US:
            marker = L"$";
            break;

        default:        /* not a recognized currency. */
            marker = NULL;
            break;
    }


    // did they supply enough space to copy in the marker plus a null?
    if( marker != NULL ) {
		reqLen = 2 * (wcslen(marker) + 1);  // don't forget unicode!
	} else {
		if( dwUnicodeChar != 0 ) {			// special unicode char?
			reqLen = 2;
		} else {							// just the ISO code.
			reqLen = 6;
		}
	}

    if( *cbLength >= reqLen ) {      
		Assert(lpwstrMarker != NULL);
        if( marker != NULL ) {
            wcscpy(lpwstrMarker, marker); 
            lpwstrMarker += wcslen(marker);
        } else {
			if( dwUnicodeChar != 0 ) {		// unicode char
				*(lpwstrMarker++) = (WCHAR)dwUnicodeChar;
			} else {
				memcpy(lpwstrMarker, Price->rgchISOCurrencyCode, (3*sizeof(WCHAR)) );
				lpwstrMarker += 3;
			}
        }
    }
    
	*cbLength = reqLen;
    return lpwstrMarker;
}
