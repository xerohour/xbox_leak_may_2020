#include <stdafx.h>
#include "lzss.h"

/*
This scheme is initiated by Ziv and Lempel [1].  A slightly modified
version is described by Storer and Szymanski [2].  An implementation
using a binary tree is proposed by Bell [3].  The algorithm is quite
simple: Keep a ring buffer, which initially contains "space" characters
only.  Read several letters from the file to the buffer.  Then search
the buffer for the longest string that matches the letters just read,
and send its length and position in the buffer.

If the buffer size is 4096 bytes, the position can be encoded in 12
bits.  If we represent the match length in four bits, the <position,
length> pair is two bytes long.  If the longest match is no more than
two characters, then we send just one character without encoding, and
restart the process with the next letter.  We must send one extra bit
each time to tell the decoder whether we are sending a <position,
length> pair or an unencoded character.
*/

CLZSS::CLZSS()
{
	textsize = 0;
	codesize = 0;
}

#ifndef _LZSS_NO_ENCODER
void CLZSS::InitTree()
{
	// For i = 0 to N - 1, rson[i] and lson[i] will be the right and
	// left children of node i.  These nodes need not be initialized.
	// Also, dad[i] is the parent of node i.  These are initialized to
	// NIL (= N), which stands for 'not used.'
	// For i = 0 to 255, rson[N + i + 1] is the root of the tree
	// for strings that begin with character i.  These are initialized
	// to NIL.  Note there are 256 trees.

	int  i;

	for (i = N + 1; i <= N + 256; i += 1)
		rson[i] = NIL;

	for (i = 0; i < N; i += 1)
		dad[i] = NIL;
}

void CLZSS::InsertNode(int r)
{
	// Inserts string of length F, text_buf[r..r+F-1], into one of the
	// trees (text_buf[r]'th tree) and returns the longest-match position
	// and length via the global variables match_position and match_length.
	// If match_length = F, then removes the old node in favor of the new
	// one, because the old one will be deleted sooner.
	// Note r plays double role, as tree node and position in buffer.

	int i, p, cmp;
	BYTE* key;

	cmp = 1;
	key = &text_buf[r];
	p = N + 1 + key[0];
	rson[r] = lson[r] = NIL;
	match_length = 0;

	for (;;)
	{
		if (cmp >= 0)
		{
			if (rson[p] != NIL)
			{
				p = rson[p];
			}
			else
			{
				rson[p] = r;
				dad[r] = p;
				return;
			}
		}
		else
		{
			if (lson[p] != NIL)
			{
				p = lson[p];
			}
			else
			{
				lson[p] = r;
				dad[r] = p;
				return;
			}
		}

		for (i = 1; i < F; i++)
		{
			cmp = key[i] - text_buf[p + i];
			if (cmp != 0)
				break;
		}
		
		if (i > match_length)
		{
			match_position = p;
			if ((match_length = i) >= F)
				break;
		}
	}

	dad[r] = dad[p];
	lson[r] = lson[p];
	rson[r] = rson[p];
	dad[lson[p]] = r;
	dad[rson[p]] = r;

	if (rson[dad[p]] == p)
		rson[dad[p]] = r;
	else
		lson[dad[p]] = r;

	dad[p] = NIL;
}

void CLZSS::DeleteNode(int p)
{
	int  q;
	
	if (dad[p] == NIL)
		return;

	if (rson[p] == NIL)
	{
		q = lson[p];
	}
	else if (lson[p] == NIL)
	{
		q = rson[p];
	}
	else
	{
		q = lson[p];
		if (rson[q] != NIL)
		{
			do
			{
				q = rson[q];
			}
			while (rson[q] != NIL);

			rson[dad[q]] = lson[q];
			dad[lson[q]] = dad[q];
			lson[q] = lson[p];
			dad[lson[p]] = q;
		}

		rson[q] = rson[p];
		dad[rson[p]] = q;
	}

	dad[q] = dad[p];
	if (rson[dad[p]] == p)
		rson[dad[p]] = q;
	else
		lson[dad[p]] = q;
	dad[p] = NIL;
}

void CLZSS::Encode()
{
	int i, c, len, r, s, last_match_length, code_buf_ptr;
	BYTE code_buf [17], mask;
	
	InitTree();  /* initialize trees */
	code_buf[0] = 0;  /* code_buf[1..16] saves eight units of code, and
		code_buf[0] works as eight flags, "1" representing that the unit
		is an unencoded letter (1 byte), "0" a position-and-length pair
		(2 bytes).  Thus, eight units require at most 16 bytes of code. */
	code_buf_ptr = mask = 1;
	s = 0;
	r = N - F;
	for (i = s; i < r; i += 1)
		text_buf[i] = SPACE;  // Clear the buffer with any character that will appear often.
	for (len = 0; len < F && (c = ReadByte()) != -1; len += 1)
		text_buf[r + len] = (BYTE)c;  // Read F bytes into the last F bytes of the buffer

	textsize = len;
	if (len == 0)
		return;

	// Insert the F strings, each of which begins with one or more 'space' characters.
	// Note the order in which these strings are inserted.  This way, degenerate trees
	// will be less likely to occur.
	for (i = 1; i <= F; i++)
		InsertNode(r - i);

	// Finally, insert the whole string just read.  The variables match_length and match_position are set.
	InsertNode(r);
	
	do
	{
		if (match_length > len)
			match_length = len;  // match_length may be spuriously long near the end of text.

		if (match_length <= THRESHOLD)
		{
			match_length = 1;  // Not long enough match.  Send one byte.
			code_buf[0] |= mask;  // 'send one byte' flag
			code_buf[code_buf_ptr++] = text_buf[r];  // Send uncoded.
		}
		else
		{
			// Send position and length pair. Note match_length > THRESHOLD.
			code_buf[code_buf_ptr++] = (BYTE)match_position;
			code_buf[code_buf_ptr++] = (BYTE)(((match_position >> 4) & 0xf0) | (match_length - (THRESHOLD + 1)));
		}

		if ((mask <<= 1) == 0)
		{
			// Shift mask left one bit.
			for (i = 0; i < code_buf_ptr; i++)  // Send at most 8 units of code together
				WriteByte(code_buf[i]);

			codesize += code_buf_ptr;
			code_buf[0] = 0;
			code_buf_ptr = mask = 1;
		}

		last_match_length = match_length;
		for (i = 0; i < last_match_length && (c = ReadByte()) != -1; i += 1)
		{
			DeleteNode(s);
			text_buf[s] = (BYTE)c;
			if (s < F - 1)
				text_buf[s + N] = (BYTE)c;
			s = (s + 1) & (N - 1);
			r = (r + 1) & (N - 1);
			InsertNode(r);
		}

		while (i++ < last_match_length)
		{
			DeleteNode(s);
			s = (s + 1) & (N - 1);
			r = (r + 1) & (N - 1);
			len -= 1;
			if (len != 0)
				InsertNode(r);
		}
	}
	while (len > 0);
	
	if (code_buf_ptr > 1)
	{
		// Send remaining code.
		for (i = 0; i < code_buf_ptr; i += 1)
			WriteByte(code_buf[i]);
		codesize += code_buf_ptr;
	}
}
#endif

void CLZSS::Decode()
{
	int i, j, k, r, c;
	UINT flags;
	
	for (i = 0; i < N - F; i++)
		text_buf[i] = SPACE;

	r = N - F;
	flags = 0;
	for (;;)
	{
		if (((flags >>= 1) & 256) == 0)
		{
			if ((c = ReadByte()) == -1)
				break;

			flags = c | 0xff00;
		}
		
		if (flags & 1)
		{
			if ((c = ReadByte()) == -1)
				break;

			WriteByte((BYTE)c);
			text_buf[r++] = (BYTE)c;
			r &= (N - 1);
		}
		else
		{
			if ((i = ReadByte()) == -1)
				break;
			if ((j = ReadByte()) == -1)
				break;

			i |= ((j & 0xf0) << 4);
			j = (j & 0x0f) + THRESHOLD;
			for (k = 0; k <= j; k++)
			{
				c = text_buf[(i + k) & (N - 1)];
				WriteByte((BYTE)c);
				text_buf[r++] = (BYTE)c;
				r &= (N - 1);
			}
		}
	}
}
