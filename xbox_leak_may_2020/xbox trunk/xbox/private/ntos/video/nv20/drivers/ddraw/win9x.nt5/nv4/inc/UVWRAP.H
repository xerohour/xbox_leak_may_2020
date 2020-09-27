#define WRAP(uv, wrap)                      \
{                                           \
    if (wrap)                               \
    {                                       \
        float el,er,em;                     \
        el=(float)fabs(uv##2 - uv##0);      \
        er=(float)fabs(uv##1 - uv##2);      \
        em=(float)fabs(uv##0 - uv##1);      \
        if (el > 0.5f && er > 0.5f)         \
        {                                   \
            if (uv##2 < uv##1)              \
            {                               \
                uv##2 += 1.0f;              \
            }                               \
            else                            \
            {                               \
                uv##1 += 1.0f;              \
                uv##0 += 1.0f;              \
            }                               \
        }                                   \
        else if (er > 0.5f && em > 0.5f)    \
        {                                   \
            if (uv##1 < uv##0)              \
            {                               \
                uv##1 += 1.0f;              \
            }                               \
            else                            \
            {                               \
                uv##0 += 1.0f;              \
                uv##2 += 1.0f;              \
            }                               \
        }                                   \
        else if (em > 0.5f && el > 0.5f)    \
        {                                   \
            if (uv##0 < uv##2)              \
            {                               \
                uv##0 += 1.0f;              \
            }                               \
            else                            \
            {                               \
                uv##2 += 1.0f;              \
                uv##1 += 1.0f;              \
            }                               \
        }                                   \
    }                                       \
}
