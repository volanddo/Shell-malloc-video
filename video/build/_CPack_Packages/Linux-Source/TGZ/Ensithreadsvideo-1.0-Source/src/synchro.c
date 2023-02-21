#include "synchro.h"
#include "ensitheora.h"


extern bool fini;
bool tailleFenetreUpdate;
bool textureEstPrete;

/* les variables pour la synchro, ici */
pthread_cond_t condSynchro;
pthread_cond_t condConsome;
pthread_cond_t condProduit;

pthread_mutex_t mutexSynchro;

int nbTextureAConsomer;

void init_synchro(void){
    pthread_mutex_init(&mutexSynchro, NULL);
    pthread_cond_init(&condSynchro, NULL);
    pthread_cond_init(&condConsome, NULL);
    pthread_cond_init(&condProduit, NULL);
    tailleFenetreUpdate = false;
    textureEstPrete = false;
    nbTextureAConsomer = 0;
}

/* l'implementation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer) {
    pthread_mutex_lock(&mutexSynchro);

    
    // Mise a jour de la taille de la fenetre
    windowsx = buffer[0].width;
    windowsy = buffer[0].height;

    //reveille processus en attente de la taille
    tailleFenetreUpdate = true;
    pthread_cond_signal(&condSynchro);

    pthread_mutex_unlock(&mutexSynchro);
    
}

void attendreTailleFenetre() {
    pthread_mutex_lock(&mutexSynchro);
    
    //attente de l'envoie de la taille de la fenetre
    while(!tailleFenetreUpdate){
        pthread_cond_wait(&condSynchro, &mutexSynchro);
    }
    tailleFenetreUpdate = false;

    pthread_mutex_unlock(&mutexSynchro);
}

void signalerFenetreEtTexturePrete() {
    pthread_mutex_lock(&mutexSynchro);

    //reveille processus en attente de la texture

    textureEstPrete = true;
    pthread_cond_signal(&condSynchro);

    pthread_mutex_unlock(&mutexSynchro);
}

void attendreFenetreTexture() {
    pthread_mutex_lock(&mutexSynchro);

    //attente que la texture soit prete
    while(!textureEstPrete){
        pthread_cond_wait(&condSynchro, &mutexSynchro);
    }
    textureEstPrete = false;

    pthread_mutex_unlock(&mutexSynchro);

}

void debutConsommerTexture() {

    // ne peut pas consommer et produire en meme temps
    // code suivant en exlusion mutuelle
    // + verification qu'il y a bien une texture
    pthread_mutex_lock(&mutexSynchro);
    while (nbTextureAConsomer <= 0){
        pthread_cond_wait(&condProduit, &mutexSynchro);
    }
    pthread_mutex_unlock(&mutexSynchro);
}

void finConsommerTexture() {
    pthread_mutex_lock(&mutexSynchro);
    // signaler une consomation
    nbTextureAConsomer -= 1;
    pthread_cond_signal(&condConsome);

    pthread_mutex_unlock(&mutexSynchro);

}


void debutDeposerTexture() {
    pthread_mutex_lock(&mutexSynchro);
    while (nbTextureAConsomer >= NBTEX){
        pthread_cond_wait(&condConsome, &mutexSynchro);
    }
    pthread_mutex_unlock(&mutexSynchro);
}

void finDeposerTexture() {
    pthread_mutex_lock(&mutexSynchro);
    // signaler une deposition
    nbTextureAConsomer += 1;
    pthread_cond_signal(&condProduit);

    pthread_mutex_unlock(&mutexSynchro);
}
