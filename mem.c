#include "mem.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>


void mem_init(char* mem, size_t taille)
{
    fb* ptrHead = *(struct fb**) mem;
    ptrHead->size = taille - sizeof(struct fb);
    ptrHead->next = NULL;
    mem_fit(&mem_fit_first);
}



void* mem_alloc(size_t size)
{
    struct fb* ptrHead = (struct fb*) get_memory_adr();
    struct fb* ptrBefore = ptrHead;

    return NULL;
}


void mem_free(void* ptr)
{
    return;
}


void mem_show(void (*print)(void * ptr, size_t size, int free))
{
    return;
}


void mem_fit(mem_fit_function_t* ptr)
{
    return;
}

struct fb* mem_fit_first(struct fb* stct, size_t size)
{
    return NULL;
}


struct fb* mem_fit_best(struct fb* stct, size_t size)
{
    return NULL;
}


struct fb* mem_fit_worst(struct fb* stct, size_t size)
{
    return NULL;
}
