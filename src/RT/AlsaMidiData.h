#ifndef _ALSAMIDIDATA_H
#define _ALSAMIDIDATA_H


// A structure to hold variables related to the ALSA API
// implementation.
struct AlsaMidiData {
    snd_seq_t * seq;

    int vport;

    snd_seq_port_subscribe_t * subscription;

    snd_midi_event_t * coder;

    unsigned int bufferSize;

    unsigned char * buffer;

    pthread_t thread;

    unsigned long long lastTime;

    // an input queue is needed to get timestamped events
    int queue_id;

};
#endif
