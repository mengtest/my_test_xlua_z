#ifndef _rc4_h
#define _rc4_h

int rc4_encrypt(const char* data, int data_len, const char* key, int key_len, char* dest);
int rc4_decrypt(const char* data, int data_len, const char* key, int key_len, char* dest);
int crc16(unsigned char*  data,int data_len);

#endif