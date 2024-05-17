/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RolandCubeAudioProcessorEditor::RolandCubeAudioProcessorEditor(RolandCubeAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to

    
    auto font = modelLabel.getFont();
    float height = font.getHeight();
    font.setHeight(height);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to
    knobLookAndFeel.setLookAndFeel(ImageCache::getFromMemory(BinaryData::knobCube_png, BinaryData::knobCube_pngSize));
    knobLead_LookAndFeel.setLookAndFeel(ImageCache::getFromMemory(BinaryData::knobCubeLead_png, BinaryData::knobCubeLead_pngSize));

    //EQ

    bassSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, BASS_ID, ampBassKnob);
    addAndMakeVisible(ampBassKnob);
    ampBassKnob.setLookAndFeel(&knobLookAndFeel);
    ampBassKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampBassKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampBassKnob.setDoubleClickReturnValue(true, 0.0);


    midSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, MID_ID, ampMidKnob);
    addAndMakeVisible(ampMidKnob);
    ampMidKnob.setLookAndFeel(&knobLookAndFeel);
    ampMidKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampMidKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampMidKnob.setDoubleClickReturnValue(true, 0.0);


    trebleSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, TREBLE_ID, ampTrebleKnob);
    addAndMakeVisible(ampTrebleKnob);
    ampTrebleKnob.setLookAndFeel(&knobLookAndFeel);
    ampTrebleKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampTrebleKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampTrebleKnob.setDoubleClickReturnValue(true, 0.0);

    // Overdrive

    modelSelectorSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, MODEL_ID, modelSelectorKnob);
    addAndMakeVisible(modelSelectorKnob);
    modelSelectorKnob.setLookAndFeel(&knobLead_LookAndFeel);
    modelSelectorKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    modelSelectorKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    modelSelectorKnob.setDoubleClickReturnValue(true, 0.0);

    driveSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, GAIN_ID, odDriveKnob);
    addAndMakeVisible(odDriveKnob);
    odDriveKnob.setLookAndFeel(&knobLookAndFeel);
    odDriveKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    odDriveKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    odDriveKnob.setDoubleClickReturnValue(true, 0.5);

    masterSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, MASTER_ID, odLevelKnob);
    addAndMakeVisible(odLevelKnob);
    odLevelKnob.setLookAndFeel(&knobLookAndFeel);
    odLevelKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    odLevelKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    odLevelKnob.setDoubleClickReturnValue(true, 0.5);

    typeButtonAttach = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(processor.treeState, TYPE_ID, typeSelector);
    typeSelector.addMouseListener(this, false);//il false dice che non lo estendo ai figli.
    typeSelector.setColour(ToggleButton::ColourIds::textColourId, Colours::red);
    typeSelector.setColour(ToggleButton::ColourIds::tickColourId, Colours::red);
    typeSelector.setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::red);
    //addAndMakeVisible(typeSelector);

    // Overall Widgets
    addAndMakeVisible(loadButton);
    loadButton.setButtonText("LOAD MODEL");
    loadButton.addListener(this);

    addAndMakeVisible(modelSelect);
    modelSelect.setColour(juce::Label::textColourId, juce::Colours::black);
    modelSelect.setScrollWheelEnabled(true);
    int c = 1;
    for (const auto& jsonFile : processor.jsonFiles) {
        modelSelect.addItem(jsonFile.getFileName(), c);
        c += 1;
    }
    modelSelect.onChange = [this] {modelSelectChanged(); };

    addAndMakeVisible(versionLabel);
    versionLabel.setText("v1.2", juce::NotificationType::dontSendNotification);
    versionLabel.setJustificationType(juce::Justification::left);
    versionLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    versionLabel.setFont(font);

    // Size of plugin GUI
    setSize(background.getWidth(), background.getHeight());
    
    //loadJsonFiles();
    //loadJson();


    resetImages();

    loadFromFolder();
}

RolandCubeAudioProcessorEditor::~RolandCubeAudioProcessorEditor()
{
    odDriveKnob.setLookAndFeel(nullptr);
    odLevelKnob.setLookAndFeel(nullptr);
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

    g.drawImage(background, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());
    g.drawImage(logo_Eq, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());
    g.drawImage(lead, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());

#endif
}

void RolandCubeAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..


    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    // Overdrive Widgets
    auto commonValue = 98.5;
    auto knobWidth = commonValue;
    auto knobHeight = commonValue;

    auto height = 62;
    auto heightLead = height + 11.5;

    //Overall Widgets
    loadButton.setBounds(186, 48, 120, 24);
    modelSelect.setBounds(52, 11, 400, 28);
    //modelLabel.setBounds(197, 2, 90, 25);
    versionLabel.setBounds(462, 632, 60, 10);

    modelSelectorKnob.setBounds(769, heightLead - 5.5, knobWidth + 10, knobHeight + 10);
    odDriveKnob.setBounds(959, heightLead, knobWidth, knobHeight);
    odLevelKnob.setBounds(1073, heightLead, knobWidth, knobHeight);

    ampBassKnob.setBounds(275, height, knobWidth, knobHeight);
    ampMidKnob.setBounds(388, height, knobWidth, knobHeight);
    ampTrebleKnob.setBounds(501, height, knobWidth, knobHeight);

    typeSelector.setBounds(background.getWidth() / 2.0, background.getHeight() - 50, 100, 30);
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

void RolandCubeAudioProcessorEditor::loadButtonClicked()
{ 
    myChooser = std::make_unique<FileChooser> ("Select a folder to load models from",
                                               processor.folder,
                                               "*.json");
 
    auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectDirectories | FileBrowserComponent::canSelectFiles;
 
    myChooser->launchAsync (folderChooserFlags, [this] (const FileChooser& chooser)                
    {
        if (!chooser.getResult().exists()) {
                return;
        }
        processor.model_loaded = false;
        Array<File> files;
        if (chooser.getResult().existsAsFile()) { // If a file is selected

            if (isValidFormat(chooser.getResult())) {
                processor.saved_model = chooser.getResult();
            }

            files = chooser.getResult().getParentDirectory().findChildFiles(2, false, "*.json");
            processor.folder = chooser.getResult().getParentDirectory();

        } else if (chooser.getResult().isDirectory()){ // Else folder is selected
            files = chooser.getResult().findChildFiles(2, false, "*.json");
            processor.folder = chooser.getResult();
        }
        
        processor.jsonFiles.clear();

        modelSelect.clear();

        if (files.size() > 0) {
            for (auto file : files) {

                if (isValidFormat(file)) {
                    modelSelect.addItem(file.getFileNameWithoutExtension(), processor.jsonFiles.size() + 1);
                    processor.jsonFiles.push_back(file);
                    processor.num_models += 1;
                }
            }
            if (chooser.getResult().existsAsFile()) {
                
                if (isValidFormat(chooser.getResult()) == true) {
                    modelSelect.setText(processor.saved_model.getFileNameWithoutExtension());
                    processor.loadConfig(processor.saved_model);
                }
            }
            else {
                if (!processor.jsonFiles.empty()) {
                    modelSelect.setSelectedItemIndex(0, juce::NotificationType::dontSendNotification);
                    modelSelectChanged();
                }
            }
        } else {
            processor.saved_model = ""; // Clear the saved model since there's nothing in the dropdown
        }
    });
    
}

void RolandCubeAudioProcessorEditor::loadFromFolder()
{
    processor.model_loaded = false;
    Array<File> files;
    files = processor.folder.findChildFiles(2, false, "*.json");

    processor.jsonFiles.clear();
    modelSelect.clear();

    if (files.size() > 0) {
        for (auto file : files) {
            
            if (isValidFormat(file)) {
                modelSelect.addItem(file.getFileNameWithoutExtension(), processor.jsonFiles.size() + 1);
                processor.jsonFiles.push_back(file);
                processor.num_models += 1;
            }
        }
        // Try to load model from saved_model, if it doesnt exist and jsonFiles is not empty, load the first model (if it exists and is valid format)
        if (!processor.jsonFiles.empty()) {
            if (processor.saved_model.existsAsFile() && isValidFormat(processor.saved_model)) {
                processor.loadConfig(processor.saved_model);
                modelSelect.setText(processor.saved_model.getFileNameWithoutExtension(), juce::NotificationType::dontSendNotification);
            } else {
                if (processor.jsonFiles[0].existsAsFile() && isValidFormat(processor.jsonFiles[0])) {
                    processor.loadConfig(processor.jsonFiles[0]);
                    modelSelect.setText(processor.jsonFiles[0].getFileNameWithoutExtension(), juce::NotificationType::dontSendNotification);
                }
            }
        }
    }
}


void RolandCubeAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    //if (button == &odFootSw) {
    //    odFootSwClicked();
    if (button == &loadButton) {
        loadButtonClicked();
    }/* else if (button == &cabOnButton) {
        cabOnButtonClicked();
    }*/
}

void RolandCubeAudioProcessorEditor::odFootSwClicked() {
    //if (processor.fw_state == 0)
    //    processor.fw_state = 1;
    //else
    //    processor.fw_state = 0;
    //resetImages();
}

void RolandCubeAudioProcessorEditor::cabOnButtonClicked() {
    if (processor.cab_state == 0) {
        processor.cab_state = 1;
    }
    else {
        processor.cab_state = 0;
    }
    resetImages();
    repaint();
}

void RolandCubeAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    // Amp
    if (slider == &ampBassKnob || slider == &ampMidKnob || slider == &ampTrebleKnob) {
        processor.set_ampEQ(ampBassKnob.getValue(), ampMidKnob.getValue(), ampTrebleKnob.getValue());
    }
}

void RolandCubeAudioProcessorEditor::modelSelectChanged()
{
    const int selectedFileIndex = modelSelect.getSelectedItemIndex();
    if (selectedFileIndex >= 0 && selectedFileIndex < processor.jsonFiles.size() && processor.jsonFiles.empty() == false) { //check if correct 
        if (processor.jsonFiles[selectedFileIndex].existsAsFile() && isValidFormat(processor.jsonFiles[selectedFileIndex])) {
            processor.loadConfig(processor.jsonFiles[selectedFileIndex]);
            processor.current_model_index = selectedFileIndex;
            processor.saved_model = processor.jsonFiles[selectedFileIndex];
        }
    }
    repaint();
}


void RolandCubeAudioProcessorEditor::resetImages()
{
    repaint();
    /*
    if (processor.fw_state == 0) {
        odFootSw.setImages(true, true, true,
            ImageCache::getFromMemory(BinaryData::footswitch_up_png, BinaryData::footswitch_up_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::footswitch_up_png, BinaryData::footswitch_up_pngSize), 1.0, Colours::transparentWhite,
            0.0);
    }
    else {
        odFootSw.setImages(true, true, true,
            ImageCache::getFromMemory(BinaryData::footswitch_down_png, BinaryData::footswitch_down_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::footswitch_down_png, BinaryData::footswitch_down_pngSize), 1.0, Colours::transparentWhite,
            0.0);
    }
    */
}
