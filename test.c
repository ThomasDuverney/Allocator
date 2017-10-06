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
void test_alloc_out_bounds(){
    printf ("Tentative d'allocation d'un nombre plus grand que la taille de la mémoire\n");
    mem_init(get_memory_adr(), get_memory_size());
    mem_alloc( get_memory_size() + 200);
    mem_show(afficher_zone);
    printf ("\x1b[32mTest OK\n\x1b[32m");
    printf ("\x1b[31mTest OK\n\x1b[31m");
    printf ("\x1b[0m");
    printf("Test\n");
}

int main(int argc, char *argv[]) {
	fprintf(stderr, "Test réalisant divers cas d'utilisation de l'allocateur mémoire"
 		"\n");
  test_alloc_out_bounds();
}
