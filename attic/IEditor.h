
#ifndef IEDITOR_H
#define IEDITOR_H

class Synth;
class Program;



class IEditor
{
public:
    virtual void clear() {}
    virtual bool setProgram() = 0;
	virtual void setSynth( Synth* synth ) = 0;
};



#endif