/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RolandCubeAudioProcessorEditor::RolandCubeAudioProcessorEditor (RolandCubeAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), treeState(vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    // Overall Widgets

    auto font = modelLabel.getFont();
    float height = font.getHeight();
    font.setHeight(height);

    // Set Widget Graphics
    knob.setLookAndFeel(ImageCache::getFromMemory(BinaryData::knob_Cube_png, BinaryData::knob_Cube_pngSize));
    
    // Overdrive

    driveSliderAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(treeState, GAIN_ID, odDriveKnob));
    addAndMakeVisible(odDriveKnob);
    odDriveKnob.setLookAndFeel(&knob);
    odDriveKnob.addListener(this);
    odDriveKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    odDriveKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    odDriveKnob.setDoubleClickReturnValue(true, 0.5);

    masterSliderAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(treeState, MASTER_ID, odLevelKnob));
    addAndMakeVisible(odLevelKnob);
    odLevelKnob.setLookAndFeel(&knob);
    odLevelKnob.addListener(this);
    odLevelKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    odLevelKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    odLevelKnob.setDoubleClickReturnValue(true, 0.5);


    bassSliderAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(treeState, BASS_ID, ampBassKnob));
    addAndMakeVisible(ampBassKnob);
    ampBassKnob.setLookAndFeel(&knob);
    ampBassKnob.addListener(this);
    ampBassKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampBassKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampBassKnob.setDoubleClickReturnValue(true, 0.0);


    midSliderAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(treeState, MID_ID, ampMidKnob));
    addAndMakeVisible(ampMidKnob);
    ampMidKnob.setLookAndFeel(&knob);
    ampMidKnob.addListener(this);
    ampMidKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampMidKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampMidKnob.setDoubleClickReturnValue(true, 0.0);


    trebleSliderAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(treeState, TREBLE_ID, ampTrebleKnob));
    addAndMakeVisible(ampTrebleKnob);
    ampTrebleKnob.setLookAndFeel(&knob);
    ampTrebleKnob.addListener(this);
    ampTrebleKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampTrebleKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampTrebleKnob.setDoubleClickReturnValue(true, 0.0);

    modelSelectorSliderAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(treeState, MODEL_ID, modelSelectorKnob));
    addAndMakeVisible(modelSelectorKnob);
    modelSelectorKnob.setLookAndFeel(&knob);
    modelSelectorKnob.addListener(this);
    modelSelectorKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    modelSelectorKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    modelSelectorKnob.setDoubleClickReturnValue(true, 0.0);

    addAndMakeVisible(versionLabel);
    versionLabel.setText("v1.2", juce::NotificationType::dontSendNotification);
    versionLabel.setJustificationType(juce::Justification::left);
    versionLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    versionLabel.setFont(font);

    // Size of plugin GUI
    setSize(500, 650);

    resetImages();
}

RolandCubeAudioProcessorEditor::~RolandCubeAudioProcessorEditor()
{
    odDriveKnob.setLookAndFeel(nullptr);
    odLevelKnob.setLookAndFeel(nullptr);
    ampBassKnob.setLookAndFeel(nullptr);
    ampMidKnob.setLookAndFeel(nullptr);
    ampTrebleKnob.setLookAndFeel(nullptr);
    modelSelectorKnob.setLookAndFeel(nullptr);
}

//==============================================================================
void RolandCubeAudioProcessorEditor::paint (juce::Graphics& g)
{
    
    // Workaround for graphics on Windows builds (clipping code doesn't work correctly on Windows)
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    //if (processor.fw_state == 0) {
    //    g.drawImageAt(background_off, 0, 0);  // Debug Line: Redraw entire background image
    g.drawImageAt(background, 0, 0);  // Debug Line: Redraw entire background image
    g.drawImageAt(logo_Eq, 0, 0);  // Debug Line: Redraw entire background image
    g.drawImageAt(lead, 0, 0);  // Debug Line: Redraw entire background image
   
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

    // Overdrive Widgets
    odDriveKnob.setBounds(168, 242, 190, 190);
    odLevelKnob.setBounds(340, 225, 62, 62);

    ampBassKnob.setBounds(113, 131, 62, 62);
    ampMidKnob.setBounds(227, 131, 62, 62);
    ampTrebleKnob.setBounds(340, 131, 62, 62);
    modelSelectorKnob.setBounds(227, 400, 62, 62);
}
void RolandCubeAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &ampBassKnob || slider == &ampMidKnob || slider == &ampTrebleKnob) {
       audioProcessor.set_ampEQ(ampBassKnob.getValue(), ampMidKnob.getValue(), ampTrebleKnob.getValue());
    }
    if (slider == &modelSelectorKnob) {
        const int selectedFileIndex = modelSelectorKnob.getValue();
        if (selectedFileIndex >= 0 && selectedFileIndex < audioProcessor.jsonFiles.size() && audioProcessor.jsonFiles.empty() == false) { //check if correct 
            if (audioProcessor.jsonFiles[selectedFileIndex].existsAsFile() && isValidFormat(audioProcessor.jsonFiles[selectedFileIndex])) {
                audioProcessor.loadConfig(audioProcessor.jsonFiles[selectedFileIndex]);
                audioProcessor.current_model_index = selectedFileIndex;
                audioProcessor.saved_model = audioProcessor.jsonFiles[selectedFileIndex];
            }
        }
    }
    repaint();
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
void RolandCubeAudioProcessorEditor::resetImages()
{
    repaint();
}
