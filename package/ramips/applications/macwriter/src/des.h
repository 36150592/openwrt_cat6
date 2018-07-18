/**
 * \file des.h
 */
#ifndef _DES_H
#define _DES_H
#define PROTO_LIST(list) ()

/* UINT4定义成四字节的字  */
typedef unsigned long int UINT4;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          DES context structure
 */
typedef struct
{
    unsigned long esk[32];     /*!< DES encryption subkeys */
    unsigned long dsk[32];     /*!< DES decryption subkeys */
}
des_context;

/**
 * \brief          Triple-DES context structure
 */
typedef struct
{
    unsigned long esk[96];     /*!< Triple-DES encryption subkeys */
    unsigned long dsk[96];     /*!< Triple-DES decryption subkeys */
}
des3_context;

/**
 * \brief          DES key schedule (56-bit)
 *
 * \param ctx      DES context to be initialized
 * \param key      8-byte secret key
 */
void des_set_key( des_context *ctx, unsigned char key[8] );

/**
 * \brief          DES block encryption (ECB mode)
 *
 * \param ctx      DES context
 * \param input    plaintext  block
 * \param output   ciphertext block
 */
void des_encrypt( des_context *ctx,
                  unsigned char input[8],
                  unsigned char output[8] );

/**
 * \brief          DES block decryption (ECB mode)
 *
 * \param ctx      DES context
 * \param input    ciphertext block
 * \param output   plaintext  block
 */
void des_decrypt( des_context *ctx,
                  unsigned char input[8],
                  unsigned char output[8] );

/**
 * \brief          DES-CBC buffer encryption
 *
 * \param ctx      DES context
 * \param iv       initialization vector (modified after use)
 * \param input    buffer holding the plaintext
 * \param output   buffer holding the ciphertext
 * \param len      length of the data to be encrypted
 */
void des_cbc_encrypt( des_context *ctx,
                      unsigned char iv[8],
                      unsigned char *input,
                      unsigned char *output,
                      int len );

/**
 * \brief          DES-CBC buffer decryption
 *
 * \param ctx      DES context
 * \param iv       initialization vector (modified after use)
 * \param input    buffer holding the ciphertext
 * \param output   buffer holding the plaintext
 * \param len      length of the data to be decrypted
 */
void des_cbc_decrypt( des_context *ctx,
                      unsigned char iv[8],
                      unsigned char *input,
                      unsigned char *output,
                      int len );

/**
 * \brief          Triple-DES key schedule (112-bit)
 *
 * \param ctx      3DES context to be initialized
 * \param key      16-byte secret key
 */
void des3_set_2keys( des3_context *ctx, unsigned char key[16] );

/**
 * \brief          Triple-DES key schedule (168-bit)
 *
 * \param ctx      3DES context to be initialized
 * \param key      24-byte secret key
 */
void des3_set_3keys( des3_context *ctx, unsigned char key[24] );

/**
 * \brief          Triple-DES block encryption (ECB mode)
 *
 * \param ctx      3DES context
 * \param input    plaintext  block
 * \param output   ciphertext block
 */
void des3_encrypt( des3_context *ctx,
                   unsigned char input[8],
                   unsigned char output[8] );

/**
 * \brief          Triple-DES block decryption (ECB mode)
 *
 * \param ctx      3DES context
 * \param input    ciphertext block
 * \param output   plaintext  block
 */
void des3_decrypt( des3_context *ctx,
                   unsigned char input[8],
                   unsigned char output[8] );

/**
 * \brief          3DES-CBC buffer encryption
 *
 * \param ctx      3DES context
 * \param iv       initialization vector (modified after use)
 * \param input    buffer holding the plaintext
 * \param output   buffer holding the ciphertext
 * \param len      length of the data to be encrypted
 */
void des3_cbc_encrypt( des3_context *ctx,
                       unsigned char iv[8],
                       unsigned char *input,
                       unsigned char *output,
                       int len );

/**
 * \brief          3DES-CBC buffer decryption
 *
 * \param ctx      3DES context
 * \param iv       initialization vector (modified after use)
 * \param input    buffer holding the ciphertext
 * \param output   buffer holding the plaintext
 * \param len      length of the data to be decrypted
 */
void des3_cbc_decrypt( des3_context *ctx,
                       unsigned char iv[8],
                       unsigned char *input,
                       unsigned char *output,
                       int len );

/*
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int des_self_test( int verbose );

#ifdef __cplusplus
}
#endif




/*
	DES.H - header file for DESC.C

	Copyright (c) J.S.A.Kapp 1994 - 1995.

	RSAEURO - RSA Library compatible with RSAREF(tm) 2.0.

	All functions prototypes are the Same as for RSAREF(tm).
	To aid compatiblity the source and the files follow the
	same naming comventions that RSAREF(tm) uses.  This should aid
	direct importing to your applications.

	This library is legal everywhere outside the US.  And should
	NOT be imported to the US and used there.

	All Trademarks Acknowledged.

	DES Code header file.

	Revision 1.00 - JSAK 23/6/95, Final Release Version
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  UINT4 subkeys[32];                                             /* subkeys */
  UINT4 iv[2];                                       /* initializing vector */
  UINT4 originalIV[2];                        /* for restarting the context */
  int encrypt;                                               /* encrypt flag */
} DES_CBC_CTX;

typedef struct {
  UINT4 subkeys[32];                                             /* subkeys */
  UINT4 iv[2];                                       /* initializing vector */
  UINT4 inputWhitener[2];                                 /* input whitener */
  UINT4 outputWhitener[2];                               /* output whitener */
  UINT4 originalIV[2];                        /* for restarting the context */
  int encrypt;                                              /* encrypt flag */
} DESX_CBC_CTX;

typedef struct {
  UINT4 subkeys[3][32];                     /* subkeys for three operations */
  UINT4 iv[2];                                       /* initializing vector */
  UINT4 originalIV[2];                        /* for restarting the context */
  int encrypt;                                              /* encrypt flag */
} DES3_CBC_CTX;

void DES_CBCInit PROTO_LIST 
  ((DES_CBC_CTX *, unsigned char *, unsigned char *, int));
int DES_CBCUpdate PROTO_LIST
  ((DES_CBC_CTX *, unsigned char *, unsigned char *, unsigned int));
void DES_CBCRestart PROTO_LIST ((DES_CBC_CTX *));

void DESX_CBCInit PROTO_LIST
  ((DESX_CBC_CTX *, unsigned char *, unsigned char *, int));
int DESX_CBCUpdate PROTO_LIST
  ((DESX_CBC_CTX *, unsigned char *, unsigned char *, unsigned int));
void DESX_CBCRestart PROTO_LIST ((DESX_CBC_CTX *));

void DES3_CBCInit PROTO_LIST 
  ((DES3_CBC_CTX *, unsigned char *, unsigned char *, int));
int DES3_CBCUpdate PROTO_LIST
  ((DES3_CBC_CTX *, unsigned char *, unsigned char *, unsigned int));
void DES3_CBCRestart PROTO_LIST ((DES3_CBC_CTX *));

#ifdef __cplusplus
}
#endif
#endif /* _DES_H_ */




