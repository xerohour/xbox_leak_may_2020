// ProjWriter.h

#ifndef _PROJWRITER_H_
#define _PROJWRITER_H_

// append a string that has quotation marks in it to another string
void WriteString( BSTR *bstrScript, BSTR bstrQuotedStr );
// append a string that has quotation marks in it to another string, with
// surrounding quotation marks
void WriteQuotedString( BSTR *bstrScript, BSTR bstrQuotedStr );
// append a string that contains multiple lines (and quotes) to another string, with
// surrounding quotation marks
void WriteMultiLineString( BSTR *bstrScript, BSTR bstrMultiLineStr );

// XML writer routines
int GetIndent();
void SetIndent(int nNewIndent);
void StartNodeHeader( IStream *xml, WCHAR* bstrName, bool bHasChildren );
void NodeAttribute( IStream *xml, WCHAR* bstrName, BSTR bstrValue );
void NodeAttributeWithSpecialChars( IStream *xml, WCHAR* bstrName, BSTR bstrValue, bool bFilenameChars = false );
void EndNodeHeader( IStream *xml, bool bHasChildren );
void EndNode( IStream *xml, WCHAR* bstrName, bool bHasChildren );

#endif // end include fence
