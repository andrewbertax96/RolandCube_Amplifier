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
class RolandCubeAudioProcessor  : public juce::AudioProcessor,
                                  public AudioProcessorValueTreeState::Listener
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

    void parameterChanged(const String& parameterID, float newValue) override;
    void set_ampEQ(float bass_slider, float mid_slider, float treble_slider);
    void applyEQ(AudioBuffer<float>& buffer, Equalizer& equalizer1, Equalizer& equalizer2, MidiBuffer& midiMessages, int totalNumInputChannels, double sampleRate);

    void applyGainSmoothing(AudioBuffer<float>& buffer, const float masterParam, float& previousMasterValue);
    void smoothPopSound(AudioBuffer<float>& buffer, const float masterParam, int& pauseVolume);

    bool isValidFormat(File configFile);
    void modelSelect(int modelParam, std::vector<File> modelType);
    void loadConfig(File configFile);
    void applyLSTM(AudioBuffer<float>& buffer, dsp::AudioBlock<float>& block, RT_LSTM& LSTM, RT_LSTM& LSTM2, bool conditioned, const float gainParam, float& previousGainValue, chowdsp::ResampledProcess<chowdsp::ResamplingTypes::SRCResampler<>>& resampler);
    void LSTMtoChannels(juce::dsp::AudioBlock<float>& block, RT_LSTM& LSTM, RT_LSTM& LSTM2, bool conditioned, float gainValue);
    
    AudioProcessorValueTreeState treeState;

    //// Files and configuration

    std::vector<File> jsonFilesGainStable;
    std::vector<File> jsonFilesParametrizedGain;

    std::vector<File> modelType;

private:

    Equalizer equalizer1; // Amp EQ
    Equalizer equalizer2; // Amp EQ

    Atomic<float> gainParam = {0.5};
    Atomic<float> masterParam = { 0.5 };
    Atomic<float> bassParam = { 0.0 };
    Atomic<float> midParam = { 0.0 };
    Atomic<float> trebleParam = { 0.0 };
    Atomic<int> modelParam = { 0 };
    Atomic<bool> gainType_Param = {false};

    bool conditioned = false;
    int pauseVolume = 3;
    const char* char_filename = "";

    int current_model_index = 0;
    File saved_model;

    float previousGainValue = 0.5;
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
