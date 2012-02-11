
#ifndef RTWRAPPER_H
#define RTWRAPPER_H

#include "Definitions.h"
#include "RT/RtAudio.h"
#include "AppOptions.h"

class RtEditor;
class MainFrame;
class Synth;
class SoundDevices;
class AppOptions;



class RtWrapper
{
public:
	int run( int nCmdShow );
	void close();

protected:
	AppOptions options_;
    MainFrame* mainFrame_;
    Synth* synth_;
	RtEditor* editor_;
    SoundDevices* devices_;
};

#endif // RTWRAPPER_H
