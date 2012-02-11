#ifndef _ALSAHANDLE_H
#define _ALSAHANDLE_H


// A structure to hold various information related to the ALSA API
// implementation.
struct AlsaHandle {
    snd_pcm_t * handles[2];

    bool synchronized;

    bool xrun[2];

    pthread_cond_t runnable;

    inline AlsaHandle() :synchronized(false) { xrun[0] = false; xrun[1] = false; };

};
#endif
