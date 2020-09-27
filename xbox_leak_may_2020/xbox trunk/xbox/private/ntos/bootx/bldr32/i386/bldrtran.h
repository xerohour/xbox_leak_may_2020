
VOID
BldrEncoderTranslate(
    UCHAR i0, 
    UCHAR i1, 
    UCHAR i2, 
    UCHAR i3,
    UCHAR* o0,
    UCHAR* o1
    )
{
    *o0 = i0 + i1;
    *o1 = i2 + i3;
}

