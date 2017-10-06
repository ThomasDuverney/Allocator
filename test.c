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
    printf ("Allocation : taille %lu\n", get_memory_size() + 200);
    mem_alloc( get_memory_size() + 200);
    printf("\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");
}

void test_alloc_size_too_small(){
    printf ("Tentative d'allocation d'un nombre plus petit que la taille autorisée (taille d'une structure de bloc occupé)\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    printf ("Allocation : taille 2\n");
    mem_alloc(2);
    printf("\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");
}

void test_alloc_memory_full(){
    printf ("Tentative d'allocation dans une mémoire pleine/sans zone disponible assez grande\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    printf ("Allocation : taille %lu\n", get_memory_size()-30);
    mem_alloc(get_memory_size()-30);
    printf ("Allocation : taille 50\n");
    mem_alloc(50);
    printf("Erreur : aucun espace libre de cette taille n'a été trouvé !\n\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");
}

void test_alloc_requiring_padding(){
    printf ("Allocation d'une taille nécessitant un padding dans la zone allouée\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    void *zone1 = mem_alloc(50);
    printf ("Allocation : taille 40\n");
    mem_alloc(40);
    printf ("Libération : zone de taille 50\n");
    mem_free(zone1);
    printf ("Allocation : taille 46\n");
    mem_alloc(46);
    printf("\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");
}

void test_alloc_creates_new_free_zone(){
    printf ("Allocation d'une taille nécessitant la création d'une nouvelle structure de zone libre\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    printf ("Allocation : taille 50\n");
    void *zone1 = mem_alloc(50);
    printf ("Allocation : taille 40\n");
    mem_alloc(40);
    printf ("Libération : zone de taille 50\n");
    mem_free(zone1);
    printf ("Allocation : taille 20\n");
    mem_alloc(20);
    printf("\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");	
}

void test_alloc_on_exact_size_of_free_block(){
    printf ("Allocation d'une taille égale à celle du bloc libre choisi pour l'allocation\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    printf ("Allocation : taille 50\n");
    void *zone1 = mem_alloc(50);
    printf ("Allocation : taille 40\n");
    mem_alloc(40);
    printf ("Libération : zone de taille 50\n");
    mem_free(zone1);
    printf ("Allocation : taille 50\n");
    mem_alloc(50);
    printf("\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");	
}

void test_free_out_of_bounds(){
    printf ("Tentative de libération de mémoire à une adresse hors de la mémoire\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    printf("Libération de mémoire à l'adresse %p dans une mémoire d'adresse maximum %p\n", 
    		get_memory_adr() + get_memory_size() + 10000, get_memory_adr() + get_memory_size());
    mem_free(get_memory_adr() + get_memory_size() + 10000);
    printf("\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");	
}

void test_free_no_fusion(){
    printf ("Libération de bloc mémoire entre deux blocs de mémoire allouée\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    printf ("Allocation : taille 50\n");
    mem_alloc(50);
    printf ("Allocation : taille 40\n");
    void* zone2 = mem_alloc(40);
    printf ("Allocation : taille 30\n");
    mem_alloc(30);
    printf ("Libération : zone de taille 40\n");
    mem_free(zone2);
    printf("\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");	
}

int main(int argc, char *argv[]) {
	fprintf(stderr, "\n****** Tests réalisant divers cas d'utilisation de l'allocateur mémoire ******\n\n\n"
 		"\n");
	printf ("TEST N°1 : ");
	test_alloc_creates_new_free_zone();
	printf ("TEST N°2 : ");
	test_alloc_requiring_padding();
	printf ("TEST N°3 : ");
	test_alloc_excessive_size();
	printf ("TEST N°4 : ");
	test_alloc_size_too_small();
	printf ("TEST N°5 : ");
	test_alloc_memory_full();
	printf ("TEST N°6 : ");
	test_alloc_on_exact_size_of_free_block();
	printf ("TEST N°7 : ");
	test_free_no_fusion();
	printf ("TEST N°8 : ");
	test_free_out_of_bounds();
	printf ("TEST N°9 : ");

	

}


