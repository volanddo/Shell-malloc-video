/******************************************************
 * Copyright Grégory Mounié 2018-2022                 *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <sys/mman.h>
#include <assert.h>
#include <stdint.h>
#include "mem.h"
#include "mem_internals.h"

unsigned long knuth_mmix_one_round(unsigned long in)
{
    return in * 6364136223846793005UL % 1442695040888963407UL;
}

void *mark_memarea_and_get_user_ptr(void *ptr, unsigned long size, MemKind k) // size c'est la taille en octets des données à stocker + 32 octets
{
    if(size == 0){
        return NULL;
    }
    uint64_t sml;
    switch(k)
    {
        case SMALL_KIND : sml = 1; break;
        case MEDIUM_KIND : sml = 2; break;
        case LARGE_KIND : sml = 3; break;
    }
    uint64_t taille_usr = (uint64_t) (size - 32); // on enlève les 32 octets du marquage pour avoir la taille qui contiendra les données
    uint64_t knuth_return = (uint64_t) knuth_mmix_one_round((unsigned long) ptr);
    uint64_t magic = (knuth_return & ~(0b11UL)) | sml;
    *( (uint64_t*) ptr ) = (uint64_t) size;
    *( (uint64_t*) ptr + 1 ) = magic;
    *( (uint64_t*) ((uint8_t*) ptr + 2*8 + taille_usr) ) = magic; // on obtient l'adresse en se déplacant octet par octet puis on dit que le pointeur pointe sur une case de 64 bits
    *( (uint64_t*) ((uint8_t*) ptr + taille_usr + 3*8) ) = (uint64_t) size; // de même

    return  ptr + 2*8;
}


Alloc mark_check_and_get_alloc(void *ptr)
{
    unsigned long size = (unsigned long) *( (uint64_t*) ptr - 2);
    unsigned long size_usr = size - 32;
    
    uint64_t knuth_return = (uint64_t) knuth_mmix_one_round((unsigned long) ((uint64_t*) ptr - 2));
    uint64_t magic = (knuth_return & ~(0b11UL));
    uint64_t magic1 = *( (uint64_t*) ptr - 1);
    uint64_t magic2 = *( (uint64_t*) ( ptr + size_usr));
    assert((magic & ~(0b11UL)) == (magic1 & ~(0b11UL))); // on vérifie si magic1 est une valeur magique ou une valeur issue d'un débordement
    assert(magic1 == magic2); // on vérifie si il y a eu débordement en comparant le magique dans la partie gauche et droite
    uint64_t sml = magic1 & (0b11UL);
    MemKind k;

    switch(sml)
    {
        case 1 : k = SMALL_KIND; break;
        case 2 : k = MEDIUM_KIND; break;
        case 3 : k = LARGE_KIND; break;
    } 
    Alloc a = {(void *)((uint64_t*) ptr - 2), k, size};
    return a;
}


unsigned long
mem_realloc_small() {
    assert(arena.chunkpool == 0);
    unsigned long size = (FIRST_ALLOC_SMALL << arena.small_next_exponant);
    arena.chunkpool = mmap(0,
			   size,
			   PROT_READ | PROT_WRITE | PROT_EXEC,
			   MAP_PRIVATE | MAP_ANONYMOUS,
			   -1,
			   0);
    if (arena.chunkpool == MAP_FAILED)
	handle_fatalError("small realloc");
    arena.small_next_exponant++;
    return size;
}

unsigned long
mem_realloc_medium() {
    uint32_t indice = FIRST_ALLOC_MEDIUM_EXPOSANT + arena.medium_next_exponant;
    assert(arena.TZL[indice] == 0);
    unsigned long size = (FIRST_ALLOC_MEDIUM << arena.medium_next_exponant);
    assert( size == (1UL << indice));
    arena.TZL[indice] = mmap(0,
			     size*2, // twice the size to allign
			     PROT_READ | PROT_WRITE | PROT_EXEC,
			     MAP_PRIVATE | MAP_ANONYMOUS,
			     -1,
			     0);
    if (arena.TZL[indice] == MAP_FAILED)
	handle_fatalError("medium realloc");
    // align allocation to a multiple of the size
    // for buddy algo
    arena.TZL[indice] += (size - (((intptr_t)arena.TZL[indice]) % size));
    arena.medium_next_exponant++;
    return size; // lie on allocation size, but never free
}


// used for test in buddy algo
unsigned int
nb_TZL_entries() {
    int nb = 0;
    
    for(int i=0; i < TZL_SIZE; i++)
	if ( arena.TZL[i] )
	    nb ++;

    return nb;
}
