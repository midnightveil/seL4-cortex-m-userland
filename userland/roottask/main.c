#include <sel4/sel4.h>
#include <stdint.h>

static inline char hexchar(unsigned int v) {
    return v < 10 ? '0' + v : ('a' - 10) + v;
}

extern void plat_uart_init(void);
extern void puts(const char *str);

void puthex32(uint32_t val) {
    char buffer[8 + 3];
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[8 + 3 - 1] = 0;
    for (unsigned i = 8 + 1; i > 1; i--) {
        buffer[i] = hexchar(val & 0xf);
        val >>= 4;
    }

    puts(buffer);
}

void start(seL4_BootInfo *bootinfo) {
    seL4_DebugPutString("hello (seL4_DebugPutString)\n");

    plat_uart_init();

    puts("hello\n");
    puthex32((uint32_t)bootinfo);

    puts("\nseL4_BootInfo");
    puts("\n  extraLen: "); puthex32(bootinfo->extraLen);
    puts("\n  nodeID: "); puthex32(bootinfo->nodeID);
    puts("\n  numNodes: "); puthex32(bootinfo->numNodes);
    puts("\n  numIOPTLevels: "); puthex32(bootinfo->numIOPTLevels);
    puts("\n  ipcBuffer: "); puthex32((uint32_t)bootinfo->ipcBuffer);

    puts("\n  empty: "); puthex32(bootinfo->empty.start); puts(".."); puthex32(bootinfo->empty.end);
    puts("\n  sharedFrames: "); puthex32(bootinfo->sharedFrames.start); puts(".."); puthex32(bootinfo->sharedFrames.end);
    puts("\n  userImageFrames: "); puthex32(bootinfo->userImageFrames.start); puts(".."); puthex32(bootinfo->userImageFrames.end);
    puts("\n  userImagePaging: "); puthex32(bootinfo->userImagePaging.start); puts(".."); puthex32(bootinfo->userImagePaging.end);
    puts("\n  ioSpaceCaps: "); puthex32(bootinfo->ioSpaceCaps.start); puts(".."); puthex32(bootinfo->ioSpaceCaps.end);
    puts("\n  extraBIPages: "); puthex32(bootinfo->extraBIPages.start); puts(".."); puthex32(bootinfo->extraBIPages.end);

    puts("\n  initThreadCNodeSizeBits: "); puthex32(bootinfo->initThreadCNodeSizeBits);
    puts("\n  initThreadDomain: "); puthex32(bootinfo->initThreadDomain);

    puts("\n  untyped: "); puthex32(bootinfo->untyped.start); puts(".."); puthex32(bootinfo->untyped.end);
    for (uint32_t i = 0; i < CONFIG_MAX_NUM_BOOTINFO_UNTYPED_CAPS; i++) {
        if (bootinfo->untypedList[i].sizeBits == 0) break;

        puts("\n  untyped[");
        puthex32(i);
        puts("] = ");
        puthex32(bootinfo->untypedList[i].paddr);
        puts("..");
        puthex32(bootinfo->untypedList[i].paddr + (1ULL << bootinfo->untypedList[i].sizeBits));
        puts(bootinfo->untypedList[i].isDevice ? " (is device)" : " (is normal)");
    }

    puts("\ndone\n");
}
