#ifndef VOICE_H
#define VOICE_H

#include "BaseDef.h"


struct Voice
{
	Voice( UINT16 group )	: pitch_( 0 ), tag_(0), active_(0), group_(group) {}

	void update( bool active, UINT16 pitch, UINT16 group, UINT32 tag );

	FLOAT pitch_;
	UINT32 tag_;
	bool active_;
	UINT16 group_;
};


#endif // VOICE_H