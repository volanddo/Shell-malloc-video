#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <pthread.h>

#include "stream_common.h"
#include "oggstream.h"
#include "synchro.h"

pthread_mutex_t mutexVideo;

int main(int argc, char *argv[]) {
    int res;

    if (argc != 2) {
	fprintf(stderr, "Usage: %s FILE", argv[0]);
	exit(EXIT_FAILURE);
    }
    assert(argc == 2);


    // Initialisation de la SDL
    res = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS);
    atexit(SDL_Quit);
    assert(res == 0);
    
    // Initialisation des mutexes
    pthread_mutex_init(&mutexVideo, NULL);
    init_synchro();

    // start the two stream readers
    pthread_t theoraThread;
    pthread_t vorbisThread;
    pthread_create(&theoraThread, NULL, theoraStreamReader, (void *)argv[1]);
    pthread_create(&vorbisThread, NULL, vorbisStreamReader, (void *)argv[1]);
    // wait audio thread
    void * status ;
    pthread_join(vorbisThread, &status);
    // 1 seconde de garde pour le son,
    sleep(1);

    // tuer les deux threads videos si ils sont bloqués
    pthread_cancel(theoraThread);
    pthread_cancel(vorbisThread);

    // attendre les 2 threads videos
    pthread_join(vorbisThread, &status);
    pthread_join(theoraThread, &status);

    
    exit(EXIT_SUCCESS);    
}
