// BaseSynth.h

#pragma once

#include "Voicer.h"



enum Parameters {
    parameterVolume = 0
};



class BaseSynth
{
public:
	BaseSynth();
	~BaseSynth() {};

protected:
	stk::Voicer m_voicer;
};






