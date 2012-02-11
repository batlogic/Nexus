#ifndef _COREMIDIDATA_H
#define _COREMIDIDATA_H


// A structure to hold variables related to the CoreMIDI API
// implementation.
struct CoreMidiData {
    MIDIClientRef client;

    MIDIPortRef port;

    MIDIEndpointRef endpoint;

    MIDIEndpointRef destinationId;

    unsigned long long lastTime;

};
#endif
