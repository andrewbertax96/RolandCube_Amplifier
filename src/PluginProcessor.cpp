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
                        std::make_unique<AudioParameterFloat>(MASTER_ID, MASTER_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5),
                        std::make_unique<AudioParameterFloat>(MODEL_ID, MODEL_NAME, NormalisableRange<float>(0.0f, 8.0f, 1.0f), 0.0) })

#endif
{
    treeState.addParameterListener(GAIN_ID, this);
    treeState.addParameterListener(BASS_ID, this);
    treeState.addParameterListener(MID_ID, this);
    treeState.addParameterListener(TREBLE_ID, this);
    treeState.addParameterListener(MASTER_ID, this);
    treeState.addParameterListener(MODEL_ID, this);
    
    pauseVolume = 3;
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
    if (parameterID == "MODEL_ID") {
        modelParam = treeState.getRawParameterValue("MODEL_ID");
    }
    else if (parameterID == "BASS_ID") {
        bassParam = treeState.getRawParameterValue("BASS_ID");
    }
    else if (parameterID == "MID_ID") {
        midParam = treeState.getRawParameterValue("MID_ID");
    }
    else if (parameterID == "TREBLE_ID") {
        trebleParam = treeState.getRawParameterValue("TREBLE_ID");
    }
    else if (parameterID == "GAIN_ID") {
        driveParam = treeState.getRawParameterValue("GAIN_ID");
    }
    else if (parameterID == "MASTER_ID") {
        masterParam = treeState.getRawParameterValue("MASTER_ID");
    }

    auto bassValue = static_cast<float> (bassParam->load());
    auto midValue = static_cast<float> (midParam->load());
    auto trebleValue = static_cast<float> (trebleParam->load());

    eq4band.setParameters(bassValue, midValue, trebleValue, 0.0);
    eq4band2.setParameters(bassValue, midValue, trebleValue, 0.0);
    
    //Parameters update  when sliders moved
}
void RolandCubeAudioProcessor::setJsonModel(const char* jsonModel)
{
    this->suspendProcessing(true);
    
    LSTM.reset();
    LSTM2.reset();

    LSTM.load_json(jsonModel);
    LSTM2.load_json(jsonModel);

    if (LSTM.input_size == 1) {
        conditioned = false;
    }
    else {
        conditioned = true;
    }

    this->suspendProcessing(false);
}
//==============================================================================
void RolandCubeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    auto modelValue = static_cast<float> (modelParam->load());
    
    if (!parametrized) {
        switch (static_cast<int>(modelValue))
        {
        case 0:
            setJsonModel(BinaryData::acousticModelGainStable_json);
            break;

        case 1:
            setJsonModel(BinaryData::blackPanelModelGainStable_json);
            break;

        case 2:
            setJsonModel(BinaryData::BritComboModelGainStable_json);
            break;

        case 3:
            setJsonModel(BinaryData::tweedModelGainStable_json);
            break;

        case 4:
            setJsonModel(BinaryData::classicModelGainStable_json);
            break;

        case 5:
            setJsonModel(BinaryData::metalModelGainStable_json);
            break;

        case 6:
            setJsonModel(BinaryData::rFierModelGainStable_json);
            break;

        case 7:
            setJsonModel(BinaryData::extremeModelGainStable_json);
            break;

        case 8:
            setJsonModel(BinaryData::dynamicAmpModelGainStable_json);
            break;

        default:
            if (modelValue > 8.0)
            {
                modelValue = 8.0;
            }
            break;
        }
    }
    else {
        switch (static_cast<int>(modelValue))
        {
        case 0:
            setJsonModel(BinaryData::acousticModelParametrizedGain_json);
            break;

        case 1:
            setJsonModel(BinaryData::blackPanelModelParametrizedGain_json);
            break;

        case 2:
            setJsonModel(BinaryData::britComboModelParametrizedGain_json);
            break;

        case 3:
            setJsonModel(BinaryData::tweedModelParametrizedGain_json);
            break;

        case 4:
            setJsonModel(BinaryData::classicModelParametrizedGain_json);
            break;

        case 5:
            setJsonModel(BinaryData::metalModelParametrizedGain_json);
            break;

        case 6:
            setJsonModel(BinaryData::rFierModelParemtrizedGain_json);
            break;

        case 7:
            setJsonModel(BinaryData::extremeModelParametrizedGain_json);
            break;

        case 8:
            setJsonModel(BinaryData::dynamicAmpModelParametrizedGain_json);
            break;

        default:
            if (modelValue > 8.0)
            {
                modelValue = 8.0;
            }
            break;
        }
    }

    cabSimIRa.load(BinaryData::default_ir_wav, BinaryData::default_ir_wavSize);
    *dcBlocker.state = *dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 35.0f);

    // prepare resampler for target sample rate: 44.1 kHz
    constexpr double targetSampleRate = 44100.0;
    //resampler.prepareWithTargetSampleRate ({ sampleRate, (uint32) samplesPerBlock, 1 }, targetSampleRate);
    //resampler.prepareWithTargetSampleRate({ sampleRate, (uint32)samplesPerBlock, 2 }, targetSampleRate);


    dsp::ProcessSpec specMono { sampleRate, static_cast<uint32> (samplesPerBlock), 1 };
    dsp::ProcessSpec spec{ sampleRate, static_cast<uint32> (samplesPerBlock), 2 };

    dcBlocker.prepare(spec);

    LSTM.reset();
    LSTM2.reset();

    // Set up IR
    cabSimIRa.prepare(spec);
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

    auto driveValue = static_cast<float> (driveParam->load());
    auto masterValue = static_cast<float> (masterParam->load());
    auto bassValue = static_cast<float> (bassParam->load());
    auto midValue = static_cast<float> (midParam->load());
    auto trebleValue = static_cast<float> (trebleParam->load());
    

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    dsp::AudioBlock<float> block(buffer);
    dsp::ProcessContextReplacing<float> context(block);

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
    
    if (fw_state == 1) {

        if (conditioned == false) {
            // Apply ramped changes for gain smoothing
            if (driveValue == previousDriveValue)
            {
                buffer.applyGain(driveValue * 2.5);
            }
            else {
                buffer.applyGainRamp(0, (int)buffer.getNumSamples(), previousDriveValue * 2.5, driveValue * 2.5);
                previousDriveValue = driveValue;
            }
            //auto block44k = resampler.processIn(block);
            auto block44k = block;
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                // Apply LSTM model
                if (channel == 0) {
                    LSTM.process(block44k.getChannelPointer(0), block44k.getChannelPointer(0), (int)block44k.getNumSamples());
                }
                else if (channel == 1) {
                    LSTM2.process(block44k.getChannelPointer(1), block44k.getChannelPointer(1), (int)block44k.getNumSamples());
                }
            }
            //resampler.processOut(block44k, block);
        }
        else {
            buffer.applyGain(1.5); // Apply default boost to help sound
            // resample to target sample rate

            //auto block44k = resampler.processIn(block);
            auto block44k = block;
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                // Apply LSTM model
                if (ch == 0) {
                    LSTM.process(block44k.getChannelPointer(0), driveValue, block44k.getChannelPointer(0), (int)block44k.getNumSamples());
                }
                else if (ch == 1) {
                    LSTM2.process(block44k.getChannelPointer(1), driveValue, block44k.getChannelPointer(1), (int)block44k.getNumSamples());
                }
            }
            //resampler.processOut(block44k, block);
        }

        dcBlocker.process(context);

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            // Apply EQ
            if (ch == 0) {
                eq4band.process(buffer.getReadPointer(0), buffer.getWritePointer(0), midiMessages, buffer.getNumSamples(), totalNumInputChannels, getSampleRate());

            }
            else if (ch == 1) {
                eq4band2.process(buffer.getReadPointer(1), buffer.getWritePointer(1), midiMessages, buffer.getNumSamples(), totalNumInputChannels, getSampleRate());
            }
        }

        if (cab_state == 1) {
            cabSimIRa.process(context); // Process IR a on channel 0
            buffer.applyGain(2.0);
            //} else {
            //    buffer.applyGain(0.7);
        }
        /*cabSimIRa.process(context); // Process IR a on channel 0
        buffer.applyGain(2.0);*/

        // Master Volume 
        // Apply ramped changes for gain smoothing
        if (masterValue == previousMasterValue)
        {
            buffer.applyGain(masterValue);
        }
        else {
            buffer.applyGainRamp(0, (int)buffer.getNumSamples(), previousMasterValue, masterValue);
            previousMasterValue = masterValue;
        }

        // Smooth pop sound when changing models
        if (pauseVolume > 0) {
            if (pauseVolume > 2)
                buffer.applyGain(0.0);
            else if (pauseVolume == 2)
                buffer.applyGainRamp(0, (int)buffer.getNumSamples(), 0, masterValue / 2);
            else
                buffer.applyGainRamp(0, (int)buffer.getNumSamples(), masterValue / 2, masterValue);
            pauseVolume -= 1;
        }
    }
    /*for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }*/
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