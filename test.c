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
    printf ("Tentative d'allocation d'un nombre plus grand que la taille de la mémoire\x1b[0m\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    printf ("Allocation : taille %lu\n", get_memory_size() + 200);
    mem_alloc( get_memory_size() + 200);
    printf("\nETAT DE LA MEMOIRE\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");
}

void test_alloc_size_too_small(){
    printf ("Tentative d'allocation d'un nombre plus petit que la taille autorisée (taille d'une structure de bloc occupé)\x1b[0m\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    printf ("Allocation : taille 2\n");
    mem_alloc(2);
    printf("\nETAT DE LA MEMOIRE\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");
}

void test_alloc_memory_full(){
    printf ("Tentative d'allocation dans une mémoire pleine/sans zone disponible assez grande\x1b[0m\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    printf ("Allocation : taille %lu\n", get_memory_size()-30);
    mem_alloc(get_memory_size()-30);
    printf ("Allocation : taille 50\n");
    mem_alloc(50);
    printf("Erreur : aucun espace libre de cette taille n'a été trouvé !\n");
    printf("\nETAT DE LA MEMOIRE\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");
}

void test_alloc_requiring_padding(){
    printf ("Allocation d'une taille nécessitant un padding dans la zone allouée\x1b[0m\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    void *zone1 = mem_alloc(50);
    printf ("Allocation : taille 40\n");
    mem_alloc(40);
    printf ("Libération : zone de taille 50\n");
    mem_free(zone1);
    printf ("Allocation : taille 46\n");
    mem_alloc(46);
    printf("\nETAT DE LA MEMOIRE\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");
}

void test_alloc_creates_new_free_zone(){
    printf ("Allocation d'une taille nécessitant la création d'une nouvelle structure de zone libre\x1b[0m\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    printf ("Allocation : taille 50\n");
    void *zone1 = mem_alloc(50);
    printf ("Allocation : taille 40\n");
    mem_alloc(40);
    printf ("Libération : zone de taille 50\n");
    mem_free(zone1);
    printf ("Allocation : taille 20\n");
    mem_alloc(20);
    printf("\nETAT DE LA MEMOIRE\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");	
}

void test_alloc_on_exact_size_of_free_block(){
    printf ("Allocation d'une taille égale à celle du bloc libre choisi pour l'allocation\x1b[0m\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    printf ("Allocation : taille 50\n");
    void *zone1 = mem_alloc(50);
    printf ("Allocation : taille 40\n");
    mem_alloc(40);
    printf ("Libération : zone de taille 50\n");
    mem_free(zone1);
    printf ("Allocation : taille 50\n");
    mem_alloc(50);
    printf("\nETAT DE LA MEMOIRE\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");	
}

void test_free_out_of_bounds(){
    printf ("Tentative de libération de mémoire à une adresse hors de la mémoire\x1b[0m\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    printf("Libération de mémoire à l'adresse %lu dans une mémoire d'adresse maximum %lu\n", 
    		get_memory_size() + 10000, get_memory_size());
    mem_free(get_memory_adr() + get_memory_size() + 10000);
    printf("\nETAT DE LA MEMOIRE\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");	
}

void test_free_no_fusion(){
    printf ("Libération de bloc mémoire entre deux blocs de mémoire allouée\x1b[0m\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    printf ("Allocation : taille 50\n");
    mem_alloc(50);
    printf ("Allocation : taille 40\n");
    void* zone2 = mem_alloc(40);
    printf ("Allocation : taille 30\n");
    mem_alloc(30);
    printf ("Libération : zone de taille 40\n");
    mem_free(zone2);
    printf("\nETAT DE LA MEMOIRE\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");	
}

void test_free_address_inside_busy_block(){
    printf ("Tentative de libération de mémoire à une adresse située à l'intérieur d'un bloc de mémoire allouée\x1b[0m\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    void *zone1 = mem_alloc(50);
    printf ("Allocation : taille 50 à l'adresse %i\n",(int) (zone1-get_memory_adr()));
    printf ("Libération de mémoire à l'adresse %i\n", (int) (zone1-get_memory_adr())+20);
    mem_free(zone1 + 20);
    printf("\nETAT DE LA MEMOIRE\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");	
}

void test_free_address_inside_free_block(){
    printf ("Tentative de libération de mémoire à une adresse située à l'intérieur d'une zone libre\x1b[0m\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    void *zone1 = mem_alloc(50);
    printf ("Allocation : taille 50 à l'adresse %i\n",(int) (zone1-get_memory_adr()));
    void *zone2 = mem_alloc(60);
    printf ("Allocation : taille 60 à l'adresse %i\n",(int) (zone2-get_memory_adr()));
    printf ("Libération de mémoire à l'adresse %i\n", (int) (zone1-get_memory_adr()));
    mem_free(zone1);
    printf ("Libération de mémoire à l'adresse %i\n", (int) (zone1-get_memory_adr())+15);
    mem_free(zone1+15);
    printf("\nETAT DE LA MEMOIRE\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");
}

void test_free_fusion_with_prev_block(){
    printf ("Libération de mémoire et fusion avec la zone libre précédente\x1b[0m\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    void *zone1 = mem_alloc(50);
    printf ("Allocation : taille 50 à l'adresse %i\n",(int) (zone1-get_memory_adr()));
    void *zone2 = mem_alloc(60);
    printf ("Allocation : taille 60 à l'adresse %i\n",(int) (zone2-get_memory_adr()));
	void *zone3 = mem_alloc(80);
    printf ("Allocation : taille 80 à l'adresse %i\n",(int) (zone3-get_memory_adr()));
    printf ("Libération de mémoire à l'adresse %i\n", (int) (zone1-get_memory_adr()));
    mem_free(zone1);
    printf ("Libération de mémoire à l'adresse %i\n", (int) (zone2-get_memory_adr()));
    mem_free(zone2);
    printf("\nETAT DE LA MEMOIRE\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");
}

void test_free_fusion_with_next_block(){
    printf ("Libération de mémoire et fusion avec la zone libre suivante\x1b[0m\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    void *zone1 = mem_alloc(50);
    printf ("Allocation : taille 50 à l'adresse %i\n",(int) (zone1-get_memory_adr()));
    void *zone2 = mem_alloc(60);
    printf ("Allocation : taille 60 à l'adresse %i\n",(int) (zone2-get_memory_adr()));
    printf ("Libération de mémoire à l'adresse %i\n", (int) (zone2-get_memory_adr()));
    mem_free(zone2);
    printf("\nETAT DE LA MEMOIRE\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");
}

void test_free_fusion_with_both_adjacent_blocks(){
    printf ("Libération de mémoire et fusion avec les deux zones libres adjacentes au bloc libéré\x1b[0m\n\n");
    mem_init(get_memory_adr(), get_memory_size());
    void *zone1 = mem_alloc(50);
    printf ("Allocation : taille 50 à l'adresse %i\n",(int) (zone1-get_memory_adr()));
    void *zone2 = mem_alloc(60);
    printf ("Allocation : taille 60 à l'adresse %i\n",(int) (zone2-get_memory_adr()));
    printf ("Libération de mémoire à l'adresse %i\n", (int) (zone1-get_memory_adr()));
    mem_free(zone1);
    printf ("Libération de mémoire à l'adresse %i\n", (int) (zone2-get_memory_adr()));
    mem_free(zone2);
    printf("\nETAT DE LA MEMOIRE\n");
    mem_show(afficher_zone);
    printf ("\x1b[32m\nTest OK\n\x1b[32m");
    printf ("\x1b[0m\n===================================================\n\n");
}

int main(int argc, char *argv[]) {
	fprintf(stderr, "\n****** Tests réalisant divers cas d'utilisation de l'allocateur mémoire ******\n\n\n"
 		"\n");
	printf("\x1b[34m\nTEST N°1\n\x1b[34m");
	test_alloc_creates_new_free_zone();
	printf("\x1b[34m\nTEST N°2\n\x1b[34m");
	test_alloc_requiring_padding();
	printf("\x1b[34m\nTEST N°3\n\x1b[34m");
	test_alloc_excessive_size();
	printf("\x1b[34m\nTEST N°4\n\x1b[34m");
	test_alloc_size_too_small();
	printf("\x1b[34m\nTEST N°5\n\x1b[34m");
	test_alloc_memory_full();
	printf("\x1b[34m\nTEST N°6\n\x1b[34m");
	test_alloc_on_exact_size_of_free_block();
	printf("\x1b[34m\nTEST N°7\n\x1b[34m");
	test_free_no_fusion();
	printf("\x1b[34m\nTEST N°8\n\x1b[34m");
	test_free_out_of_bounds();
	printf("\x1b[34m\nTEST N°9\n\x1b[34m");
	test_free_address_inside_busy_block();
	printf("\x1b[34m\nTEST N°10\n\x1b[34m");
	test_free_address_inside_free_block();
	printf("\x1b[34m\nTEST N°11\n\x1b[34m");
	test_free_fusion_with_prev_block();
	printf("\x1b[34m\nTEST N°12\n\x1b[34m");
	test_free_fusion_with_next_block();
	printf("\x1b[34m\nTEST N°13\n\x1b[34m");
	test_free_fusion_with_both_adjacent_blocks();

}


