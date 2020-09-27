#pragma once

#define N		 4096	// size of ring buffer
#define F		   18	// upper limit for match_length
#define THRESHOLD	2   // encode string into position and length if match_length is greater than this
#define NIL			N	// index for root of binary search trees
#define SPACE	' '


class CLZSS
{
public:
	CLZSS();

	virtual int ReadByte() = 0; // return EOF (-1) at end
	virtual void WriteByte(BYTE b) = 0;

	void Decode();

#ifndef _LZSS_NO_ENCODER
	void Encode();
#endif

private:
	BYTE text_buf [N + F - 1];

#ifndef _LZSS_NO_ENCODER
	void InitTree();
	void InsertNode(int r);
	void DeleteNode(int p);

	UINT textsize;
	UINT codesize;

	int match_position;
	int match_length;

	int lson [N + 1];
	int rson [N + 257];
	int dad [N + 1];
#endif
};

