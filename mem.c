#include "mem.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>


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
    bb_t * ptrBuzyBlock;  // le pointeur de la structure busy block

    if(size < sizeof(fb_t)){ // On vérifie que la taille demandée à allouer n'est pas trop petite
        size = sizeof(fb_t);
    }

    fb_t* ptrHead = *(fb_t**) get_memory_adr();
    fb_t* ptrFreeBlock = mem_fit_first(ptrHead, size);

    if(ptrFreeBlock != NULL){
        // FONCTIONNE PAS ! 
        /*if(size + sizeof(bb_t) < ptrFreeBlock->size + sizeof(fb_t)){
            size = ptrFreeBlock->size + sizeof(fb_t) - sizeof(bb_t);    //filling the full zone
        }*/
        ptrPreviousBlock = find_prev_free_block((char*)ptrFreeBlock);
        // Dans le cas ou la zone à alloué fait exactement la taille de la zone libre on ne créait pas de structure fb_t après la zone à alloué
        if(ptrFreeBlock->size  - sizeof(bb_t) == size){
            if(ptrPreviousBlock == NULL){
                *((fb_t**) get_memory_adr()) = ptrFreeBlock->next;
            }else{
                ptrPreviousBlock->next = ptrFreeBlock->next;
            }
        }else{
             // Dans le cas ou l'élément précédent est ma tête
            if(ptrPreviousBlock == NULL){
                fb_t * ptrNewfb = (fb_t*)((char*)ptrFreeBlock + size + sizeof(bb_t));// On place une nouvelle structure fb_t après la zone alloué
                ptrNewfb->size = ptrHead->size - size - sizeof(bb_t);// Calcul de la nouvelle taille de la zone libre
                ptrNewfb->next = NULL;
                *((fb_t**) get_memory_adr()) = ptrNewfb;

            }else{
              //Dans le cas contraire on link l'element précedent avec l'élément suivant de ptrFreeBloc si il n'est pas NULL
              if(ptrFreeBlock->next != NULL)
                ptrPreviousBlock->next = ptrFreeBlock->next;
              else
                ptrPreviousBlock->next = NULL;
            }
        }

        ptrBuzyBlock = (bb_t *) ptrFreeBlock;
        ptrBuzyBlock->size = size;
        ptrNewBlock = (char *) ptrBuzyBlock +  sizeof(bb_t);

        return ptrNewBlock;

    }

    return NULL;
}


void mem_free(void* zone){
    fb_t* ptrHead = *(fb_t**) get_memory_adr();
	  fb_t* ptrFreeBefore = find_prev_free_block((char*)zone);
    bb_t * ptrZoneToFree = (bb_t *)(zone - sizeof(bb_t));

  //Dans le cas ou il n'y a pas d'élément à gauche dans la liste des zones libres
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
            print(ptrCurrent + sizeof(fb_t), ((fb_t*)ptrCurrent)->size, 1);
            ptrCurrent = (char*)ptrCurrent + ((fb_t*)ptrCurrent)->size + sizeof(fb_t);
            ptrFreeBlock = ptrFreeBlock->next;
        }
        else
        {
            print(ptrCurrent + sizeof(bb_t), ((bb_t*)ptrCurrent)->size, 0);
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
    fb_t* ptrCurrent = list;
    while(ptrCurrent!=NULL && ptrCurrent->size < size+sizeof(bb_t)){
        ptrCurrent = ptrCurrent->next;
    }
    if(ptrCurrent!=NULL && ptrCurrent->size >= size+sizeof(bb_t)){
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
    fb_t* ptrBefore;
    if( (char*)ptrCurrent < ptrBlock){

        while(ptrCurrent!=NULL &&  (char*)ptrCurrent < ptrBlock){
             ptrBefore = ptrCurrent;
             ptrCurrent = ptrCurrent->next;
        }
        return ptrBefore;

    }else{
        return NULL;
    }
}

// fb_t * find_prev_element(fb_t* ptrBlock){
//     fb_t* ptrHead = *(fb_t**) get_memory_adr();
//     fb_t* ptrCurrent = ptrHead;
//     fb_t* ptrBefore;

//     while(ptrCurrent!=NULL &&  ptrCurrent < ptrFreeBlock){
//          ptrBefore = ptrCurrent;
//          ptrCurrent = ptrCurrent->next;
//     }

//     return ptrBefore;
//}


//if (((char*)ptrFreeBefore + size(fb_t) + ptrFreeBefore->size) < zone)   //block preceeding zone is busy
    //test block following zone
// if (ptrHead == NULL)	//memory full
// zone = (bb_t*) zone;
// fb_t* newFb;
// newFb->size = (sizeof(bb_t) + zone->size) - sizeof(fb_t);
