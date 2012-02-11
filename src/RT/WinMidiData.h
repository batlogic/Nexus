#ifndef _WINMIDIDATA_H
#define _WINMIDIDATA_H


#include "RtMidi.h"

// A structure to hold variables related to the CoreMIDI API
// implementation.
struct WinMidiData {
    // Handle to Midi Input Device
    HMIDIIN inHandle;

    // Handle to Midi Output Device
    HMIDIOUT outHandle;

    DWORD lastTime;

    RtMidiIn::MidiMessage message;

    LPMIDIHDR sysexBuffer[RT_SYSEX_BUFFER_COUNT];

};
#endif
