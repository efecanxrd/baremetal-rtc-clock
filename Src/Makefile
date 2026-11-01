CC = arm-none-eabi-gcc
MACH = cortex-m4
CFLAGS = -c -mcpu=$(MACH) -mthumb -std=gnu11 -Wall -O0 -g -I. -I../Inc -I../drivers/inc

LDFLAGS = -mcpu=$(MACH) -mthumb -nostartfiles --specs=nano.specs --specs=nosys.specs -T stm32_ls.ld -Wl,-Map=final.map -lc -lm

OBJS = main.o \
       stm32_startup.o \
       ds1307.o \
       i2c_lcd.o \
       stm32f407xx_gpio_driver.o \
       stm32f407xx_i2c_driver.o

all: final.elf

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: ../drivers/src/%.c
	$(CC) $(CFLAGS) -o $@ $<

final.elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -rf *.o *.elf *.map

load:
	openocd -f board/stm32f4discovery.cfg
