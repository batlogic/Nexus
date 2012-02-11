#ifndef _JACKHANDLE_H
#define _JACKHANDLE_H


// A structure to hold various information related to the Jack API
// implementation.
struct JackHandle {
    jack_client_t * client;

    jack_port_t * * ports[2];

    std::string deviceName[2];

    bool xrun[2];

    pthread_cond_t condition;

    // Tracks callback counts when draining
    int drainCounter;

    // Indicates if stop is initiated from callback or not.
    bool internalDrain;

    inline JackHandle() :client(0), drainCounter(0), internalDrain(false) { ports[0] = 0; ports[1] = 0; xrun[0] = false; xrun[1] = false; };

};
#endif
