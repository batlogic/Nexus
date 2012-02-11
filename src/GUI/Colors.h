
#ifndef COLORS_H
#define COLORS_H

#include "../Definitions.h"
#include "../VST/aeffguieditor.h"
#include "../XML/ticpp.h"

using namespace ticpp;


inline CColor MakeCColor (const string& hexCode , unsigned char alpha = 255)
{
	CColor c;
    istringstream sRed( hexCode.substr( 0, 2 )); 
    istringstream sGreen( hexCode.substr( 2, 2 )); 
    istringstream sBlue( hexCode.substr( 4, 2 )); 

    int red, green ,blue;
    sRed   >> hex >> red; 
    sGreen >> hex >> green; 
    sBlue  >> hex >> blue; 
    c = MakeCColor( (unsigned char)red, (unsigned char)green, (unsigned char)blue, alpha );
	return c;
}


class ColorScheme
{
public:
    static void init( const string& path, ColorScheme& scheme );

    string name_;
    CCoord wireWidth_;

    CColor fill1; 
    CColor fill2; 
    CColor fill3; 
    CColor text1;
    CColor text2; 
    CColor text3;
    CColor selection;   // selection in TextEdit, SelectBox, GridBox, TabButton
    CColor control1;    // handle Slider, TabButtons
    CColor control2;    // bkgnd Slider
    CColor wire1;
    CColor wire2;
    CColor module;
    CColor moduleMono;
    CColor moduleText1;
    CColor moduleText2;
    CColor modulePort1;
    CColor modulePort2;
    CColor moduleFrame1;      // frame ModuleBox
    CColor moduleFrame2;      // frame ModuleBoxSelected
    CColor moduleFrame3;      // frame ModuleBoxFocus
    CColor master;
    CColor masterPort1;
    CColor masterPort2;
    CColor masterText1;
    CColor masterText2;
    CColor masterFrame1;
    CColor masterFrame2;
    CColor masterFrame3;
    CColor dlgFill1;
    CColor dlgFill2;
    CColor dlgCaption;
    CColor dlgText; 
    CColor dlgTextBkgnd; 
    CColor appFrame; 
    CColor appCaptionBar; 
    CColor appCaption; 
    CColor appMin;
    CColor appMax;
    CColor appClose;

protected:
    bool load( Element* element );
    void makeDefault();
};

extern ColorScheme colors;
extern vector< ColorScheme > gColorSchemes;


#endif COLORS_H