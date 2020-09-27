NV_ESCAPE_ID and NV_ESCAPE_ID_RETURN define a GDI ESCAPE to verify what driver
is currently running.  IHV's that receive display driver source code should
substitute their own private mechanism.  Note that it is a WHQL failure to have
IHV property pages and NVIDIA property pages active at the same time.
  
  
IHV's that do not receive display driver source code should define the
following in NVVER.H, and also make sure that the filenames for the control
panel binaries are the same as NVIDIA's (this prevents the two sets of pages
being active at the same time).

#define NV_ESCAPE_ID                'nv'
#define NV_ESCAPE_ID_RETURN         0x3031564E  // "NV10"
