#include "lua.h"
#include "lauxlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rc4.h"
#include "base64.h"
#include "md5.h"
#include "crc.h"


#ifndef luaL_newlib /* using LuaJIT */
/*
** set functions from list 'l' into table at top - 'nup'; each
** function gets the 'nup' elements at the top as upvalues.
** Returns with only the table at the stack.
*/
LUALIB_API void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
#ifdef luaL_checkversion
	luaL_checkversion(L);
#endif
	luaL_checkstack(L, nup, "too many upvalues");
	for (; l->name != NULL; l++) {  /* fill the table with given functions */
		int i;
		for (i = 0; i < nup; i++)  /* copy upvalues to the top */
			lua_pushvalue(L, -nup);
		lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
		lua_setfield(L, -(nup + 2), l->name);
	}
	lua_pop(L, nup);  /* remove upvalues */
}

#define luaL_newlibtable(L,l) \
	lua_createtable(L, 0, sizeof(l)/sizeof((l)[0]) - 1)

#define luaL_newlib(L,l)  (luaL_newlibtable(L,l), luaL_setfuncs(L,l,0))

#endif

#define SMALL_CHUCK 256

static int lua_rc4_encrypt(lua_State *L)
{
	size_t dataLen = 0;
	const char *data = luaL_checklstring(L, 1, &dataLen);
	size_t keyLen = 0;
	const char *key = luaL_checklstring(L, 2, &keyLen);
	if(keyLen == 0 || dataLen == 0)
	{
		lua_pushnil(L);
		lua_pushboolean(L, 0);
		return 2;
	}
	char tmp[SMALL_CHUCK] = {0};
	char *copyData = tmp;

	int newDataLen = dataLen + 2;
	if (newDataLen > SMALL_CHUCK)
	{
		copyData = (char *)lua_newuserdata(L, newDataLen);
	}

	newDataLen = rc4_encrypt(data,(int)dataLen,key,(int)keyLen,copyData);

	lua_pushlstring(L, copyData, newDataLen);
	lua_pushboolean(L, 1);
	return 2;
}

static int lua_rc4_decrypt(lua_State *L)
{
	size_t dataLen = 0;
	const char *data = luaL_checklstring(L, 1, &dataLen);
	size_t keyLen = 0;
	const char *key = luaL_checklstring(L, 2, &keyLen);
	if(keyLen == 0 || dataLen < 2)
	{
		lua_pushnil(L);
		lua_pushboolean(L, 0);
		return 2;
	}

	char tmp[SMALL_CHUCK] = {0};
	char *copyData = tmp;
	if (dataLen > SMALL_CHUCK)
	{
		copyData = (char *)lua_newuserdata(L, dataLen);
	}

	int newDataLen = rc4_decrypt(data,(int)dataLen,key,(int)keyLen,copyData);

	if (newDataLen == -1)
	{
		lua_pushnil(L);
		lua_pushboolean(L, 0);
	}
	else
	{
		lua_pushlstring(L, copyData, newDataLen);
		lua_pushboolean(L, 1);
	}
	return 2;
}

static int lua_crc16(lua_State *L)
{
	size_t dataLen;
	const char* data = luaL_checklstring(L, 1, &dataLen);

	int crcNum = crc16((unsigned char* )data,(int)dataLen);
	lua_pushnumber(L, crcNum);

	return 1;
}

static int lua_b64_encrypt(lua_State *L)
{
	size_t srcLen = 0;
	const char *src = luaL_checklstring(L, 1, &srcLen);
	char tmp[SMALL_CHUCK] = { 0 };
	char *dest = tmp;

	size_t destLen = (size_t)(srcLen / 3 + 1) * 4;
	if (destLen > SMALL_CHUCK)
	{
		dest = (char *)lua_newuserdata(L, destLen);
	}

	int dest_len = base64_encrypt((unsigned char const*)src, srcLen, dest);

	lua_pushlstring(L, dest, dest_len);
	lua_pushnumber(L, dest_len);
	return 2;
}

static int lua_b64_decrypt(lua_State *L)
{
	size_t srcLen = 0;
	const char *src = luaL_checklstring(L, 1, &srcLen);

	char tmp[SMALL_CHUCK] = { 0 };
	char *dest = tmp;

	size_t destLen = (size_t)(srcLen / 4 + 1) * 3;
	if (destLen > SMALL_CHUCK)
	{
		dest = (char *)lua_newuserdata(L, destLen);
	}

	int dest_len = base64_decrypt(src,(unsigned char *)dest);

	lua_pushlstring(L, dest, dest_len);
	lua_pushnumber(L, dest_len);
	return 2;
}

static int lua_md5(lua_State *L)
{
	size_t srcLen = 0;
	const char *src = luaL_checklstring(L, 1, &srcLen);

	MD5_CTX ctx;
	unsigned char dest[33] = {0};
	unsigned char md[16];
	char temp[3] = {'\0'};
	int i;

	MD5Init(&ctx);
    MD5Update(&ctx, (unsigned char*)src, srcLen);
    MD5Final(&ctx, md); 
	for(i = 0;i < 16; i++) {
		sprintf(temp,"%02X",md[i]);
		strcat(dest, temp);
	}
	
	lua_pushlstring(L, dest, 32);
	return 1;
}

static int lua_md5i(lua_State *L)
{
	size_t srcLen = 0;
	const char *src = luaL_checklstring(L, 1, &srcLen);

	MD5_CTX ctx;
	unsigned char dest[33] = {0};
	unsigned char md[16];
	char temp[3] = {'\0'};
	int i;

	MD5Init(&ctx);
    MD5Update(&ctx, (unsigned char*)src, srcLen);
    MD5Final(&ctx, md);
	for(i = 0;i < 16; i++) {
		sprintf(temp,"%02x",md[i]);
		strcat(dest, temp);
	}

	lua_pushlstring(L, dest, 32);
	return 1;
}

static int lua_makesign(lua_State *L)
{
	size_t dataLen = 0;
	char *data = (char *)luaL_checklstring(L, 1, &dataLen);
	char key[64] = {0x50, 0x51, 0x70, 0x47, 0x69, 0x48, 0x43, 0x43, 0x36, 0x4a, 0x75, 0x74, 0x69, 0x50, 0x70, 0x64, 0x49, 0x62, 0x7a, 0x4d, 0x57, 0x61, 0x50, 0x6c, 0x4d, 0x67, 0x48, 0x70, 0x43, 0x31, 0x6a, 0x50, 0x6e, 0x44, 0x71, 0x6c, 0x65, 0x5a, 0x48, 0x46, 0x62, 0x4b, 0x6a, 0x6d, 0x49, 0x4e, 0x54, 0x62, 0x59, 0x68, 0x50, 0x7a, 0x38, 0x5a, 0x71, 0x79, 0x32, 0x4e, 0x78, 0x57, 0x6c, 0x43, 0x49, 0x36};

	int i;
	for (i = 0; i < 64; i++) {
		int k = dataLen - 64 + i;
		data[k] = key[i];
	}

	MD5_CTX ctx;
	unsigned char dest[33] = {0};
	unsigned char md[16];
	char temp[3] = {'\0'};

	MD5Init(&ctx);
    MD5Update(&ctx, (unsigned char*)data, dataLen);
    MD5Final(&ctx, md);
	for(i = 0;i < 16; i++) {
		sprintf(temp,"%02x",md[i]);
		strcat(dest, temp);
	}

	lua_pushlstring(L, dest, 32);
	return 1;
}

LUALIB_API int
luaopen_rc4(lua_State *L)
{
#ifdef luaL_checkversion
	luaL_checkversion(L);
#endif
	luaL_Reg regs[] = {
		{ "encrypt", lua_rc4_encrypt },
		{ "decrypt", lua_rc4_decrypt },
		{ "crc16", lua_crc16},
		{ "b64_encrypt", lua_b64_encrypt },
		{ "b64_decrypt", lua_b64_decrypt },
		{ "md5", lua_md5 },
		{ "md5i", lua_md5i },
		{ "makesign", lua_makesign },
		{ NULL, NULL }
	};
	luaL_newlib(L, regs);
	return 1;
}
