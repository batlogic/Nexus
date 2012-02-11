
#include "Fonts.h"



static PirulenFont pirulen8( "Pirulen", 8, 1 );
static PirulenFont pirulen9( "Pirulen", 9, 1 );
static PirulenFont pirulen14( "Pirulen", 14, 1 );

static CFontDesc arial9( "Arial", 9 );
static CFontDesc arial10( "Arial", 10 );
static CFontDesc arial11( "Arial", 11 );

static CFontDesc lucida10( "Lucida Console", 10 );
static CFontDesc lucida11( "Lucida Console", 11 );


const CFontRef fontPirulen8  = &pirulen8;
const CFontRef fontPirulen9  = &pirulen9;
const CFontRef fontPirulen14 = &pirulen14;

const CFontRef fontArial9    = &arial9;
const CFontRef fontArial10   = &arial10;
const CFontRef fontArial11   = &arial11;

const CFontRef fontLucida10  = &lucida10;
const CFontRef fontLucida11  = &lucida11;



//-------------------------------------------------------
// class PirulenFont
//-------------------------------------------------------

PirulenFont::PirulenFont( const char* name, int size, int resourceId ) 
	: CFontDesc( name, size ),
	  resourceId_( resourceId )
{
	style = kBoldFace;
}


void PirulenFont::loadPlatformFont()
{
	WCHAR tempName [200];
	mbstowcs (tempName, name, 200);
	platformFont = (void*)new Gdiplus::Font( 
		tempName, 
		(Gdiplus::REAL)size, 
		Gdiplus::FontStyleBold, // Pirulen font only has bold style
		Gdiplus::UnitPixel, 
		Fonts::getInstance()->getCollection());
}


void PirulenFont::unloadPlatformFont()
{
	freePlatformFont();
}



//-------------------------------------------------------
// class FontResources
//-------------------------------------------------------

extern void* hInstance;

Fonts* Fonts::gInstance = NULL;

PirulenFont* Fonts::fonts_[] = {
	&pirulen8, &pirulen9, &pirulen14
};


void Fonts::loadFonts()
{
	if( gInstance == NULL )
    {
        GDIPlusGlobals::enter ();
	    for( UINT16 i=0; i<ARRAY_LENGTH( Fonts::fonts_ ); i++ ) 
	    {
		    PirulenFont* desc = Fonts::fonts_[ i ];
		    if( getInstance()->loadFont( desc->resourceId_ ))
				desc->loadPlatformFont();
	    }
    }
}


Fonts* Fonts::getInstance()
{
	if( gInstance == NULL ) {
		gInstance = new Fonts();
	}
	return gInstance;
}


bool Fonts::loadFont( UINT16 rscId )
{
#ifdef GDIPLUS
	HRSRC rsrc = FindResourceA( (HINSTANCE)hInstance, MAKEINTRESOURCEA( rscId ), "BINARY");
	if( rsrc ) 
	{
		HGLOBAL mem = LoadResource( (HINSTANCE)hInstance, rsrc );
		void *data  = LockResource (mem );
		size_t len  = SizeofResource( (HINSTANCE)hInstance, rsrc );

		Gdiplus::Status nResults = collection_.AddMemoryFont( data, len );
		return nResults == Gdiplus::Ok;
	}
#endif
	return false;
}


void Fonts::cleanup() 
{ 
    for( UINT16 i=0; i<ARRAY_LENGTH( fonts_ ); i++ ) {
		fonts_[ i ]->unloadPlatformFont();
	}
}



