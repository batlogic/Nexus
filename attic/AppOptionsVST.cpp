


#include "AppOptions.h"
#include "Definitions.h"



//--------------------------------------------------------------
// class AppOptions
//--------------------------------------------------------------
#ifdef _RT_AUDIO
AppOptions::AppOptions( int argc, TCHAR* argv[] ) :
    SoundOptions(),
    posXFrame_( CW_USEDEFAULT ), 
    posYFrame_( CW_USEDEFAULT )
{
    readOptionFile();
    readCmdLine( argc, argv );
}

#else

AppOptions::AppOptions() :
    SoundOptions(),
    posXFrame_( 0 ), 
    posYFrame_( 0 )
{
    readOptionFile();
}
#endif


void AppOptions::readOptionFile()
{
	try {
      	Document doc = Document( ".\\nexus.options" );
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
#ifdef _RT_AUDIO
            else if( name == "Position" ) {
                iter->GetAttribute( "x", &posXFrame_ );
                iter->GetAttribute( "y", &posYFrame_ );
            }
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
#endif
        }
	}
	catch( const exception& e ) {            // file error, use default program
		TRACE( e.what() );
	}
}


void AppOptions::saveOptionFile()
{
	Document doc;
	Declaration* declaration = new Declaration(  "1.0", "", "no" );
	doc.LinkEndChild( declaration );

	Element* root = new Element( "NexusOptions" );

    Element* bankPath = new Element( "BankPath", bankPath_ );
    root->LinkEndChild( bankPath );

#ifdef _RT_AUDIO
    Element* position  = new Element( "Position" );
    position->SetAttribute( "x", posXFrame_ );
    position->SetAttribute( "y", posYFrame_ );
    root->LinkEndChild( position );

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
#endif

    doc.LinkEndChild( root );
	doc.SaveFile( ".\\nexus.options" );
	doc.Clear();
}

#ifdef _RT_AUDIO

enum { OPT_PORT, OPT_DEVICE, OPT_BUFFER, OPT_RATE, OPT_HELP };

CmdLineParser::SOption g_cmdOptions[] = 
{
    { OPT_PORT, L"-p",      SO_REQ_CMB }, 
    { OPT_DEVICE, L"-d",    SO_REQ_CMB },
	{ OPT_BUFFER, L"-b",    SO_REQ_CMB },
    { OPT_BUFFER, L"-sr",   SO_REQ_CMB },
    { OPT_HELP, L"-?",	    SO_NONE    },
    SO_END_OF_OPTIONS                    
};


void AppOptions::readCmdLine( int argc, TCHAR* argv[] )
{
	CmdLineParser args( argc, argv, g_cmdOptions );
	
    while( args.Next() ) 
	{ 
        if( args.LastError() == SO_SUCCESS ) 
		{
            if( args.OptionId() == OPT_HELP ) 
            {
	            cout << "\nUsage: ";
	            cout << "nexus [options]" << endl;
	            cout << "-p:   [asio|ds] audio out port" << endl;
	            cout << "-d:   [0..n] audio out device" << endl;
	            cout << "-b:   [0..n] audio buffer size" << endl;
                cout << "-sr:  [0..n] sample rate" << endl;
                return;
            }
			if( wcscmp( args.OptionText(), L"-p") == 0 ) {
				if( wcsicmp( args.OptionArg(), L"asio") == 0 ) audioPort_ = RtAudio::WINDOWS_ASIO;
				if( wcsicmp( args.OptionArg(), L"ds") == 0 )   audioPort_ = RtAudio::WINDOWS_DS;
			}
			if( wcscmp( args.OptionText(), L"-d") == 0 ) {
				audioOutDevice_ = _wtoi( args.OptionArg() );
			}
			if( wcscmp( args.OptionText(), L"-b") == 0 ) {
				bufferSize_ = _wtoi( args.OptionArg() );
			}
			if( wcscmp( args.OptionText(), L"-sr") == 0 ) {
				sampleRate_ = _wtoi( args.OptionArg() );
			}
        }
        else {
			cout <<  "Invalid argument: " << args.OptionText() << "\n";
            return;
        }
    }
}


void AppOptions::readMidiInPorts( Element* element )
{
	midiInPorts_.clear();

    Iterator< Element > iter( "Port" );
	for( iter = iter.begin( element ); iter != iter.end(); iter++ )
	{
        int id = -1;
        string name;
        iter->GetAttribute( "id", &id );
        iter->GetAttribute( "name", &name );
        if( id >= 0 )
            midiInPorts_.insert( MidiPortMap::value_type( id, name ));
	}
}


void AppOptions::writeMidiInPorts( Element* element )
{
    for( MidiPortMap::iterator it=midiInPorts_.begin(); it!=midiInPorts_.end(); it++ )
    {
        Element* port = new Element( "Port" );
        port->SetAttribute( "id", it->first );
        port->SetAttribute( "name", it->second );
        element->LinkEndChild( port );
    }
}

#endif // _RT_AUDIO


