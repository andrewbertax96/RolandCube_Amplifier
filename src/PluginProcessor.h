/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <nlohmann/json.hpp>
#include "RTNeuralLSTM.h"
#include "cabSimulation.h"
#include "Equalizer.h"

#define GAIN_ID "drive"
#define GAIN_NAME "Drive"
#define MASTER_ID "level"
#define MASTER_NAME "Level"
#define BASS_ID "bass"
#define BASS_NAME "Bass"
#define MID_ID "mid"
#define MID_NAME "Mid"
#define TREBLE_ID "treble"
#define TREBLE_NAME "Treble"
#define MODEL_ID "model"
#define MODEL_NAME "Model"
#define TYPE_ID "type"
#define TYPE_NAME "Type"

//==============================================================================
/**
*/
class RolandCubeAudioProcessor  : public juce::AudioProcessor
    //,
                                  //public AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    RolandCubeAudioProcessor();
    ~RolandCubeAudioProcessor() override;
    
    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    #ifndef JucePlugin_PreferredChannelConfigurations
     bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    #endif

    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;

    //==============================================================================
    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //void parameterChanged(const String& parameterID, float newValue) override;
    void set_ampEQ(float bass_slider, float mid_slider, float treble_slider);
    /*void applyEQ(AudioBuffer<float>& buffer, Equalizer& equalizer1, Equalizer& equalizer2, MidiBuffer& midiMessages, int totalNumInputChannels, double sampleRate);

    void applyGainSmoothing(AudioBuffer<float>& buffer, const float masterParam, float& previousMasterValue);
    void smoothPopSound(AudioBuffer<float>& buffer, const float masterParam, int& pauseVolume);*/

    void loadConfig(File configFile);
    //void applyLSTM(AudioBuffer<float>& buffer, dsp::AudioBlock<float>& block, RT_LSTM& LSTM, RT_LSTM& LSTM2, bool conditioned, const float gainParam, float& previousGainValue, chowdsp::ResampledProcess<chowdsp::ResamplingTypes::SRCResampler<>>& resampler);
    //void LSTMtoChannels(juce::dsp::AudioBlock<float>& block, RT_LSTM& LSTM, RT_LSTM& LSTM2, bool conditioned, float gainValue);
    
    AudioProcessorValueTreeState treeState;

    //// Files and configuration

    //bool conditioned = false;
    //int pauseVolume = 3;
    //const char* char_filename = "";

    //int current_model_index = 0;
    //std::vector<File> jsonFiles;
    //File saved_model;
    
    // Pedal/amp states
    int fw_state = 1;       // 0 = off, 1 = on
    int cab_state = 1; // 0 = off, 1 = on

    File currentDirectory = File::getCurrentWorkingDirectory().getFullPathName();
    int current_model_index = 0;

    Array<File> fileArray;
    std::vector<File> jsonFiles;
    int num_models = 0;
    File folder = File::getSpecialLocation(File::userDesktopDirectory);
    File saved_model;

    //AudioProcessorValueTreeState treeState;

    bool conditioned = false;

    const char* char_filename = "";

    int pauseVolume = 3;

    bool model_loaded = false;
private:

    Equalizer equalizer1; // Amp EQ
    Equalizer equalizer2; // Amp EQ

    /*Atomic<float> gainParam = {0.0};
    Atomic<float> masterParam = { 0.5 };
    Atomic<float> bassParam = { 0.0 };
    Atomic<float> midParam = { 0.0 };
    Atomic<float> trebleParam = { 0.0 };
    Atomic<float> modelParam = { 0.0 };
    Atomic<bool> typeParam = {false};*/

    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* masterParam = nullptr;
    std::atomic<float>* bassParam = nullptr;
    std::atomic<float>* midParam = nullptr;
    std::atomic<float>* trebleParam = nullptr;
    Atomic<bool> typeParam = { false }; 
    
    //float previousGainValue = 0.5;
    float previousDriveValue = 0.5;
    float previousMasterValue = 0.5;

    RT_LSTM LSTM;
    RT_LSTM LSTM2;

    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> dcBlocker;

    chowdsp::ResampledProcess<chowdsp::ResamplingTypes::SRCResampler<>> resampler;

    // IR processing
    CabSimulation cabSimIRa;
     
     
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RolandCubeAudioProcessor)
};
