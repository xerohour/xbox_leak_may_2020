// ProjWriter.cpp

#include "stdafx.h"

const WCHAR XML_ENTITY_QUOTE[] = L"&quot;";
const WCHAR XML_ENTITY_APOSTROPHE[] = L"&apos;";
const WCHAR XML_ENTITY_LESSTHAN[] = L"&lt;";
const WCHAR XML_ENTITY_GREATERTHAN[] = L"&gt;";
const WCHAR XML_ENTITY_AMPERSAND[] = L"&amp;";


// XML writer routines
static int indent = 0;

static inline int StrByteLen( WCHAR *str )
{
	return (int)(wcslen( str ) * sizeof(WCHAR));
}

static void Indent( IStream *xml, int n )
{
	ULONG cbWritten;
	// if there's nothing to do, bail
	if( !n ) 
		return;
	// allocate string on stack
	WCHAR* strIndent = (WCHAR*)_alloca( (n+1) * sizeof(WCHAR) );

	// fill it with tabs
	if( strIndent )
	{
		for( int i = 0; i <= n; i++ )
			strIndent[i] = L'\t';
		strIndent[n] = 0;
	}
	else
		strIndent = L"";
	xml->Write( (void*)strIndent, n * sizeof(WCHAR), &cbWritten );
	return;
}

int GetIndent() { return indent; }
void SetIndent(int nNewIndent) { indent = nNewIndent; }

void StartNodeHeader( IStream *xml, WCHAR* wszName, bool bHasChildren )
{
	ULONG cbWritten;

	// indent one level
	Indent( xml, indent++ );

	xml->Write( (void*)L"<", sizeof(WCHAR), &cbWritten );
	xml->Write( (void*)wszName, StrByteLen( wszName ), &cbWritten );
}

void NodeAttribute( IStream *xml, WCHAR* wszName, WCHAR* wszValue )
{
	ULONG cbWritten;
	
	// if we were handed a null for value, fail gracefully
	if( !wszValue )
		return;
		
	xml->Write( (void*)L"\r\n", StrByteLen( L"\r\n" ), &cbWritten );
	// indent to the current level 
	Indent( xml, indent );
	xml->Write( (void*)wszName, StrByteLen( wszName ), &cbWritten );
	xml->Write( (void*)L"=\"", StrByteLen( L"=\"" ), &cbWritten );
	xml->Write( (void*)wszValue, StrByteLen( wszValue ), &cbWritten );
	xml->Write( (void*)L"\"", StrByteLen( L"\"" ), &cbWritten );
}

void NodeAttributeWithSpecialChars( IStream *xml, WCHAR* wszName, WCHAR* wszValue, bool bFilenameChars )
{
	ULONG cbWritten;
	
	// if we were handed a null for value, fail gracefully
	if( !wszValue )
		return;
		
	// replace all the special chars with their XML entity equivalents
	CStringW strValue = wszValue;
	// all special chars
	if( !bFilenameChars )
	{
		strValue.Replace( L"&", XML_ENTITY_AMPERSAND );
		strValue.Replace( L"\"", XML_ENTITY_QUOTE );
		strValue.Replace( L"'", XML_ENTITY_APOSTROPHE );
		strValue.Replace( L"<", XML_ENTITY_LESSTHAN );
		strValue.Replace( L">", XML_ENTITY_GREATERTHAN );
	}
	// filenames can only have & and '
	else
	{
		strValue.Replace( L"&", XML_ENTITY_AMPERSAND );
		strValue.Replace( L"'", XML_ENTITY_APOSTROPHE );
	}

	
	xml->Write( (void*)L"\r\n", StrByteLen( L"\r\n" ), &cbWritten );
	// indent to the current level 
	Indent( xml, indent );
	xml->Write( (void*)wszName, StrByteLen( wszName ), &cbWritten );
	xml->Write( (void*)L"=\"", StrByteLen( L"=\"" ), &cbWritten );
	xml->Write( (void*)strValue.GetBuffer( 0 ), StrByteLen( strValue.GetBuffer( 0 ) ), &cbWritten );
	xml->Write( (void*)L"\"", StrByteLen( L"\"" ), &cbWritten );
}

void EndNodeHeader( IStream *xml, bool bHasChildren )
{
	ULONG cbWritten;

	if( bHasChildren )
	{
		xml->Write( (void*)L">\r\n", StrByteLen( L">\r\n" ), &cbWritten );
	}
	else
	{
		xml->Write( (void*)L"/>\r\n", StrByteLen( L"/>\r\n" ), &cbWritten );
	}
}

void EndNode( IStream *xml, WCHAR* wszName, bool bHasChildren )
{
	ULONG cbWritten;

	// there's only an end node tag if there were children...
	if( bHasChildren )
	{
		// indent to the current level and reduce the level for following nodes
		Indent( xml, --indent );

		xml->Write( (void*)L"</", StrByteLen( L"</" ), &cbWritten );
		xml->Write( (void*)wszName, StrByteLen( wszName ), &cbWritten );
		xml->Write( (void*)L">\r\n", StrByteLen( L">\r\n" ), &cbWritten );
	}
	else
	{
		indent--;
	}
}
