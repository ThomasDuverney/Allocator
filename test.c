#include "mem.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>

// FONCTIONS D'AFFICHAGE DE LA MÉMOIRE
void afficher_zone(void *adresse, size_t taille, int free)
{
  printf("Zone %s, Adresse : %lx, Taille : %lu\n", free?"libre":"occupee",
         (unsigned long) adresse, (unsigned long) taille);
}

void afficher_zone_libre(void *adresse, size_t taille, int free)
{
  if (free)
	  afficher_zone(adresse, taille, 1);
}

void afficher_zone_occupee(void *adresse, size_t taille, int free)
{
  if (!free)
	  afficher_zone(adresse, taille, 0);
}

// FONCTIONS TEST SUR L'ALLOCATEUR
void test_alloc_excessive_size(){
    printf ("Tentative d'allocation d'un nombre plus grand que la taille de la mémoire\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    mem_alloc( get_memory_size() + 200);
    mem_show(afficher_zone);
    printf ("\x1b[32mTest OK\n\x1b[32m");
    printf ("\x1b[0m--------------------------");
}

void test_alloc_size_too_small(){
    printf ("\n\nTentative d'allocation d'un nombre plus petit que la taille autorisée (taille d'une structure de bloc occupé)\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    mem_alloc(2);
    mem_show(afficher_zone);
    printf ("\x1b[32mTest OK\n\x1b[32m");
    printf ("\x1b[0m");
}

void test_alloc_memory_full(){
    printf ("\n\nTentative d'allocation dans une mémoire pleine/sans zone disponible assez grande\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    mem_alloc(get_memory_size()-30);
    mem_alloc(50);
    mem_show(afficher_zone);
    printf ("\x1b[32mTest OK\n\x1b[32m");
    printf ("\x1b[0m");
}

void test_alloc_requiring_padding(){
    printf ("\n\nAllocation d'une taille nécessitant un padding dans la zone allouée\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    void *zone1 = mem_alloc(50);
    mem_alloc(40);
    mem_free(zone1);
    mem_alloc(46);
    mem_show(afficher_zone);
    printf ("\x1b[32mTest OK\n\x1b[32m");
    printf ("\x1b[0m");
}

int main(int argc, char *argv[]) {
	fprintf(stderr, "\n****** Tests réalisant divers cas d'utilisation de l'allocateur mémoire ******\n"
 		"\n");
  test_alloc_excessive_size();
  test_alloc_size_too_small();
  test_alloc_memory_full();
  test_alloc_requiring_padding();
}


