/*
 * ldpreloadhook - a quick open/close/ioctl/read/write/free/strcmp/strncmp symbol hooker
 * Copyright (C) 2012-2013 Pau Oliva Fora <pof@eslack.org>
 *
 * Based on vsound 0.6 source code:
 *   Copyright (C) 2004 Nathan Chantrell <nsc@zorg.org>
 *   Copyright (C) 2003 Richard Taylor <r.taylor@bcs.org.uk>
 *   Copyright (C) 2000,2001 Erik de Castro Lopo <erikd@zip.com.au>
 *   Copyright (C) 1999 James Henstridge <james@daa.com.au>
 * Based on esddsp utility that is part of esound:
 *   Copyright (C) 1998, 1999 Manish Singh <yosh@gimp.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * 1) Compile:
 *   gcc -fPIC -c -o hook.o hook.c
 *   gcc -shared -o hook.so hook.o -ldl
 * 2) Usage:
 *   LD_PRELOAD="./hook.so" command
 *   LD_PRELOAD="./hook.so" SPYFILE="/file/to/spy" command
 *   LD_PRELOAD="./hook.so" SPYFILE="/file/to/spy" DELIMITER="***" command
 * to spy the content of buffers free'd by free(), set the environment
 * variable SPYFREE, for example:
 *   LD_PRELOAD="./hook.so" SPYFREE=1 command
 * to spy the strings compared using strcmp(), set the environment
 * variable SPYSTR, for example:
 *   LD_PRELOAD="./hook.so" SPYSTR=1 command
 * to spy memcpy() buffers set the env variable SPYMEM
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <syslog.h>
#include "fanspeed_control.h"


#ifndef RTLD_NEXT
#define RTLD_NEXT ((void *) -1l)
#endif

#define REAL_LIBC RTLD_NEXT

typedef int request_t;


static void _libhook_init() __attribute__ ((constructor));
static void _libhook_init() {   
	/* causes segfault on some android, uncomment if you need it */
	//unsetenv("LD_PRELOAD");
	syslog(3, "[] Hooking!\n");
    init_fanspeed_control();
}

int get_pwm_from_config(int config) {
    switch(config) {
    case 1: // stop
        return 0;
    case 2: // ultra low
        return 10;
    case 3: // very low
        return 30;
    case 4: // low
        return 50;
    case 5: // middle
        return 80;
    case 6: // high
        return 100;
    case 7: // very high
        return 150;
    case 8: // ultra high
        return 180;
    case 9: // full
        return 200;
    }
    if (config < 1000) {
        return 100;
    }
    return 255 * (((config + 24) & 0xff) % 100) / 100;
}

int ioctl (int fd, request_t request, ...){	

	static int (*func_ioctl) (int, request_t, void *, void *, void *, void *) = NULL;
	va_list args;
	void *argp;

	if (! func_ioctl)
		func_ioctl = (int (*) (int, request_t, void *, void *, void *, void *)) dlsym (REAL_LIBC, "ioctl");
	va_start (args, request);
	argp = va_arg (args, void *);
    void *argp1 = va_arg (args, void *);
    void *argp2 = va_arg (args, void *);
    void *argp3 = va_arg (args, void *);
	va_end (args);
    
    if ((unsigned int)request == 0xC00C4B0C) {
        unsigned int *speedConf = (unsigned int *)argp;
        unsigned int pwm = get_pwm_from_config(speedConf[2]);
        change_fanspeed(pwm);
        syslog(3, "Changing fan speed to %d pwm %d\n", speedConf[2], pwm);
        return 0;
    }

    //syslog(3, "HOOK: ioctl (fd=%d, request=%p, argp=%p [%02X])\n", fd, request, argp);
    return func_ioctl (fd, request, argp, argp1, argp2, argp3);
}