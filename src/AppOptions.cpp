
#include "AppOptions.h"
#include "Definitions.h"
#include "Utils.h"


//--------------------------------------------------------------
// class AppOptions
//--------------------------------------------------------------
#ifdef _RT_AUDIO

#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

namespace po = boost::program_options;


AppOptions::AppOptions() :
    SoundOptions(),
    windowLeft_( 0 ), 
    windowTop_( 0 ), 
    windowRight_( windowLeft_+850+8 ), 
    windowBottom_( windowTop_+450+27 ), 
    windowState_( SW_SHOW ),
    optionFilename_( "nexus_standalone.options" ),
    colorScheme_( "Dark" ),
    sortColumn_( 0 ),
    sortType_( 1 ),
    autosave_( true ),
    tuning_( 0 )
{
	TiXmlBase::SetCondenseWhiteSpace( false );      	
}


void AppOptions::open( int argc, TCHAR* argv[], const string& path, const string& filename )
{
    appPath_        = path;
    optionFilename_ = path + filename;
    
	po::options_description desc( "Allowed options" );
	desc.add_options()
		( "help,h", "show help message" )
		( "option-file,o", po::value<string>(),  "option file" )
		( "port,p",        po::value<string>(), "[asio|ds]: audio out port" )
		( "device,d",      po::value<int>(),     "[0..n]: audio out device" )
		( "buffer-size,b", po::value<int>(),    "[0..n]: audio buffer size" )
		( "sample-rate,s", po::value<int>(),    "[0..n]: sample rate" );

	po::variables_map vm;
	try {
		po::store( po::parse_command_line( argc, argv, desc ), vm );
		po::notify( vm );    
	}
	catch( const exception& e ) {
		TRACE( e.what() );
	}

	if (vm.count( "help" )) {
		ostringstream os;
		os << desc << endl;
		throw exception( os.str().c_str() );
	}
	if( vm.count( "option-file" )) {
		optionFilename_ = vm[ "option-file" ].as<string>();
	}

	readOptionFile();

	if( vm.count( "port" )) {
		string port = vm[ "port" ].as<string>();
		if( port == "asio" ) audioPort_ = RtAudio::WINDOWS_ASIO;
		if( port == "ds" ) audioPort_ = RtAudio::WINDOWS_DS;
	}
	if( vm.count( "device" )) {
		audioOutDevice_ = vm[ "device" ].as<int>();
	}
	if( vm.count( "buffer-size" )) {
		bufferSize_ = vm[ "buffer-size" ].as<int>();
	}
	if( vm.count( "sample-rate" )) {
		sampleRate_ = vm[ "sample-rate" ].as<int>();
	}
}

#else

AppOptions::AppOptions() :
    windowLeft_( 0 ), 
    windowTop_( 0 ), 
    windowRight_( 850 ), 
    windowBottom_( 450 ), 
    windowState_( SW_SHOW ),
    sortColumn_( 0 ),
    sortType_( 1 ),
    autosave_( true ),
    tuning_( 0 )
{}


void AppOptions::open( const string& appPath, const string& filename )
{
    appPath_        = appPath;
    optionFilename_ = appPath + filename;
    readOptionFile();
}

#endif


void AppOptions::readOptionFile()
{
	try {
		Document doc = Document( optionFilename_ );
        doc.LoadFile();

	    Element* rootElement = doc.FirstChildElement( "NexusOptions", true );

        Iterator< Element > iter;
        string name;
        string text;

        for( iter = iter.begin( doc.FirstChildElement()); iter != iter.end(); iter++ )
        {
            iter->GetValue( &name ); 

            if( name == "BankPath" ) {
                bankPath_ = iter->GetText( false );
            }
            if( name == "Autosave" ) {
                iter->GetText( &autosave_, false );
            }
            if( name == "ColorScheme" ) {
                iter->GetText( &colorScheme_, false );
            }
            else if( name == "Columns" ) {
                iter->GetText( &columnMetrics_, false );
                iter->GetAttribute( "sort", &sortColumn_, false );
                iter->GetAttribute( "dir", &sortType_, false );
            }
            else if( name == "Window" ) {
                iter->GetAttribute( "left",   &windowLeft_,   false );
                iter->GetAttribute( "top",    &windowTop_,    false );
                iter->GetAttribute( "right",  &windowRight_,  false );
                iter->GetAttribute( "bottom", &windowBottom_, false );
                iter->GetAttribute( "state",  &windowState_,     false );
            }
            else if( name == "Tune" ) {
                iter->GetText( &tuning_, false );
            }

#ifdef _RT_AUDIO
            else if( name == "AudioPort" ) {
                iter->GetText( (int*)&audioPort_, false );
            }
            else if( name == "AudioOutDevice" ) {
                iter->GetText( &audioOutDevice_, false );
            }
            else if( name == "AudioOutChannel" ) {
                iter->GetText( &audioOutChannel_, false );
            }
            else if( name == "MidiInPorts" ) {
                readMidiInPorts( iter.Get() );
            }
            else if( name == "BufferSize" ) {
                iter->GetText( &bufferSize_, false );
            }
            else if( name == "SampleRate" ) {
                iter->GetText( &sampleRate_, false );
            }
            else if( name == "AudioRun" ) {
                iter->GetText( &audioRun_, false );
            }
#endif
        }
	}
	catch( const exception& e ) {            // file error, use default program
		TRACE( e.what() );
	}
}


void AppOptions::close()
{
    if( optionFilename_.empty() )
        return;
    
    Document doc;
	Declaration* declaration = new Declaration(  "1.0", "", "no" );
	doc.LinkEndChild( declaration );

	Element* root = new Element( "NexusOptions" );

    Element* bankPath = new Element( "BankPath", bankPath_ );
    root->LinkEndChild( bankPath );

    Element* autosave = new Element( "Autosave", autosave_ );
    root->LinkEndChild( autosave );

    Element* colorScheme = new Element( "ColorScheme", colorScheme_ );
    root->LinkEndChild( colorScheme );

    Element* columns = new Element( "Columns", columnMetrics_ );
    columns->SetAttribute( "sort", sortColumn_ );
    columns->SetAttribute( "dir", sortType_ );
    root->LinkEndChild( columns );

    Element* position  = new Element( "Window" );
    position->SetAttribute( "left",   windowLeft_ );
    position->SetAttribute( "top",    windowTop_ );
    position->SetAttribute( "right",  windowRight_ );
    position->SetAttribute( "bottom", windowBottom_ );
    position->SetAttribute( "state",  windowState_ );
    root->LinkEndChild( position );

    Element* tuning = new Element( "Tuning", tuning_ );
    root->LinkEndChild( tuning );
    
#ifdef _RT_AUDIO    
    Element* audioOutPort = new Element( "AudioPort", (int)audioPort_ );
    root->LinkEndChild( audioOutPort );
	
    Element* audioOutDevice = new Element( "AudioOutDevice", audioOutDevice_ );
    root->LinkEndChild( audioOutDevice );

    Element* audioOutChannel = new Element( "AudioOutChannel", audioOutChannel_ );
    root->LinkEndChild( audioOutChannel );

    Element* midiInPorts = new Element( "MidiInPorts" );
    writeMidiInPorts( midiInPorts );
    root->LinkEndChild( midiInPorts );

    Element* bufferSize = new Element( "BufferSize", bufferSize_ );
    root->LinkEndChild( bufferSize );

    Element* sampleRate = new Element( "SampleRate", sampleRate_ );
    root->LinkEndChild( sampleRate );

    Element* audioRun = new Element( "AudioRun", audioRun_ );
    root->LinkEndChild( audioRun );
#endif

    doc.LinkEndChild( root );
	doc.SaveFile( optionFilename_ );
	doc.Clear();
}

#ifdef _RT_AUDIO

void AppOptions::readMidiInPorts( Element* element )
{
	midiInPorts_.clear();

    Iterator< Element > iter( "Port" );
	for( iter = iter.begin( element ); iter != iter.end(); iter++ )
	{
        string name;
        iter->GetText( &name, false );

        if( name.empty() == false ) {
            midiInPorts_.insert( MidiPortMap::value_type( name, -1 ));
        }
	}
}


void AppOptions::writeMidiInPorts( Element* element )
{
    for( MidiPortMap::iterator it=midiInPorts_.begin(); it!=midiInPorts_.end(); it++ )
    {
        Element* port = new Element( "Port", it->first );
        element->LinkEndChild( port );
    }
}

/*
void AppOptions::readCmdLine( int argc, TCHAR* argv[] )
{
	po::options_description desc( "Allowed options" );
	desc.add_options()
		( "help,h", "show help message" )
		( "option-file,o", po::value<string>(),  "option file" )
		( "port,p",        po::value<string>(), "[asio|ds]: audio out port" )
		( "device,d",      po::value<int>(),     "[0..n]: audio out device" )
		( "buffer-size,b", po::value<int>(),    "[0..n]: audio buffer size" )
		( "sample-rate,s", po::value<int>(),    "[0..n]: sample rate" );

	po::variables_map vm;
	try {
		po::store( po::parse_command_line( argc, argv, desc ), vm );
		po::notify( vm );    
	}
	catch( const exception& e ) {
		TRACE( e.what() );
	}

	if (vm.count( "help" )) {
		cout << desc << endl;
		return;
	}
	if( vm.count( "option-file" )) {
		optionFilename_ = vm[ "option-file" ].as<string>();
	}
	if( vm.count( "port" )) {
		string port = vm[ "port" ].as<string>();
		if( port == "asio" ) audioPort_ = RtAudio::WINDOWS_ASIO;
		if( port == "ds" ) audioPort_ = RtAudio::WINDOWS_DS;
	}
	if( vm.count( "device" )) {
		audioOutDevice_ = vm[ "device" ].as<int>();
	}
	if( vm.count( "buffer-size" )) {
		bufferSize_ = vm[ "buffer-size" ].as<int>();
	}
	if( vm.count( "sample-rate" )) {
		sampleRate_ = vm[ "sample-rate" ].as<int>();
	}
}
*/

#endif // _RT_AUDIO

