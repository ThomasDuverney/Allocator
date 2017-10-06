#include "mem.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>

mem_fit_function_t * search_function;

void mem_init(char* mem, size_t taille)
{
    *((fb_t**) mem) = (fb_t*)(mem + sizeof(fb_t*));
    fb_t* ptrHead = *((fb_t**) mem);
    ptrHead->size = taille - sizeof(fb_t) - sizeof(char *);
    ptrHead->next = NULL;
    mem_fit(mem_fit_first);
}

void* mem_alloc(size_t size)
{
    void * ptrNewBlock; // address of the pointer to allocate
    fb_t * ptrPreviousBlock;
    bb_t * ptrBusyBlock;  

    if(size < sizeof(bb_t)){ // checking that the size to allocate is not too small
    						 // if so, raise it to the size of a struct of a busy block.
    	printf("Info : La taille à allouer a été augmentée jusqu'a la taille minimale d'allocation permise !\n");
        size = sizeof(bb_t);
    }
    else if(size > get_memory_size()){
    	printf("Erreur : La taille à allouer est supérieure à la taille allouable totale !\n");
    	return NULL;
    }

    fb_t* ptrHead = *(fb_t**) get_memory_adr();			
    fb_t* ptrFreeBlock = search_function(ptrHead, size);	// pointer on the block great enough to allow the allocation

    if(ptrFreeBlock != NULL){
        // padding when cannot add structure + 1oct after the block newly allocated
        if ((size > ptrFreeBlock->size - sizeof(bb_t)) && (size < sizeof(fb_t) + ptrFreeBlock->size - sizeof(bb_t)))
        {
			size = sizeof(fb_t) + ptrFreeBlock->size - sizeof(bb_t);
			printf("Info: La taille à allouer a été augmentée pour remplir le reste du bloc mémoire !\n");
        }


        ptrPreviousBlock = find_prev_free_block((char*)ptrFreeBlock);
        // memory amount to allocate matches exactly the available size : no need to create a new free zone.
        if(size == sizeof(fb_t) + ptrFreeBlock->size - sizeof(bb_t))
        {
            if(ptrPreviousBlock == NULL){	//no smaller free block before the free block that is great enough to allocate
                *((fb_t**) get_memory_adr()) = ptrFreeBlock->next;
            }else{
                ptrPreviousBlock->next = ptrFreeBlock->next;
            }
        }else{
        	fb_t * ptrNewfb = NULL;
            if(ptrPreviousBlock == NULL){	// no smaller free block before the free block that is great enough to allocate
            		ptrNewfb = (fb_t*)((char*)ptrFreeBlock + size + sizeof(bb_t));	// setting a new structure of free block after the block allocated
	                ptrNewfb->size = ptrFreeBlock->size - size - sizeof(bb_t);	// computing the size of the new structure of free block
	                ptrNewfb->next = ptrFreeBlock->next;
	                *((fb_t**) get_memory_adr()) = ptrNewfb;
            }
            else{	// existence of some smaller free blocks before the block allocated
            		ptrNewfb = (fb_t*)((char*)ptrFreeBlock + size + sizeof(bb_t));	// setting a new structure of free block after the block allocated
	                ptrNewfb->size = ptrFreeBlock->size - size - sizeof(bb_t);		// computing the size of the new structure of free block
	                ptrNewfb->next = ptrFreeBlock->next;
	                ptrPreviousBlock->next = ptrNewfb;
            }
            printf("Info : Nouvelle zone libre créée : taille %lu !\n", ptrNewfb->size + sizeof(bb_t));
        }

        ptrBusyBlock = (bb_t *) ptrFreeBlock;
        ptrBusyBlock->size = size;
        ptrNewBlock = (char *) ptrBusyBlock +  sizeof(bb_t);	// considering that the pointer is set before the structure

        return ptrNewBlock;	

    }

    return NULL;
}
void mem_free(void* zone){
    fb_t* ptrHead = *(fb_t**) get_memory_adr();
	  fb_t* ptrFreeBefore = find_prev_free_block((char*)zone);
    bb_t * ptrZoneToFree = (bb_t *)(zone - sizeof(bb_t));

    if( zone > get_memory_adr() + get_memory_size()){
        printf("L'adresse mémoire à libérer n'existe pas !\n");
        return;
    }
    else if (!is_on_busy_struct((char *)ptrZoneToFree))
    {
    	printf("L'adresse mémoire à libérer n'est pas correcte !\n");
        return;  
    }
    // no smaller free block before the free block that is great enough to allocate
    if(ptrFreeBefore == NULL){
            // checking whether the next memory zone is free or not
            if( zone + ptrZoneToFree->size ==  (char*)ptrHead ){
             	// merging the zones and updating ptrHead
             	((fb_t *)ptrZoneToFree)->size = ptrZoneToFree->size + sizeof(bb_t)+ ptrHead->size;
            	((fb_t *)ptrZoneToFree)->next = ptrHead->next;
            	printf("Info : Fusion du bloc libre avec le bloc libre suivant !\n");
            }else{
                ((fb_t *)ptrZoneToFree)->size = ptrZoneToFree->size + sizeof(bb_t) - sizeof(fb_t);
                ((fb_t *)ptrZoneToFree)->next = ptrHead;
            }
            *((fb_t**) get_memory_adr()) =  ((fb_t *)ptrZoneToFree);

    // no smaller free block before the free block that is great enough to allocate
    }else {
        fb_t * ptrNewFb = NULL;

        // memory zone preceeding the block allocated is free
        if( (char*)ptrFreeBefore+ ptrFreeBefore->size + sizeof(fb_t) == (char*)ptrZoneToFree){
            ptrNewFb = ptrFreeBefore;
            ptrNewFb->size = ptrFreeBefore->size + sizeof(bb_t) + ptrZoneToFree->size;
            ptrNewFb->next  = ptrFreeBefore->next;
            printf("Info : Fusion du bloc libre avec le bloc libre précédent !\n");
        // memory zone preceeding the block allocated is busy
        }else{
            ptrNewFb = (fb_t *) ptrZoneToFree;
            ptrNewFb->size = ptrZoneToFree->size + sizeof(bb_t) - sizeof(fb_t);
            ptrNewFb->next = ptrFreeBefore->next;
            ptrFreeBefore->next = ptrNewFb;
        }
        // memory zone succeeding the block allocated is free
        if( zone + ptrZoneToFree->size == ptrFreeBefore->next){
            ptrNewFb->size += ptrFreeBefore->next->size + sizeof(fb_t);
            ptrNewFb->next = ptrFreeBefore->next->next;
            ptrFreeBefore->next = ptrNewFb;
            printf("Info : Fusion du bloc libre avec le bloc libre suivant !\n");
        }

    }
}

void mem_show(void (*print)(void * ptr, size_t size, int free)){
    fb_t* ptrFreeBlock = *(fb_t**) get_memory_adr();
    char* ptrCurrent = get_memory_adr() + sizeof(char*); //first addressable memory cell

    do
    {
        if (ptrCurrent == (char*)ptrFreeBlock)
        {
            print(ptrCurrent, ((fb_t*)ptrCurrent)->size + sizeof(fb_t) - sizeof(bb_t), 1);
            ptrCurrent = (char*)ptrCurrent + ((fb_t*)ptrCurrent)->size + sizeof(fb_t);
            ptrFreeBlock = ptrFreeBlock->next;
        }
        else
        {
            print(ptrCurrent, ((bb_t*)ptrCurrent)->size, 0);
            ptrCurrent = (char*)ptrCurrent + ((bb_t*)ptrCurrent)->size + sizeof(bb_t);
        }
    } while(ptrCurrent < (char*)(get_memory_adr() + get_memory_size()));

    return;
}

void mem_fit(mem_fit_function_t* ptr)
{
    search_function = ptr;
}

fb_t * mem_fit_first(fb_t* list, size_t size)
{
    fb_t* ptrCurrent = list;
    while(ptrCurrent!=NULL && ptrCurrent->size + sizeof(fb_t) < size + sizeof(bb_t)){
        ptrCurrent = ptrCurrent->next;
    }
    if(ptrCurrent!=NULL && ptrCurrent->size +sizeof(fb_t) >= size + sizeof(bb_t)){
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

fb_t * find_prev_free_block(char* ptrBlock){	// returns NULL if the previous pointer in the chain is ptrHead
    fb_t* ptrHead = *(fb_t**) get_memory_adr();
    fb_t* ptrCurrent = ptrHead;
    fb_t* ptrBefore = NULL;
    if((char*)ptrCurrent < ptrBlock){

        while(ptrCurrent != NULL &&  (char*)ptrCurrent < ptrBlock){
             ptrBefore = ptrCurrent;
             ptrCurrent = ptrCurrent->next;
        }
        return ptrBefore;

    }else{
        return NULL;
    }
}

int is_on_busy_struct(char * ptrZoneToFree){	// return 1 only if ptrZoneToFree matches the beginning of a busy memory zone
    char * ptrCurrent = get_memory_adr() + sizeof(char *);  //never NULL
    fb_t* ptrPreviousFreeBlock = find_prev_free_block((char*) ptrZoneToFree);


    if (ptrPreviousFreeBlock == NULL)	// no free block before ptrZoneToFree
    {
        while (ptrCurrent < ptrZoneToFree)	// travelling through the chained list
        {
            ptrCurrent += ((bb_t*)ptrCurrent)->size + sizeof(bb_t);
        }
        if (ptrCurrent == (char*)ptrZoneToFree)	// found a structure of busy block
            return 1;
        return 0;
    }
    else	// existence of at least one structure of free zone before ptrZoneToFree
    {
        if ((char*)ptrPreviousFreeBlock + ptrPreviousFreeBlock->size + sizeof(fb_t) > ptrZoneToFree)	// ptrZoneToFree points inside a free block
            return 0;

        ptrCurrent = (char*) ptrPreviousFreeBlock + ptrPreviousFreeBlock->size + sizeof(fb_t);

        while (ptrCurrent < ptrZoneToFree)	// travelling through the chained list
        {
            ptrCurrent += sizeof(bb_t) + ((bb_t*)ptrCurrent)->size;
        }
        if (ptrCurrent == (char*) ptrZoneToFree)	// found a structure of busy block
            return 1;
        return 0;
    }

}
