#ifndef _OSSHANDLE_H
#define _OSSHANDLE_H


// A structure to hold various information related to the OSS API
// implementation.
struct OssHandle {
    // device ids
    int id[2];

    bool xrun[2];

    bool triggered;

    pthread_cond_t runnable;

    inline OssHandle() :triggered(false) { id[0] = 0; id[1] = 0; xrun[0] = false; xrun[1] = false; };

};
#endif
