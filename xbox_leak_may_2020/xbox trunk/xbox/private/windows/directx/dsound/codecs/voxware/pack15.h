#ifndef PACK15_H
#define PACK15_H

unsigned short VoxPackVR15(void *pvQuan_mblk,unsigned char *PacketBuffer,
				unsigned short *ByteOffset,  unsigned char *BitOffset,
            unsigned short BufferSize);


unsigned short VoxUnPackVR15(void *pvQuan_mblk, unsigned char *PacketBuffer,
				unsigned short *ByteOffset,  unsigned char *BitOffset,
            unsigned short BufferSize);

#endif /* PACK15_H */



