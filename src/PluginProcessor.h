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
#include "myLookAndFeel.h"

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

    void set_ampEQ(float bass_slider, float mid_slider, float treble_slider);
    void parameterChanged(const String& parameterID, float newValue) override;
    void setJsonModel(const char* jsonModel);
    void setLSTM(bool parametrized, float modelValue);
    void applyLSTMtoChannels(dsp::AudioBlock<float>& block, int totalNumChannels, RT_LSTM& LSTM, RT_LSTM& LSTM2, bool conditioned, float driveValue);
    AudioProcessorValueTreeState treeState;

    // Files and configuration

    // Pedal/amp states
    int fw_state = 1;       // 0 = off, 1 = on
    int cab_state = 1; // 0 = off, 1 = on

    bool conditioned = false;
    int pauseVolume = 3;

private:

    Equalizer equalizer1; // Amp EQ
    Equalizer equalizer2; // Amp EQ

    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* masterParam = nullptr;
    std::atomic<float>* bassParam = nullptr;
    std::atomic<float>* midParam = nullptr;
    std::atomic<float>* trebleParam = nullptr;
    std::atomic<float>* modelParam = nullptr;

    bool parametrized = false;
    float previousDriveValue = 0.5;
    float previousMasterValue = 0.5;
    //float steppedValue1 = 0.0;

    RT_LSTM LSTM;
    RT_LSTM LSTM2;

    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> dcBlocker;

    chowdsp::ResampledProcess<chowdsp::ResamplingTypes::SRCResampler> resampler;
    // IR processing
    CabSimulation cabSimIRa;
     
     
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RolandCubeAudioProcessor)
};
