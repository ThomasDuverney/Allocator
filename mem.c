#include "mem.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>


void mem_init(char* mem, size_t taille)
{
    fb* ptrHead = *(fb**) mem;
	ptrHead->busy = "0";
    ptrHead->size = taille - sizeof(struct fb);
    ptrHead->next = NULL;
    mem_fit(&mem_fit_first);
}



void* mem_alloc(size_t size)
{
	//can allocate to not-2-power address ?

    fb* ptrHead = *(fb**) get_memory_adr();
    ptrNew = mem_fit_first(ptrHead, size);
	if (ptrNew == NULL)
		return NULL;
	ptrNew->busy = "1";
	ptrNew->size = size;
	ptrNew->next = NULL;
	ptrHead = ptrHead + size + sizeof(struct fb);
	return ptrNew;
}


void mem_free(void* zone)
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

struct fb* mem_fit_first(fb* list, size_t size)
{
    fb* ptrHead = list;
    fb* ptrCurrent = ptrHead;
    while(ptrCurrent!=NULL && ptrCurrent->size <= size+sizeof(struct fb)){
        ptrCurrent = ptrCurrent->next;
    }
    if(ptrCurrent->size >= size){
        return ptrCurrent;
    }else {
        return NULL;
    }
}


struct fb* mem_fit_best(struct fb* stct, size_t size)
{
    return NULL;
}


struct fb* mem_fit_worst(struct fb* stct, size_t size)
{
    return NULL;
}
