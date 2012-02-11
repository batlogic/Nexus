
#include "Definitions.h"
#include "MidiEvent.h"


MidiEvent::MidiEvent( vector<unsigned char>* bytes, double timeStamp )
	: deltaFrames_(0)
{
	for( int i=0; i < 4; i++) data_[i] = 0;
	UINT8 byte0=0, byte1=0, byte2=0;

	if( bytes->size() >= 1 ) {
		byte0 = (UINT8)bytes->at( 0 );

		if( bytes->size() >= 2 ) {
			byte1 = (UINT8)bytes->at( 1 );

			if( bytes->size() >= 3 ) {
				byte2 = (UINT8)bytes->at( 2 );
			}
		}
	}
	data_[0] = byte0 & 0xFF;
	data_[1] = byte1;
	data_[2] = byte2;
	channel_ = byte0 & 0x0F;
	time_    = 0.0; // realtime messages should have delta time = 0.0 ???
}


MidiEvent::MidiEvent( char bytes[4], int deltaFrames )
	: time_(0)
{
	data_[0]     = (UINT8)bytes[0] & 0xFF;
	data_[1]     = (UINT8)bytes[1];
	data_[2]     = (UINT8)bytes[2];
	channel_     = (UINT8)bytes[0] & 0x0F;
	deltaFrames_ = deltaFrames;
}


void MidiEvent::dump() const
{
	ostringstream os;
	os << "MidiEvent: [";

	for( int i=0; i<4; i++ ) {
		os << (long)data_[i];
		if( i < 3 ) os << ", ";
	}
	os<< "]" << endl;
	cout << os;
}



