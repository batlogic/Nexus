#ifndef _ASIOHANDLE_H
#define _ASIOHANDLE_H


struct ASIOBufferInfo;

struct AsioHandle {
    // Tracks callback counts when draining
    int drainCounter;

    // Indicates if stop is initiated from callback or not.
    bool internalDrain;

    ASIOBufferInfo * bufferInfos;

    HANDLE condition;

    inline AsioHandle() :drainCounter(0), internalDrain(false), bufferInfos(0) {};

};
#endif
