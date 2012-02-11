
#ifndef FONTS_H
#define FONTS_H

#include "Editor.h"


class PirulenFont : public CFontDesc
{
public:
	PirulenFont( const char* name, int size, int resourceId );

	void loadPlatformFont();
	void unloadPlatformFont();

	int resourceId_;
};

extern const CFontRef fontPirulen8;
extern const CFontRef fontPirulen9;
extern const CFontRef fontPirulen14;

extern const CFontRef fontArial9;
extern const CFontRef fontArial10;
extern const CFontRef fontArial11;

extern const CFontRef fontLucida10;
extern const CFontRef fontLucida11;


class Fonts : public CFontDesc
{
public:
	static Fonts* getInstance();

	static void loadFonts();
	static void cleanup();
	
	static PirulenFont* fonts_[];
	
	Gdiplus::PrivateFontCollection* getCollection() { return &collection_; }

protected:
	static Fonts* gInstance;
	bool loadFont( UINT16 rscId );
	Gdiplus::PrivateFontCollection collection_;
};

#endif // FONTS_H