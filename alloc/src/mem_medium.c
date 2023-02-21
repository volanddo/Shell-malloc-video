/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <stdint.h>
#include <assert.h>
#include "mem.h"
#include "mem_internals.h"

unsigned int puiss2(unsigned long size) {
    unsigned int p=0;
    size = size -1; // allocation start in 0
    while(size) {  // get the largest bit
	p++;
	size >>= 1;
    }
    if (size > (1 << p))
	p++;
    return p;
}

void *getBuddy(void * ptr, uint64_t size){
    return (void *)( (uint64_t)ptr ^ size );
}
void addTZL(int i, void ** elem){
    *elem = arena.TZL[i];
    arena.TZL[i] = (void *)elem;

}

void *get_previous(void *start_list, void * elem){ //  gerer le cas start_list = buddy
    if(start_list == NULL){
        return 0;
    }
    if(*(void**)start_list == elem)
    {
        return start_list;
    }
    return get_previous(*((void **)start_list),elem);
}



void sliceBlockRec(int indice) { // découpe récursivement de l'index >= indice du premier bloc existant jusqu'à indice

    if (arena.TZL[indice] == 0) {
        sliceBlockRec(indice + 1);
    }

    void **address_of_block = (void**) arena.TZL[indice]; // on fait pointer arena.TZL sur le bloc suivant
    arena.TZL[indice] = (void*) *address_of_block;

    intptr_t intaddress_of_block = ( intptr_t ) address_of_block;
    intptr_t intfirst_add_of_block = intaddress_of_block;
    intptr_t intsecond_add_of_block = (intptr_t) getBuddy((void*) intfirst_add_of_block, (1 << (indice-1))); // 1 << (indice-1) rpz la taille du bloc

    void **first_add_of_block = (void**) intfirst_add_of_block;
    void **second_add_of_block = (void**) intsecond_add_of_block;

    void *old_head_block = arena.TZL[indice - 1];
    *first_add_of_block = (void*) intsecond_add_of_block;
    *second_add_of_block = old_head_block;
    arena.TZL[indice - 1] = (void*) first_add_of_block;
    
}

void *
emalloc_medium(unsigned long size)
{
    assert(size < LARGEALLOC);
    assert(size > SMALLALLOC);
    // Compute TLZ index with puiss2()
    int indice = puiss2(size); // donne l'indice du tableau TLZ du bloc de taille size
    int indice_max = FIRST_ALLOC_MEDIUM_EXPOSANT + arena.medium_next_exponant; // indice du bloc de taille max

    //if no bloc before (FIRST_ALLOC_MEDIUM_EXPOSANT + arena.medium_next_exponant)
    //    mem_realloc_medium();
    for (int i = indice; i < indice_max + 1; i++) { // si il n'y a aucun bloc de taille supérieure ou égale à size alors
        if (arena.TZL[i] != 0) {                    // cela signifique que l'on ne pourra pas en découpant allouer un bloc
            break;                                  // il faut donc appeler mem_realloc_medium()
        }
        if (i == indice_max) {
            mem_realloc_medium();
        }
    }
    //if no bloc available, découper, récursivement
    //    sliceBlockRec();
    if (arena.TZL[indice] == 0) {
        sliceBlockRec(indice + 1);
    }
    //if block available : mark it and return it adress
    void **addr_block_available = (void**) arena.TZL[indice];
    arena.TZL[indice] = *addr_block_available;
    
    //return mark_memarea_and_get_user_ptr(...);
    return mark_memarea_and_get_user_ptr(addr_block_available, size+32, MEDIUM_KIND);
}


void efree_medium(Alloc a) {

    // compute body adress
    void * buddy = getBuddy(a.ptr, a.size);

    // check if budy in valid arena.TZL

    if (buddy == arena.TZL[puiss2(a.size)])
    {
        arena.TZL[puiss2(a.size)] = *((void **)buddy);
    }
    else{
        //  else remove buddy from the list,
        void ** previous = (void **)get_previous(arena.TZL[puiss2(a.size)], buddy);
        if (previous == NULL || previous == 0){
            //  If not budy, put a in the list
            addTZL(puiss2(a.size), (void **)a.ptr);
            return ;
        }

        *previous = *(void **) buddy;

    }
    //  merge them in the next list, and do it again
    a.size = 2*a.size;
    if ( (uint64_t)a.ptr > (uint64_t)buddy){
        a.ptr = buddy;
    }


    if(puiss2(a.size) == FIRST_ALLOC_MEDIUM_EXPOSANT + arena.medium_next_exponant){
        void ** ptr = (void ** )a.ptr;
        *ptr = arena.TZL[puiss2(a.size)];
        arena.TZL[puiss2(a.size)] = a.ptr;
    }
    else{
        efree_medium(a);
    }

}
