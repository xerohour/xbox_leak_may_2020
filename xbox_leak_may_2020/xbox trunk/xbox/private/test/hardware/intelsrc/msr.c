/****************************************
*                                       *
*     MSR Read/Write Utility            *
*     Russell Smith                     *
*     Intel.OPSD.BVL                    *
*     last edit: 26-Feb-99              *
*                                       *
*****************************************/



#include "stdio.h"
#include "stdlib.h"
#include "conio.h"
#include "time.h"
#include "process.h"
#include "dos.h"
#include "mem.h"
#include "ctype.h"
#include "string.h"


#define FALSE 0
#define TRUE 1
#define uchar unsigned char
#define uint unsigned int

#define out_of_range(bit) (bit>63) || (bit<0)

enum {READ, WRITE};
enum {FULL, SINGLE, RANGE};

// function prototypes

char top_level_menu(void);
void banner(void);
char usage(void);
char bad_range(void);
char bad_order(void);
char bad_syntax(void);
char bad_data(void);
void rdmsr(long addess, char *where);
void wrmsr(long addess, char *mask, char *bits);
char *get_psp_command_tail();
char pattern_match(char *text, char *pattern);
char scan_digits(char **ptr, char radix);
char isadigit(char ch, char howmany);
void display_full_msr(char *buffer, char base);
void display_binary(char *buffer, char left, char right);
void display_hex(char *buffer);
char bit_extract(char *buffer, char bitposition);
void bit_insert(char *buffer, char bitposition, char bit);
void shift_left_qword(char *buffer, char bit);
void shift_right_qword(char *buffer, char bit);
void set_attrib(int row, int col, char background, char foreground);

// global variables

char *temp, *delim;
int n, ch;
char type = FULL;
char direction = READ;
char base = 16;
unsigned long msr_address;
char leftbit, rightbit, singlebit;
char msr_buffer[8], input_buffer[8];
char and_buffer[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
char or_buffer[8] = {0,0,0,0,0,0,0,0};
	


	
void main(int argc, char *argv[])
{
	if (argc<2) exit(usage());  //just show usage message if null command line
/*	
  	get pointer to DOS command tail	and do a rigorous syntax check for all possible
  	valid command lines that fit the format "address [bit range] [=data] [/b]". uses
  	template matching routine pattern_match to scan for legal combinations of hex,
  	decimal and binary numbers, delimiters and switch characters.
*/
	temp = get_psp_command_tail();
	if (!(pattern_match(temp, "X"          ) ||
		  pattern_match(temp, "X /b"       ) ||
	      pattern_match(temp, "X=X"        ) ||
		  pattern_match(temp, "X=B /b"     ) ||
		  pattern_match(temp, "X D"        ) ||
		  pattern_match(temp, "X D=B"      ) ||
		  pattern_match(temp, "X D:D"      ) ||
		  pattern_match(temp, "X D:D /b"   ) ||
		  pattern_match(temp, "X D:D=X"    ) ||
		  pattern_match(temp, "X D:D=B /b" ))) exit(bad_syntax());
/*
	check last arg for /b switch. change radix to 2 from 16 and decrement argc if found.
*/	 	
	delim = strchr(argv[argc-1], '/');
	if (delim)
	{
		if (toupper(*(++delim)) == 'B') base = 2;
		argc--;
	}
/*
	parse first arg as hex MSR address.
*/
	msr_address = strtol(argv[1], &temp, 16);
/*
	check second arg for bit range specifier. if colon found, parse decimal numbers on
	both sides and check for legal values (0..63) and precedence (left# bigger than right#).
	if no colon, just parse a single decimal number.
*/
	if (argc == 3)
	{
		delim = strchr(argv[2], ':');
		if (delim)
		{
			leftbit = strtol(argv[2], &temp, 10);
			rightbit = strtol(delim+1, &temp, 10);
			type = RANGE;
			if (out_of_range(leftbit) || out_of_range(rightbit)) exit(bad_range());
			if (rightbit>=leftbit) exit(bad_order());
		}
		else
		{
			singlebit = strtol(argv[2], &temp, 10);
			type = SINGLE;
			if (out_of_range(singlebit)) exit(bad_range());
		}
	}
/*
	check last arg for write specifier (equals sign). if found parse the data according
	to binary/hex setting and verify that it will fit in MSR bit field specified.
*/
	delim = strchr(argv[argc-1], '=');
	if (delim++)
	{
		if (base==2)
		{
			while(isadigit(ch = *delim++, 2))
				shift_left_qword(input_buffer, ch);
		}
		else
		{
			while(isadigit(ch = *delim++, 16))
			{
				if (ch > '9') ch += 9;
				shift_left_qword(input_buffer, ch>>3);
				shift_left_qword(input_buffer, ch>>2);
				shift_left_qword(input_buffer, ch>>1);
				shift_left_qword(input_buffer, ch);
			}
		}
		direction = WRITE;
		for (n=63; n>0; n--)
		{
			if (bit_extract(input_buffer, n)) break;
		}
		if ((type == SINGLE) && (n > 0)) exit(bad_data());
		if ((type == RANGE) && (n > (leftbit-rightbit))) exit(bad_data());
	}
	
//	finally finished parsing, now really do something

	if (direction == READ)
	{
		rdmsr(msr_address, msr_buffer);
		switch(type)
		{
			case FULL:
					display_full_msr(msr_buffer, base);
					break;
			case SINGLE:
					printf("%d", bit_extract(msr_buffer, singlebit));
					break;
			case RANGE:
					for (n=leftbit; n>=rightbit; n--)
						shift_left_qword(or_buffer, bit_extract(msr_buffer, n));
					if (base==2)
						display_binary(or_buffer, leftbit-rightbit, 0);
					else
						display_hex(or_buffer);
					break;
		}
	}
	else
	{
		switch(type)
		{
			case FULL:
					wrmsr(msr_address, and_buffer, input_buffer);
					display_full_msr(input_buffer, base);
					break;
			case SINGLE:
					bit_insert(and_buffer, singlebit, 0);
					bit_insert(or_buffer, singlebit, input_buffer[0] & 1);
					wrmsr(msr_address, and_buffer, or_buffer);
					display_full_msr(or_buffer, base);
					break;
			case RANGE:
					for (n=rightbit; n<=leftbit; n++)
					{
						bit_insert(and_buffer, n, 0);
						bit_insert(or_buffer, n, input_buffer[0] & 1);
						shift_right_qword(input_buffer, 0);
					}
					wrmsr(msr_address, and_buffer, or_buffer);
					display_full_msr(or_buffer, base);
					break;
		}
	}
	exit(0);
 }


void banner(void)
{
	printf("ษอออออออออออออออออออออออออออออออออออออออออออป\n");
	printf("บ        MSR Read/Write Utility  1.0        บ\n");
	printf("บ                                           บ\n");
	printf("บ        OPSD Board Validation Lab          บ\n");
	printf("บ     copyright (c) Intel Corp. 1999        บ\n");
	printf("ศอออออออออออออออออออออออออออออออออออออออออออผ\n\n");
}


char usage(void)
{
	banner();
	printf("Usage:   MSR address [bit range] [=data] [/b] \n\n");
	printf("         MSR address                  display contents of MSR \n");
	printf("         MSR address=data             write data to MSR \n");
	printf("         MSR address bit#             display single bit of MSR \n");
	printf("         MSR address bit#=data        write data to single bit in MSR \n");
	printf("         MSR address bit#:bit#        display contents of bit range of MSR \n");
	printf("         MSR address bit#:bit#=data   write data to bit range in MSR \n");
	printf("note: \n");
	printf("         data is entered and displayed as hexadecimal \n");
	printf("         by default or binary if /b switch present. \n");
	printf("         bit positions are always specified as decimal. \n");
	return 255;
}

char bad_range(void)
{
	banner();
	printf("invalid bit# specified, must be between 0 and 63\n");
	return 255;
}


char bad_order(void)
{
	banner();
	printf("left bit# must be greater than right bit#\n");
	return 255;
}


char bad_data(void)
{
	banner();
	printf("write data is too big for bit field specified\n");
	return 255;
}


char bad_syntax(void)
{
	banner();
	printf("syntax error in command line. type MSR<cr> to see usage screen\n");
	return 255;
}


void display_full_msr(char *buffer, char base)
{
	char n;
	
	if (base==2)
	{
		for (n=63; n >= 0; n--)
		{
			printf("%d", bit_extract(buffer, n));    //display in binary
			set_attrib(wherey(), wherex()-1, BLACK, (n&8)?LIGHTGRAY:WHITE);
		}
	}
	else
		for (n=7; n >= 0; n--)
			printf("%02X", (uchar)buffer[n]);    //display in hexadecimal
}



void set_attrib(int row, int col, char background, char foreground)
{
	char buffer[2];
	
	gettext(col, row, col, row, buffer);
    buffer[1] = foreground + (background << 4);
	puttext(col, row, col, row, buffer);
}


void display_binary(char *buffer, char left, char right)
{
	char n;
	
	for (n=left; n >= right; n--)
		printf("%d", bit_extract(buffer, n));
}



void display_hex(char *buffer)
{
	char n, ch;
	char done_non_zero_digit = FALSE;
	
	for (n=0; n < 15; n++)
	{
		ch = (buffer[7] >> 4) & 0x0F;
		if (ch)
		{
			printf("%X", ch);
			done_non_zero_digit = TRUE;
		}
		else
		{
			if (done_non_zero_digit)
				printf("0");
		}
		shift_left_qword(buffer, 0);
		shift_left_qword(buffer, 0);
		shift_left_qword(buffer, 0);
		shift_left_qword(buffer, 0);
	}
	printf("%X", (buffer[7] >> 4) & 0x0F);
}



char bit_extract(char *buffer, char bitposition)
{
	if (buffer[bitposition >> 3] & (1 << (bitposition & 7)))
		return 1;
	else
		return 0;
}



void bit_insert(char *buffer, char bitposition, char bit)
{
	char index, mask;
	
	index = bitposition >> 3;
	mask = 1 << (bitposition & 7);
	buffer[index] &= ~mask;
	if (bit)
		buffer[index] |= mask;
}


char pattern_match(char *text, char *pattern)
{
	char ch;
	
	while(ch = *pattern++)
	{
		switch(ch)
		{
			case 'X':	if (!scan_digits(&text, 16))         return FALSE; break;
			case 'D':	if (!scan_digits(&text, 10))         return FALSE; break;
			case 'B':	if (!scan_digits(&text,  2))         return FALSE; break;
			default:	if (toupper(*text++) != toupper(ch)) return FALSE;
		}
	}
	if (*text)
		return FALSE;
	else
		return TRUE;
}


char scan_digits(char **ptr, char radix)
{
	char *start;

	start = *ptr;
	while (isadigit(**ptr, radix)) (*ptr)++;
	if (start == *ptr)
		return FALSE;
	else
		return TRUE;
}


char isadigit(char ch, char howmany)
{
	char n;
	static char lookup[]="0123456789ABCDEF";
	
	for (n=0; n<howmany; n++)
	{
		if (toupper(ch) == lookup[n]) return TRUE;	
	}
	return FALSE;
}