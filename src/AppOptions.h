
#ifndef APPOPTIONS_H
#define APPOPTIONS_H

#include "Definitions.h"
#include "RT/RtAudio.h"
#include "SoundDevices.h"

#include "XML/ticpp.h"
using namespace ticpp;

#define TCHAR wchar_t


//----------------------------------------------------------
// class AppOptions
//----------------------------------------------------------

#ifdef _RT_AUDIO
class AppOptions : public SoundOptions
{
public: 
    AppOptions();

    void open( int argc, TCHAR* argv[], const string& path, const string& filename );

protected:
    void readMidiInPorts( Element* element );
    void writeMidiInPorts( Element* element );

#else

class AppOptions
{
public:
    AppOptions();
    void open( const string& path, const string& optionPath );

#endif

public:
    void close();
    
    long windowLeft_, windowTop_, windowRight_, windowBottom_;
    long windowState_;
    string appPath_;
    string optionFilename_;
    string bankPath_;
    string colorScheme_;
    string columnMetrics_;
    INT32 sortColumn_;
    INT32 sortType_;
    INT16 tuning_;
    bool autosave_;

protected:
    void readOptionFile();
};

#endif // APPOPTIONS_H