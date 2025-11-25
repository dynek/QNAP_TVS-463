#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

#define SIO_INDEX 0x2E
#define SIO_DATA  (SIO_INDEX + 1)

static inline void sio_outb(unsigned char reg, unsigned char val) {
    outb(reg, SIO_INDEX);
    outb(val, SIO_DATA);
}

static inline unsigned char sio_inb(unsigned char reg) {
    outb(reg, SIO_INDEX);
    return inb(SIO_DATA);
}

static void superio_enter(void) {
    outb(0x87, SIO_INDEX);
    outb(0x87, SIO_INDEX);
}

static void superio_exit(void) {
    outb(0xAA, SIO_INDEX);
}

static void select_ldn(unsigned char ldn) {
    sio_outb(0x07, ldn);
}

int main(void) {
    if (ioperm(SIO_INDEX, 2, 1)) {
        perror("ioperm");
        return 1;
    }

    superio_enter();

    // LDN 06 = GPIO block with I92 / IE2
    select_ldn(0x06);

    // Show before
    unsigned char b2a = sio_inb(0x2A);
    unsigned char b30 = sio_inb(0x30);

    printf("Before: CR2A=%02X CR30=%02X\n", b2a, b30);

    // Apply QTS values
    sio_outb(0x2A, 0x84); // LCD
    sio_outb(0x30, 0x01); // Buttons + LEDs

    // Read back
    unsigned char a2a = sio_inb(0x2A);
    unsigned char a30 = sio_inb(0x30);

    printf("After : CR2A=%02X CR30=%02X\n", a2a, a30);

    superio_exit();
    ioperm(SIO_INDEX, 2, 0);
    return 0;
}