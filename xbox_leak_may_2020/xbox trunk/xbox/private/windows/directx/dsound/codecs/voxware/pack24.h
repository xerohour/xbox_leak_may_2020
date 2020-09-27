#ifndef PACK24_H
#define PACK24_H

unsigned short VoxPackRT24(void *pvQuan_mblk,unsigned char *PacketBuffer,
				unsigned short *ByteOffset,  unsigned char *BitOffset,
            unsigned short BufferSize);


unsigned short VoxUnPackRT24(void *pvQuan_mblk, unsigned char *PacketBuffer,
				unsigned short *ByteOffset,  unsigned char *BitOffset,
            unsigned short BufferSize);

#endif /* PACK24_H */



