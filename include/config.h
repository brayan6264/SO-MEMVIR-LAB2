#ifndef CONFIG_H
#define CONFIG_H

#define PAGE_SIZE 4096u // tamano de pagina en bytes (4KB)
#define PT1_BITS 10
#define PT2_BITS 10
#define OFFSET_BITS 12
#define PT_ENTRIES 1024 // 2^10 entradas por tabla
#define DEFAULT_PHYS_MEM_BYTES (256u * 1024u)

#endif
