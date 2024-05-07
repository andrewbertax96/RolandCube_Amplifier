/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


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

#include <nlohmann/json.hpp>
#include "RTNeuralLSTM.h"
#include "Eq4Band.h"
#include "CabSim.h"

//==============================================================================
/**
*/
class RolandCubeAudioProcessor : public AudioProcessor
{
public:
    //==============================================================================
    RolandCubeAudioProcessor();
    ~RolandCubeAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

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
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    bool isValidFormat(File configFile);

    void setJsonFiles();

    void set_ampEQ(float bass_slider, float mid_slider, float treble_slider);

    // Files and configuration
    void loadConfig(File configFile);

    // Files and configuration

    bool conditioned = false;
    int pauseVolume = 3;
    const char* char_filename = "";
    int fw_state = 1;
    int cab_state = 1;
    int current_model_index = 0;
    std::vector<File> jsonFiles;
    File saved_model;

    File folder = File::getSpecialLocation(File::userDesktopDirectory);
    AudioProcessorValueTreeState treeState;

    bool model_loaded = false;

private:

    Eq4Band eq4band; // Amp EQ
    Eq4Band eq4band2; // Amp EQ

    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* masterParam = nullptr;
    std::atomic<float>* bassParam = nullptr;
    std::atomic<float>* midParam = nullptr;
    std::atomic<float>* trebleParam = nullptr;
    std::atomic<float>* modelParam = nullptr;
    Atomic<bool> typeParam = {false};

    float previousDriveValue = 0.5;
    float previousMasterValue = 0.5;
    //float steppedValue1 = 0.0;

    RT_LSTM LSTM;
    RT_LSTM LSTM2;

    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> dcBlocker;

    chowdsp::ResampledProcess<chowdsp::ResamplingTypes::SRCResampler<>> resampler;
   
    // IR processing
    CabSim cabSimIRa;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RolandCubeAudioProcessor)
};
