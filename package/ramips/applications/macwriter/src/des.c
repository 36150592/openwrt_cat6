/*
 *  FIPS-46-3 compliant Triple-DES implementation
 *
 *  Copyright (C) 2006-2007  Christophe Devine
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License, version 2.1 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */
/*
 *  DES, on which TDES is based, was originally designed by IBM in
 *  1974 and adopted as a standard by NIST (formerly NBS).
 *
 *  http://csrc.nist.gov/publications/fips/fips46-3/fips46-3.pdf
 */
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <string.h>

//#include "xyssl/des.h"
#include "des.h"
/*
 * 32-bit integer manipulation macros (big endian)
 */
#if  1//def RSA_DES_CRYPT_FUNC
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ] << 24 )        \
        | ( (unsigned long) (b)[(i) + 1] << 16 )        \
        | ( (unsigned long) (b)[(i) + 2] <<  8 )        \
        | ( (unsigned long) (b)[(i) + 3]       );       \
}
#endif
#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

/*
 * Expanded DES S-boxes
 */
static const unsigned long SB1[64] =
{
    0x01010400, 0x00000000, 0x00010000, 0x01010404,
    0x01010004, 0x00010404, 0x00000004, 0x00010000,
    0x00000400, 0x01010400, 0x01010404, 0x00000400,
    0x01000404, 0x01010004, 0x01000000, 0x00000004,
    0x00000404, 0x01000400, 0x01000400, 0x00010400,
    0x00010400, 0x01010000, 0x01010000, 0x01000404,
    0x00010004, 0x01000004, 0x01000004, 0x00010004,
    0x00000000, 0x00000404, 0x00010404, 0x01000000,
    0x00010000, 0x01010404, 0x00000004, 0x01010000,
    0x01010400, 0x01000000, 0x01000000, 0x00000400,
    0x01010004, 0x00010000, 0x00010400, 0x01000004,
    0x00000400, 0x00000004, 0x01000404, 0x00010404,
    0x01010404, 0x00010004, 0x01010000, 0x01000404,
    0x01000004, 0x00000404, 0x00010404, 0x01010400,
    0x00000404, 0x01000400, 0x01000400, 0x00000000,
    0x00010004, 0x00010400, 0x00000000, 0x01010004
};

static const unsigned long SB2[64] =
{
    0x80108020, 0x80008000, 0x00008000, 0x00108020,
    0x00100000, 0x00000020, 0x80100020, 0x80008020,
    0x80000020, 0x80108020, 0x80108000, 0x80000000,
    0x80008000, 0x00100000, 0x00000020, 0x80100020,
    0x00108000, 0x00100020, 0x80008020, 0x00000000,
    0x80000000, 0x00008000, 0x00108020, 0x80100000,
    0x00100020, 0x80000020, 0x00000000, 0x00108000,
    0x00008020, 0x80108000, 0x80100000, 0x00008020,
    0x00000000, 0x00108020, 0x80100020, 0x00100000,
    0x80008020, 0x80100000, 0x80108000, 0x00008000,
    0x80100000, 0x80008000, 0x00000020, 0x80108020,
    0x00108020, 0x00000020, 0x00008000, 0x80000000,
    0x00008020, 0x80108000, 0x00100000, 0x80000020,
    0x00100020, 0x80008020, 0x80000020, 0x00100020,
    0x00108000, 0x00000000, 0x80008000, 0x00008020,
    0x80000000, 0x80100020, 0x80108020, 0x00108000
};

static const unsigned long SB3[64] =
{
    0x00000208, 0x08020200, 0x00000000, 0x08020008,
    0x08000200, 0x00000000, 0x00020208, 0x08000200,
    0x00020008, 0x08000008, 0x08000008, 0x00020000,
    0x08020208, 0x00020008, 0x08020000, 0x00000208,
    0x08000000, 0x00000008, 0x08020200, 0x00000200,
    0x00020200, 0x08020000, 0x08020008, 0x00020208,
    0x08000208, 0x00020200, 0x00020000, 0x08000208,
    0x00000008, 0x08020208, 0x00000200, 0x08000000,
    0x08020200, 0x08000000, 0x00020008, 0x00000208,
    0x00020000, 0x08020200, 0x08000200, 0x00000000,
    0x00000200, 0x00020008, 0x08020208, 0x08000200,
    0x08000008, 0x00000200, 0x00000000, 0x08020008,
    0x08000208, 0x00020000, 0x08000000, 0x08020208,
    0x00000008, 0x00020208, 0x00020200, 0x08000008,
    0x08020000, 0x08000208, 0x00000208, 0x08020000,
    0x00020208, 0x00000008, 0x08020008, 0x00020200
};

static const unsigned long SB4[64] =
{
    0x00802001, 0x00002081, 0x00002081, 0x00000080,
    0x00802080, 0x00800081, 0x00800001, 0x00002001,
    0x00000000, 0x00802000, 0x00802000, 0x00802081,
    0x00000081, 0x00000000, 0x00800080, 0x00800001,
    0x00000001, 0x00002000, 0x00800000, 0x00802001,
    0x00000080, 0x00800000, 0x00002001, 0x00002080,
    0x00800081, 0x00000001, 0x00002080, 0x00800080,
    0x00002000, 0x00802080, 0x00802081, 0x00000081,
    0x00800080, 0x00800001, 0x00802000, 0x00802081,
    0x00000081, 0x00000000, 0x00000000, 0x00802000,
    0x00002080, 0x00800080, 0x00800081, 0x00000001,
    0x00802001, 0x00002081, 0x00002081, 0x00000080,
    0x00802081, 0x00000081, 0x00000001, 0x00002000,
    0x00800001, 0x00002001, 0x00802080, 0x00800081,
    0x00002001, 0x00002080, 0x00800000, 0x00802001,
    0x00000080, 0x00800000, 0x00002000, 0x00802080
};

static const unsigned long SB5[64] =
{
    0x00000100, 0x02080100, 0x02080000, 0x42000100,
    0x00080000, 0x00000100, 0x40000000, 0x02080000,
    0x40080100, 0x00080000, 0x02000100, 0x40080100,
    0x42000100, 0x42080000, 0x00080100, 0x40000000,
    0x02000000, 0x40080000, 0x40080000, 0x00000000,
    0x40000100, 0x42080100, 0x42080100, 0x02000100,
    0x42080000, 0x40000100, 0x00000000, 0x42000000,
    0x02080100, 0x02000000, 0x42000000, 0x00080100,
    0x00080000, 0x42000100, 0x00000100, 0x02000000,
    0x40000000, 0x02080000, 0x42000100, 0x40080100,
    0x02000100, 0x40000000, 0x42080000, 0x02080100,
    0x40080100, 0x00000100, 0x02000000, 0x42080000,
    0x42080100, 0x00080100, 0x42000000, 0x42080100,
    0x02080000, 0x00000000, 0x40080000, 0x42000000,
    0x00080100, 0x02000100, 0x40000100, 0x00080000,
    0x00000000, 0x40080000, 0x02080100, 0x40000100
};

static const unsigned long SB6[64] =
{
    0x20000010, 0x20400000, 0x00004000, 0x20404010,
    0x20400000, 0x00000010, 0x20404010, 0x00400000,
    0x20004000, 0x00404010, 0x00400000, 0x20000010,
    0x00400010, 0x20004000, 0x20000000, 0x00004010,
    0x00000000, 0x00400010, 0x20004010, 0x00004000,
    0x00404000, 0x20004010, 0x00000010, 0x20400010,
    0x20400010, 0x00000000, 0x00404010, 0x20404000,
    0x00004010, 0x00404000, 0x20404000, 0x20000000,
    0x20004000, 0x00000010, 0x20400010, 0x00404000,
    0x20404010, 0x00400000, 0x00004010, 0x20000010,
    0x00400000, 0x20004000, 0x20000000, 0x00004010,
    0x20000010, 0x20404010, 0x00404000, 0x20400000,
    0x00404010, 0x20404000, 0x00000000, 0x20400010,
    0x00000010, 0x00004000, 0x20400000, 0x00404010,
    0x00004000, 0x00400010, 0x20004010, 0x00000000,
    0x20404000, 0x20000000, 0x00400010, 0x20004010
};

static const unsigned long SB7[64] =
{
    0x00200000, 0x04200002, 0x04000802, 0x00000000,
    0x00000800, 0x04000802, 0x00200802, 0x04200800,
    0x04200802, 0x00200000, 0x00000000, 0x04000002,
    0x00000002, 0x04000000, 0x04200002, 0x00000802,
    0x04000800, 0x00200802, 0x00200002, 0x04000800,
    0x04000002, 0x04200000, 0x04200800, 0x00200002,
    0x04200000, 0x00000800, 0x00000802, 0x04200802,
    0x00200800, 0x00000002, 0x04000000, 0x00200800,
    0x04000000, 0x00200800, 0x00200000, 0x04000802,
    0x04000802, 0x04200002, 0x04200002, 0x00000002,
    0x00200002, 0x04000000, 0x04000800, 0x00200000,
    0x04200800, 0x00000802, 0x00200802, 0x04200800,
    0x00000802, 0x04000002, 0x04200802, 0x04200000,
    0x00200800, 0x00000000, 0x00000002, 0x04200802,
    0x00000000, 0x00200802, 0x04200000, 0x00000800,
    0x04000002, 0x04000800, 0x00000800, 0x00200002
};

static const unsigned long SB8[64] =
{
    0x10001040, 0x00001000, 0x00040000, 0x10041040,
    0x10000000, 0x10001040, 0x00000040, 0x10000000,
    0x00040040, 0x10040000, 0x10041040, 0x00041000,
    0x10041000, 0x00041040, 0x00001000, 0x00000040,
    0x10040000, 0x10000040, 0x10001000, 0x00001040,
    0x00041000, 0x00040040, 0x10040040, 0x10041000,
    0x00001040, 0x00000000, 0x00000000, 0x10040040,
    0x10000040, 0x10001000, 0x00041040, 0x00040000,
    0x00041040, 0x00040000, 0x10041000, 0x00001000,
    0x00000040, 0x10040040, 0x00001000, 0x00041040,
    0x10001000, 0x00000040, 0x10000040, 0x10040000,
    0x10040040, 0x10000000, 0x00040000, 0x10001040,
    0x00000000, 0x10041040, 0x00040040, 0x10000040,
    0x10040000, 0x10001000, 0x10001040, 0x00000000,
    0x10041040, 0x00041000, 0x00041000, 0x00001040,
    0x00001040, 0x00040040, 0x10000000, 0x10041000
};

/*
 * PC1: left and right halves bit-swap
 */
static const unsigned long LHs[16] =
{
    0x00000000, 0x00000001, 0x00000100, 0x00000101,
    0x00010000, 0x00010001, 0x00010100, 0x00010101,
    0x01000000, 0x01000001, 0x01000100, 0x01000101,
    0x01010000, 0x01010001, 0x01010100, 0x01010101
};

static const unsigned long RHs[16] =
{
    0x00000000, 0x01000000, 0x00010000, 0x01010000,
    0x00000100, 0x01000100, 0x00010100, 0x01010100,
    0x00000001, 0x01000001, 0x00010001, 0x01010001,
    0x00000101, 0x01000101, 0x00010101, 0x01010101,
};

/*
 * Initial Permutation macro
 */
#define DES_IP(X,Y)                                             \
{                                                               \
    T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
    T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
    T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
    T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
    Y = ((Y << 1) | (Y >> 31)) & 0xFFFFFFFF;                    \
    T = (X ^ Y) & 0xAAAAAAAA; Y ^= T; X ^= T;                   \
    X = ((X << 1) | (X >> 31)) & 0xFFFFFFFF;                    \
}

/*
 * Final Permutation macro
 */
#define DES_FP(X,Y)                                             \
{                                                               \
    X = ((X << 31) | (X >> 1)) & 0xFFFFFFFF;                    \
    T = (X ^ Y) & 0xAAAAAAAA; X ^= T; Y ^= T;                   \
    Y = ((Y << 31) | (Y >> 1)) & 0xFFFFFFFF;                    \
    T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
    T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
    T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
    T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
}

/*
 * DES round macro
 */
#define DES_ROUND(X,Y)                          \
{                                               \
    T = *SK++ ^ X;                              \
    Y ^= SB8[ (T      ) & 0x3F ] ^              \
         SB6[ (T >>  8) & 0x3F ] ^              \
         SB4[ (T >> 16) & 0x3F ] ^              \
         SB2[ (T >> 24) & 0x3F ];               \
                                                \
    T = *SK++ ^ ((X << 28) | (X >> 4));         \
    Y ^= SB7[ (T      ) & 0x3F ] ^              \
         SB5[ (T >>  8) & 0x3F ] ^              \
         SB3[ (T >> 16) & 0x3F ] ^              \
         SB1[ (T >> 24) & 0x3F ];               \
}

static void des_main_ks( unsigned long SK[32], unsigned char key[8] )
{
    int i;
    unsigned long X, Y, T;

    GET_UINT32_BE( X, key, 0 );
    GET_UINT32_BE( Y, key, 4 );

    /*
     * Permuted Choice 1
     */
    T =  ((Y >>  4) ^ X) & 0x0F0F0F0F;  X ^= T; Y ^= (T <<  4);
    T =  ((Y      ) ^ X) & 0x10101010;  X ^= T; Y ^= (T      );

    X =   (LHs[ (X      ) & 0xF] << 3) | (LHs[ (X >>  8) & 0xF ] << 2)
        | (LHs[ (X >> 16) & 0xF] << 1) | (LHs[ (X >> 24) & 0xF ]     )
        | (LHs[ (X >>  5) & 0xF] << 7) | (LHs[ (X >> 13) & 0xF ] << 6)
        | (LHs[ (X >> 21) & 0xF] << 5) | (LHs[ (X >> 29) & 0xF ] << 4);

    Y =   (RHs[ (Y >>  1) & 0xF] << 3) | (RHs[ (Y >>  9) & 0xF ] << 2)
        | (RHs[ (Y >> 17) & 0xF] << 1) | (RHs[ (Y >> 25) & 0xF ]     )
        | (RHs[ (Y >>  4) & 0xF] << 7) | (RHs[ (Y >> 12) & 0xF ] << 6)
        | (RHs[ (Y >> 20) & 0xF] << 5) | (RHs[ (Y >> 28) & 0xF ] << 4);

    X &= 0x0FFFFFFF;
    Y &= 0x0FFFFFFF;

    /*
     * calculate subkeys
     */
    for( i = 0; i < 16; i++ )
    {
        if( i < 2 || i == 8 || i == 15 )
        {
            X = ((X <<  1) | (X >> 27)) & 0x0FFFFFFF;
            Y = ((Y <<  1) | (Y >> 27)) & 0x0FFFFFFF;
        }
        else
        {
            X = ((X <<  2) | (X >> 26)) & 0x0FFFFFFF;
            Y = ((Y <<  2) | (Y >> 26)) & 0x0FFFFFFF;
        }

        *SK++ =   ((X <<  4) & 0x24000000) | ((X << 28) & 0x10000000)
                | ((X << 14) & 0x08000000) | ((X << 18) & 0x02080000)
                | ((X <<  6) & 0x01000000) | ((X <<  9) & 0x00200000)
                | ((X >>  1) & 0x00100000) | ((X << 10) & 0x00040000)
                | ((X <<  2) & 0x00020000) | ((X >> 10) & 0x00010000)
                | ((Y >> 13) & 0x00002000) | ((Y >>  4) & 0x00001000)
                | ((Y <<  6) & 0x00000800) | ((Y >>  1) & 0x00000400)
                | ((Y >> 14) & 0x00000200) | ((Y      ) & 0x00000100)
                | ((Y >>  5) & 0x00000020) | ((Y >> 10) & 0x00000010)
                | ((Y >>  3) & 0x00000008) | ((Y >> 18) & 0x00000004)
                | ((Y >> 26) & 0x00000002) | ((Y >> 24) & 0x00000001);

        *SK++ =   ((X << 15) & 0x20000000) | ((X << 17) & 0x10000000)
                | ((X << 10) & 0x08000000) | ((X << 22) & 0x04000000)
                | ((X >>  2) & 0x02000000) | ((X <<  1) & 0x01000000)
                | ((X << 16) & 0x00200000) | ((X << 11) & 0x00100000)
                | ((X <<  3) & 0x00080000) | ((X >>  6) & 0x00040000)
                | ((X << 15) & 0x00020000) | ((X >>  4) & 0x00010000)
                | ((Y >>  2) & 0x00002000) | ((Y <<  8) & 0x00001000)
                | ((Y >> 14) & 0x00000808) | ((Y >>  9) & 0x00000400)
                | ((Y      ) & 0x00000200) | ((Y <<  7) & 0x00000100)
                | ((Y >>  7) & 0x00000020) | ((Y >>  3) & 0x00000011)
                | ((Y <<  2) & 0x00000004) | ((Y >> 21) & 0x00000002);
    }
}

/*
 * DES key schedule (56-bit)
 */
void des_set_key( des_context *ctx, unsigned char key[8] )
{
    int i;

    des_main_ks( ctx->esk, key );

    for( i = 0; i < 32; i += 2 )
    {
        ctx->dsk[i    ] = ctx->esk[30 - i];
        ctx->dsk[i + 1] = ctx->esk[31 - i];
    }
}

static void des_crypt( unsigned long SK[32],
                       unsigned char input[8],
                       unsigned char output[8] )
{
    unsigned long X, Y, T;

    GET_UINT32_BE( X, input, 0 );
    GET_UINT32_BE( Y, input, 4 );

    DES_IP( X, Y );

    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );

    DES_FP( Y, X );

    PUT_UINT32_BE( Y, output, 0 );
    PUT_UINT32_BE( X, output, 4 );
}

/*
 * DES block encryption (ECB mode)
 */
void des_encrypt( des_context *ctx,
                  unsigned char input[8],
                  unsigned char output[8] )
{
    des_crypt( ctx->esk, input, output );
}

/*
 * DES block decryption (ECB mode)
 */
void des_decrypt( des_context *ctx,
                  unsigned char input[8],
                  unsigned char output[8] )
{
    des_crypt( ctx->dsk, input, output );
}

/*
 * DES-CBC buffer encryption
 */
void des_cbc_encrypt( des_context *ctx,
                      unsigned char iv[8],
                      unsigned char *input,
                      unsigned char *output,
                      int len )
{
    int i;

    while( len > 0 )
    {
        for( i = 0; i < 8; i++ )
            output[i] = input[i] ^ iv[i];

        des_crypt( ctx->esk, output, output );
        memcpy( iv, output, 8 );

        input  += 8;
        output += 8;
        len    -= 8;
    }
}

/*
 * DES-CBC buffer decryption
 */
void des_cbc_decrypt( des_context *ctx,
                      unsigned char iv[8],
                      unsigned char *input,
                      unsigned char *output,
                      int len )
{
    int i;
    unsigned char temp[8];

    while( len > 0 )
    {
        memcpy( temp, input, 8 );
        des_crypt( ctx->dsk, input, output );

        for( i = 0; i < 8; i++ )
            output[i] = output[i] ^ iv[i];

        memcpy( iv, temp, 8 );

        input  += 8;
        output += 8;
        len    -= 8;
    }
}

/*
 * Triple-DES key schedule (112-bit)
 */
void des3_set_2keys( des3_context *ctx, unsigned char key[16] )
{
    int i;

    des_main_ks( ctx->esk     , key     );
    des_main_ks( ctx->dsk + 32, key + 8 );

    for( i = 0; i < 32; i += 2 )
    {
        ctx->dsk[i     ] = ctx->esk[30 - i];
        ctx->dsk[i +  1] = ctx->esk[31 - i];

        ctx->esk[i + 32] = ctx->dsk[62 - i];
        ctx->esk[i + 33] = ctx->dsk[63 - i];

        ctx->esk[i + 64] = ctx->esk[     i];
        ctx->esk[i + 65] = ctx->esk[ 1 + i];

        ctx->dsk[i + 64] = ctx->dsk[     i];
        ctx->dsk[i + 65] = ctx->dsk[ 1 + i];
    }
}

/*
 * Triple-DES key schedule (168-bit)
 */
void des3_set_3keys( des3_context *ctx, unsigned char key[24] )
{
    int i;

    des_main_ks( ctx->esk     , key      );
    des_main_ks( ctx->dsk + 32, key +  8 );
    des_main_ks( ctx->esk + 64, key + 16 );

    for( i = 0; i < 32; i += 2 )
    {
        ctx->dsk[i     ] = ctx->esk[94 - i];
        ctx->dsk[i +  1] = ctx->esk[95 - i];

        ctx->esk[i + 32] = ctx->dsk[62 - i];
        ctx->esk[i + 33] = ctx->dsk[63 - i];

        ctx->dsk[i + 64] = ctx->esk[30 - i];
        ctx->dsk[i + 65] = ctx->esk[31 - i];
    }
}

static void des3_crypt( unsigned long SK[96],
                        unsigned char input[8],
                        unsigned char output[8] )
{
    unsigned long X, Y, T;

    GET_UINT32_BE( X, input, 0 );
    GET_UINT32_BE( Y, input, 4 );

    DES_IP( X, Y );

    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );

    DES_ROUND( X, Y );  DES_ROUND( Y, X );
    DES_ROUND( X, Y );  DES_ROUND( Y, X );
    DES_ROUND( X, Y );  DES_ROUND( Y, X );
    DES_ROUND( X, Y );  DES_ROUND( Y, X );
    DES_ROUND( X, Y );  DES_ROUND( Y, X );
    DES_ROUND( X, Y );  DES_ROUND( Y, X );
    DES_ROUND( X, Y );  DES_ROUND( Y, X );
    DES_ROUND( X, Y );  DES_ROUND( Y, X );

    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );
    DES_ROUND( Y, X );  DES_ROUND( X, Y );

    DES_FP( Y, X );

    PUT_UINT32_BE( Y, output, 0 );
    PUT_UINT32_BE( X, output, 4 );
}

/*
 * Triple-DES block encryption (ECB mode)
 */
void des3_encrypt( des3_context *ctx,
                   unsigned char input[8],
                   unsigned char output[8] )
{
    des3_crypt( ctx->esk, input, output );
}

/*
 * Triple-DES block decryption (ECB mode)
 */
void des3_decrypt( des3_context *ctx,
                   unsigned char input[8],
                   unsigned char output[8] )
{
    des3_crypt( ctx->dsk, input, output );
}

/*
 * 3DES-CBC buffer encryption
 */
void des3_cbc_encrypt( des3_context *ctx,
                       unsigned char iv[8],
                       unsigned char *input,
                       unsigned char *output,
                       int len )
{
    int i;

    while( len > 0 )
    {
        for( i = 0; i < 8; i++ )
            output[i] = input[i] ^ iv[i];

        des3_crypt( ctx->esk, output, output );
        memcpy( iv, output, 8 );

        input  += 8;
        output += 8;
        len    -= 8;
    }
}

/*
 * 3DES-CBC buffer decryption
 */
void des3_cbc_decrypt( des3_context *ctx,
                       unsigned char iv[8],
                       unsigned char *input,
                       unsigned char *output,
                       int len )
{
    int i;
    unsigned char temp[8];

    while( len > 0 )
    {
        memcpy( temp, input, 8 );
        des3_crypt( ctx->dsk, input, output );

        for( i = 0; i < 8; i++ )
            output[i] = output[i] ^ iv[i];

        memcpy( iv, temp, 8 );

        input  += 8;
        output += 8;
        len    -= 8;
    }
}

static const char _des_src[] = "_des_src";

#if defined(SELF_TEST)

#include <stdio.h>

/*
 * DES/3DES test vectors (source: NIST, tripledes-vectors.zip)
 */
static const unsigned char DES3_keys[24] =
{
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
    0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23
};

static const unsigned char DES3_init[8] =
{
    0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74
};

static const unsigned char DES3_enc_test[3][8] =
{
    { 0x6A, 0x2A, 0x19, 0xF4, 0x1E, 0xCA, 0x85, 0x4B },
    { 0x03, 0xE6, 0x9F, 0x5B, 0xFA, 0x58, 0xEB, 0x42 },
    { 0xDD, 0x17, 0xE8, 0xB8, 0xB4, 0x37, 0xD2, 0x32 }
};
    
static const unsigned char DES3_dec_test[3][8] =
{
    { 0xCD, 0xD6, 0x4F, 0x2F, 0x94, 0x27, 0xC1, 0x5D },
    { 0x69, 0x96, 0xC8, 0xFA, 0x47, 0xA2, 0xAB, 0xEB },
    { 0x83, 0x25, 0x39, 0x76, 0x44, 0x09, 0x1A, 0x0A }
};

/*
 * Checkup routine
 */
int des_self_test( int verbose )
{
    int i, j, u, v;
    des_context ctx;
    des3_context ctx3;
    unsigned char buf[8];

    for( i = 0; i < 6; i++ )
    {
        u = i >> 1;
        v = i  & 1;

        if( verbose != 0 )
            printf( "  DES%c-EBC-%3d (%s): ",
                    ( u == 0 ) ? ' ' : '3', 64 + u * 64,
                    ( v == 0 ) ? "enc" : "dec" );

        memcpy( buf, DES3_init, 8 );

        if( u == 0 )
               des_set_key( &ctx,  (unsigned char *) DES3_keys );

        if( u == 1 )
            des3_set_2keys( &ctx3, (unsigned char *) DES3_keys );

        if( u == 2 )
            des3_set_3keys( &ctx3, (unsigned char *) DES3_keys );

        for( j = 0; j < 10000; j++ )
        {
            if( u == 0 )
            {
                if( v == 0 ) des_encrypt( &ctx, buf, buf );
                if( v == 1 ) des_decrypt( &ctx, buf, buf );
            }
            else
            {
                if( v == 0 ) des3_encrypt( &ctx3, buf, buf );
                if( v == 1 ) des3_decrypt( &ctx3, buf, buf );
            }
        }

        if( ( v == 0 && memcmp( buf, DES3_enc_test[u], 8 ) != 0 ) ||
            ( v == 1 && memcmp( buf, DES3_dec_test[u], 8 ) != 0 ) )
        {
            if( verbose != 0 )
                printf( "failed\n" );

            return( 1 );
        }

        if( verbose != 0 )
            printf( "passed\n" );
    }

    if( verbose != 0 )
        printf( "\n" );

    return( 0 );
}
#else
int des_self_test( int verbose )
{
    return( 0 );
}
#endif

#endif




/*
	DESC.C - Data Encryption Standard routines for RSAEURO

	Copyright (c) J.S.A.Kapp 1994 - 1995.

	RSAEURO - RSA Library compatible with RSAREF(tm) 2.0.

	All functions prototypes are the Same as for RSAREF(tm).
	To aid compatiblity the source and the files follow the
	same naming comventions that RSAREF(tm) uses.  This should aid
	direct importing to your applications.

	This library is legal everywhere outside the US.  And should
	NOT be imported to the US and used there.

	Based on Outerbridge's D3DES (V5.09) 1992 Vintage.

	DESX(tm) - RSA Data Security.

	DES386 to be define ONLY in conjunction with 386 compiled
	code.

	All Trademarks Acknowledged.

	Revision history
	0.90 First revision, this was the original retrofitted D3DES
	version.

	0.91 Second revision, retrofitted new S-box array and new desfunc
	routine.  Marginally quicker code improves DES throughput.

	0.92 Current revision, added support for 386 assembler desfunc
	routine, with altered S boxes and key generation to support easier
	S box look up.  Code that uses 386 desfunc is about 80K per sec
	faster than RSAREF(tm) code.
*/

#ifdef RSAEURO_PKCS8

static UINT2 bytebit[8] = {
	0200, 0100, 040, 020, 010, 04, 02, 01
};

static UINT4 bigbyte[24] = {
	0x800000L, 0x400000L, 0x200000L, 0x100000L,
	0x80000L,  0x40000L,  0x20000L,  0x10000L,
	0x8000L,   0x4000L,   0x2000L,   0x1000L,
	0x800L,    0x400L,    0x200L,    0x100L,
	0x80L,     0x40L,     0x20L,     0x10L,
	0x8L,      0x4L,      0x2L,      0x1L
};

static unsigned char totrot[16] = {
	1, 2, 4, 6, 8, 10, 12, 14, 15, 17, 19, 21, 23, 25, 27, 28
};

static unsigned char pc1[56] = {
	56, 48, 40, 32, 24, 16,  8,      0, 57, 49, 41, 33, 25, 17,
	 9,  1, 58, 50, 42, 34, 26,     18, 10,  2, 59, 51, 43, 35,
	62, 54, 46, 38, 30, 22, 14,      6, 61, 53, 45, 37, 29, 21,
	13,  5, 60, 52, 44, 36, 28,     20, 12,  4, 27, 19, 11,  3
};

static unsigned char pc2[48] = {
	13, 16, 10, 23,  0,  4,  2, 27, 14,  5, 20,  9,
	22, 18, 11,  3, 25,  7, 15,  6, 26, 19, 12,  1,
	40, 51, 30, 36, 46, 54, 29, 39, 50, 44, 32, 47,
	43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31
};

#ifndef DES386

UINT4 Spbox[8][64] = {
	0x01010400L, 0x00000000L, 0x00010000L, 0x01010404L,
	0x01010004L, 0x00010404L, 0x00000004L, 0x00010000L,
	0x00000400L, 0x01010400L, 0x01010404L, 0x00000400L,
	0x01000404L, 0x01010004L, 0x01000000L, 0x00000004L,
	0x00000404L, 0x01000400L, 0x01000400L, 0x00010400L,
	0x00010400L, 0x01010000L, 0x01010000L, 0x01000404L,
	0x00010004L, 0x01000004L, 0x01000004L, 0x00010004L,
	0x00000000L, 0x00000404L, 0x00010404L, 0x01000000L,
	0x00010000L, 0x01010404L, 0x00000004L, 0x01010000L,
	0x01010400L, 0x01000000L, 0x01000000L, 0x00000400L,
	0x01010004L, 0x00010000L, 0x00010400L, 0x01000004L,
	0x00000400L, 0x00000004L, 0x01000404L, 0x00010404L,
	0x01010404L, 0x00010004L, 0x01010000L, 0x01000404L,
	0x01000004L, 0x00000404L, 0x00010404L, 0x01010400L,
	0x00000404L, 0x01000400L, 0x01000400L, 0x00000000L,
	0x00010004L, 0x00010400L, 0x00000000L, 0x01010004L,
	0x80108020L, 0x80008000L, 0x00008000L, 0x00108020L,
	0x00100000L, 0x00000020L, 0x80100020L, 0x80008020L,
	0x80000020L, 0x80108020L, 0x80108000L, 0x80000000L,
	0x80008000L, 0x00100000L, 0x00000020L, 0x80100020L,
	0x00108000L, 0x00100020L, 0x80008020L, 0x00000000L,
	0x80000000L, 0x00008000L, 0x00108020L, 0x80100000L,
	0x00100020L, 0x80000020L, 0x00000000L, 0x00108000L,
	0x00008020L, 0x80108000L, 0x80100000L, 0x00008020L,
	0x00000000L, 0x00108020L, 0x80100020L, 0x00100000L,
	0x80008020L, 0x80100000L, 0x80108000L, 0x00008000L,
	0x80100000L, 0x80008000L, 0x00000020L, 0x80108020L,
	0x00108020L, 0x00000020L, 0x00008000L, 0x80000000L,
	0x00008020L, 0x80108000L, 0x00100000L, 0x80000020L,
	0x00100020L, 0x80008020L, 0x80000020L, 0x00100020L,
	0x00108000L, 0x00000000L, 0x80008000L, 0x00008020L,
	0x80000000L, 0x80100020L, 0x80108020L, 0x00108000L,
	0x00000208L, 0x08020200L, 0x00000000L, 0x08020008L,
	0x08000200L, 0x00000000L, 0x00020208L, 0x08000200L,
	0x00020008L, 0x08000008L, 0x08000008L, 0x00020000L,
	0x08020208L, 0x00020008L, 0x08020000L, 0x00000208L,
	0x08000000L, 0x00000008L, 0x08020200L, 0x00000200L,
	0x00020200L, 0x08020000L, 0x08020008L, 0x00020208L,
	0x08000208L, 0x00020200L, 0x00020000L, 0x08000208L,
	0x00000008L, 0x08020208L, 0x00000200L, 0x08000000L,
	0x08020200L, 0x08000000L, 0x00020008L, 0x00000208L,
	0x00020000L, 0x08020200L, 0x08000200L, 0x00000000L,
	0x00000200L, 0x00020008L, 0x08020208L, 0x08000200L,
	0x08000008L, 0x00000200L, 0x00000000L, 0x08020008L,
	0x08000208L, 0x00020000L, 0x08000000L, 0x08020208L,
	0x00000008L, 0x00020208L, 0x00020200L, 0x08000008L,
	0x08020000L, 0x08000208L, 0x00000208L, 0x08020000L,
	0x00020208L, 0x00000008L, 0x08020008L, 0x00020200L,
	0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
	0x00802080L, 0x00800081L, 0x00800001L, 0x00002001L,
	0x00000000L, 0x00802000L, 0x00802000L, 0x00802081L,
	0x00000081L, 0x00000000L, 0x00800080L, 0x00800001L,
	0x00000001L, 0x00002000L, 0x00800000L, 0x00802001L,
	0x00000080L, 0x00800000L, 0x00002001L, 0x00002080L,
	0x00800081L, 0x00000001L, 0x00002080L, 0x00800080L,
	0x00002000L, 0x00802080L, 0x00802081L, 0x00000081L,
	0x00800080L, 0x00800001L, 0x00802000L, 0x00802081L,
	0x00000081L, 0x00000000L, 0x00000000L, 0x00802000L,
	0x00002080L, 0x00800080L, 0x00800081L, 0x00000001L,
	0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
	0x00802081L, 0x00000081L, 0x00000001L, 0x00002000L,
	0x00800001L, 0x00002001L, 0x00802080L, 0x00800081L,
	0x00002001L, 0x00002080L, 0x00800000L, 0x00802001L,
	0x00000080L, 0x00800000L, 0x00002000L, 0x00802080L,
	0x00000100L, 0x02080100L, 0x02080000L, 0x42000100L,
	0x00080000L, 0x00000100L, 0x40000000L, 0x02080000L,
	0x40080100L, 0x00080000L, 0x02000100L, 0x40080100L,
	0x42000100L, 0x42080000L, 0x00080100L, 0x40000000L,
	0x02000000L, 0x40080000L, 0x40080000L, 0x00000000L,
	0x40000100L, 0x42080100L, 0x42080100L, 0x02000100L,
	0x42080000L, 0x40000100L, 0x00000000L, 0x42000000L,
	0x02080100L, 0x02000000L, 0x42000000L, 0x00080100L,
	0x00080000L, 0x42000100L, 0x00000100L, 0x02000000L,
	0x40000000L, 0x02080000L, 0x42000100L, 0x40080100L,
	0x02000100L, 0x40000000L, 0x42080000L, 0x02080100L,
	0x40080100L, 0x00000100L, 0x02000000L, 0x42080000L,
	0x42080100L, 0x00080100L, 0x42000000L, 0x42080100L,
	0x02080000L, 0x00000000L, 0x40080000L, 0x42000000L,
	0x00080100L, 0x02000100L, 0x40000100L, 0x00080000L,
	0x00000000L, 0x40080000L, 0x02080100L, 0x40000100L,
	0x20000010L, 0x20400000L, 0x00004000L, 0x20404010L,
	0x20400000L, 0x00000010L, 0x20404010L, 0x00400000L,
	0x20004000L, 0x00404010L, 0x00400000L, 0x20000010L,
	0x00400010L, 0x20004000L, 0x20000000L, 0x00004010L,
	0x00000000L, 0x00400010L, 0x20004010L, 0x00004000L,
	0x00404000L, 0x20004010L, 0x00000010L, 0x20400010L,
	0x20400010L, 0x00000000L, 0x00404010L, 0x20404000L,
	0x00004010L, 0x00404000L, 0x20404000L, 0x20000000L,
	0x20004000L, 0x00000010L, 0x20400010L, 0x00404000L,
	0x20404010L, 0x00400000L, 0x00004010L, 0x20000010L,
	0x00400000L, 0x20004000L, 0x20000000L, 0x00004010L,
	0x20000010L, 0x20404010L, 0x00404000L, 0x20400000L,
	0x00404010L, 0x20404000L, 0x00000000L, 0x20400010L,
	0x00000010L, 0x00004000L, 0x20400000L, 0x00404010L,
	0x00004000L, 0x00400010L, 0x20004010L, 0x00000000L,
	0x20404000L, 0x20000000L, 0x00400010L, 0x20004010L,
	0x00200000L, 0x04200002L, 0x04000802L, 0x00000000L,
	0x00000800L, 0x04000802L, 0x00200802L, 0x04200800L,
	0x04200802L, 0x00200000L, 0x00000000L, 0x04000002L,
	0x00000002L, 0x04000000L, 0x04200002L, 0x00000802L,
	0x04000800L, 0x00200802L, 0x00200002L, 0x04000800L,
	0x04000002L, 0x04200000L, 0x04200800L, 0x00200002L,
	0x04200000L, 0x00000800L, 0x00000802L, 0x04200802L,
	0x00200800L, 0x00000002L, 0x04000000L, 0x00200800L,
	0x04000000L, 0x00200800L, 0x00200000L, 0x04000802L,
	0x04000802L, 0x04200002L, 0x04200002L, 0x00000002L,
	0x00200002L, 0x04000000L, 0x04000800L, 0x00200000L,
	0x04200800L, 0x00000802L, 0x00200802L, 0x04200800L,
	0x00000802L, 0x04000002L, 0x04200802L, 0x04200000L,
	0x00200800L, 0x00000000L, 0x00000002L, 0x04200802L,
	0x00000000L, 0x00200802L, 0x04200000L, 0x00000800L,
	0x04000002L, 0x04000800L, 0x00000800L, 0x00200002L,
	0x10001040L, 0x00001000L, 0x00040000L, 0x10041040L,
	0x10000000L, 0x10001040L, 0x00000040L, 0x10000000L,
	0x00040040L, 0x10040000L, 0x10041040L, 0x00041000L,
	0x10041000L, 0x00041040L, 0x00001000L, 0x00000040L,
	0x10040000L, 0x10000040L, 0x10001000L, 0x00001040L,
	0x00041000L, 0x00040040L, 0x10040040L, 0x10041000L,
	0x00001040L, 0x00000000L, 0x00000000L, 0x10040040L,
	0x10000040L, 0x10001000L, 0x00041040L, 0x00040000L,
	0x00041040L, 0x00040000L, 0x10041000L, 0x00001000L,
	0x00000040L, 0x10040040L, 0x00001000L, 0x00041040L,
	0x10001000L, 0x00000040L, 0x10000040L, 0x10040000L,
	0x10040040L, 0x10000000L, 0x00040000L, 0x10001040L,
	0x00000000L, 0x10041040L, 0x00040040L, 0x10000040L,
	0x10040000L, 0x10001000L, 0x10001040L, 0x00000000L,
	0x10041040L, 0x00041000L, 0x00041000L, 0x00001040L,
	0x00001040L, 0x00040040L, 0x10000000L, 0x10041000L
};

#else
	/* S box tables for assembler desfunc */

unsigned long Spbox[8][64] = {
	0x04041000,0x00000000,0x00040000,0x04041010,
	0x04040010,0x00041010,0x00000010,0x00040000,
	0x00001000,0x04041000,0x04041010,0x00001000,
	0x04001010,0x04040010,0x04000000,0x00000010,
	0x00001010,0x04001000,0x04001000,0x00041000,
	0x00041000,0x04040000,0x04040000,0x04001010,
	0x00040010,0x04000010,0x04000010,0x00040010,
	0x00000000,0x00001010,0x00041010,0x04000000,
	0x00040000,0x04041010,0x00000010,0x04040000,
	0x04041000,0x04000000,0x04000000,0x00001000,
	0x04040010,0x00040000,0x00041000,0x04000010,
	0x00001000,0x00000010,0x04001010,0x00041010,
	0x04041010,0x00040010,0x04040000,0x04001010,
	0x04000010,0x00001010,0x00041010,0x04041000,
	0x00001010,0x04001000,0x04001000,0x00000000,
	0x00040010,0x00041000,0x00000000,0x04040010,
	0x00420082,0x00020002,0x00020000,0x00420080,
	0x00400000,0x00000080,0x00400082,0x00020082,
	0x00000082,0x00420082,0x00420002,0x00000002,
	0x00020002,0x00400000,0x00000080,0x00400082,
	0x00420000,0x00400080,0x00020082,0x00000000,
	0x00000002,0x00020000,0x00420080,0x00400002,
	0x00400080,0x00000082,0x00000000,0x00420000,
	0x00020080,0x00420002,0x00400002,0x00020080,
	0x00000000,0x00420080,0x00400082,0x00400000,
	0x00020082,0x00400002,0x00420002,0x00020000,
	0x00400002,0x00020002,0x00000080,0x00420082,
	0x00420080,0x00000080,0x00020000,0x00000002,
	0x00020080,0x00420002,0x00400000,0x00000082,
	0x00400080,0x00020082,0x00000082,0x00400080,
	0x00420000,0x00000000,0x00020002,0x00020080,
	0x00000002,0x00400082,0x00420082,0x00420000,
	0x00000820,0x20080800,0x00000000,0x20080020,
	0x20000800,0x00000000,0x00080820,0x20000800,
	0x00080020,0x20000020,0x20000020,0x00080000,
	0x20080820,0x00080020,0x20080000,0x00000820,
	0x20000000,0x00000020,0x20080800,0x00000800,
	0x00080800,0x20080000,0x20080020,0x00080820,
	0x20000820,0x00080800,0x00080000,0x20000820,
	0x00000020,0x20080820,0x00000800,0x20000000,
	0x20080800,0x20000000,0x00080020,0x00000820,
	0x00080000,0x20080800,0x20000800,0x00000000,
	0x00000800,0x00080020,0x20080820,0x20000800,
	0x20000020,0x00000800,0x00000000,0x20080020,
	0x20000820,0x00080000,0x20000000,0x20080820,
	0x00000020,0x00080820,0x00080800,0x20000020,
	0x20080000,0x20000820,0x00000820,0x20080000,
	0x00080820,0x00000020,0x20080020,0x00080800,
	0x02008004,0x00008204,0x00008204,0x00000200,
	0x02008200,0x02000204,0x02000004,0x00008004,
	0x00000000,0x02008000,0x02008000,0x02008204,
	0x00000204,0x00000000,0x02000200,0x02000004,
	0x00000004,0x00008000,0x02000000,0x02008004,
	0x00000200,0x02000000,0x00008004,0x00008200,
	0x02000204,0x00000004,0x00008200,0x02000200,
	0x00008000,0x02008200,0x02008204,0x00000204,
	0x02000200,0x02000004,0x02008000,0x02008204,
	0x00000204,0x00000000,0x00000000,0x02008000,
	0x00008200,0x02000200,0x02000204,0x00000004,
	0x02008004,0x00008204,0x00008204,0x00000200,
	0x02008204,0x00000204,0x00000004,0x00008000,
	0x02000004,0x00008004,0x02008200,0x02000204,
	0x00008004,0x00008200,0x02000000,0x02008004,
	0x00000200,0x02000000,0x00008000,0x02008200,
	0x00000400,0x08200400,0x08200000,0x08000401,
	0x00200000,0x00000400,0x00000001,0x08200000,
	0x00200401,0x00200000,0x08000400,0x00200401,
	0x08000401,0x08200001,0x00200400,0x00000001,
	0x08000000,0x00200001,0x00200001,0x00000000,
	0x00000401,0x08200401,0x08200401,0x08000400,
	0x08200001,0x00000401,0x00000000,0x08000001,
	0x08200400,0x08000000,0x08000001,0x00200400,
	0x00200000,0x08000401,0x00000400,0x08000000,
	0x00000001,0x08200000,0x08000401,0x00200401,
	0x08000400,0x00000001,0x08200001,0x08200400,
	0x00200401,0x00000400,0x08000000,0x08200001,
	0x08200401,0x00200400,0x08000001,0x08200401,
	0x08200000,0x00000000,0x00200001,0x08000001,
	0x00200400,0x08000400,0x00000401,0x00200000,
	0x00000000,0x00200001,0x08200400,0x00000401,
	0x80000040,0x81000000,0x00010000,0x81010040,
	0x81000000,0x00000040,0x81010040,0x01000000,
	0x80010000,0x01010040,0x01000000,0x80000040,
	0x01000040,0x80010000,0x80000000,0x00010040,
	0x00000000,0x01000040,0x80010040,0x00010000,
	0x01010000,0x80010040,0x00000040,0x81000040,
	0x81000040,0x00000000,0x01010040,0x81010000,
	0x00010040,0x01010000,0x81010000,0x80000000,
	0x80010000,0x00000040,0x81000040,0x01010000,
	0x81010040,0x01000000,0x00010040,0x80000040,
	0x01000000,0x80010000,0x80000000,0x00010040,
	0x80000040,0x81010040,0x01010000,0x81000000,
	0x01010040,0x81010000,0x00000000,0x81000040,
	0x00000040,0x00010000,0x81000000,0x01010040,
	0x00010000,0x01000040,0x80010040,0x00000000,
	0x81010000,0x80000000,0x01000040,0x80010040,
	0x00800000,0x10800008,0x10002008,0x00000000,
	0x00002000,0x10002008,0x00802008,0x10802000,
	0x10802008,0x00800000,0x00000000,0x10000008,
	0x00000008,0x10000000,0x10800008,0x00002008,
	0x10002000,0x00802008,0x00800008,0x10002000,
	0x10000008,0x10800000,0x10802000,0x00800008,
	0x10800000,0x00002000,0x00002008,0x10802008,
	0x00802000,0x00000008,0x10000000,0x00802000,
	0x10000000,0x00802000,0x00800000,0x10002008,
	0x10002008,0x10800008,0x10800008,0x00000008,
	0x00800008,0x10000000,0x10002000,0x00800000,
	0x10802000,0x00002008,0x00802008,0x10802000,
	0x00002008,0x10000008,0x10802008,0x10800000,
	0x00802000,0x00000000,0x00000008,0x10802008,
	0x00000000,0x00802008,0x10800000,0x00002000,
	0x10000008,0x10002000,0x00002000,0x00800008,
	0x40004100,0x00004000,0x00100000,0x40104100,
	0x40000000,0x40004100,0x00000100,0x40000000,
	0x00100100,0x40100000,0x40104100,0x00104000,
	0x40104000,0x00104100,0x00004000,0x00000100,
	0x40100000,0x40000100,0x40004000,0x00004100,
	0x00104000,0x00100100,0x40100100,0x40104000,
	0x00004100,0x00000000,0x00000000,0x40100100,
	0x40000100,0x40004000,0x00104100,0x00100000,
	0x00104100,0x00100000,0x40104000,0x00004000,
	0x00000100,0x40100100,0x00004000,0x00104100,
	0x40004000,0x00000100,0x40000100,0x40100000,
	0x40100100,0x40000000,0x00100000,0x40004100,
	0x00000000,0x40104100,0x00100100,0x40000100,
	0x40100000,0x40004000,0x40004100,0x00000000,
	0x40104100,0x00104000,0x00104000,0x00004100,
	0x00004100,0x00100100,0x40000000,0x40104000,
};

#endif

void unscrunch PROTO_LIST ((unsigned char *, UINT4 *));
void scrunch PROTO_LIST ((UINT4 *, unsigned char *));
void deskey PROTO_LIST ((UINT4 *, unsigned char *, int));
static void cookey PROTO_LIST ((UINT4 *, UINT4 *, int));
void desfunc PROTO_LIST ((UINT4 *, UINT4 *));

/* Initialize context.  Caller must zeroize the context when finished. */

void DES_CBCInit(context, key, iv, encrypt)
DES_CBC_CTX *context;           /* context */
unsigned char *key;             /* key */
unsigned char *iv;              /* initializing vector */
int encrypt;                                                                            /* encrypt flag (1 = encrypt, 0 = decrypt) */
{
	/* Save encrypt flag to context. */
	context->encrypt = encrypt;

	/* Pack initializing vector into context. */

	scrunch(context->iv, iv);
	scrunch(context->originalIV, iv);

	/* Precompute key schedule */

	deskey(context->subkeys, key, encrypt);
}

/* DES-CBC block update operation. Continues a DES-CBC encryption
	 operation, processing eight-byte message blocks, and updating
	 the context.

	 This requires len to be a multiple of 8.
*/
int DES_CBCUpdate(context, output, input, len)
DES_CBC_CTX *context;           /* context */
unsigned char *output;          /* output block */
unsigned char *input;           /* input block */
unsigned int len;                                                               /* length of input and output blocks */
{
	UINT4 inputBlock[2], work[2];
	unsigned int i;

	if(len % 8)                                                                             /* block size check */
		return(RE_LEN);

	for(i = 0; i < len/8; i++) {
		scrunch(inputBlock, &input[8*i]);

		/* Chain if encrypting. */

		if(context->encrypt == 0) {
			*work = *inputBlock;
			*(work+1) = *(inputBlock+1);
		}else{
			*work = *inputBlock ^ *context->iv;
			*(work+1) = *(inputBlock+1) ^ *(context->iv+1);
		}

		desfunc(work, context->subkeys);

		/* Chain if decrypting, then update IV. */

		if(context->encrypt == 0) {
			*work ^= *context->iv;
			*(work+1) ^= *(context->iv+1);
			*context->iv = *inputBlock;
			*(context->iv+1) = *(inputBlock+1);
		}else{
			*context->iv = *work;
			*(context->iv+1) = *(work+1);
		}
		unscrunch (&output[8*i], work);
	}

	/* Clear sensitive information. */

	R_memset((POINTER)inputBlock, 0, sizeof(inputBlock));
	R_memset((POINTER)work, 0, sizeof(work));

	return(IDOK);
}

void DES_CBCRestart(context)
DES_CBC_CTX *context;                                                                           /* context */
{
	/* Restore the original IV */

	*context->iv = *context->originalIV;
	*(context->iv+1) = *(context->originalIV+1);
}

/* Initialize context.  Caller should clear the context when finished.
	 The key has the DES key, input whitener and output whitener concatenated.
	 This is the RSADSI special DES implementation.
*/
void DESX_CBCInit(context, key, iv, encrypt)
DESX_CBC_CTX *context;          /* context */
unsigned char *key;             /* DES key and whiteners */
unsigned char *iv;              /* DES initializing vector */
int encrypt;                    /* encrypt flag (1 = encrypt, 0 = decrypt) */
{
	/* Save encrypt flag to context. */

	context->encrypt = encrypt;

	/* Pack initializing vector and whiteners into context. */

	scrunch(context->iv, iv);
	scrunch(context->inputWhitener, key + 8);
	scrunch(context->outputWhitener, key + 16);
	/* Save the IV for use in Restart */
	scrunch(context->originalIV, iv);

	/* Precompute key schedule. */

	deskey (context->subkeys, key, encrypt);
}

/* DESX-CBC block update operation. Continues a DESX-CBC encryption
	 operation, processing eight-byte message blocks, and updating
	 the context.  This is the RSADSI special DES implementation.

	 Requires len to a multiple of 8.
*/

int DESX_CBCUpdate (context, output, input, len)
DESX_CBC_CTX *context;          /* context */
unsigned char *output;          /* output block */
unsigned char *input;           /* input block */
unsigned int len;               /* length of input and output blocks */
{
	UINT4 inputBlock[2], work[2];
	unsigned int i;

	if(len % 8)                                                                             /* Length check */
		return(RE_LEN);

	for(i = 0; i < len/8; i++)  {
		scrunch(inputBlock, &input[8*i]);

		/* Chain if encrypting, and xor with whitener. */

		if(context->encrypt == 0) {
			*work = *inputBlock ^ *context->outputWhitener;
			*(work+1) = *(inputBlock+1) ^ *(context->outputWhitener+1);
		}else{
			*work = *inputBlock ^ *context->iv ^ *context->inputWhitener;
			*(work+1) = *(inputBlock+1) ^ *(context->iv+1) ^ *(context->inputWhitener+1);
		}

		desfunc(work, context->subkeys);

		/* Xor with whitener, chain if decrypting, then update IV. */

		if(context->encrypt == 0) {
			*work ^= *context->iv ^ *context->inputWhitener;
			*(work+1) ^= *(context->iv+1) ^ *(context->inputWhitener+1);
			*(context->iv) = *inputBlock;
			*(context->iv+1) = *(inputBlock+1);
		}else{
			*work ^= *context->outputWhitener;
			*(work+1) ^= *(context->outputWhitener+1);
			*context->iv = *work;
			*(context->iv+1) = *(work+1);
		}
		unscrunch(&output[8*i], work);
	}

	R_memset((POINTER)inputBlock, 0, sizeof(inputBlock));
	R_memset((POINTER)work, 0, sizeof(work));

	return(IDOK);
}

void DESX_CBCRestart(context)
DESX_CBC_CTX *context;          /* context */
{
	/* Restore the original IV */
	*context->iv = *context->originalIV;
	*(context->iv+1) = *(context->originalIV+1);
}

/* Initialize context.  Caller must zeroize the context when finished. */

void DES3_CBCInit(context, key, iv, encrypt)
DES3_CBC_CTX *context;          /* context */
unsigned char *key;             /* key */
unsigned char *iv;              /* initializing vector */
int encrypt;                    /* encrypt flag (1 = encrypt, 0 = decrypt) */
{
	/* Copy encrypt flag to context. */
	context->encrypt = encrypt;

	/* Pack initializing vector into context. */

	scrunch(context->iv, iv);

	/* Save the IV for use in Restart */
	scrunch(context->originalIV, iv);

	/* Precompute key schedules. */

	deskey(context->subkeys[0], encrypt ? key : &key[16], encrypt);
	deskey(context->subkeys[1], &key[8], !encrypt);
	deskey(context->subkeys[2], encrypt ? &key[16] : key, encrypt);
}

int DES3_CBCUpdate(context, output, input, len)
DES3_CBC_CTX *context;          /* context */
unsigned char *output;          /* output block */
unsigned char *input;           /* input block */
unsigned int len;               /* length of input and output blocks */
{
	UINT4 inputBlock[2], work[2];
	unsigned int i;

	if(len % 8)                  /* length check */
		return(RE_LEN);

	for(i = 0; i < len/8; i++) {
		scrunch(inputBlock, &input[8*i]);

		/* Chain if encrypting. */

		if(context->encrypt == 0) {
			*work = *inputBlock;
			*(work+1) = *(inputBlock+1);
		}
		else {
			*work = *inputBlock ^ *context->iv;
			*(work+1) = *(inputBlock+1) ^ *(context->iv+1);
		}

		desfunc(work, context->subkeys[0]);
		desfunc(work, context->subkeys[1]);
		desfunc(work, context->subkeys[2]);

		/* Chain if decrypting, then update IV. */

		if(context->encrypt == 0) {
			*work ^= *context->iv;
			*(work+1) ^= *(context->iv+1);
			*context->iv = *inputBlock;
			*(context->iv+1) = *(inputBlock+1);
		}
		else {
			*context->iv = *work;
			*(context->iv+1) = *(work+1);
		}
		unscrunch(&output[8*i], work);
	}

	R_memset((POINTER)inputBlock, 0, sizeof(inputBlock));
	R_memset((POINTER)work, 0, sizeof(work));

	return (0);
}

void DES3_CBCRestart (context)
DES3_CBC_CTX *context;          /* context */
{
	/* Restore the original IV */
	*context->iv = *context->originalIV;
	*(context->iv+1) = *(context->originalIV+1);
}

void scrunch (into, outof)
UINT4 *into;
unsigned char *outof;
{
	*into    = (*outof++ & 0xffL) << 24;
	*into   |= (*outof++ & 0xffL) << 16;
	*into   |= (*outof++ & 0xffL) << 8;
	*into++ |= (*outof++ & 0xffL);
	*into    = (*outof++ & 0xffL) << 24;
	*into   |= (*outof++ & 0xffL) << 16;
	*into   |= (*outof++ & 0xffL) << 8;
	*into   |= (*outof   & 0xffL);
}

void unscrunch(into, outof)
unsigned char *into;
UINT4 *outof;
{
	*into++ = (unsigned char)((*outof >> 24) & 0xffL);
	*into++ = (unsigned char)((*outof >> 16) & 0xffL);
	*into++ = (unsigned char)((*outof >>  8) & 0xffL);
	*into++ = (unsigned char)( *outof++      & 0xffL);
	*into++ = (unsigned char)((*outof >> 24) & 0xffL);
	*into++ = (unsigned char)((*outof >> 16) & 0xffL);
	*into++ = (unsigned char)((*outof >>  8) & 0xffL);
	*into   = (unsigned char)( *outof        & 0xffL);
}

/* Compute DES Subkeys */

void deskey(subkeys, key, encrypt)
UINT4 subkeys[32];
unsigned char key[8];
int encrypt;
{
	UINT4 kn[32];
	int i, j, l, m, n;
	unsigned char pc1m[56], pcr[56];

	for(j = 0; j < 56; j++) {
		l = pc1[j];
		m = l & 07;
		pc1m[j] = (unsigned char)((key[l >> 3] & bytebit[m]) ? 1 : 0);
	}
	for(i = 0; i < 16; i++) {
		m = i << 1;
		n = m + 1;
		kn[m] = kn[n] = 0L;
		for(j = 0; j < 28; j++) {
			l = j + totrot[i];
			if(l < 28) pcr[j] = pc1m[l];
			else pcr[j] = pc1m[l - 28];
		}
		for(j = 28; j < 56; j++) {
			l = j + totrot[i];
			if(l < 56) pcr[j] = pc1m[l];
			else pcr[j] = pc1m[l - 28];
		}
		for(j = 0; j < 24; j++) {
			if(pcr[pc2[j]])
				kn[m] |= bigbyte[j];
			if(pcr[pc2[j+24]])
				kn[n] |= bigbyte[j];
		}
	}
	cookey(subkeys, kn, encrypt);

#ifdef DES386
	for(i=0;i < 32;i++)
		subkeys[i] <<= 2;
#endif

	R_memset((POINTER)pc1m, 0, sizeof(pc1m));
	R_memset((POINTER)pcr, 0, sizeof(pcr));
	R_memset((POINTER)kn, 0, sizeof(kn));
}

static void cookey(subkeys, kn, encrypt)
UINT4 *subkeys;
UINT4 *kn;
int encrypt;
{
	UINT4 *cooked, *raw0, *raw1;
	int increment;
	unsigned int i;

	raw1 = kn;
	cooked = encrypt ? subkeys : &subkeys[30];
	increment = encrypt ? 1 : -3;

	for (i = 0; i < 16; i++, raw1++) {
		raw0 = raw1++;
		*cooked    = (*raw0 & 0x00fc0000L) << 6;
		*cooked   |= (*raw0 & 0x00000fc0L) << 10;
		*cooked   |= (*raw1 & 0x00fc0000L) >> 10;
		*cooked++ |= (*raw1 & 0x00000fc0L) >> 6;
		*cooked    = (*raw0 & 0x0003f000L) << 12;
		*cooked   |= (*raw0 & 0x0000003fL) << 16;
		*cooked   |= (*raw1 & 0x0003f000L) >> 4;
		*cooked   |= (*raw1 & 0x0000003fL);
		cooked += increment;
	}
}

#ifndef DES386 /* ignore C version in favor of 386 ONLY desfunc */

#define	F(l,r,key){\
	work = ((r >> 4) | (r << 28)) ^ *key;\
	l ^= Spbox[6][work & 0x3f];\
	l ^= Spbox[4][(work >> 8) & 0x3f];\
	l ^= Spbox[2][(work >> 16) & 0x3f];\
	l ^= Spbox[0][(work >> 24) & 0x3f];\
	work = r ^ *(key+1);\
	l ^= Spbox[7][work & 0x3f];\
	l ^= Spbox[5][(work >> 8) & 0x3f];\
	l ^= Spbox[3][(work >> 16) & 0x3f];\
	l ^= Spbox[1][(work >> 24) & 0x3f];\
}

/* This desfunc code is marginally quicker than that uses in
	 RSAREF(tm)
*/

void desfunc(block,ks)
UINT4 *block;		/* Data block */
UINT4 *ks;	/* Key schedule */
{
	unsigned long left,right,work;

	left = block[0];
	right = block[1];

	work = ((left >> 4) ^ right) & 0x0f0f0f0f;
	right ^= work;
	left ^= work << 4;
	work = ((left >> 16) ^ right) & 0xffff;
	right ^= work;
	left ^= work << 16;
	work = ((right >> 2) ^ left) & 0x33333333;
	left ^= work;
	right ^= (work << 2);
	work = ((right >> 8) ^ left) & 0xff00ff;
	left ^= work;
	right ^= (work << 8);
	right = (right << 1) | (right >> 31);
	work = (left ^ right) & 0xaaaaaaaa;
	left ^= work;
	right ^= work;
	left = (left << 1) | (left >> 31);

	/* Now do the 16 rounds */
	F(left,right,&ks[0]);
	F(right,left,&ks[2]);
	F(left,right,&ks[4]);
	F(right,left,&ks[6]);
	F(left,right,&ks[8]);
	F(right,left,&ks[10]);
	F(left,right,&ks[12]);
	F(right,left,&ks[14]);
	F(left,right,&ks[16]);
	F(right,left,&ks[18]);
	F(left,right,&ks[20]);
	F(right,left,&ks[22]);
	F(left,right,&ks[24]);
	F(right,left,&ks[26]);
	F(left,right,&ks[28]);
	F(right,left,&ks[30]);

	right = (right << 31) | (right >> 1);
	work = (left ^ right) & 0xaaaaaaaa;
	left ^= work;
	right ^= work;
	left = (left >> 1) | (left  << 31);
	work = ((left >> 8) ^ right) & 0xff00ff;
	right ^= work;
	left ^= work << 8;
	work = ((left >> 2) ^ right) & 0x33333333;
	right ^= work;
	left ^= work << 2;
	work = ((right >> 16) ^ left) & 0xffff;
	left ^= work;
	right ^= work << 16;
	work = ((right >> 4) ^ left) & 0x0f0f0f0f;
	left ^= work;
	right ^= work << 4;

	*block++ = right;
	*block = left;
}

#endif /* DES386 endif */

#endif

