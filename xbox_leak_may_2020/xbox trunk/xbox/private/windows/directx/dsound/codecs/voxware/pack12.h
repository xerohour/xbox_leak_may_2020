#ifndef PACK12_H
#define PACK12_H

unsigned short VoxPackVR12(void *pvQuan_mblk,unsigned char *PacketBuffer,
				unsigned short *ByteOffset,  unsigned char *BitOffset,
            unsigned short BufferSize);


unsigned short VoxUnPackVR12(void *pvQuan_mblk, unsigned char *PacketBuffer,
				unsigned short *ByteOffset,  unsigned char *BitOffset,
            unsigned short BufferSize);

#endif /* PACK12_H */



