#ifndef PACK29_H
#define PACK29_H

unsigned short VoxPackRT29(void *pvQuan_mblk,unsigned char *PacketBuffer,
				unsigned short *ByteOffset,  unsigned char *BitOffset,
            unsigned short BufferSize);


unsigned short VoxUnPackRT29(void *pvQuan_mblk, unsigned char *PacketBuffer,
				unsigned short *ByteOffset,  unsigned char *BitOffset,
            unsigned short BufferSize);

#endif /* PACKRT29_H */



