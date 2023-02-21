/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <assert.h>
#include <stdint.h>
#include "mem.h"
#include "mem_internals.h"

void *get_next(void ** ptr) {
    return *ptr;
}
void set_next(void ** ptr, void * next) {
    *ptr = next;
}

void DivideIntoBlocks_MemoryLane(unsigned long sizeAlloced) {
    // Prend en entrée l'adresse du début (arena.chunkpool) 
    // De la bande mémoire libre (sizeAlloced)
    // ---> la découpe en block de mémoire à allouer
    // CHUNKSIZE : size of a chunck
    
    // We check if sizeAlloced is correct

    assert(sizeAlloced >= CHUNKSIZE);
    uint64_t blockNumberToAlloc = sizeAlloced/CHUNKSIZE;

    // Get 1st adress
    void **adrOfBloc = (void **) arena.chunkpool;

    // For each adress of the linked list, we write the next on it
    //ref :- 0x01 - 0x02 -0x03 - 0x04 - 0x05 -
    //val :- 0x03 - ---- -0x05 - ---- - NULL -
    // work because 96 > 64 (size of an adress)
    for(int i = 0; i < blockNumberToAlloc-1; i++) {
        void * next_block = (void*) adrOfBloc + CHUNKSIZE;
        *adrOfBloc = next_block;
        adrOfBloc = (void **) next_block;
    }
    // for the last adress, we write NULL
    *adrOfBloc = NULL;
}

void *
emalloc_small(unsigned long size) // size c'est la taille de données que l'on veut stocker
{
    assert(size <= SMALLALLOC);
    if (arena.chunkpool == NULL) {
        unsigned long sizeAlloced = mem_realloc_small(); // si rien à allouer, on obtient l'adresse de début d'une nouvelle bande mémoire dans arena.chunkpool. size représenta la taille de la bande mémoire allouée
        DivideIntoBlocks_MemoryLane(sizeAlloced);

    }

    void * nextBlock = get_next((void **)arena.chunkpool);
    void * ptr = mark_memarea_and_get_user_ptr(arena.chunkpool, CHUNKSIZE, SMALL_KIND);
    arena.chunkpool = (void*) nextBlock;

    return ptr;
}

void efree_small(Alloc a) {
    set_next((void **) a.ptr, arena.chunkpool);
    arena.chunkpool = a.ptr; // on fait pointer arena.chunkpool sur la nouvelle tête de liste
}


