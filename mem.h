#ifndef __MEM_H
#define __MEM_H
#include <stddef.h>

struct fb{
    size_t size;
    struct fb *next;
};

void mem_init(char* mem, size_t taille);
void* mem_alloc(size_t size);
void mem_free(void* ptr);
size_t mem_get_size(void * ptr);

/* Itérateur sur le contenu de l'allocateur */
void mem_show(void (*print)(void *, size_t size, int free));

/* Choix de la strategie et strategies usuelles */
typedef struct fb* (mem_fit_function_t)(struct fb *, size_t);

void mem_fit(mem_fit_function_t* ptr);
struct fb* mem_fit_first(struct fb* stct, size_t size);

/* Si vous avez le temps */
struct fb* mem_fit_best(struct fb* stct, size_t size);
struct fb* mem_fit_worst(struct fb* stct, size_t size);

#endif
