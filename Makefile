all:
	gcc -Wall -Wextra -Wwrite-strings -fPIC -c -o scemd_hijack.o scemd_hijack.c
	gcc -Wall -Wextra -Wwrite-strings -shared -o scemd_hijack.so scemd_hijack.o -ldl
	gcc -Wall -fPIC -DBINARY='"/usr/syno/bin/scemd.orig"' -DPRELOADLIB='"/usr/syno/bin/scemd_hijack.so"' -o scemd ldpreload_wrapper.c

clean:
	rm -f *.so *.o
