objects = fanspeed_control.o scemd_hijack.o

CFLAGS = -Wall -Wextra -Wwrite-strings -fPIC

all: $(objects) scemd_hijack.so scemd

$(objects) : %.o: %.c
	cc $(CFLAGS) -c $< -o $@

scemd_hijack.so : $(objects)
	cc -ldl -lpthread -shared -o scemd_hijack.so $(objects)

scemd: ldpreload_wrapper.c
	cc -Wall -fPIC -DBINARY='"/usr/syno/bin/scemd.orig"' -DPRELOADLIB='"/usr/syno/bin/scemd_hijack.so"' -o scemd ldpreload_wrapper.c

clean:
	rm -f *.so *.o
