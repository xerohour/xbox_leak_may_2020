Graphics memory integrity test -- vigil.exe
--------------------------------------------------------------------

Usage:

    vigil

Description:

    This test allocates nearly all available memory (from the lower
    64M on 128M development kits) and uses it for resources in the
    form of vertex buffers, textures, and index lists.  All
    allocated resources are read by the GPU every frame to build an
    image in the frame buffer.  The frame buffer (color and alpha)
    and depth buffer (z and stencil) are written to every frame by
    the GPU.  The CPU reads from all resource memory as well as
    front, back, and depth buffers every frame and verfies the
    memory contents have not changed since the time of initialization.

Environment variable options:

    VIGILVERBOSE            - Output detailed debugging information.

    VIGILNORESOURCECHECK    - Only verify the frame buffers and
                              depth buffer each frame.  Errors in
                              the textures, vertex buffers, and
                              index lists will still likely propagate 
                              to the frame buffer and cause errors to
                              be reported, but the specific resources
                              with corrupt memory will not be 
                              identified until an error first appears
                              in the frame buffer.  Compiling with
                              this option will significantly boost
                              the frame rate.

    VIGILNOTEXTUREWRITES    - Do not perform GPU writes to all created
                              textures every frame.  If this option is
                              not defined, the system will perform a GPU
                              copy of the memory from all textures into
                              a temporary buffer, a CPU clear of the
                              textures, and another GPU copy of the
                              memory from the temporary buffer back
                              into the textures to restore them.

    VIGILGENERATECRCS       - Generate the CRC file used to verify
                              the frame buffer, depth buffer, and
                              resources.
