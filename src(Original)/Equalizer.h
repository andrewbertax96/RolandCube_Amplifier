/*
  ==============================================================================

  Equalizer

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================

class Equalizer
{
public:
    Equalizer();
    void process (const float* inData, float* outData, MidiBuffer& midiMessages, const int numSamples, const int numInputChannels, const int sampleRate);
    void setParameters(float bass_slider, float mid_slider, float treble_slider, float presence_slider);
    void resetSampleRate();

private:
    // Tone Knob related variables
    float cDenorm = 10e-30;
    float cAmpDB = 8.65617025;

    int bass_frequency = 180;
    int mid_frequency = 1000;
    int treble_frequency = 4000;
    //int bass_frequency = 200;
    //int mid_frequency = 2000;
    //int treble_frequency = 5000;
    // 
    //int presence_frequency = 5500;

    int srate = 44100;  // Set default

    float pi = 3.1415926;

    float outVol;
    float xHI = 0.0;//
    float a0HI = 0.0;//
    float b1HI = 0.0;
    float xMID = 0.0;
    float a0MID = 0.0;
    float b1MID = 0.0;
    float xLOW = 0.0;
    float a0LOW = 0.0;
    float b1LOW = 0.0;

    float lVol = 0.0;
    float lmVol = 0.0;
    float hmVol = 0.0;
    float hVol = 0.0;

    float s0 = 0.0;
    float low0 = 0.0;
    float tmplMID = 0.0;
    float spl0 = 0.0;
    float hi0 = 0.0;
    float midS0 = 0.0;
    float highS0 = 0.0;
    float tmplHI = 0.0;
    float lowS0 = 0.0;
    float tmplLOW = 0.0;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Equalizer)
};