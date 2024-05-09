/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RolandCubeAudioProcessor::RolandCubeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
    ),

    treeState(*this, nullptr, "PARAMETER", 
                        { 
                            std::make_unique<AudioParameterFloat>(GAIN_ID, GAIN_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f),
                            std::make_unique<AudioParameterFloat>(BASS_ID, BASS_NAME, NormalisableRange<float>(-8.0f, 8.0f, 0.01f), 0.0f),
                            std::make_unique<AudioParameterFloat>(MID_ID, MID_NAME, NormalisableRange<float>(-8.0f, 8.0f, 0.01f), 0.0f),
                            std::make_unique<AudioParameterFloat>(TREBLE_ID, TREBLE_NAME, NormalisableRange<float>(-8.0f, 8.0f, 0.01f), 0.0f),
                            std::make_unique<AudioParameterFloat>(MASTER_ID, MASTER_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5),
                            std::make_unique<AudioParameterFloat>(MODEL_ID, MODEL_NAME, NormalisableRange<float>(0.0f, 8.0f, 1.0f), 0.0),
                            //std::make_unique<AudioParameterBool>(TYPE_ID, TYPE_NAME, typeParam.get()) 
                        }
    )

    
#endif
{
    driveParam = treeState.getRawParameterValue (GAIN_ID);
    masterParam = treeState.getRawParameterValue (MASTER_ID);
    bassParam = treeState.getRawParameterValue (BASS_ID);
    midParam = treeState.getRawParameterValue (MID_ID);
    trebleParam = treeState.getRawParameterValue (TREBLE_ID);
    modelParam = treeState.getRawParameterValue (MODEL_ID);
    //typeParam = treeState.getRawParameterValue (TYPE_ID);

    //setJsonFiles();
    //const int selectedFileIndex = static_cast<float> (modelParam->load());
    //if (selectedFileIndex >= 0 && selectedFileIndex < jsonFiles.size() && jsonFiles.empty() == false) { //check if correct 
    //    if (jsonFiles[selectedFileIndex].existsAsFile() && isValidFormat(jsonFiles[selectedFileIndex])) {
    //        loadConfig(jsonFiles[selectedFileIndex]);
    //        current_model_index = selectedFileIndex;
    //        saved_model = jsonFiles[selectedFileIndex];
    //    }
    //}
    if (!jsonFiles.empty()) {
        if (saved_model.existsAsFile() && isValidFormat(saved_model)) {
            loadConfig(saved_model);
            //modelSelect.setText(processor.saved_model.getFileNameWithoutExtension(), juce::NotificationType::dontSendNotification);
        }
        else {
            if (jsonFiles[0].existsAsFile() && isValidFormat(jsonFiles[0])) {
                loadConfig(jsonFiles[0]);
                //modelSelect.setText(processor.jsonFiles[0].getFileNameWithoutExtension(), juce::NotificationType::dontSendNotification);
            }
        }
    }

  


    auto bassValue = static_cast<float> (bassParam->load());
    auto midValue = static_cast<float> (midParam->load());
    auto trebleValue = static_cast<float> (trebleParam->load());

    eq4band.setParameters(bassValue, midValue, trebleValue, 0.0);
    eq4band2.setParameters(bassValue, midValue, trebleValue, 0.0);

    pauseVolume = 3;
    cabSimIRa.load(BinaryData::default_ir_wav, BinaryData::default_ir_wavSize);

}

RolandCubeAudioProcessor::~RolandCubeAudioProcessor()
{
}

//==============================================================================
const String RolandCubeAudioProcessor::getName() const
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

const String RolandCubeAudioProcessor::getProgramName (int index)
{
    return {};
}

void RolandCubeAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void RolandCubeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    *dcBlocker.state = *dsp::IIR::Coefficients<float>::makeHighPass (sampleRate, 35.0f);

    // prepare resampler for target sample rate: 44.1 kHz
    constexpr double targetSampleRate = 44100.0;
    //resampler.prepareWithTargetSampleRate ({ sampleRate, (uint32) samplesPerBlock, 1 }, targetSampleRate);
    resampler.prepareWithTargetSampleRate({ sampleRate, (uint32)samplesPerBlock, 2 }, targetSampleRate);


    dsp::ProcessSpec specMono { sampleRate, static_cast<uint32> (samplesPerBlock), 1 };
    dsp::ProcessSpec spec{ sampleRate, static_cast<uint32> (samplesPerBlock), 2 };

    dcBlocker.prepare (spec); 

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
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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


void RolandCubeAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    auto driveValue = static_cast<float> (driveParam->load());
    auto masterValue = static_cast<float> (masterParam->load());
    auto bassValue = static_cast<float> (bassParam->load());
    auto midValue = static_cast<float> (midParam->load());
    auto trebleValue = static_cast<float> (trebleParam->load());

    // Setup Audio Data
    const int numSamples = buffer.getNumSamples();
    const int numInputChannels = getTotalNumInputChannels();
    const int sampleRate = getSampleRate();

    dsp::AudioBlock<float> block(buffer);
    dsp::ProcessContextReplacing<float> context(block);

    // Overdrive Pedal ================================================================== 
    if (fw_state == 1 && model_loaded == true) {
        
        if (conditioned == false) {
            // Apply ramped changes for gain smoothing
            if (driveValue == previousDriveValue)
            {
                buffer.applyGain(driveValue*2.5);
            }
             else {
                buffer.applyGainRamp(0, (int) buffer.getNumSamples(), previousDriveValue * 2.5, driveValue * 2.5);
                previousDriveValue = driveValue;
            }
            //auto block44k = resampler.processIn(block);
            //for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            //{
            //    // Apply LSTM model
            //    if (ch == 0) {
            //        LSTM.process(block44k.getReadPointer(0), block44k.getWritePointer(0), (int)block44k.getNumSamples());
            //    }
            //    else if (ch == 1) {
            //        LSTM2.process(block44k.getReadPointer(1), block44k.getWritePointer(1), (int)block44k.getNumSamples());
            //    }
            //}
            //resampler.processOut(block44k, block);
        } else {
            buffer.applyGain(1.5); // Apply default boost to help sound
            // resample to target sample rate
            
            //auto block44k = resampler.processIn(block);
            //for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            //{
            //    // Apply LSTM model
            //    if (ch == 0) {
            //        LSTM.process(block44k.getReadPointer(0), driveValue, block44k.getWritePointer(0), (int)block44k.getNumSamples());
            //    }
            //    else if (ch == 1) {
            //        LSTM2.process(block44k.getReadPointer(1), driveValue, block44k.getWritePointer(1), (int)block44k.getNumSamples());
            //    }
            //}
            //resampler.processOut(block44k, block);
        }

        //dcBlocker.process(context);

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            // Apply EQ
            if (ch == 0) {
                eq4band.process(buffer.getReadPointer(0), buffer.getWritePointer(0), midiMessages, numSamples, numInputChannels, sampleRate);
            
            }
            else if (ch == 1) {
                eq4band2.process(buffer.getReadPointer(1), buffer.getWritePointer(1), midiMessages, numSamples, numInputChannels, sampleRate);
            }
        }

        if (cab_state == 1) {
            cabSimIRa.process(context); // Process IR a on channel 0
            buffer.applyGain(2.0);
        //} else {
        //    buffer.applyGain(0.7);
        }

        // Master Volume 
        // Apply ramped changes for gain smoothing
        if (masterValue == previousMasterValue)
        {
            buffer.applyGain(masterValue);
        }
        else {
            buffer.applyGainRamp(0, (int) buffer.getNumSamples(), previousMasterValue, masterValue);
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
}

//==============================================================================
bool RolandCubeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* RolandCubeAudioProcessor::createEditor()
{
    return new RolandCubeAudioProcessorEditor (*this);
}

//==============================================================================
void RolandCubeAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    auto state = treeState.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    xml->setAttribute ("fw_state", fw_state);
    //xml->setAttribute("folder", folder.getFullPathName().toStdString());
    xml->setAttribute("saved_model", saved_model.getFullPathName().toStdString());
    xml->setAttribute("current_model_index", current_model_index);
    xml->setAttribute ("cab_state", cab_state);
    copyXmlToBinary (*xml, destData);

}

void RolandCubeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (treeState.state.getType()))
        {
            treeState.replaceState (juce::ValueTree::fromXml (*xmlState));
            fw_state = xmlState->getBoolAttribute ("fw_state");
            File temp_saved_model = xmlState->getStringAttribute("saved_model");
            saved_model = temp_saved_model;
            cab_state = xmlState->getBoolAttribute ("cab_state");

            current_model_index = xmlState->getIntAttribute("current_model_index");
            File temp = xmlState->getStringAttribute("folder");
            //folder = temp;
            /*if (auto* editor = dynamic_cast<RolandCubeAudioProcessorEditor*> (getActiveEditor()))
                editor->resetImages();*/

            if (saved_model.existsAsFile()) {
                loadConfig(saved_model);
            }          

        }
    }
}
bool RolandCubeAudioProcessor::isValidFormat(File configFile)
{
    // Read in the JSON file
    String path = configFile.getFullPathName();
    const char* char_filename = path.toUTF8();

    std::ifstream i2(char_filename);
    nlohmann::json weights_json;
    i2 >> weights_json;

    int hidden_size_temp = 0;
    std::string network = "";

    // Check that the hidden_size and unit_type fields exist and are correct
    if (weights_json.contains("/model_data/unit_type"_json_pointer) == true && weights_json.contains("/model_data/hidden_size"_json_pointer) == true) {
        // Get the input size of the JSON file
        int input_size_json = weights_json["/model_data/hidden_size"_json_pointer];
        std::string network_temp = weights_json["/model_data/unit_type"_json_pointer];

        network = network_temp;
        hidden_size_temp = input_size_json;
    }
    else {
        return false;
    }

    if (hidden_size_temp == 40 && network == "LSTM") {
        return true;
    }
    else {
        return false;
    }
}

void RolandCubeAudioProcessor::setJsonFiles()
{
    if (typeParam.get() == false)
    {
        jsonFiles = {
            File(BinaryData::acousticModelGainStable_json),
            File(BinaryData::blackPanelModelGainStable_json),
            File(BinaryData::BritComboModelGainStable_json),
            File(BinaryData::tweedModelGainStable_json),
            File(BinaryData::classicModelGainStable_json),
            File(BinaryData::metalModelGainStable_json),
            File(BinaryData::rFierModelGainStable_json),
            File(BinaryData::extremeModelGainStable_json),
            File(BinaryData::dynamicAmpModelGainStable_json)
        };
    }
    else
    {
        jsonFiles = {
            File(BinaryData::acousticModelParametrizedGain_json),
            File(BinaryData::blackPanelModelParametrizedGain_json),
            File(BinaryData::britComboModelParametrizedGain_json),
            File(BinaryData::tweedModelParametrizedGain_json),
            File(BinaryData::classicModelParametrizedGain_json),
            File(BinaryData::metalModelParametrizedGain_json),
            File(BinaryData::rFierModelParemtrizedGain_json),
            File(BinaryData::extremeModelParametrizedGain_json),
            File(BinaryData::dynamicAmpModelParametrizedGain_json)
        };
    }
}


void RolandCubeAudioProcessor::set_ampEQ(float bass_slider, float mid_slider, float treble_slider)
{
    eq4band.setParameters(bass_slider, mid_slider, treble_slider, 0.0f);
    eq4band2.setParameters(bass_slider, mid_slider, treble_slider, 0.0f);
}

void RolandCubeAudioProcessor::loadConfig(File configFile)
{
    this->suspendProcessing(true);
    pauseVolume = 3;
    String path = configFile.getFullPathName();
    char_filename = path.toUTF8();

    LSTM.reset();
    LSTM2.reset();

    LSTM.load_json(char_filename);
    LSTM2.load_json(char_filename);

    if (LSTM.input_size == 1) {
        conditioned = false;
    } else {
        conditioned = true;
    }

    //saved_model = configFile;
    model_loaded = true;
    this->suspendProcessing(false);
}



//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RolandCubeAudioProcessor();
}
