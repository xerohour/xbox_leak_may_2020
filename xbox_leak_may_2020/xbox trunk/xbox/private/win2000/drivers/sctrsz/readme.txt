Sector Size Converter:

This code builds with the Windows 2000 DDK.  It produces the driver Sctrsize.sys.  At this time there is no inf
file for this driver and had been tested by manually changed the registry.

The intent of this driver is to enable the use of mass storage devices with block sizes greater than 512 bytes by
converting the block size to 512 bytes.  The code is intended for fixed-media devices and may have difficulty
with removal media.  Particularly, if different configurations of removal media are allowed.

The driver should be installed as a lower-filter driver for disk.sys.  

For simplicity of development the driver is based off of the generic filter.sys sample in the Windows 2000 DDK.

The filter does all filtering in a worker thread which is created during IRP_MN_START_DEVICE. IRPs with SRB requests
SCSIOP_READ_CAPACITY, SCSIOP_READ,and SCSIOP_WRITE are queued to a single queue and pulled off in FIFO order
by the worker thread which handles IRPs synchronously.

The first time a SCSIOP_READ_CAPACITY is encountered it is forward down the stack.  On the way back up,
the driver records the block size of the device and allocates a buffer of the same size for use during
SCSIOP_READ and SCSIOP_WRITE.  The block size is changed to 512 bytes increasing the number of blocks
appriopriately before completing the IRP.  Subsequent SCSIOP_READ_CAPACITY IRPs are completed without forwarding
first.

SCSIOP_READ and SCSIOP_WRITE prior to the first SCSIOP_READ_CAPACITY are failed with a device not ready error.

All SCSIOP_READ and SCSIOP_WRITE requests are broken into three areaa:  An intial number of sectors that
do not begin on a block alignment, a contiguous middle number of sectors that begin and end on a block
alignment, and a final number of sectors that do not end on a block alignment.

For SCSIOP_READ the first group of sectors is read and the copied into the caller's buffer.  The middle
sectors are read straight into the caller's buffers, and the final group of sectors is read and copied
into the caller's buffer.

For SCSIOP_WRITE the first and last group of sectors must first read the whole block containing the target sectors.
Copy over the sectors to be modified and then write out the whole block.  The middle group of sectors is
written directly from the caller's buffer.

TESTING STATUS:

The driver was tested and works in transparent mode (i.e. where the block size is 512 bytes).
I did not have working hardware to test the filter with the intended 8k blocks.  I verified through CATC and
stepping through the code with a debugger that the remaining problems appear to have been with the firmware
I had.  It was not properly updating the sections of blocks beyond the first 512 bytes.
