#ifndef _base64_h
#define _base64_h

int base64_encrypt(unsigned char const* bytes_to_encode, unsigned int in_len,char* dest);
int base64_decrypt(const char* encoded_string,unsigned char *ret);

#endif