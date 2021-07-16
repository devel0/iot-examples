#include <util-mem.h>

#include <malloc.h>

// https://github.com/stm32duino/STM32Examples/blob/master/examples/Benchmarking/MemoryAllocationStatistics/MemoryAllocationStatistics.ino
extern "C" char *sbrk(int i);
/* Use linker definition */
extern char _end;
extern char _sdata;
extern char _estack;
extern char _Min_Stack_Size;

static char *ramstart = &_sdata;
static char *ramend = &_estack;

static char *minSP = (char *)(ramend - &_Min_Stack_Size);

unsigned int GetMemFree()
{
    char *heapend = (char *)sbrk(0);
                char *stack_ptr = (char *)__get_MSP();
                struct mallinfo mi = mallinfo();
auto mfree = ((stack_ptr < minSP) ? stack_ptr : minSP) - heapend + mi.fordblks;

return mfree;
}
