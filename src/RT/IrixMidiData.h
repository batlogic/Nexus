#ifndef _IRIXMIDIDATA_H
#define _IRIXMIDIDATA_H


// A structure to hold variables related to the IRIX API
// implementation.
struct IrixMidiData {
    MDport port;

    pthread_t thread;

};
#endif
