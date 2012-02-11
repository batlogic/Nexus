#ifndef RTMIDIINPUT_H
#define RTMIDIINPUT_H


#include <queue>
#include <map>
#include "Mutex.h"
#include "RtMidi.h"
#include "MidiEvent.h"



/***************************************************/
/*! \class MidiInput
\brief Input control message parser.

	This class reads and parses control MIDI messages using the RtMidi class.

	The realtime message acquisition mechanisms take place asynchronously, 
	filling the message queue. A call to popMessage() will pop the next available control
	message from the queue and return it via the referenced MidiEvent
	structure.
*/
/***************************************************/

const int DEFAULT_QUEUE_LIMIT = 200;


class RtMidiInput
{
public:

	// This structure is used to share data among the various realtime threads
	// It must be public.
	struct Data 
	{
		std::queue<MidiEvent> queue;
		unsigned int queueLimit;
		Mutex mutex;

		Data() : queueLimit(0)  {}
	};

	RtMidiInput();
	~RtMidiInput();

	// Pop the next message from the queue and write it to the referenced message structure.
	// Returns false if there is no message in the queue.
	//
	bool popEvent( MidiEvent& evt );

	//! Push the referenced event onto the event stack.
	void pushEvent( const MidiEvent& evt );

	// Returns true if any events are available
	bool hasEvent() { return data_.queue.empty() == false; }

	bool openPort( int port=0, bool ignoreMidiSysex=true, bool ignoreMidiTime=true, bool ignoreMidiSense=true  );
	void closePort( int portNum );
	void closeAllPorts();

    bool isPortOpen( int portNum );
    string getPortName( int portNum );

protected:
	Data data_;
	std::map<int, RtMidiIn*> ports_;
};

#endif // RTMIDIINPUT_H

