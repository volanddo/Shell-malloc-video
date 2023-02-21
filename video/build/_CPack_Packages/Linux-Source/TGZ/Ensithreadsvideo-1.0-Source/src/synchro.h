#ifndef SYNCHRO_H
#define SYNCHRO_H

#include <stdbool.h>
#include "ensitheora.h"
#include <pthread.h>

extern bool fini;
extern bool tailleFenetreUpdate;
extern bool textureEstPrete;

/* les variables pour la synchro, ici */
extern pthread_cond_t condSynchro;
extern pthread_cond_t condConsome;
extern pthread_cond_t condProduit;

extern pthread_mutex_t mutexSynchro;


extern int nbTextureAConsomer;

/*Fonction initialisation signal et mutex*/
void init_synchro(void);

/* Fonctions de synchro à implanter */

void envoiTailleFenetre(th_ycbcr_buffer buffer);
void attendreTailleFenetre();

void attendreFenetreTexture();
void signalerFenetreEtTexturePrete();

void debutConsommerTexture();
void finConsommerTexture();

void debutDeposerTexture();
void finDeposerTexture();

#endif
