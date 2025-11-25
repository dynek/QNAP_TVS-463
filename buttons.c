#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

#define SYSIO_BASE   0x0A00
#define SYSIO_INDEX  (SYSIO_BASE + 5)   // 0xA05
#define SYSIO_DATA   (SYSIO_BASE + 6)   // 0xA06

struct btn_desc {
    const char *name;
    unsigned char index;  // Ixx
    unsigned char bit;    // Bn
};

static struct btn_desc buttons[] = {
    { "reset_button",    0x92, 1 },  // RESET_BUTTON    = SIO:I92:B1
    { "usb_copy_button", 0xE2, 2 },  // USB_COPY_BUTTON = SIO:IE2:B2
    { NULL, 0, 0 }
};

static unsigned char sio_read_reg(unsigned char idx)
{
    outb(idx, SYSIO_INDEX);      // select register
    return inb(SYSIO_DATA);      // read data
}

int main(void)
{
    if (ioperm(SYSIO_INDEX, 2, 1)) {
        perror("ioperm");
        return 1;
    }

    for (int i = 0; buttons[i].name; i++) {
        struct btn_desc *b = &buttons[i];
        unsigned char v = sio_read_reg(b->index);
        int state = (v >> b->bit) & 1;

        printf("%s = %d\n", b->name, state);
    }

    ioperm(SYSIO_INDEX, 2, 0);
    return 0;
}