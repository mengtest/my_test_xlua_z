#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crc.h"

void rc4_crypt(unsigned char *key, size_t keyLen, unsigned char *Data, size_t dataLen) //加解密
{
	unsigned char s[256] = {0};
	int i =0, j = 0;
	unsigned char tmp = 0;
	for (i=0; i<256; i++) 
	{
		s[i] = i;
	}
	for (i=0; i<256; i++) 
	{
		j = (j+s[i] + key[i%keyLen]) % 256;
		tmp = s[i];
		s[i] = s[j]; //交换s[i]和s[j]
		s[j] = tmp;
	}
	//crypt decrypt
	i = 0, j = 0;
	int t = 0;
	unsigned long k = 0;
	for(k=0; k<dataLen; k++) 
	{
		i = (i+1)%256;
		j = (j+s[i])%256;
		tmp = s[i];
		s[i] = s[j]; //交换s[x]和s[y]
		s[j] = tmp;
		t = (s[i]+s[j])%256;
		Data[k] ^= s[t];
	}
}

int rc4_encrypt(const char*  data, int data_len, const char* key, int key_len, char* dest)
{
	int new_data_len = data_len + 2;
	unsigned char* copy_data = dest;
	memcpy(copy_data, data, data_len);
	crc crcNum = crcFast((const unsigned char*)data, data_len);
	copy_data[new_data_len - 2] = crcNum >> 8 & 0xff;
	copy_data[new_data_len - 1] = crcNum & 0xff;
	rc4_crypt((unsigned char*)key, key_len, (unsigned char*)copy_data, new_data_len);

	return new_data_len;
}

int rc4_decrypt(const char*  data, int data_len, const char* key, int key_len, char* dest)
{
	unsigned char* copy_data = dest;
	memcpy(copy_data, data, data_len);

	rc4_crypt((unsigned char*)key, key_len, (unsigned char*)copy_data, data_len);

	crc currCrc = crcFast((const unsigned char*)copy_data, data_len - 2 );
	crc originalCrc = ((crc)copy_data[data_len - 2]) << 8 | ((crc)copy_data[data_len - 1] & 0xff);
	
	if (currCrc == originalCrc)
	{
		return data_len - 2;
	}

	return -1;
}

int crc16(unsigned char*  data,int data_len)
{
	if(data_len > 0)
	{
		crc crcNum = crcFast((const unsigned char*)data, data_len);
		return (int)crcNum;
	}

	return 0;
}