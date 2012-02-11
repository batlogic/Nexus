
#include "Colors.h"
#include "../Utils.h"

ColorScheme colors;
vector< ColorScheme > gColorSchemes;



void ColorScheme::init( const string& path, ColorScheme& defaultScheme )
{
    gColorSchemes.clear();

    defaultScheme.makeDefault();
    gColorSchemes.push_back( defaultScheme );

    try {
        string filename = path + SKIN_FILENAME;
        Document doc = Document( filename );
        doc.LoadFile();

	    Element* rootElement = doc.FirstChildElement( "NexusColors", true );
	
	    Iterator< Element > it( "Scheme" );
	    for( it = it.begin( rootElement ); it != it.end(); it++ )
	    {
		    Element* schemeElement = it.Get();
            ColorScheme scheme;
            scheme.makeDefault();
            if( scheme.load( schemeElement )) {
                gColorSchemes.push_back( scheme );
            }
	    }
    }
    catch( const exception& e ) {
        TRACE( e.what() );
    }
}


bool ColorScheme::load( Element* element )
{
    CColor* colors[] = {
    &fill1,&fill2,&fill3,&text1,&text2,&text3,
    &selection,&control1,&control2,&wire1,&wire2,
    &module,&moduleMono,&modulePort1,&modulePort2,&moduleText1,&moduleText2,&moduleFrame1,&moduleFrame2,&moduleFrame3,
    &master,&masterPort1,&masterPort2,&masterText1,&masterText2,&masterFrame1,&masterFrame2,&masterFrame3,
    &dlgFill1,&dlgFill2,&dlgCaption,&dlgText,&dlgTextBkgnd,
    &appFrame,&appCaptionBar,&appCaption,&appMin,&appMax,&appClose
    };

    string names[] = {
    "fill1","fill2","fill3","text1","text2","text3",
    "selection","control1","control2","wire1","wire2",
    "module","moduleMono","modulePort1","modulePort2","moduleText1","moduleText2","moduleFrame1","moduleFrame2","moduleFrame3",
    "master","masterPort1","masterPort2","masterText1","masterText2","masterFrame1","masterFrame2","masterFrame3",
    "dlgFill1","dlgFill2","dlgCaption","dlgText","dlgTextBkgnd",
    "appFrame","appCaptionBar","appCaption","appMin","appMax","appClose"
    };
        
    try {
	    element->GetAttribute( "name", &name_ );
        element->GetAttribute( "wires", &wireWidth_ );

	    Iterator< Element > it( "Color" );
	    for( it = it.begin( element ); it != it.end(); it++ )
	    {
		    Element* colorElement = it.Get();

            string name, code;
            int alpha = 255;

            it->GetAttribute( "name", &name );
            it->GetAttribute( "code", &code );
            it->GetAttribute( "alpha", &alpha, false );

            CColor color = MakeCColor( code, alpha );

            for( UINT16 i=0; i<ARRAY_LENGTH( names ); i++ ) {
	            if( name == names[i] ) {
		            *colors[i] = color;
                    break;
                }
            }
	    }
	}
	catch( const exception& e )	{
        TRACE( e.what() );
        return false;
	}
    return true;
}


void ColorScheme::makeDefault()
{
    name_           = "Dark";
    fill1		    = MakeCColor( "444D50", 255 );
    fill2		    = MakeCColor( "56666B", 255 );
    fill3		    = MakeCColor( "97A4A6", 255 );
    text1		    = MakeCColor( "444d50", 255 );
    text2		    = MakeCColor( "B8C2C4", 255 );
    text3		    = MakeCColor( "97A4A6", 255 );
    selection       = MakeCColor( "D1802E", 255 );
    control1        = MakeCColor( "444d50", 255 );
    control2        = MakeCColor( "879699", 255 );
    wire1           = MakeCColor( "56666B", 255 );
    wire2           = MakeCColor( "B72E00", 255 );
    module          = MakeCColor( "56666B",  99 );
    moduleMono      = MakeCColor( "56666B",  22 );
    moduleText1     = MakeCColor( "B8C2C4", 255 );
    moduleText2     = MakeCColor( "D1802E", 255 );
    modulePort1     = MakeCColor( "D1802E", 255 );
    modulePort2     = MakeCColor( "B72E00", 255 );
    moduleFrame1    = MakeCColor( "56666B", 255 );
    moduleFrame2    = MakeCColor( "879699", 255 );
    moduleFrame3    = MakeCColor( "D1802E", 255 );
    master          = MakeCColor( "D1802E", 255 );
    masterPort1     = MakeCColor( "444D50", 255 ); 
    masterPort2     = MakeCColor( "B72E00", 255 );
    masterText1     = MakeCColor( "444d50", 255 );
    masterText2     = MakeCColor( "B72E00", 255 );
    masterFrame1    = MakeCColor( "D1802E", 255 );
    masterFrame2    = MakeCColor( "444d50", 255 );
    masterFrame3    = MakeCColor( "B72E00", 255 );
    dlgFill1        = MakeCColor( "929292", 255 );
    dlgFill2        = MakeCColor( "A4A4A4", 255 );
    dlgCaption      = MakeCColor( "878787", 100 );
    dlgText		    = MakeCColor( "000000", 255 );
    dlgTextBkgnd    = MakeCColor( "C4C4C4", 255 );
    appFrame        = MakeCColor( "56666B", 255 );
    appCaptionBar   = MakeCColor( "56666B", 255 );
    appCaption	    = MakeCColor( "B8C2C4", 255 );
    appMin          = MakeCColor( "D1802E", 255 );
    appMax          = MakeCColor( "D1802E", 255 );
    appClose        = MakeCColor( "B72E00", 255 );

    wireWidth_      = 2;
}



