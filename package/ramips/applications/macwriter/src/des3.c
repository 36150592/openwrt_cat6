#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "des.h"

#define KEY_SIZE 1024
#define EXPONENT 65537


char hex2char(char *what) {
    register char digit;
    
    digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A') + 10 : (what[0] - '0'));
    digit *= 16;
    digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A') + 10 : (what[1] - '0'));
    
    return digit;
}

void hex2mem(char *src, int src_len, char *dest, int dest_len) {

	char *p;
	register int i;

	i = 0;
	p = src;
	memset(dest, 0, dest_len);

	while(src[i * 2] != '\0') {
		dest[i] = hex2char(p + i * 2);
		i++;
		if (((i * 2) >= (src_len - 1)) || i > dest_len - 1) {
			break;
		}
	}
}

void mem2hex(unsigned char *src, int src_len, char *dest, int dest_len)
{
	int i,j;

	memset(dest, 0, dest_len);
	j = 0;
	for(i = 0; i < src_len; i++)
	{
		sprintf(dest+j,"%02x", src[i]);
		j += 2;

		if(j >= dest_len)
			return;
	}
}

int des3_get_mac(unsigned char key[24],unsigned char inData[], unsigned char outMAC[8], unsigned int InputDataLen)
{
	int iLen,zlen;  
	unsigned char *input_bits;
	unsigned char *output_bits;
	des3_context *des3;	

	input_bits = (unsigned char *)malloc(InputDataLen+8);
	if(input_bits == NULL)
		return 0;
	memset(input_bits,0,InputDataLen+8);	
		
	des3 = (des3_context *)malloc(sizeof(des3_context));
	if(des3 == NULL)
	{
		free(input_bits);
		return 0;	
	}
	memset(des3,0,sizeof(des3_context));
	
	des3_set_3keys(des3, (unsigned char *)key);
	
	memcpy(input_bits,inData,InputDataLen);
	iLen = InputDataLen;
	zlen=iLen % 8;
	if(zlen!=0)
	{
		zlen = 8 - iLen % 8;
		memset(&input_bits[iLen],0,zlen);
		
	}

	iLen += zlen;
	output_bits = (unsigned char *)malloc(iLen);
	if(output_bits == NULL)
	{
		free(des3);
		free(input_bits);
		return 0;	
	}	
		
	des3_cbc_encrypt( des3, 
					 (unsigned char *)outMAC, 
					 (unsigned char *)input_bits,
					 (unsigned char *)output_bits, 
					 iLen);
	free(des3);
	free(input_bits);
	free(output_bits);
	return 1;
}

int des3_decode(unsigned char key[24],unsigned char inData[],unsigned char outData[],unsigned int iDataLen)
{
	int iLen,zlen,iRet;
	des3_context des3;	

	unsigned char temp[8];
	unsigned char key_bits[24];
	unsigned char *input_bits,*input_bits_backup;

	//exchang((unsigned char *)key_bits,(char *)key);
	memcpy((unsigned char *)key_bits,(char *)key,24);
	des3_set_3keys(&des3, (unsigned char *)key_bits);

	input_bits = (unsigned char *)malloc(iDataLen+8);
	input_bits_backup = input_bits;
	if(input_bits == NULL)
		return 0;

	memcpy(input_bits,inData,iDataLen);
	iLen = iDataLen;//exchang(input_bits,inData);
	zlen=iLen % 8;
	if(zlen!=0)
	{
		zlen = 8 - iLen % 8;
		memset(&input_bits[iLen],0,zlen);
		
	}

	iLen += zlen;
	iRet = iLen;
	memset(outData,0,iLen+1);
    while( iLen > 0 )
    {
		memcpy(temp,input_bits,8);
        des3_decrypt(&des3, temp, outData );

        input_bits  += 8;
        outData += 8;
        iLen    -= 8;
    }

    free(input_bits_backup);
	return iRet;
}


int des3_encode(unsigned char key[24],unsigned char inData[],unsigned char outData[],unsigned int iDataLen)
{
	int iLen,zlen,iRet;
	des3_context des3;	

	unsigned char temp[8];
	unsigned char key_bits[24];
	unsigned char *input_bits=NULL,*input_bits_backup;

	memcpy((unsigned char *)key_bits,(char *)key,24);
	des3_set_3keys(&des3, (unsigned char *)key_bits);

	input_bits = (unsigned char *)malloc(iDataLen+8);
	input_bits_backup = input_bits;
	if(input_bits == NULL)
		return 0;

	memcpy(input_bits,inData,iDataLen);
	iLen = iDataLen;
	zlen=iLen % 8;
	if(zlen!=0)
	{
		zlen = 8 - iLen % 8;
		memset(&input_bits[iLen],0,zlen);
		
	}

	iLen += zlen;
	iRet = iLen;
	memset(outData,0,iLen+1);
    while( iLen > 0 )
    {
		memcpy(temp,input_bits,8);
        des3_encrypt(&des3, temp, outData );

        input_bits  += 8;
        outData += 8;
        iLen    -= 8;
    }

    free(input_bits_backup);
	return iRet;
}


#if 0
void main()
{
	int i,j;
	unsigned char DesKey[]="1B8A1F3A6D98273CF4569686F324785182DCE88934A35E6B";
	unsigned char dt[] = "123456780124 1123123123123123123123123123123123w";
	unsigned char dt_En[100];
	unsigned char dt_De[100];

	i = des3_Encode((unsigned char *)DesKey,
					(unsigned char *)dt,
					(unsigned char *)dt_En,
					(unsigned int)strlen((char *)dt));
	printf("%d=%s\n",i,dt_En);
	j = des3_Decode((unsigned char *)DesKey,
					(unsigned char *)dt_En,
					(unsigned char *)dt_De,
					(unsigned int)i);
	printf("%d=%s\n",j,dt_De);
}
#endif

