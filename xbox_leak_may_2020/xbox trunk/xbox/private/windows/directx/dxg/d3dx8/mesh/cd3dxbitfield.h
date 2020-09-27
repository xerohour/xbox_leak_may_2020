
#define BITSPERINT      32
#define BITSHIFT        5 // 2^5 == 32 bits == 1 DWORD
#define BITFIELDSIZE    8 // 8 * 32 = 256 bits

class CD3DXBitField256
{
    DWORD bits[BITFIELDSIZE];
    static const DWORD nibbleCount[];

public:
    CD3DXBitField256()
    {
        memset(bits, 0, BITFIELDSIZE * sizeof(DWORD));
    }

    void Set(DWORD bit)
    {
        bits[bit >> BITSHIFT] |= 1 << (bit & (BITSPERINT - 1));
    }

    BOOL IsSet(DWORD bit)
    {
        return bits[bit >> BITSHIFT] & (1 << (bit & (BITSPERINT - 1)));
    }

    BOOL IsSubset(CD3DXBitField256& sub)
    {
        if (((bits[0] & sub.bits[0]) == sub.bits[0]) &&
            ((bits[1] & sub.bits[1]) == sub.bits[1]) &&
            ((bits[2] & sub.bits[2]) == sub.bits[2]) &&
            ((bits[3] & sub.bits[3]) == sub.bits[3]) &&
            ((bits[4] & sub.bits[4]) == sub.bits[4]) &&
            ((bits[5] & sub.bits[5]) == sub.bits[5]) &&
            ((bits[6] & sub.bits[6]) == sub.bits[6]) &&
            ((bits[7] & sub.bits[7]) == sub.bits[7]))
            return TRUE;
        return FALSE;
    }

    void Clear(DWORD bit)
    {
        bits[bit >> BITSHIFT] &= ~(1 << (bit & (BITSPERINT - 1)));
    }

    void Or(CD3DXBitField256& src1, CD3DXBitField256& src2)
    {
        bits[0] = src1.bits[0] | src2.bits[0];
        bits[1] = src1.bits[1] | src2.bits[1];
        bits[2] = src1.bits[2] | src2.bits[2];
        bits[3] = src1.bits[3] | src2.bits[3];
        bits[4] = src1.bits[4] | src2.bits[4];
        bits[5] = src1.bits[5] | src2.bits[5];
        bits[6] = src1.bits[6] | src2.bits[6];
        bits[7] = src1.bits[7] | src2.bits[7];
    }

    void Xor(CD3DXBitField256& src1, CD3DXBitField256& src2)
    {
        bits[0] = src1.bits[0] ^ src2.bits[0];
        bits[1] = src1.bits[1] ^ src2.bits[1];
        bits[2] = src1.bits[2] ^ src2.bits[2];
        bits[3] = src1.bits[3] ^ src2.bits[3];
        bits[4] = src1.bits[4] ^ src2.bits[4];
        bits[5] = src1.bits[5] ^ src2.bits[5];
        bits[6] = src1.bits[6] ^ src2.bits[6];
        bits[7] = src1.bits[7] ^ src2.bits[7];
    }
    void And(CD3DXBitField256& src1, CD3DXBitField256& src2)
    {
        bits[0] = src1.bits[0] & src2.bits[0];
        bits[1] = src1.bits[1] & src2.bits[1];
        bits[2] = src1.bits[2] & src2.bits[2];
        bits[3] = src1.bits[3] & src2.bits[3];
        bits[4] = src1.bits[4] & src2.bits[4];
        bits[5] = src1.bits[5] & src2.bits[5];
        bits[6] = src1.bits[6] & src2.bits[6];
        bits[7] = src1.bits[7] & src2.bits[7];
    }
    DWORD Count()
    {
        DWORD   cOnes   = 0;                                    
                                                                
        for (DWORD i = 0; i < 8; i++)                           
        {                                                       
            DWORD   n   = bits[i];                              
                                                                
            while (n)                                           
            {                                                   
                n &= ~(n & ~(n - 1));                           
                                                                
                cOnes++;                                        
            }                                                   
        }                                                       
                                                                
        return cOnes;                                           
    }
};