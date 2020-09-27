#ifndef PACK28_H
#define PACK28_H

unsigned short VoxPackRT28(void *pvQuan_mblk,unsigned char *PacketBuffer,
				unsigned short *ByteOffset,  unsigned char *BitOffset,
            unsigned short BufferSize);


unsigned short VoxUnPackRT28(void *pvQuan_mblk, unsigned char *PacketBuffer,
				unsigned short *ByteOffset,  unsigned char *BitOffset,
            unsigned short BufferSize);

#endif /* PACK28_H */



