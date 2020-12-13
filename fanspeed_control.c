#include <assert.h>
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>

static const uint8_t port = 0x2e;
static uint16_t ecbar = 0x00;
static uint fanpwm = 0;
static pthread_t worker_tid;

void iowrite(uint8_t reg, uint8_t val) {
  outb(reg, port);
  outb(val, port + 1);
}

uint8_t ioread(uint8_t reg) {
  outb(reg, port);
  return inb(port + 1);
}

void ecwrite(uint8_t reg, uint8_t val) {
  outb(reg, ecbar + 5);
  outb(val, ecbar + 6);
}

uint8_t ecread(uint8_t reg) {
  outb(reg, ecbar + 5);
  return inb(ecbar + 6);
}

void *fan_speed_worker() {
    while (1) {
        //syslog(3, "setting fanpwm to %d", fanpwm);
        ecwrite(0x63, fanpwm);
        ecwrite(0x6b, fanpwm);
        ecwrite(0x73, fanpwm);
        sleep(1);
    }
    return NULL;
}

void change_fanspeed(int pwm) {
    if (worker_tid == 0) {
        int err = pthread_create(&worker_tid, NULL, fan_speed_worker, NULL);
        syslog(3, "Create worker got: %d\n", err);
    }
    fanpwm = pwm;
}

void init_fanspeed_control() {
    printf("getting io port...\n");
    // Obtain access to IO ports
    syslog(3, "iopl ret: %d\n", iopl(3));
    
    syslog(3, "init 8728e...\n");
    // Initialize the IT8772E
    outb(0x87, port);
    outb(0x01, port);
    outb(0x55, port);
    outb(0x55, port);
    
    syslog(3, "assert it's 8728e...\n");
    // Sanity check that this is the 8772
    uint id1 = ioread(0x20);
    syslog(3, "id1: %d\n", id1);
    uint id2 = ioread(0x21);
    syslog(3, "id2: %d\n", id2);
    assert(id1 == 0x87);
    assert(id2 == 0x28);
    
    // Set LDN = 4 to access environment registers
    iowrite(0x07, 0x04);
    
    // Activate environment controller (EC)
    iowrite(0x30, 0x01);
    
    // Read EC bar
    ecbar = (ioread(0x60) << 8) + ioread(0x61);
    syslog(3, "Got ecbar: %d\n", ecbar);
    
    // Set software operation
    ecwrite(0x15, 0x00);
    ecwrite(0x16, 0x00);
    ecwrite(0x17, 0x00);
    
    // Initialize the PWM value
    //uint8_t pwm = 40;
    //change_fanspeed(pwm);
}
