/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RolandCubeAudioProcessor::RolandCubeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
         .withInput("Input", AudioChannelSet::stereo(), true)
#endif
         .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
     ),

    treeState(*this, nullptr, "PARAMETER", { std::make_unique<AudioParameterFloat>(GAIN_ID, GAIN_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f),
                        std::make_unique<AudioParameterFloat>(BASS_ID, BASS_NAME, NormalisableRange<float>(-8.0f, 8.0f, 0.01f), 0.0f),
                        std::make_unique<AudioParameterFloat>(MID_ID, MID_NAME, NormalisableRange<float>(-8.0f, 8.0f, 0.01f), 0.0f),
                        std::make_unique<AudioParameterFloat>(TREBLE_ID, TREBLE_NAME, NormalisableRange<float>(-8.0f, 8.0f, 0.01f), 0.0f),
                        std::make_unique<AudioParameterFloat>(MASTER_ID, MASTER_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5) })

#endif
{
    treeState.addParameterListener(GAIN_ID, this);
    treeState.addParameterListener(BASS_ID, this);
    treeState.addParameterListener(MID_ID, this);
    treeState.addParameterListener(TREBLE_ID, this);
    treeState.addParameterListener(MASTER_ID, this);

    /*driveParam = treeState.getRawParameterValue(GAIN_ID);
    masterParam = treeState.getRawParameterValue(MASTER_ID);
    bassParam = treeState.getRawParameterValue(BASS_ID);
    midParam = treeState.getRawParameterValue(MID_ID);
    trebleParam = treeState.getRawParameterValue(TREBLE_ID);*/

    /*auto bassValue = static_cast<float> (bassParam->load());
    auto midValue = static_cast<float> (midParam->load());
    auto trebleValue = static_cast<float> (trebleParam->load());*/

    //eq4band.setParameters(bassValue, midValue, trebleValue, 0.0);
    //eq4band2.setParameters(bassValue, midValue, trebleValue, 0.0);

    pauseVolume = 3;

    //cabSimIRa.load(BinaryData::default_ir_wav, BinaryData::default_ir_wavSize);
}

RolandCubeAudioProcessor::~RolandCubeAudioProcessor()
{
}

//==============================================================================
const juce::String RolandCubeAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RolandCubeAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RolandCubeAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RolandCubeAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RolandCubeAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RolandCubeAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RolandCubeAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RolandCubeAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RolandCubeAudioProcessor::getProgramName (int index)
{
    return {};
}

void RolandCubeAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}
void RolandCubeAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
    //Parameters update  when sliders moved

    /*//low/high pass
    if (parameterID == ParameterID::fHP) {
        highPassFilter_R.setParameter(Filter::Parameter::Frequency, newValue);
        highPassFilter_L.setParameter(Filter::Parameter::Frequency, newValue);
    }
    else if (parameterID == ParameterID::fLP) {
        lowPassFilter_R.setParameter(Filter::Parameter::Frequency, newValue);
        lowPassFilter_L.setParameter(Filter::Parameter::Frequency, newValue);
    }

    //lowShelf
    else if (parameterID == ParameterID::fLS) {
        lowShelfFilter_R.setParameter(Filter::Parameter::Frequency, newValue);
        lowShelfFilter_L.setParameter(Filter::Parameter::Frequency, newValue);
    }
    else if (parameterID == ParameterID::gLS)
    {
        lowShelfFilter_R.setParameter(Filter::Parameter::gain, newValue);
        lowShelfFilter_L.setParameter(Filter::Parameter::gain, newValue);
    }

    //highShelf
    else if (parameterID == ParameterID::fHS) {
        highShelfFilter_R.setParameter(Filter::Parameter::Frequency, newValue);
        highShelfFilter_L.setParameter(Filter::Parameter::Frequency, newValue);
    }
    else if (parameterID == ParameterID::gHS) {
        highShelfFilter_R.setParameter(Filter::Parameter::gain, newValue);
        highShelfFilter_L.setParameter(Filter::Parameter::gain, newValue);
    }

    //Peak1
    else if (parameterID == ParameterID::fPK1) {
        peakFilter1_R.setParameter(Filter::Parameter::Frequency, newValue);
        peakFilter1_L.setParameter(Filter::Parameter::Frequency, newValue);
    }
    else if (parameterID == ParameterID::gPK1) {
        peakFilter1_R.setParameter(Filter::Parameter::gain, newValue);
        peakFilter1_L.setParameter(Filter::Parameter::gain, newValue);
    }
    else if (parameterID == ParameterID::qPK1) {
        peakFilter1_R.setParameter(Filter::Parameter::BW, newValue);
        peakFilter1_L.setParameter(Filter::Parameter::BW, newValue);
    }

    //Peak2
    else if (parameterID == ParameterID::fPK2) {
        peakFilter2_R.setParameter(Filter::Parameter::Frequency, newValue);
        peakFilter2_L.setParameter(Filter::Parameter::Frequency, newValue);
    }
    else if (parameterID == ParameterID::gPK2) {
        peakFilter2_R.setParameter(Filter::Parameter::gain, newValue);
        peakFilter2_L.setParameter(Filter::Parameter::gain, newValue);
    }
    else if (parameterID == ParameterID::qPK2) {
        peakFilter2_R.setParameter(Filter::Parameter::BW, newValue);
        peakFilter2_L.setParameter(Filter::Parameter::BW, newValue);
    }

    //Master
    else if (parameterID == ParameterID::master)
        masterDB = newValue;*/
}
//==============================================================================
void RolandCubeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void RolandCubeAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RolandCubeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void RolandCubeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool RolandCubeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RolandCubeAudioProcessor::createEditor()
{
    return new RolandCubeAudioProcessorEditor (*this, treeState);
}

//==============================================================================
void RolandCubeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RolandCubeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void RolandCubeAudioProcessor::set_ampEQ(float bass_slider, float mid_slider, float treble_slider)
{
    //eq4band.setParameters(bass_slider, mid_slider, treble_slider, 0.0f);
    //eq4band2.setParameters(bass_slider, mid_slider, treble_slider, 0.0f);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RolandCubeAudioProcessor();
}