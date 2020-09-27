#ifndef __NV_EAP_H__
#define __NV_EAP_H__

// software methods for the EP...
#define NV1BAE_PIO_SET_OUTBUF_BA(i)                             (0x00000000+(i)*8) /* -W-4A */
#define NV1BAE_PIO_SET_OUTBUF_BA__SIZE_1                        4 /* */ 
#define NV1BAE_PIO_SET_OUTBUF_BA_ADDRESS                        23:0 /* -W-UF */
#define NV1BAE_PIO_SET_OUTBUF_LEN(i)                            (0x00000004+(i)*8) /* -W-4A */
#define NV1BAE_PIO_SET_OUTBUF_LEN__SIZE_1                       4 /* */ 
#define NV1BAE_PIO_SET_OUTBUF_LEN_VALUE                         23:0 /* -W-UF */
#define NV1BAE_PIO_SET_CURRENT_OUTBUF_SGE                       0x00000028 /* -W-4R */
#define NV1BAE_PIO_SET_CURRENT_OUTBUF_SGE_HANDLE                31:0 /* -W-VF */
#define NV1BAE_PIO_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA           0x0000002C /* -W-4R */
#define NV1BAE_PIO_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA_HANDLE    31:0 /* -W-VF */
#define NV1BAE_PIO_SET_CURRENT_OUTBUF_SGE_OFFSET                0x00000030 /* -W-4R */
#define NV1BAE_PIO_SET_CURRENT_OUTBUF_SGE_OFFSET_PARAMETER      31:12 /* -W-VF */
#define NV1BAE_PIO_SET_STATE                                    0x00000034
#define NV1BAE_PIO_SET_STATE_VALUE                              1:0
#define NV1BAE_PIO_SET_STATE_OFF                                0x00
#define NV1BAE_PIO_SET_STATE_ON                                 0x01
#define NV1BAE_PIO_SET_STATE_DSP_ON                             0x03
#define NV1BAE_PIO_SET_AC3_FIFO                                 0x00000038
#define NV1BAE_PIO_SET_AC3_FIFO_ANALOG_INDEX                    2:0
#define NV1BAE_PIO_SET_AC3_FIFO_DIGITAL_INDEX                   5:3

#endif

