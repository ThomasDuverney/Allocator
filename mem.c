#include "mem.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>


void mem_init(char* mem, size_t taille)
{
    *((fb_t**) mem) = (fb_t*)(mem + sizeof(fb_t*));
    fb_t* ptrHead = *((fb_t**) mem);
    ptrHead->size = taille;
    ptrHead->next = NULL;
    mem_fit(mem_fit_first);
}

void* mem_alloc(size_t size)
{
    void * ptrNewBlock; // l'adresse du block à allouer
    fb_t * ptrPreviousBlock;
    bb_t * ptrBuzyBlock;  // le pointeur de la structure busy block

    fb_t* ptrHead = *(fb_t**) get_memory_adr();
    fb_t* ptrFreeBlock = mem_fit_first(ptrHead, size);

    if(size < sizeof(fb_t)){ // On vérifie que la taille demandé à allouer n'est pas trop petite
        size = sizeof(fb_t);
    }

    if(ptrFreeBlock != NULL){
        ptrPreviousBlock = find_prev_element(ptrFreeBlock);
        if(ptrPreviousBlock != NULL){
            if(ptrPreviousBlock == ptrHead){                                                           // Dans le cas ou l'élément précédent est ma tête
                fb_t * ptrNewfb = (fb_t*)((char*)ptrFreeBlock + size + sizeof(bb_t));                  // On place une nouvelle structure fb_t après la zone alloué
                ptrNewfb->size = ptrHead->size - size - sizeof(fb_t);
                                                // Calcul de la nouvelle taille de la zone libre
                ptrNewfb->next = NULL;
                *((fb_t**) get_memory_adr()) = ptrNewfb;

            }else{
              //Dans le cas contraire on link l'element précedent avec l'élément suivant de ptrFreeBloc si il n'est pas NULL
              if(ptrFreeBlock->next != NULL)
                ptrPreviousBlock->next = ptrFreeBlock->next;
              else
                ptrPreviousBlock->next = NULL;
            }

            ptrBuzyBlock = (bb_t *) ptrFreeBlock;
            ptrBuzyBlock->size = size;
            ptrNewBlock = (char *) ptrBuzyBlock +  sizeof(bb_t);

            return ptrNewBlock;
        }
    }

    return NULL;
}


void mem_free(void* zone)
{
    return;
}


void mem_show(void (*print)(void * ptr, size_t size, int free))
{
    fb_t* ptrFreeBlock = *(fb_t**) get_memory_adr();
    char* ptrCurrent = get_memory_adr() + sizeof(char*); //first addressable memory cell

    do
    {
        if (ptrCurrent == (char*)ptrFreeBlock)
        {
            print(ptrCurrent, ((fb_t*)ptrCurrent)->size, 0);
            ptrCurrent = (char*)ptrCurrent + ((fb_t*)ptrCurrent)->size + sizeof(fb_t);
        }
        else
        {
            print(ptrCurrent, ((bb_t*)ptrCurrent)->size, 1);
            ptrCurrent = (char*)ptrCurrent + ((bb_t*)ptrCurrent)->size + sizeof(bb_t);
        }
    } while(ptrCurrent < (char*)(get_memory_adr() + get_memory_size()));

    return;
}

void mem_fit(mem_fit_function_t* ptr)
{
    return;
}

fb_t * mem_fit_first(fb_t* list, size_t size)
{
    fb_t* ptrHead = list;
    fb_t* ptrCurrent = ptrHead;
    while(ptrCurrent!=NULL && ptrCurrent->size <= size+sizeof(fb_t)){
        ptrCurrent = ptrCurrent->next;
    }
    if(ptrCurrent!=NULL && ptrCurrent->size >= size){
        return ptrCurrent;
    }else {
        return NULL;
    }
}


fb_t * mem_fit_best(fb_t* stct, size_t size)
{
    return NULL;
}


fb_t * mem_fit_worst(fb_t* stct, size_t size)
{
    return NULL;
}

fb_t * find_prev_element(fb_t* ptrFreeBlock){
    fb_t* ptrHead = *(fb_t**) get_memory_adr();
    fb_t* ptrCurrent = ptrHead;
    fb_t* ptrBefore;

    while(ptrCurrent!=NULL &&  ptrCurrent < ptrFreeBlock){
         ptrBefore = ptrCurrent;
         ptrCurrent = ptrCurrent->next;
    }

    return ptrBefore;
}
