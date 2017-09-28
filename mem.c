#include "mem.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>


void mem_init(char* mem, size_t taille)
{
    fb* ptrHead = *(fb**) mem;
    ptrHead->size = taille;
    ptrHead->next = NULL;
    mem_fit(&mem_fit_first);
}



void* mem_alloc(size_t size)
{
	//can allocate to not-2-power address ?
    void * ptrNewBlock; // l'adresse du block à alloué
    fb * ptrPreviousBlock;
    bb * ptrBuzyBlock;  // le pointeur de la structure busy block

    fb* ptrHead = *(fb**) get_memory_adr();
    fb* ptrFreeBloc = mem_fit_first(ptrHead, size);

    if(size < sizeof(fb)){ // On vérifie que la taille demandé à allouer n'est pas trop petite
        size = sizeof(fb);
    }

    if(ptrFreeBloc != NULL){
        ptrPreviousBlock = find_preced_element(ptrFreeBloc);
        if(ptrPreviousBlock == ptrHead){                         // Dans le cas ou l'élément précédent est ma tête
            ptrFreeBloc = ptrFreeBloc + sizeof( fb *);           // Décalage de ptrFreeBloc pour ne pas écrasé la tête de la liste
            fb * ptrNewFb = ptrFreeBloc + size;                  // On place une nouvelle structure fb après la zone alloué
            ptrNewFb->size = ptrHead->size - size - sizeof( fb); // Calcul de la nouvelle taille de la zone libre

        }else{
            //Dans le cas contraire on link l'element précedent avec l'élément suivant de ptrFreeBloc
            ptrPreviousBlock->next = ptrFreeBloc->next;
        }

        ptrBuzyBlock = (bb *) ptrFreeBloc;
        ptrBuzyBlock->size= size;
        ptrNewBlock = (void *) ptrBuzyBlock +  sizeof(bb);

        return ptrNewBlock;
    }
    /*	if (ptrNew == NULL)
    		return NULL;
    	ptrNew->busy = "1";
    	ptrNew->size = size;
    	ptrNew->next = NULL;
    	ptrHead = ptrHead + size + sizeof(struct fb);
    	return ptrNew;
    */
    return NULL;
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

struct fb* find_preced_element(fb* ptrFreeBloc){
    fb* ptrHead = *(fb**) get_memory_adr();
    fb* ptrCurrent = ptrHead;
    fb* ptrBefore;

    while(ptrCurrent!=NULL &&  ptrCurrent < ptrFreeBloc){
         ptrBefore = ptrCurrent;
         ptrCurrent = ptrCurrent->next;
    }

    return ptrBefore;
}
