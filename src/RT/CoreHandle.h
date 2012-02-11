#ifndef _COREHANDLE_H
#define _COREHANDLE_H


// The OS X CoreAudio API is designed to use a separate callback
// procedure for each of its audio devices.  A single RtAudio duplex
// stream using two different devices is supported here, though it
// cannot be guaranteed to always behave correctly because we cannot
// synchronize these two callbacks.
//
// A property listener is installed for over/underrun information.
// However, no functionality is currently provided to allow property
// listeners to trigger user handlers because it is unclear what could
// be done if a critical stream parameter (buffer size, sample rate,
// device disconnect) notification arrived.  The listeners entail
// quite a bit of extra code and most likely, a user program wouldn't
// be prepared for the result anyway.  However, we do provide a flag
// to the client callback function to inform of an over/underrun.
//
// The mechanism for querying and setting system parameters was
// updated (and perhaps simplified) in OS-X version 10.4.  However,
// since 10.4 support is not necessarily available to all users, I've
// decided not to update the respective code at this time.  Perhaps
// this will happen when Apple makes 10.4 free for everyone. :-)
// A structure to hold various information related to the CoreAudio API
// implementation.
struct CoreHandle {
    // device ids
    AudioDeviceID id[2];

    AudioDeviceIOProcID procId[2];

    // device stream index (or first if using multiple)
    UInt32 iStream[2];

    // number of streams to use
    UInt32 nStreams[2];

    bool xrun[2];

    char * deviceBuffer;

    pthread_cond_t condition;

    // Tracks callback counts when draining
    int drainCounter;

    // Indicates if stop is initiated from callback or not.
    bool internalDrain;

    inline CoreHandle() :deviceBuffer(0), drainCounter(0), internalDrain(false) { nStreams[0] = 1; nStreams[1] = 1; id[0] = 0; id[1] = 0; xrun[0] = false; xrun[1] = false; };

};
#endif
