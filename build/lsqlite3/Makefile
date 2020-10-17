PLAT ?= none
PLATS = staticlib win linux

CC = gcc
LUA_INCLUDE = "../lua53/src/"
LUA_LIB = "../lua53/src/"

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

win : MTWIST_LIBS := -I$(LUA_INCLUDE) -L$(LUA_LIB) -llua53
win : SHARED +=  -g

linux : MTWIST_LIBS := -I$(LUA_INCLUDE) -L$(LUA_LIB) -lrt
linux : SHARED += -fPIC -O2

LUA_CLIB_PATH = .
LUA_CLIB = lsqlite3

$(LUA_CLIB_PATH) :
	mkdir $(LUA_CLIB_PATH)

win : \
  $(foreach v, $(LUA_CLIB), $(LUA_CLIB_PATH)/$(v).dll)

linux : \
  $(foreach v, $(LUA_CLIB), $(LUA_CLIB_PATH)/$(v).so)


$(LUA_CLIB_PATH)/lsqlite3.dll : lsqlite3.c sqlite3.c | $(LUA_CLIB_PATH)
	$(CC) $(SHARED) $^ -o$@ $(CFLAGS) $(MTWIST_LIBS)

$(LUA_CLIB_PATH)/lsqlite3.so : lsqlite3.c sqlite3.c | $(LUA_CLIB_PATH)
	$(CC) $(SHARED) $^ -o$@ $(CFLAGS) $(MTWIST_LIBS)
	
clean :
	rm -f $(LUA_CLIB_PATH)/*.so $(LUA_CLIB_PATH)/*.dll $(FILES)

FILES = sqlite3.o lsqlite3.o
objects = $(FILES)

staticlib : $(objects)
	ar rcs liblsqlite3.a $(objects)

$(objects): %.o: %.c
	$(CC) -c $< -o $@ -I${LUA_PATH}