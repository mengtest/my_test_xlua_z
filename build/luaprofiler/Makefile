LUALIB=-I../lua53/src/ -L../lua53/src/ -llua53

mingw : profiler.dll
linux : profiler.so

profiler.dll : profiler.c	
	gcc -Wall --shared -o $@ $^ $(LUALIB) $(LUALIB)

profiler.so : profiler.c
	gcc -Wall -fPIC --shared -o $@ $^ $(LUALIB)

clean :
	rm -f profiler.dll profiler.so
