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
    void * ptrNewBlock; // l'adresse du block à allouer
    fb_t * ptrPreviousBlock;
    bb_t * ptrBusyBlock;  // le pointeur de la structure busy block

    if(size < sizeof(bb_t)){ // On vérifie que la taille demandée à allouer n'est pas trop petite
        size = sizeof(bb_t);
    }

    fb_t* ptrHead = *(fb_t**) get_memory_adr();			// pointeur tête
    fb_t* ptrFreeBlock = search_function(ptrHead, size);	//pointeur sur le premier bloc libre de taille suffisante

    if(ptrFreeBlock != NULL){
        //padding when cannot add structure + 1oct after the block newly allocated
        if (size > ptrFreeBlock->size - sizeof(bb_t))

			size = sizeof(fb_t) + ptrFreeBlock->size - sizeof(bb_t);


        ptrPreviousBlock = find_prev_free_block((char*)ptrFreeBlock);
        // Dans le cas ou la zone à allouer fait exactement la taille de la zone libre on ne créait pas de structure fb_t après la zone à allouer
        if(size == sizeof(fb_t) + ptrFreeBlock->size - sizeof(bb_t))
        {
            if(ptrPreviousBlock == NULL){	//aucun bloc libre avant le bloc libre qui est assez grand
                *((fb_t**) get_memory_adr()) = ptrFreeBlock->next;
            }else{
                ptrPreviousBlock->next = ptrFreeBlock->next;
            }
        }else{
            if(ptrPreviousBlock == NULL){	//aucun bloc libre avant le bloc libre qui est assez grand
            		fb_t * ptrNewfb = (fb_t*)((char*)ptrFreeBlock + size + sizeof(bb_t));// On place une nouvelle structure fb_t après la zone allouée
	                ptrNewfb->size = ptrFreeBlock->size - size - sizeof(bb_t);// Calcul de la nouvelle taille de la zone libre
	                ptrNewfb->next = ptrFreeBlock->next;
	                *((fb_t**) get_memory_adr()) = ptrNewfb;
            }
            else{	//Il y a des blocs libres avant
            		fb_t * ptrNewfb = (fb_t*)((char*)ptrFreeBlock + size + sizeof(bb_t));// On place une nouvelle structure fb_t après la zone allouée
	                ptrNewfb->size = ptrFreeBlock->size - size - sizeof(bb_t);// Calcul de la nouvelle taille de la zone libre
	                ptrNewfb->next = ptrFreeBlock->next;
	                ptrPreviousBlock->next = ptrNewfb;
            }
        }

        ptrBusyBlock = (bb_t *) ptrFreeBlock;
        ptrBusyBlock->size = size;
        ptrNewBlock = (char *) ptrBusyBlock +  sizeof(bb_t);	// à changer si on veut placer le pointeur de la zone après la struct

        return ptrNewBlock;

    }

    return NULL;
}
void mem_free(void* zone){
    fb_t* ptrHead = *(fb_t**) get_memory_adr();
	  fb_t* ptrFreeBefore = find_prev_free_block((char*)zone);
    bb_t * ptrZoneToFree = (bb_t *)(zone - sizeof(bb_t));

    if( zone > get_memory_adr() + get_memory_size() || !is_on_busy_struct((char *)ptrZoneToFree)){
        printf("L'adresse mémoire à libérer n'est pas correcte\n");
        return;
    }
        //Dans le cas ou il n'y a aucune zone libre avant
    if(ptrFreeBefore == NULL){
            //On vérifie si il y a pas une structure fb_t juste après la zone à libérer
            if( zone + ptrZoneToFree->size ==  (char*)ptrHead ){
              // On fusionne les zones et refait le linkage avec ptrHEAD
              ((fb_t *)ptrZoneToFree)->size = ptrZoneToFree->size + sizeof(bb_t)+ ptrHead->size;
              ((fb_t *)ptrZoneToFree)->next = ptrHead->next;

            }else{
                ((fb_t *)ptrZoneToFree)->size = ptrZoneToFree->size + sizeof(bb_t) - sizeof(fb_t);
                ((fb_t *)ptrZoneToFree)->next = ptrHead;
            }
            *((fb_t**) get_memory_adr()) =  ((fb_t *)ptrZoneToFree);

    // Dans le cas ou il y'a un element à gauche dans la liste des zones libres
    }else {
        fb_t * ptrNewFb;

         // Dans le cas où cet élement précede la zone à libérer
        if( (char*)ptrFreeBefore+ ptrFreeBefore->size + sizeof(fb_t) == (char*)ptrZoneToFree){
            ptrNewFb = ptrFreeBefore;
            ptrNewFb->size = ptrFreeBefore->size + sizeof(bb_t) + ptrZoneToFree->size;
            ptrNewFb->next  = ptrFreeBefore->next;
            /* INITIALISER LE NEXT SI ÇA PETE ! */
        // Dans le cas où cette élement ne précède pas la zone à libérer
        }else{
            ptrNewFb = (fb_t *) ptrZoneToFree;
            ptrNewFb->size = ptrZoneToFree->size + sizeof(bb_t) - sizeof(fb_t);
            ptrNewFb->next = ptrFreeBefore->next;
            ptrFreeBefore->next = ptrNewFb;
        }
        // Dans le cas ou il y'a une structure fb_t à droite qui suit la zone à libérer
        if( zone + ptrZoneToFree->size == ptrFreeBefore->next){
            ptrNewFb->size += ptrFreeBefore->next->size + sizeof(fb_t);
            ptrNewFb->next = ptrFreeBefore->next->next;
            ptrFreeBefore->next = ptrNewFb;
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

fb_t * find_prev_free_block(char* ptrBlock){
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

int is_on_busy_struct(char * ptrZoneToFree){
    char * ptrCurrent = get_memory_adr() + sizeof(char *);  //never NULL
    fb_t* ptrPreviousFreeBlock = find_prev_free_block((char*) ptrZoneToFree);


    if (ptrPreviousFreeBlock == NULL)
    {
        while (ptrCurrent < ptrZoneToFree)
        {
            ptrCurrent += ((bb_t*)ptrCurrent)->size + sizeof(bb_t);
        }
        if (ptrCurrent == (char*)ptrZoneToFree)
            return 1;
        return 0;
    }
    else
    {
        if ((char*)ptrPreviousFreeBlock + ptrPreviousFreeBlock->size + sizeof(fb_t) > ptrZoneToFree)
            return 0;

        ptrCurrent = (char*) ptrPreviousFreeBlock + ptrPreviousFreeBlock->size + sizeof(fb_t);

        while (ptrCurrent < ptrZoneToFree)
        {
            ptrCurrent += sizeof(bb_t) + ((bb_t*)ptrCurrent)->size;
        }
        if (ptrCurrent == (char*) ptrZoneToFree)
            return 1;
        return 0;
    }

}
