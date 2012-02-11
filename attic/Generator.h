
#ifndef GENERATOR_H
#define GENERATOR_H

#include "Module.h"
#include "Buffer.h"


/***************************************************/
/*! \class Generator
\brief Abstract unit generator parent class.

This class provides limited common functionality for unit
generator sample-source subclasses.  It is general enough to
support both monophonic and polyphonic output classes.

by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

class Generator : public Module
{
public:

	//! Class constructor.
	Generator()	: Module()
	{ 
		lastFrame_.resize( 1, 1, 0.0 ); 
	};

	//! Return the number of output channels for the class.
	unsigned int channelsOut( void ) const { return lastFrame_.channels(); };

	//! Return a Buffer reference to the last output sample frame.
	const Buffer& lastFrame( void ) const { return lastFrame_; };

	//virtual Buffer& process( Buffer& frames, unsigned int channel = 0 ) = 0;

protected:
	Buffer lastFrame_;
};


#endif // GENERATOR_H
