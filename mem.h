#ifndef __MEM_H
#define __MEM_H
#include <stddef.h>

typedef struct fb fb_t;
typedef struct bb bb_t;

/* Struct for free block*/
struct fb{
    size_t size;
    struct fb *next;
};
/* Struct for busy block*/
struct bb{
    size_t size;
};

void mem_init(char* mem, size_t taille);
void* mem_alloc(size_t size);
void mem_free(void* ptr);
size_t mem_get_size(void * ptr);

/* Itérateur sur le contenu de l'allocateur */
void mem_show(void (*print)(void *, size_t size, int free));

/* Choix de la strategie et strategies usuelles */
typedef fb_t * (mem_fit_function_t)(fb_t *, size_t);

void mem_fit(mem_fit_function_t* ptr);
fb_t* mem_fit_first(fb_t* stct, size_t size);

/* Si vous avez le temps */
fb_t* mem_fit_best(fb_t* stct, size_t size);
fb_t* mem_fit_worst(fb_t* stct, size_t size);

/* Function to find the previous element of ptrFreeBloc*/
fb_t* find_prev_free_block(char* ptrBlock);

int is_on_busy_struct(char * ptrZoneToFree);
#endif
