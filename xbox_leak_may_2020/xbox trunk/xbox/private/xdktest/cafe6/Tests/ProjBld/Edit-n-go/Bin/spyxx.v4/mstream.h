//-----------------------------------------------------------------------------
//  mstream.h
//
//  Copyright (C) 1993, Microsoft Corporation
//
//  Purpose:
//
//  Header for Spy Message Streams.
//
//  Notes:
//
//  Message streams are initialized with the fill character as '0'
//  and the case for printing hex values as "ios::uppercase".  This
//  is because it is so commonly used to print dword values in hex,
//  and our format for that is a width of 8, zero padded and in hex.
//  This is different than standard C++ streams, which default to
//  a fill character of ' ' and lowercase for alphabetic hex digits.
//
//  DWORD values are always printed as 8 digit hex values (ex. "0010FF4E").
//  To print a value in this format, simply cast it to a DWORD in the stream.
//
//  INT's are always printed in decimal.
//
//  mstream is the base class.  It contains the support for formatting
//  different data types properly.
//
//  CMsgStream is a derived class that writes to the CMsgLog
//  output windows.  It is used to write out the messages that
//  get logged there.
//
//  CMsgParmStream is another derived class that writes to the listbox
//  in the Property Inspector.  It is used to write out the message
//  parameters in that dialog, specifically details about parameters
//  that are pointers to structures (captured by the hook).
//
//  Revision History:
//
//  04/02/93 byrond Created.
//
//-----------------------------------------------------------------------------

#include <iostream.h>
#include <strstrea.h>
#include <iomanip.h>



#define MMANIP(T) __MMANIP_##T
#define MAPP(T) __MAPP_##T

#define MMANIPdeclare(T)  \
class MMANIP(T) { \
public: \
	MMANIP(T)(mstream& (*f)(mstream&,T), T t) { _fp = f; _tp = t; } \
	friend mstream& operator<<(mstream& s, MMANIP(T) & sm) { (*sm._fp)(s,sm._tp); return s; } \
private:	\
	mstream& (* _fp)(mstream&,T); \
	T _tp; \
};  \
class MAPP(T) { \
public: \
	MAPP(T)(mstream& (*f)(mstream&,T)) { _fp = f; } \
	MMANIP(T) operator()(T t) { return MMANIP(T)(_fp,t); } \
private:	\
	mstream& (* _fp)(mstream&,T); \
};


class mstream: public ostrstream
{
	public:
		mstream() : ostrstream()
		{
			//
			// By default, mstreams pad with zeros and print
			// hex digits in uppercase.
			//
			this->fill('0');
			this->setf(ios::uppercase);
		}

		mstream& operator<<(HWND hwnd);

		mstream& operator<<(int i)
		{
			return (mstream&)ostrstream::operator<<(i);
		}

		mstream& operator<<(unsigned int ui)
		{
			return (mstream&)ostrstream::operator<<(ui);
		}

		mstream& operator<<(long l)
		{
			return (mstream&)ostrstream::operator<<(l);
		}

		mstream& operator<<(WORD w);	   // unsigned short.
		mstream& operator<<(DWORD dw);	 // unsigned long.

		mstream& operator<<(char c)
		{
			return (mstream&)ostrstream::operator<<(c);
		}

		mstream& operator<<(unsigned char uc)
		{
			return (mstream&)ostrstream::operator<<(uc);
		}

		mstream& operator<<(char * psz)
		{
			return (mstream&)ostrstream::operator<<(psz);
		}

		inline mstream& operator<<(mstream& (*f)(mstream&))
		 { (void)(*f)(*this); return *this; }
		inline mstream& operator<<(ostrstream& (*f)(ostrstream&))
		 { (void)(*f)(*this); return *this; }
		inline mstream& operator<<(ostream& (*f)(ostream&))
		 { (void)(*f)(*this); return *this; }
		inline mstream& operator<<(ios& (*f)(ios&))
		 { (void)(*f)(*this); return *this; }
};



class CMsgView;

class CMsgStream: public mstream
{
	public:
		void SetView(CMsgView *pMsgView)
		{
			m_pMsgView = pMsgView;
		}

		void SetMSDPointer(PMSGSTREAMDATA pmsd)
		{
			m_pmsd = pmsd;
		}

		PMSGSTREAMDATA GetMSDPointer()
		{
			return m_pmsd;
		}

		void EndLine();

	private:
		CMsgView *m_pMsgView;
		PMSGSTREAMDATA m_pmsd;
};




class CMsgParmStream: public mstream
{
	public:
		CMsgParmStream() : mstream()
		{
			m_nIndent = 0;
			m_cxMaxWidth = 0;
		}

	public:
		void SetOutputLB(CListBox *pListBox)
		{
			m_pListBox = pListBox;
		}

		void EndLine();

		void Indent()
		{
			m_nIndent++;
		}

		void Outdent()
		{
			ASSERT(m_nIndent);
			m_nIndent--;
		}

		int GetMaxLineWidth()
		{
			return m_cxMaxWidth;
		}

	private:
		CListBox *m_pListBox;
		int m_nIndent;
		int m_cxMaxWidth;
};





