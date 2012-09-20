#ifndef GPIO_MMAP_H
#define GPIO_MMAP_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef WIN32
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#else
#include <fcntl.h>
#include <io.h>
#endif

#define GPIO_BASE 0x80018000

#define GPIO_WRITE_PIN(gpio,value) GPIO_WRITE((gpio)>>5, (gpio)&31, value)
#define GPIO_WRITE(bank,pin,value) (gpio_mmap[0x140+((bank)<<2)+((value)?1:2)] = 1<<(pin))

#define GPIO_READ_PIN(gpio) GPIO_READ((gpio)>>5, (gpio)&31)
#define GPIO_READ(bank,pin) ((gpio_mmap[0x180+((bank)<<2)] >> (pin)) & 1)

int *gpio_map();
int gpio_rd(long offset);
void gpio_wr(long offset, long value);
void gpio_output(int bank, int pin);
void gpio_input(int bank, int pin);


#endif //GPIO_MMAP_H