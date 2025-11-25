#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/io.h>

#define SYSIO_BASE   0x0A00
#define SYSIO_INDEX  (SYSIO_BASE + 5)   // 0xA05
#define SYSIO_DATA   (SYSIO_BASE + 6)   // 0xA06

struct led_desc {
    const char *name;
    unsigned char index;   // Ixx
    unsigned char bit;     // Bn
};

static struct led_desc leds[] = {
    { "status_green",   0x91, 2 },  // STATUS_GREEN_LED = SIO:I91:B2
    { "status_red",     0x91, 3 },  // STATUS_RED_LED   = SIO:I91:B3
    { "front_usb",      0xE1, 7 },  // FRONT_USB_LED    = SIO:IE1:B7
    { "disk1_err",      0x81, 0 },  // Disk1 ERR_LED    = SIO:I81:B0
    { "disk2_err",      0x81, 1 },  // Disk2 ERR_LED    = SIO:I81:B1
    { "disk3_err",      0x81, 2 },  // Disk3 ERR_LED    = SIO:I81:B2
    { "disk4_err",      0x81, 3 },  // Disk4 ERR_LED    = SIO:I81:B3
    { NULL, 0, 0 }
};

static struct led_desc *find_led(const char *name)
{
    for (int i = 0; leds[i].name; i++) {
        if (strcmp(leds[i].name, name) == 0)
            return &leds[i];
    }
    return NULL;
}

static unsigned char sio_read_reg(unsigned char idx)
{
    outb(idx, SYSIO_INDEX);          // select register
    return inb(SYSIO_DATA);          // read data
}

static void sio_write_reg(unsigned char idx, unsigned char val)
{
    outb(idx, SYSIO_INDEX);          // select register
    outb(val, SYSIO_DATA);           // write data
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr,
            "Usage: %s <led_name> <0|1>\n"
            "Available led_name:\n"
            "  status_green\n"
            "  status_red\n"
            "  front_usb\n"
            "  disk1_err\n"
            "  disk2_err\n"
            "  disk3_err\n"
            "  disk4_err\n",
            argv[0]);
        return 1;
    }

    const char *name = argv[1];
    int val = atoi(argv[2]);
    if (val != 0 && val != 1) {
        fprintf(stderr, "Value must be 0 or 1\n");
        return 1;
    }

    struct led_desc *led = find_led(name);
    if (!led) {
        fprintf(stderr, "Unknown LED '%s'\n", name);
        return 1;
    }

    // need access to 0xA05 and 0xA06
    if (ioperm(SYSIO_INDEX, 2, 1)) {
        perror("ioperm");
        return 1;
    }

    unsigned char v = sio_read_reg(led->index);

    if (val)
        v |=  (1u << led->bit);
    else
        v &= ~(1u << led->bit);

    sio_write_reg(led->index, v);

    ioperm(SYSIO_INDEX, 2, 0);
    return 0;
}