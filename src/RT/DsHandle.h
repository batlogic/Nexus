#ifndef _DSHANDLE_H
#define _DSHANDLE_H


// A structure to hold various information related to the DirectSound
// API implementation.
struct DsHandle {
    // Tracks callback counts when draining
    unsigned int drainCounter;

    // Indicates if stop is initiated from callback or not.
    bool internalDrain;

    void * id[2];

    void * buffer[2];

    bool xrun[2];

    UINT bufferPointer[2];

    DWORD dsBufferSize[2];

    // the number of bytes ahead of the safe pointer to lead by.
    DWORD dsPointerLeadTime[2];

    HANDLE condition;

    inline DsHandle() :drainCounter(0), internalDrain(false) { id[0] = 0; id[1] = 0; buffer[0] = 0; buffer[1] = 0; xrun[0] = false; xrun[1] = false; bufferPointer[0] = 0; bufferPointer[1] = 0; };

};
#endif
