#include "base64.h"
#include <string.h>

#if defined(EZ_ANDROID)
#define _U      0x01
#define _L      0x02
#define _N      0x04
#define _S      0x08
#define _P      0x10
#define _C      0x20
#define _X      0x40
#define _B      0x80
extern const char       *_ctype_;
int isalnum(int c)
{
	return (c == -1 ? 0 : ((_ctype_ + 1)[(unsigned char)c] & (_U|_L|_N)));
}
#else
#include <ctype.h>
#endif
const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" ;
int is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+' ) || (c == '/'));
}

int base64_encrypt(unsigned char const* bytes_to_encode, unsigned int in_len,char* dest) 
{
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];
	int index = 0;
	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++){
				dest[index]= base64_chars[char_array_4[i]];
				index++;
			}
			i = 0;
		}
	}

	if (i)
	{
		for(j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++){
			dest[index]= base64_chars[char_array_4[j]];
			index++;
		}
		while((i++ < 3)){
			dest[index]= '=';
			index++;
		}


	}

	return index;

}

int base64_decrypt(const char* encoded_string,unsigned char *ret) 
{
	int in_len = strlen(encoded_string);
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	int index = 0;
	//TODO:: here has leak
	while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i ==4) {
			for (i = 0; i <4; i++){
				/*	char_array_4[i] = base64_chars.find(char_array_4[i]);*/
				int pos = -1;
				int index;
				for (index = 0;index<strlen(base64_chars);index++)
				{
					if (char_array_4[i]==base64_chars[index])
					{
						pos = index;
					}
				}
				char_array_4[i]  =pos;
			}
			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++){
				ret[index] = char_array_3[i];
				index++;
			}
			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++){
			int pos = -1;
			int index;
			for (index = 0;index<strlen(base64_chars);index++)
			{
				if (char_array_4[j]==base64_chars[index])
				{
					pos = index;
				}
			}
			char_array_4[j]  =pos;

		}

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) {
			ret[index] = char_array_3[j];
			index++;
		}
	}

	return index;
}