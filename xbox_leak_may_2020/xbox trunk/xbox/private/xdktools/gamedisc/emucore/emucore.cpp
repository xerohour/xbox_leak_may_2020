
HRESULT StartEmulating(void)
{
    HRESULT hr;

    /* If we're already emulating, we can't restart */
    if(g_pfsys)
        return E_INVALIDARG;
       
    /* Start up IDE */
    g_pfsys = NULL;
    hr = StartIDE();
    
    /* We're good to go */
    return hr;
}

HRESULT StopEmulating(void)
{
    /* Shut down the worker thread if it's out there */
    StopIDE();
    g_pfsys = NULL;
    return S_OK;
}
