
// cklibver
// Check XBE library versions

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*PLIB_ERROR_FUNCTION)(PXBEIMAGE_LIBRARY_VERSION, int);

int CheckLibraryApprovalStatus(PXBEIMAGE_LIBRARY_VERSION plibvXapi,
    PXBEIMAGE_LIBRARY_VERSION rglibv, int clibv, PLIB_ERROR_FUNCTION pfnErr);

#ifdef __cplusplus
}
#endif
