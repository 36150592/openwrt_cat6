#ifndef _SSL_INTERFACE_H_
#define _SSL_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

int Hex2Mem(unsigned char * dest,char * src);
void RSA_Decode(char *N,int iN,
				char *E,int iE,
				char *X,int iX,
				char *Y,int *iY);
void rsa_decode(char* n,char* e,char* x,char *y,int *yLen);				
extern void rsa_encode(char* n,char* d,char* x,char *y,int *yLen);
extern int exchang(unsigned char *buff,char * str);
extern char des3_GetMAC(unsigned char key[24],unsigned char inData[],unsigned int InputDataLen,unsigned char outMAC[8]);
extern int des3_decode(unsigned char key[24],unsigned char inData[],unsigned char outData[],unsigned int iDataLen);
extern int des3_encode(unsigned char key[24],unsigned char inData[],unsigned char outData[],unsigned int iDataLen);
extern char debugcrypt(unsigned char path);

#endif
