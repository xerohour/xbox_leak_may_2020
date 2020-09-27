/*
 * xlat.c
 *
 * Translate
 */

#include "xonp.h"
#include "xonver.h"

void NEAR init_decoder_translation(t_decoder_context *context)
{
	context->dec_instr_pos = 0;
}

void NEAR decoder_translate_e8(t_decoder_context *context, byte *mem, long bytes)
{
	ulong   end_instr_pos;
	byte    temp[6];
	byte    *mem_backup;

	if (bytes <= 6)
	{
		context->dec_instr_pos += bytes;
		return;
	}

	mem_backup = mem;

	/* backup these bytes */
	memcpy(temp, &mem[bytes-6], 6);

	/* overwrite them with 0xE8 */
	memset(&mem[bytes-6], 0xE8, 6);

	end_instr_pos = context->dec_instr_pos + bytes - 6;

	while (1)
	{
		unsigned long   absolute;

		/*
		 * We are guaranteed to hit one of the 6 0xE8's we stuck at the
		 * end of the buffer, even if we ran into some corrupted data
		 * that resulted in our jumping over 5 bytes due to a translation
		 */
		while (*mem++ != 0xE8)
			context->dec_instr_pos++;

		if (context->dec_instr_pos >= end_instr_pos)
			break;

		/*
		 * There are 5 or more bytes in the buffer
		 * (i.e. E8 xx xx xx xx)
		 *
		 * We have a complete offset available to (potentially) translate
		 */
		absolute = *(UNALIGNED ulong *) mem;

		if (absolute < context->dec_current_file_size)
		{
			/* absolute >= 0 && absolute < dec_current_file_size */

			*(UNALIGNED ulong *) mem = absolute - context->dec_instr_pos;
		}
		else if ((ulong) (-(long) absolute) <= context->dec_instr_pos)
		{
			/* absolute >= -instr_pos && absolute < 0 */
			*(UNALIGNED ulong *) mem = absolute + context->dec_current_file_size;
		}

		mem += 4;
		context->dec_instr_pos += 5;
	}

	context->dec_instr_pos = end_instr_pos + 6;

	/* restore these bytes */
	memcpy(&mem_backup[bytes-6], temp, 6);
}

