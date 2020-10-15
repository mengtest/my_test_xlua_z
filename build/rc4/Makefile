PLAT ?= none
PLATS = win linux

CC = gcc

.PHONY : none $(PLATS) clean

#ifneq ($(PLAT), none)
.PHONY : default
default :
	$(MAKE) $(PLAT)
#endif

none :
	@echo "Please do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "   $(PLATS)"

win : PLAT = win
linux : PLAT = linux

CFLAGS :=
MTWIST_LIBS :=
SHARED := --shared -Wall

win : MTWIST_LIBS := -I../lua53/src/ -L../lua53/src/ -llua53
win : SHARED +=  -g

linux : MTWIST_LIBS := -I../lua53/src/ -L../lua53/src/ -lrt
linux : SHARED += -fPIC -O2

LUA_CLIB_PATH = .
LUA_CLIB = rc4

$(LUA_CLIB_PATH) :
	mkdir $(LUA_CLIB_PATH)

win : \
  $(foreach v, $(LUA_CLIB), $(LUA_CLIB_PATH)/$(v).dll)

linux : \
  $(foreach v, $(LUA_CLIB), $(LUA_CLIB_PATH)/$(v).so)


$(LUA_CLIB_PATH)/rc4.dll : lrc4.c crc.c rc4.c base64.c md5.c | $(LUA_CLIB_PATH)
	$(CC) $(SHARED) $^ -o$@ $(CFLAGS) $(MTWIST_LIBS)

$(LUA_CLIB_PATH)/rc4.so : lrc4.c crc.c rc4.c base64.c md5.c | $(LUA_CLIB_PATH)
	$(CC) $(SHARED) $^ -o$@ $(CFLAGS) $(MTWIST_LIBS)

clean :
	rm -f $(LUA_CLIB_PATH)/*.so $(LUA_CLIB_PATH)/*.dll
