/***************************************************/
/*! \class RtMidiInput
\brief Midi input message parser.

This class reads and parses control MIDI messages using the RtMidi class.

The realtime message acquisition mechanisms take place asynchronously, 
filling the message queue. A call to popMessage() will pop the next available control
message from the queue and return it via the referenced MidiEvent
structure.

*/
/***************************************************/
#include "RtMidiInput.h"
#include "RtError.h"



RtMidiInput::RtMidiInput()
{
	data_.queueLimit = DEFAULT_QUEUE_LIMIT;
}


RtMidiInput::~RtMidiInput()
{
	// Clear the queue in case any thread is waiting on its limit.
	data_.mutex.lock();
	while( data_.queue.size() ) {
		data_.queue.pop();
	}
	data_.mutex.unlock();

	closeAllPorts();
}


bool RtMidiInput::popEvent( MidiEvent& evt )
{
	if( data_.queue.empty() == true ) {	
		return false;
	}

	// Copy queued message to the message pointer structure and then "pop" it.
	data_.mutex.lock();
	evt = data_.queue.front();
	data_.queue.pop();
	data_.mutex.unlock();

	return true;
}


void RtMidiInput::pushEvent( const MidiEvent& evt )
{
	data_.mutex.lock();
	data_.queue.push( evt );
	data_.mutex.unlock();
}


void midiHandler( double timeStamp, std::vector<unsigned char> *bytes, void *ptr )
{
	RtMidiInput::Data *data = (RtMidiInput::Data*)ptr;
	MidiEvent evt( bytes, timeStamp );

	while( data->queue.size() >= data->queueLimit ) {
		::Sleep( 50 );
	}

	data->mutex.lock();
	data->queue.push( evt );
	data->mutex.unlock();
}


bool RtMidiInput::openPort( int portNum, bool ignoreMidiSysex, bool ignoreMidiTime, bool ignoreMidiSense )
{
	RtMidiIn* port;

	if( isPortOpen( portNum) ) {
		EXCEPTION( runtime_error, "port %d is already open.", portNum );
	}

	try {
		port = new RtMidiIn();
		port->setCallback( &midiHandler, (void*)&data_ );
		port->ignoreTypes( true, false, true );
		if( portNum == -1 ) {
			port->openVirtualPort();
		}
		else {
			port->openPort( (unsigned int)portNum );
		}
	}
	catch ( RtError &error ) {
		EXCEPTION( runtime_error, "error creating RtMidiIn instance (%s)", error.getMessage() );
	}

	ports_[portNum] = port;
	return true;
}


void RtMidiInput::closePort( int portNum )
{
	if( isPortOpen( portNum ) == false ) {
		EXCEPTION( runtime_error, "port %d is not open.", portNum );
	}

	RtMidiIn* port = ports_[portNum];
	if( port != NULL ) {
		delete port;
	}
	ports_.erase( portNum );
}


void RtMidiInput::closeAllPorts()
{
	std::map<int, RtMidiIn*>::iterator it;

	for( it=ports_.begin(); it!=ports_.end(); it++ ) {
		RtMidiIn* port = (*it).second;
		delete port;
	}
	ports_.clear();
}


bool RtMidiInput::isPortOpen( int portNum )
{
    return ports_.count( portNum ) != 0;
}


string RtMidiInput::getPortName( int portNum )
{
	if( isPortOpen( portNum ) ) 
    {
        RtMidiIn* port = ports_[ portNum ];
        if( port ) {
            return port->getPortName( portNum );
        }
	}
    return "";    
}

