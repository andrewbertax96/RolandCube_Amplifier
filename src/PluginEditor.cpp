/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RolandCubeAudioProcessorEditor::RolandCubeAudioProcessorEditor (RolandCubeAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to
    knobLookAndFeel.setLookAndFeel(ImageCache::getFromMemory(BinaryData::knobCube_png, BinaryData::knobCube_pngSize));
    knobLead_LookAndFeel.setLookAndFeel(ImageCache::getFromMemory(BinaryData::knobCubeLead_png, BinaryData::knobCubeLead_pngSize));

    //EQ

    //bassSliderAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(treeState, BASS_ID, ampBassKnob));
    bassSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, BASS_ID, ampBassKnob);

    addAndMakeVisible(ampBassKnob);
    ampBassKnob.setLookAndFeel(&knobLookAndFeel);
    ampBassKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampBassKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampBassKnob.setDoubleClickReturnValue(true, 0.0);


    //midSliderAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(treeState, MID_ID, ampMidKnob));
    midSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, MID_ID, ampMidKnob);

    addAndMakeVisible(ampMidKnob);
    ampMidKnob.setLookAndFeel(&knobLookAndFeel);
    ampMidKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampMidKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampMidKnob.setDoubleClickReturnValue(true, 0.0);


    //trebleSliderAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(treeState, TREBLE_ID, ampTrebleKnob));
    trebleSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, TREBLE_ID, ampTrebleKnob);

    addAndMakeVisible(ampTrebleKnob);
    ampTrebleKnob.setLookAndFeel(&knobLookAndFeel);
    ampTrebleKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampTrebleKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampTrebleKnob.setDoubleClickReturnValue(true, 0.0);

    // Overdrive

    //modelSelectorSliderAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(treeState, MODEL_ID, modelSelectorKnob));
    modelSelectorSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, MODEL_ID, modelSelectorKnob);

    addAndMakeVisible(modelSelectorKnob);
    modelSelectorKnob.setLookAndFeel(&knobLead_LookAndFeel);
    modelSelectorKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    modelSelectorKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    modelSelectorKnob.setDoubleClickReturnValue(true, 0.0);

    //gainSliderAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(treeState, GAIN_ID, gainKnob));
    gainSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, GAIN_ID, gainKnob);

    addAndMakeVisible(gainKnob);
    gainKnob.setLookAndFeel(&knobLookAndFeel);
    gainKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    gainKnob.setDoubleClickReturnValue(true, 0.5);

    //volumeSliderAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(treeState, MASTER_ID, volumeKnob));
    volumeSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, MASTER_ID, volumeKnob);

    addAndMakeVisible(volumeKnob);
    volumeKnob.setLookAndFeel(&knobLookAndFeel);
    volumeKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    volumeKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    volumeKnob.setDoubleClickReturnValue(true, 0.5);


    typeSelector.addMouseListener(this, false);//il false dice che non lo estendo ai figli.
    typeSelector.setColour(ToggleButton::ColourIds::textColourId, Colours::red);
    typeSelector.setColour(ToggleButton::ColourIds::tickColourId, Colours::red);
    typeSelector.setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::red);
    //addAndMakeVisible(typeSelector);

    // Size of plugin GUI
    setSize(background.getWidth(), background.getHeight());
    //resetImages();
    loadJsonFiles();
    //loadFromFolder();
}

RolandCubeAudioProcessorEditor::~RolandCubeAudioProcessorEditor()
{
    gainKnob.setLookAndFeel(nullptr);
    volumeKnob.setLookAndFeel(nullptr);
    ampBassKnob.setLookAndFeel(nullptr);
    ampMidKnob.setLookAndFeel(nullptr);
    ampTrebleKnob.setLookAndFeel(nullptr);
    modelSelectorKnob.setLookAndFeel(nullptr);
    typeSelector.setLookAndFeel(nullptr);
}

//==============================================================================
void RolandCubeAudioProcessorEditor::paint (Graphics& g)
{
    // Workaround for graphics on Windows builds (clipping code doesn't work correctly on Windows)
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    g.drawImageAt(background, 0, 0);
    g.drawImageAt(logo_Eq, 80, 20);
    g.drawImageAt(lead, background.getWidth() / 2.0 + 53.5, 10);

    g.setColour(Colours::dimgrey);
    Rectangle<float>backgroundRect(0, 0, getWidth(), getHeight());
    g.drawRoundedRectangle(backgroundRect, 10.0, 4.0);

    g.setColour(Colours::darkgrey);
    Rectangle<float>eqRect(65, 5, logo_Eq.getWidth() + 30, lead.getHeight() + 6.5);
    g.drawRoundedRectangle(eqRect, 15.0, 4.0);

    Rectangle<float>leadRect(background.getWidth() / 2.0 + 46, 5, lead.getWidth() + 15, lead.getHeight() + 7.5);
    g.drawRoundedRectangle(leadRect, 15.0, 4.0);
#else
// Redraw only the clipped part of the background image

    juce::Rectangle<int> ClipRect = g.getClipBounds();
    //if (processor.fw_state == 0) {
    //    g.drawImage(background_off, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());

    g.drawImage(background_on, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());
    g.drawImage(logo_Eq, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());
    g.drawImage(lead, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());
#endif
}

void RolandCubeAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    //Overall Widgets
    auto commonValue = 98.5;
    auto knobWidth = commonValue;
    auto knobHeight = commonValue;

    auto height = 62;
    auto heightLead = height + 11.5;

    modelSelectorKnob.setBounds(769, heightLead - 5.5, knobWidth + 10, knobHeight + 10);
    gainKnob.setBounds(959, heightLead, knobWidth, knobHeight);
    volumeKnob.setBounds(1073, heightLead, knobWidth, knobHeight);

    ampBassKnob.setBounds(275, height, knobWidth, knobHeight);
    ampMidKnob.setBounds(388, height, knobWidth, knobHeight);
    ampTrebleKnob.setBounds(501, height, knobWidth, knobHeight);

    typeSelector.setBounds(background.getWidth() / 2.0, background.getHeight() - 50, 100, 30);
}

void RolandCubeAudioProcessorEditor::loadJsonFiles()
{
    processor.jsonFiles.clear();
    File resourcesDirectory = File::getSpecialLocation(File::currentApplicationFile).getChildFile("resources");

    // Verifica che la directory "resources" esista
    if (!resourcesDirectory.exists())
    {
        DBG("Error: Resources directory not found.");
        return;
    }

    // Carica i file JSON dalla cartella "gainStable"
    File gainStableDirectory = resourcesDirectory.getChildFile("gainStable");
    Array<File> gainStableFiles;
    gainStableDirectory.findChildFiles(gainStableFiles, File::findFiles, false, "*.json");

    // Aggiungi i file JSON trovati all'array jsonFiles
    for (const auto& file : gainStableFiles)
    {
        processor.jsonFiles.push_back(file);
        //processor.jsonFiles.add(file);
    }

    // Carica i file JSON dalla cartella "parametrizedGain"
    File parametrizedGainDirectory = resourcesDirectory.getChildFile("parametrizedGain");
    Array<File> parametrizedGainFiles;
    parametrizedGainDirectory.findChildFiles(parametrizedGainFiles, File::findFiles, false, "*.json");

    // Aggiungi i file JSON trovati all'array jsonFiles
    for (const auto& file : parametrizedGainFiles)
    {
        processor.model_loaded = false;
        processor.jsonFiles.push_back(file);
        //jsonFiles.add(file);
    }
    // Try to load model from saved_model, if it doesnt exist and jsonFiles is not empty, load the first model (if it exists and is valid format)
    if (!processor.jsonFiles.empty()) {
        if (processor.saved_model.existsAsFile() && isValidFormat(processor.saved_model)) {
            processor.loadConfig(processor.saved_model);
            //modelSelect.setText(processor.saved_model.getFileNameWithoutExtension(), juce::NotificationType::dontSendNotification);
        }
        else {
            if (processor.jsonFiles[0].existsAsFile() && isValidFormat(processor.jsonFiles[0])) {
                processor.loadConfig(processor.jsonFiles[0]);
                //modelSelect.setText(processor.jsonFiles[0].getFileNameWithoutExtension(), juce::NotificationType::dontSendNotification);
            }
        }
    }
}

bool RolandCubeAudioProcessorEditor::isValidFormat(File configFile)
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
    } else {
        return false;
    }
    
    if (hidden_size_temp == 40 && network == "LSTM") {
        return true;
    } else {
        return false;
    }
}



void RolandCubeAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    const int selectedFileIndex = modelSelectorKnob.getValue();
    if (selectedFileIndex >= 0 && selectedFileIndex < processor.jsonFiles.size() && processor.jsonFiles.empty() == false) { //check if correct 
        if (processor.jsonFiles[selectedFileIndex].existsAsFile() && isValidFormat(processor.jsonFiles[selectedFileIndex])) {
            processor.loadConfig(processor.jsonFiles[selectedFileIndex]);
            processor.current_model_index = selectedFileIndex;
            processor.saved_model = processor.jsonFiles[selectedFileIndex];
        }
    }
    // Amp
    if (slider == &ampBassKnob || slider == &ampMidKnob || slider == &ampTrebleKnob) {
        processor.set_ampEQ(ampBassKnob.getValue(), ampMidKnob.getValue(), ampTrebleKnob.getValue());
    }
}



//void RolandCubeAudioProcessorEditor::resetImages()
//{
//    repaint();
//    /*
//    if (processor.fw_state == 0) {
//        odFootSw.setImages(true, true, true,
//            ImageCache::getFromMemory(BinaryData::footswitch_up_png, BinaryData::footswitch_up_pngSize), 1.0, Colours::transparentWhite,
//            Image(), 1.0, Colours::transparentWhite,
//            ImageCache::getFromMemory(BinaryData::footswitch_up_png, BinaryData::footswitch_up_pngSize), 1.0, Colours::transparentWhite,
//            0.0);
//    }
//    else {
//        odFootSw.setImages(true, true, true,
//            ImageCache::getFromMemory(BinaryData::footswitch_down_png, BinaryData::footswitch_down_pngSize), 1.0, Colours::transparentWhite,
//            Image(), 1.0, Colours::transparentWhite,
//            ImageCache::getFromMemory(BinaryData::footswitch_down_png, BinaryData::footswitch_down_pngSize), 1.0, Colours::transparentWhite,
//            0.0);
//    }
//    */
//    // Set On/Off cab graphic
//    if (processor.cab_state == 0) {
//        cabOnButton.setImages(true, true, true,
//            ImageCache::getFromMemory(BinaryData::cab_switch_off_png, BinaryData::cab_switch_off_pngSize), 1.0, Colours::transparentWhite,
//            Image(), 1.0, Colours::transparentWhite,
//            ImageCache::getFromMemory(BinaryData::cab_switch_off_png, BinaryData::cab_switch_off_pngSize), 1.0, Colours::transparentWhite,
//            0.0);
//    }
//    else {
//        cabOnButton.setImages(true, true, true,
//            ImageCache::getFromMemory(BinaryData::cab_switch_on_png, BinaryData::cab_switch_on_pngSize), 1.0, Colours::transparentWhite,
//            Image(), 1.0, Colours::transparentWhite,
//            ImageCache::getFromMemory(BinaryData::cab_switch_on_png, BinaryData::cab_switch_on_pngSize), 1.0, Colours::transparentWhite,
//            0.0);
//    }
//}
