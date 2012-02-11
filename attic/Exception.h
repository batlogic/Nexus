#ifndef ERROR_H
#define ERROR_H

#include "Definitions.h"


//! Error handling class.
class Exception
{
public:
	enum Type 
	{
		STATUS,
		WARNING,
		DEBUG_WARNING,
		MEMORY_ALLOCATION,
		MEMORY_ACCESS,
		FUNCTION_ARGUMENT,
		FILE_NOT_FOUND,
		FILE_UNKNOWN_FORMAT,
		FILE_ERROR,
		PROCESS_THREAD,
		AUDIO_SYSTEM,
		MIDI_SYSTEM,
		MEMORY_ERROR,
		DRIVER_ERROR,
		UNSPECIFIED
	};


public:
	//! The constructor.
	Exception( const string& message, Type type = UNSPECIFIED ) throw()
		: message_( message ), 
		  type_( type ) 
	{}

	//! The destructor.
	virtual ~Exception(void) throw()
	{};

	//! Prints thrown error message to stderr.
	virtual void printMessage(void) throw() { cerr << '\n' << message_ << "\n\n"; }

	//! Returns the thrown error message type.
	virtual const Type& getType(void) { return type_; }

	//! Returns the thrown error message string.
	virtual const string& getMessage(void) { return message_; }

	//! Returns the thrown error message as a C string.
	virtual const char *getMessageCString(void) { return message_.c_str(); }

	static void doThrow();
	
	static void process( Type type );

	static void print( const string& message ) { cout << message; }

	//! Static function for error reporting and handling using c-strings.
	static void process( const char* message, Type type );

	//! Static function for error reporting and handling using c++ strings.
	static void process( const string& message, Type type );

	//! Toggle display of WARNING and STATUS messages.
	static void showWarnings( bool status ) { showWarnings_ = status; }

	//! Toggle display of error messages before throwing exceptions.
	static void printErrors( bool status ) { printErrors_ = status; }

protected:
	string message_;
	Type type_;

	static bool showWarnings_;
	static bool printErrors_;
};






#endif