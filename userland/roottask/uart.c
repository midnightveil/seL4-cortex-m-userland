#include <stdint.h>

/*
 * Derived from https://github.com/carlosftm/RPi-Pico2-Baremetal/blob/f3d4793/02_BlockingUART/02_BlockingUART.c#L111-L149
 * GPL-3.0 license.
 */

#define BIT(n) (1ULL << (n))

#define REG32(address, offset) ((volatile uint32_t *)((address) + (offset)))

/* Address map for APB bus segment, Table 13 (section 2.2.4) of RP2350 datasheet */
#define CLOCKS_BASE     0x40010000
#define RESETS_BASE     0x40020000
#define IO_BANK0_BASE   0x40028000
#define PADS_BANK0_BASE 0x40038000
#define XOSC_BASE       0x40048000
#define UART0_BASE      0x40070000

/* 2.2.6. Core-local peripherals (SIO) */
#define SIO_BASE        0xd0000000

/* Define Atomic Register Access
   See section 2.1.3 "Atomic Register Access" on RP2350 datasheet */
#define WRITE_NORMAL (0x0000)   // normal read write access
#define WRITE_XOR    (0x1000)   // atomic XOR on write
#define WRITE_SET    (0x2000)   // atomic bitmask set on write
#define WRITE_CLR    (0x3000)   // atomic bitmask clear on write

void plat_uart_init(void) {
    // Setup XOC clock to drive the GPIO (Pico2 board as a ABM8-272-T3 crystal that oscillates at 12MHz)
    *REG32(XOSC_BASE, 0)      = 0x00000aa0;                //  XOC range 1-15MHz (Crystal Oschillator)
    *REG32(XOSC_BASE, 0x0c)   = 0x000000c4;                //  Startup Delay (default = 50,000 cycles aprox.)
    *REG32(XOSC_BASE, 0x2000) = 0x00FAB000;                //  Enable XOC
    while (!(*REG32(XOSC_BASE, 4) & BIT(31)));             //  Wait for XOC stable

    // Configure source clock for components (see datasheet RP2350 Chapter 8. "Clocks")
    *REG32(CLOCKS_BASE, 0x3C) = 0;                         //  CLK SYS CTRL = XOC (for processor, bus frabric & memories)
    *REG32(CLOCKS_BASE, 0x48) = ((1 << 11) | ( 4 << 5));   //  CLK_PERI_CTRL = XOC (for perifery UART and SPI) + Enable

    // De-asserts the reset of UART0
    *REG32(RESETS_BASE + WRITE_CLR, 0x0) = BIT(26);        // De-assert the reset from UART0
    while (!(*REG32(RESETS_BASE, 0x08) & BIT(26)));        // Wait for UART0 to be ready

    // Configure GPIO25 to use function 5 (SIO) to controll the GPIO by software
    *REG32(IO_BANK0_BASE, 0xcc) = 5;                       // IO GPIO25 uses SIO
    *REG32(IO_BANK0_BASE, 0x04) = 2;                       // IO GPIO0 uses UART TX
    *REG32(IO_BANK0_BASE, 0x0c) = 2;                       // IO GPIO1 uses UART RX

    // Enable GPIO out in SIO register
    *REG32(SIO_BASE + WRITE_SET, 0x038) = BIT(25);         // SIO OE (output enable) for Pin25

    // Configure the pad control (new on RP2350)
    *REG32(PADS_BANK0_BASE + WRITE_CLR, 0x68) = BIT(8);    // Remove GPIO25 pad isolation
    *REG32(PADS_BANK0_BASE + WRITE_CLR, 0x04) = BIT(8);    // Remove UART0TX pad isolation
    *REG32(PADS_BANK0_BASE + WRITE_CLR, 0x08) = BIT(8);    // Remove UART0RX pad isolation
    *REG32(PADS_BANK0_BASE + WRITE_SET, 0x08) = BIT(6);    // Enable UART0RX pad for input

    // Configure UART0
    //   Baud: For a baud rate of 115200 with UARTCLK = 12MHz then:
    //   Baud Rate Divisor = 12000000/(16 * 115200) ~= 6.5104
    *REG32(UART0_BASE, 0x24) = 6;                          // UARTIBRD_H: Integer part of the baudrate divisor
    *REG32(UART0_BASE, 0x28) = 5104;                       // UARTFBRD_L: Decimal part of the baudrate divisor
    *REG32(UART0_BASE, 0x2c) = (( 0x3 << 5 ) | BIT(4));    // UARTLCR_H: Word lenght = 8, FIFO RX/TX enabled
    *REG32(UART0_BASE, 0x30) = (BIT(9) | BIT(8) | BIT(0)); // UARTCR: UART Enabled, Tx enabled, Rx enabled
}

#define UARTDR                    0x000
#define UARTFR                    0x018

#define PL011_UARTFR_TXFF         BIT(5)
#define PL011_UARTFR_RXFE         BIT(4)

void putc(const char c) {
    while ((*REG32(UART0_BASE, UARTFR) & PL011_UARTFR_TXFF) != 0);

    *REG32(UART0_BASE, UARTDR) = c;
}

void puts(const char *str) {
    char prev = 0;
    while (*str) {
        if (*str == '\n' && prev != '\r') {
            putc('\r');
        }

        putc(*str);
        prev = *str;
        str++;
    }
}
