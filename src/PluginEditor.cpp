/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RolandCubeAudioProcessorEditor::RolandCubeAudioProcessorEditor(RolandCubeAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{

    auto font = modelLabel.getFont();
    float height = font.getHeight();
    font.setHeight(height);

    

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to
    knobLookAndFeel.setLookAndFeel(ImageCache::getFromMemory(BinaryData::knobCube_png, BinaryData::knobCube_pngSize));
    knobLead_LookAndFeel.setLookAndFeel(ImageCache::getFromMemory(BinaryData::knobCubeLead_png, BinaryData::knobCubeLead_pngSize));

    //EQ

    bassSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, BASS_ID, ampBassKnob);
    addAndMakeVisible(ampBassKnob);
    ampBassKnob.setLookAndFeel(&knobLookAndFeel);
    ampBassKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampBassKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampBassKnob.setDoubleClickReturnValue(true, 0.0);


    midSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, MID_ID, ampMidKnob);
    addAndMakeVisible(ampMidKnob);
    ampMidKnob.setLookAndFeel(&knobLookAndFeel);
    ampMidKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampMidKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampMidKnob.setDoubleClickReturnValue(true, 0.0);


    trebleSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, TREBLE_ID, ampTrebleKnob);
    addAndMakeVisible(ampTrebleKnob);
    ampTrebleKnob.setLookAndFeel(&knobLookAndFeel);
    ampTrebleKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampTrebleKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampTrebleKnob.setDoubleClickReturnValue(true, 0.0);

    // Overdrive

    modelSelectorSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, MODEL_ID, modelSelectorKnob);
    addAndMakeVisible(modelSelectorKnob);
    modelSelectorKnob.setLookAndFeel(&knobLead_LookAndFeel);
    modelSelectorKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    modelSelectorKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    modelSelectorKnob.setDoubleClickReturnValue(true, 0.0);

    gainSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, GAIN_ID, gainKnob);
    addAndMakeVisible(gainKnob);
    gainKnob.setLookAndFeel(&knobLookAndFeel);
    gainKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    gainKnob.setDoubleClickReturnValue(true, 0.5);

    volumeSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, MASTER_ID, volumeKnob);
    addAndMakeVisible(volumeKnob);
    volumeKnob.setLookAndFeel(&knobLookAndFeel);
    volumeKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    volumeKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    volumeKnob.setDoubleClickReturnValue(true, 0.5);

    typeButtonAttach = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, TYPE_ID, typeSelector);
    typeSelector.addMouseListener(this, false);//il false dice che non lo estendo ai figli.
    typeSelector.setColour(ToggleButton::ColourIds::textColourId, Colours::red);
    typeSelector.setColour(ToggleButton::ColourIds::tickColourId, Colours::red);
    typeSelector.setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::red);
    addAndMakeVisible(typeSelector);

    // Overall Widgets
    addAndMakeVisible(loadButton);
    loadButton.setButtonText("LOAD MODEL");
    loadButton.addListener(this);

    addAndMakeVisible(modelSelect);
    modelSelect.setColour(juce::Label::textColourId, juce::Colours::black);
    modelSelect.setScrollWheelEnabled(true);
    int c = 1;
    for (const auto& jsonFile : audioProcessor.jsonFiles) {
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

    loadFromFolder();
}

RolandCubeAudioProcessorEditor::~RolandCubeAudioProcessorEditor()
{
    gainKnob.setLookAndFeel(nullptr);
    volumeKnob.setLookAndFeel(nullptr);
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

    
    g.drawImageAt(background, 0, 0);  
    g.drawImageAt(logo_Eq, 80, 20);
    g.drawImageAt(lead, background.getWidth()/2.0 + 53.5, 10);
    
    g.setColour(Colours::dimgrey);
    Rectangle<float>backgroundRect(0, 0, getWidth(), getHeight());
    g.drawRoundedRectangle(backgroundRect, 10.0, 4.0); 
    
    g.setColour(Colours::darkgrey);
    Rectangle<float>eqRect(65, 5, logo_Eq.getWidth()+30, lead.getHeight() + 6.5);
    g.drawRoundedRectangle(eqRect, 15.0, 4.0);
    
    Rectangle<float>leadRect(background.getWidth() / 2.0 + 46, 5, lead.getWidth()+15, lead.getHeight()+7.5);
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

    modelSelectorKnob.setBounds(769, heightLead-5.5, knobWidth+10, knobHeight+10);
    gainKnob.setBounds(959, heightLead, knobWidth, knobHeight);
    volumeKnob.setBounds(1073, heightLead, knobWidth, knobHeight);

    ampBassKnob.setBounds(275, height, knobWidth, knobHeight);
    ampMidKnob.setBounds(388, height, knobWidth, knobHeight);
    ampTrebleKnob.setBounds(501, height, knobWidth, knobHeight);

    typeSelector.setBounds(background.getWidth()/2.0, background.getHeight()-50,100,30);
}
void RolandCubeAudioProcessorEditor::loadJsonFiles()
{
    audioProcessor.jsonFiles.clear();

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

    if (gainStableFiles.size() > 0) {
        for (auto file : gainStableFiles) {
            if (isValidFormat(file)) {
                audioProcessor.jsonFiles.push_back(file);
                DBG("File: " << file.getFullPathName());
            }
        }

        //// Try to load model from saved_model, if it doesnt exist and jsonFiles is not empty, load the first model (if it exists and is valid format)
        //if (!audioProcessor.jsonFiles.empty()) {
        //    if (audioProcessor.saved_model.existsAsFile() && isValidFormat(audioProcessor.saved_model)) {
        //        audioProcessor.loadConfig(audioProcessor.saved_model);
        //    }
        //    else {
        //        if (audioProcessor.jsonFiles[0].existsAsFile() && isValidFormat(audioProcessor.jsonFiles[0])) {
        //            audioProcessor.loadConfig(audioProcessor.jsonFiles[0]);
        //        }
        //    }
        //}
    }


    //if (!typeSelector.getToggleState()) {

    //    // Carica i file JSON dalla cartella "gainStable"
    //    File gainStableDirectory = resourcesDirectory.getChildFile("gainStable");
    //    Array<File> gainStableFiles;
    //    gainStableDirectory.findChildFiles(gainStableFiles, File::findFiles, false, "*.json");

    //    // Aggiungi i file JSON trovati all'array jsonFiles
    //    for (const auto& file : gainStableFiles)
    //    {
    //        processor.jsonFiles.push_back(file);
    //        //processor.jsonFiles.add(file);
    //    }
    //}
    //else {

    //    // Carica i file JSON dalla cartella "parametrizedGain"
    //    File parametrizedGainDirectory = resourcesDirectory.getChildFile("parametrizedGain");
    //    Array<File> parametrizedGainFiles;
    //    parametrizedGainDirectory.findChildFiles(parametrizedGainFiles, File::findFiles, false, "*.json");

    //    // Aggiungi i file JSON trovati all'array jsonFiles
    //    for (const auto& file : parametrizedGainFiles)
    //    {
    //        processor.model_loaded = false;
    //        processor.jsonFiles.push_back(file);
    //        //jsonFiles.add(file);
    //    }
    //}

}

void RolandCubeAudioProcessorEditor::loadJson()
{
    // Costruisci il percorso completo del file JSON
    File jsonFile("C:/Users/andre/Desktop/RolandCube_Amplifier/resources/jSonModels/gainStable/classicModelGainStable.json");

    // Controlla se il file JSON esiste
    if (jsonFile.existsAsFile())
    {
        // Legge il contenuto del file JSON
        String jsonContent = jsonFile.loadFileAsString();

        // Stampa il contenuto del file JSON a scopo di debug
        DBG("Contenuto del file JSON: " + jsonContent);

        // Ora puoi fare qualsiasi cosa con il contenuto del file JSON, ad esempio passarlo al processore audio
    }
    else
    {
        DBG("Errore: il file JSON non esiste: " + jsonFile.getFullPathName());
    }
    //File resourcesDirectory = File::getSpecialLocation(File::currentApplicationFile)
    //    .getChildFile("resources")
    //    .getChildFile("jSonModels")
    //    .getChildFile("gainStable");
    //File jsonFile = resourcesDirectory.getChildFile("classicModelGainStable.json");

    //// Controlla se il file JSON esiste
    //if (jsonFile.existsAsFile())
    //{
    //    // Legge il contenuto del file JSON
    //    String jsonContent = jsonFile.loadFileAsString();

    //    // Stampa il contenuto del file JSON a scopo di debug
    //    DBG("Contenuto del file JSON: " + jsonContent);

    //    // Ora puoi fare qualsiasi cosa con il contenuto del file JSON, ad esempio passarlo al processore audio
    //}
    //else
    //{
    //    DBG("Errore: il file JSON non esiste: " + jsonFile.getFullPathName());
    //}
    audioProcessor.saved_model = jsonFile;
    audioProcessor.loadConfig(audioProcessor.saved_model);
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

void RolandCubeAudioProcessorEditor::loadButtonClicked()
{
    myChooser = std::make_unique<FileChooser>("Select a folder to load models from",
        audioProcessor.folder,
        "*.json");

    auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectDirectories | FileBrowserComponent::canSelectFiles;

    myChooser->launchAsync(folderChooserFlags, [this](const FileChooser& chooser)
        {
            if (!chooser.getResult().exists()) {
                return;
            }
            audioProcessor.model_loaded = false;
            Array<File> files;
            if (chooser.getResult().existsAsFile()) { // If a file is selected

                if (isValidFormat(chooser.getResult())) {
                    audioProcessor.saved_model = chooser.getResult();
                }

                files = chooser.getResult().getParentDirectory().findChildFiles(2, false, "*.json");
                audioProcessor.folder = chooser.getResult().getParentDirectory();

            }
            else if (chooser.getResult().isDirectory()) { // Else folder is selected
                files = chooser.getResult().findChildFiles(2, false, "*.json");
                audioProcessor.folder = chooser.getResult();
            }

            audioProcessor.jsonFiles.clear();

            modelSelect.clear();

            if (files.size() > 0) {
                for (auto file : files) {

                    if (isValidFormat(file)) {
                        modelSelect.addItem(file.getFileNameWithoutExtension(), audioProcessor.jsonFiles.size() + 1);
                        audioProcessor.jsonFiles.push_back(file);
                        audioProcessor.num_models += 1;
                    }
                }
                if (chooser.getResult().existsAsFile()) {

                    if (isValidFormat(chooser.getResult()) == true) {
                        modelSelect.setText(audioProcessor.saved_model.getFileNameWithoutExtension());
                        audioProcessor.loadConfig(audioProcessor.saved_model);
                    }
                }
                else {
                    if (!audioProcessor.jsonFiles.empty()) {
                        modelSelect.setSelectedItemIndex(0, juce::NotificationType::dontSendNotification);
                        modelSelectChanged();
                    }
                }
            }
            else {
                audioProcessor.saved_model = ""; // Clear the saved model since there's nothing in the dropdown
            }
        });

}

void RolandCubeAudioProcessorEditor::loadFromFolder()
{
    audioProcessor.model_loaded = false;
    Array<File> files;
    files = audioProcessor.folder.findChildFiles(2, false, "*.json");

    audioProcessor.jsonFiles.clear();
    modelSelect.clear();

    if (files.size() > 0) {
        for (auto file : files) {

            if (isValidFormat(file)) {
                modelSelect.addItem(file.getFileNameWithoutExtension(), audioProcessor.jsonFiles.size() + 1);
                audioProcessor.jsonFiles.push_back(file);
                audioProcessor.num_models += 1;
            }
        }
        // Try to load model from saved_model, if it doesnt exist and jsonFiles is not empty, load the first model (if it exists and is valid format)
        if (!audioProcessor.jsonFiles.empty()) {
            if (audioProcessor.saved_model.existsAsFile() && isValidFormat(audioProcessor.saved_model)) {
                audioProcessor.loadConfig(audioProcessor.saved_model);
                modelSelect.setText(audioProcessor.saved_model.getFileNameWithoutExtension(), juce::NotificationType::dontSendNotification);
            }
            else {
                if (audioProcessor.jsonFiles[0].existsAsFile() && isValidFormat(audioProcessor.jsonFiles[0])) {
                    audioProcessor.loadConfig(audioProcessor.jsonFiles[0]);
                    modelSelect.setText(audioProcessor.jsonFiles[0].getFileNameWithoutExtension(), juce::NotificationType::dontSendNotification);
                }
            }
        }
    }
}

void RolandCubeAudioProcessorEditor::resetImages()
{
    repaint();
}


void RolandCubeAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    //if (button == &odFootSw) {
    //    odFootSwClicked();
    if (button == &loadButton) {
        loadButtonClicked();
    }/*
    else if (button == &cabOnButton) {
        cabOnButtonClicked();
    }*/
}
void RolandCubeAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    // Amp
    if (slider == &ampBassKnob || slider == &ampMidKnob || slider == &ampTrebleKnob) {
        audioProcessor.set_ampEQ(ampBassKnob.getValue(), ampMidKnob.getValue(), ampTrebleKnob.getValue());
    }
}
void RolandCubeAudioProcessorEditor::modelSelectChanged()
{
    const int selectedFileIndex = modelSelect.getSelectedItemIndex();
    if (selectedFileIndex >= 0 && selectedFileIndex < audioProcessor.jsonFiles.size() && audioProcessor.jsonFiles.empty() == false) { //check if correct 
        if (audioProcessor.jsonFiles[selectedFileIndex].existsAsFile() && isValidFormat(audioProcessor.jsonFiles[selectedFileIndex])) {
            audioProcessor.loadConfig(audioProcessor.jsonFiles[selectedFileIndex]);
            audioProcessor.current_model_index = selectedFileIndex;
            audioProcessor.saved_model = audioProcessor.jsonFiles[selectedFileIndex];
        }
    }
    repaint();
}