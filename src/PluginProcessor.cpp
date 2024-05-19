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
                                            std::make_unique<AudioParameterBool>(TYPE_ID, TYPE_NAME, typeParam.get() )
                                        })
#endif
{
    driveParam = treeState.getRawParameterValue(GAIN_ID);
    masterParam = treeState.getRawParameterValue(MASTER_ID);
    bassParam = treeState.getRawParameterValue(BASS_ID);
    midParam = treeState.getRawParameterValue(MID_ID);
    trebleParam = treeState.getRawParameterValue(TREBLE_ID);

    auto bassValue = static_cast<float> (bassParam->load());
    auto midValue = static_cast<float> (midParam->load());
    auto trebleValue = static_cast<float> (trebleParam->load());

    equalizer1.setParameters(bassValue, midValue, trebleValue, 0.0);
    equalizer2.setParameters(bassValue, midValue, trebleValue, 0.0);

    pauseVolume = 3;

    cabSimIRa.load(BinaryData::default_ir_wav, BinaryData::default_ir_wavSize);

   /*treeState.addParameterListener(GAIN_ID, this);
   treeState.addParameterListener(MASTER_ID, this);
   treeState.addParameterListener(BASS_ID, this);
   treeState.addParameterListener(MID_ID, this);
   treeState.addParameterListener(TREBLE_ID, this);
   treeState.addParameterListener(MODEL_ID, this);
   treeState.addParameterListener(TYPE_ID, this);*/

   //MemoryInputStream jsonStream(BinaryData::acousticModelGainStable_json, BinaryData::acousticModelGainStable_jsonSize, false);
   //auto jsonInput = nlohmann::json::parse(jsonStream.readEntireStreamAsString().toStdString());
   //std::string filenameStr = jsonInput;

   //      //Converte la stringa C++ in un oggetto File
   //File saved_model = File(filenameStr);
   //auto jsonInput = nlohmann::json::parse(jsonStream.readEntireStreamAsString().toStdString());
   //neuralNet[0] = RTNeural::json_parser::parseJson<float>(jsonInput);
   //neuralNet[1] = RTNeural::json_parser::parseJson<float>(jsonInput);

   //loadConfig(saved_model);

   /*cabSimIRa.load(BinaryData::default_ir_wav, BinaryData::default_ir_wavSize);
   pauseVolume = 3;*/
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

//void RolandCubeAudioProcessor::parameterChanged(const String& parameterID, float newValue)
//{
//    if (parameterID == MODEL_ID) {
//        modelParam = newValue;
//    }
//    else if(parameterID == TYPE_ID){
//        typeParam = newValue;
//    }
//    else if (parameterID == GAIN_ID) {
//        gainParam = newValue;
//    }
//    else if (parameterID == MASTER_ID) {
//        masterParam = newValue;
//    }
//    else if (parameterID == BASS_ID) {
//        bassParam = newValue;
//    }
//    else if (parameterID == MID_ID) {
//        midParam = newValue;
//    }
//    else if (parameterID == TREBLE_ID) {
//        trebleParam = newValue;
//    }
//
//    
//
//    //const int selectedFileIndex = modelParam.get();
//    //if (selectedFileIndex >= 0 && selectedFileIndex < jsonFiles.size() && jsonFiles.empty() == false) { //check if correct 
//    //    if (jsonFiles[selectedFileIndex].existsAsFile() && isValidFormat(jsonFiles[selectedFileIndex])) {
//    //        loadConfig(jsonFiles[selectedFileIndex]);
//    //        current_model_index = selectedFileIndex;
//    //        saved_model = jsonFiles[selectedFileIndex];
//    //    }
//    //}
//
//    set_ampEQ(bassParam.get(), midParam.get(), trebleParam.get());
//}
//==============================================================================
void RolandCubeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    *dcBlocker.state = *dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 35.0f);

    // prepare resampler for target sample rate: 44.1 kHz
    constexpr double targetSampleRate = 44100.0;
    //resampler.prepareWithTargetSampleRate ({ sampleRate, (uint32) samplesPerBlock, 1 }, targetSampleRate);
    resampler.prepareWithTargetSampleRate({ sampleRate, (uint32)samplesPerBlock, 2 }, targetSampleRate);


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

    /*auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();*/

    auto driveValue = static_cast<float> (driveParam->load());
    auto masterValue = static_cast<float> (masterParam->load());
    auto bassValue = static_cast<float> (bassParam->load());
    auto midValue = static_cast<float> (midParam->load());
    auto trebleValue = static_cast<float> (trebleParam->load());

    // Setup Audio Data
    const int numSamples = buffer.getNumSamples();
    const int numInputChannels = getTotalNumInputChannels();
    const int sampleRate = getSampleRate();
    
    //auto gainValue = gainParam.get();
    //auto masterValue = masterParam.get();

    dsp::AudioBlock<float> block(buffer);
    dsp::ProcessContextReplacing<float> context(block);

    /*for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());*/
    
    if (fw_state == 1 && model_loaded == true) {

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
            auto block44k = resampler.processIn(block);
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                // Apply LSTM model
                if (ch == 0) {
                    LSTM.process(block44k.getChannelPointer(0), block44k.getChannelPointer(0), (int)block44k.getNumSamples());
                }
                else if (ch == 1) {
                    LSTM2.process(block44k.getChannelPointer(1), block44k.getChannelPointer(1), (int)block44k.getNumSamples());
                }
            }
            resampler.processOut(block44k, block);
        }
        else {
            buffer.applyGain(1.5); // Apply default boost to help sound
            // resample to target sample rate

            auto block44k = resampler.processIn(block);
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
            resampler.processOut(block44k, block);
        }

        dcBlocker.process(context);

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            // Apply EQ
            if (ch == 0) {
                equalizer1.process(buffer.getReadPointer(0), buffer.getWritePointer(0), midiMessages, numSamples, numInputChannels, sampleRate);

            }
            else if (ch == 1) {
                equalizer2.process(buffer.getReadPointer(1), buffer.getWritePointer(1), midiMessages, numSamples, numInputChannels, sampleRate);
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
    ////Apply Model & EQ
    //applyLSTM(buffer, block, LSTM, LSTM2, conditioned, gainValue, previousGainValue, resampler);
    //
    //dcBlocker.process(context);
    //applyEQ(buffer, equalizer1, equalizer2, midiMessages, totalNumInputChannels, getSampleRate());

    //cabSimIRa.process(context); // Process IR a on channel 0
    //
    //    
    //// Master Volume 
    //applyGainSmoothing(buffer, masterValue, previousMasterValue); // Apply ramped changes for gain smoothing
    //smoothPopSound(buffer, masterValue, pauseVolume); // Smooth pop sound when changing models
}

//==============================================================================
bool RolandCubeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RolandCubeAudioProcessor::createEditor()
{
    return new RolandCubeAudioProcessorEditor (*this);
}

//==============================================================================
void RolandCubeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    /*MemoryOutputStream stream(destData, false);
    treeState.state.writeToStream(stream);*/
    
    /*auto state = treeState.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    xml->setAttribute("saved_model", saved_model.getFullPathName().toStdString());
    xml->setAttribute("current_model_index", current_model_index);
    copyXmlToBinary (*xml, destData);*/

    auto state = treeState.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    xml->setAttribute("fw_state", fw_state);
    xml->setAttribute("folder", folder.getFullPathName().toStdString());
    xml->setAttribute("saved_model", saved_model.getFullPathName().toStdString());
    xml->setAttribute("current_model_index", current_model_index);
    xml->setAttribute("cab_state", cab_state);
    copyXmlToBinary(*xml, destData);
}

void RolandCubeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    /*ValueTree tree = ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        treeState.state = tree;
    }*/

    /*std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (treeState.state.getType()))
        {
            treeState.replaceState (juce::ValueTree::fromXml (*xmlState));
            File temp_saved_model = xmlState->getStringAttribute("saved_model");
            saved_model = temp_saved_model;

            current_model_index = xmlState->getIntAttribute("current_model_index");
            if (saved_model.existsAsFile()) {
                loadConfig(saved_model);
            }          

        }
    }*/

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(treeState.state.getType()))
        {
            treeState.replaceState(juce::ValueTree::fromXml(*xmlState));
            fw_state = xmlState->getBoolAttribute("fw_state");
            File temp_saved_model = xmlState->getStringAttribute("saved_model");
            saved_model = temp_saved_model;
            cab_state = xmlState->getBoolAttribute("cab_state");

            current_model_index = xmlState->getIntAttribute("current_model_index");
            File temp = xmlState->getStringAttribute("folder");
            folder = temp;
            if (auto* editor = dynamic_cast<RolandCubeAudioProcessorEditor*> (getActiveEditor()))
                editor->resetImages();

            if (saved_model.existsAsFile()) {
                loadConfig(saved_model);
            }

        }
    }
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
    }
    else {
        conditioned = true;
    }

    //saved_model = configFile;
    model_loaded = true;
    this->suspendProcessing(false);
}

//void RolandCubeAudioProcessor::applyLSTM(AudioBuffer<float>& buffer, dsp::AudioBlock<float>& block, RT_LSTM& LSTM, RT_LSTM& LSTM2, bool conditioned, const float gainParam, float& previousGainValue, chowdsp::ResampledProcess<chowdsp::ResamplingTypes::SRCResampler<>>& resampler)
//{
//    //auto block44k = resampler.processIn(block);
//    if (conditioned == false)
//    {
//        // Apply ramped changes for gain smoothing
//        if (gainParam == previousGainValue)
//        {
//            buffer.applyGain(gainParam * 2.5);
//        }
//        else
//        {
//            buffer.applyGainRamp(0, (int)buffer.getNumSamples(), previousGainValue * 2.5, gainParam * 2.5);
//            previousGainValue = gainParam;
//        }
//        
//        auto block44k = resampler.processIn(block);
//        LSTMtoChannels(block44k, LSTM, LSTM2, conditioned, gainParam);
//        resampler.processOut(block44k, block);
//    }
//    else
//    {
//        buffer.applyGain(1.5); // Apply default boost to help sound
//        auto block44k = resampler.processIn(block);
//        LSTMtoChannels(block44k, LSTM, LSTM2, conditioned, gainParam);
//        resampler.processOut(block44k, block);
//    }
//    //resampler.processOut(block44k, block);
//}
//
//void RolandCubeAudioProcessor::LSTMtoChannels(juce::dsp::AudioBlock<float>& block, RT_LSTM& LSTM, RT_LSTM& LSTM2, bool conditioned, float gainValue)
//{
//    //LSTM.process(block44k.getChannelPointer(0), block44k.getChannelPointer(0), (int)block44k.getNumSamples());
//    //block44k.getChannelPointer(0)
//   
//
//    //auto block44k = resampler.processIn(block);
//
//    auto block44k_ChannelPointer_0 = block.getChannelPointer(0);
//    auto block44k_ChannelPointer_1 = block.getChannelPointer(1);
//
//    if (conditioned == false) {
//
//        LSTM.process(block44k_ChannelPointer_0, block44k_ChannelPointer_0, block.getNumSamples());
//        LSTM2.process(block44k_ChannelPointer_1, block44k_ChannelPointer_1, block.getNumSamples());
//    }
//    else {
//
//        LSTM.process(block44k_ChannelPointer_0, gainValue, block44k_ChannelPointer_0, block.getNumSamples());
//        LSTM2.process(block44k_ChannelPointer_1, gainValue, block44k_ChannelPointer_1, block.getNumSamples());
//    }
//}


void RolandCubeAudioProcessor::set_ampEQ(float bass_slider, float mid_slider, float treble_slider)
{
    equalizer1.setParameters(bass_slider, mid_slider, treble_slider, 0.0f);
    equalizer2.setParameters(bass_slider, mid_slider, treble_slider, 0.0f);
}

//void RolandCubeAudioProcessor::applyEQ(AudioBuffer<float>& buffer, Equalizer& equalizer1, Equalizer& equalizer2, MidiBuffer& midiMessages, int totalNumInputChannels, double sampleRate) 
//{
//
//    const float* bufferReader_R = buffer.getReadPointer(0);
//    const float* bufferReader_L = buffer.getReadPointer(1);
//    const int numSamples = buffer.getNumSamples();
//    float* bufferWriter_R = buffer.getWritePointer(0);
//    float* bufferWriter_L = buffer.getWritePointer(1);
//
//    equalizer1.process(bufferReader_R, bufferWriter_R, midiMessages, numSamples, totalNumInputChannels, sampleRate);
//    equalizer2.process(bufferReader_L, bufferWriter_L, midiMessages, numSamples, totalNumInputChannels, sampleRate);
//}
//
//void RolandCubeAudioProcessor::applyGainSmoothing(AudioBuffer<float>& buffer, const float masterParam, float& previousMasterValue)
//{
//    buffer.applyGain(4.0);
//    if (masterParam == previousMasterValue)
//    {
//        buffer.applyGain(masterParam);
//    }
//    else
//    {
//        buffer.applyGainRamp(0, (int)buffer.getNumSamples(), previousMasterValue, masterParam);
//        previousMasterValue = masterParam;
//    }
//}
//
//void RolandCubeAudioProcessor::smoothPopSound(AudioBuffer<float>& buffer, const float masterParam, int& pauseVolume)
//{
//    if (pauseVolume > 0) {
//        if (pauseVolume > 2)
//            buffer.applyGain(0.0);
//        else if (pauseVolume == 2)
//            buffer.applyGainRamp(0, (int)buffer.getNumSamples(), 0, masterParam / 2);
//        else
//            buffer.applyGainRamp(0, (int)buffer.getNumSamples(), masterParam / 2, masterParam);
//
//        pauseVolume -= 1;
//    }
//}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RolandCubeAudioProcessor();
}