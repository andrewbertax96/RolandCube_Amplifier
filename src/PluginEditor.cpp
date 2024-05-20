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

    // Size of plugin GUI
    setSize(background.getWidth(), background.getHeight());
    //loadJsonFiles();
    loadJson();
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

    modelSelectorKnob.setBounds(769, heightLead-5.5, knobWidth+10, knobHeight+10);
    gainKnob.setBounds(959, heightLead, knobWidth, knobHeight);
    volumeKnob.setBounds(1073, heightLead, knobWidth, knobHeight);

    ampBassKnob.setBounds(275, height, knobWidth, knobHeight);
    ampMidKnob.setBounds(388, height, knobWidth, knobHeight);
    ampTrebleKnob.setBounds(501, height, knobWidth, knobHeight);
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

File findProjectRoot(File currentDir, const String& projectName) {
    while (!currentDir.isRoot() && !currentDir.getFileName().equalsIgnoreCase(projectName)) {
        currentDir = currentDir.getParentDirectory();
    }
    return currentDir;
}

void RolandCubeAudioProcessorEditor::loadJson()
{
    //jsonFile = "C:/Users/andre/Desktop/RolandCube_Amplifier/resources/jSonModels/gainStable/acousticModelGainStable.json";

    File jsonFile;

    // Ottieni il percorso dell'eseguibile corrente
    File executableFile = File::getSpecialLocation(File::currentExecutableFile);

    // Stampa il percorso dell'eseguibile corrente per il debug
    DBG("Percorso dell'eseguibile corrente: " + executableFile.getFullPathName());

  
    // Risali fino alla cartella principale del progetto `RolandCube_Amplifier`
    File projectRoot = findProjectRoot(executableFile, "RolandCube_Amplifier");

    // Stampa il percorso della cartella principale del progetto per il debug
    DBG("Percorso della cartella principale del progetto: " + projectRoot.getFullPathName());

    // Costruisci il percorso relativo del file JSON basato sulla cartella principale del progetto
    if (typeParam.get() == false) {
        jsonFile = projectRoot.getChildFile("resources/jSonModels/gainStable/acousticModelGainStable.json");
    }
    else {
        jsonFile = projectRoot.getChildFile("resources/jSonModels/parametrizedGain/acousticModelParametrizedGain.json");
    }

    // Controlla se il file JSON esiste
    if (jsonFile.existsAsFile()) {
        // Legge il contenuto del file JSON
        String jsonContent = jsonFile.loadFileAsString();

        // Stampa il contenuto del file JSON a scopo di debug
        DBG("Contenuto del file JSON: " + jsonContent);

        // Ora puoi fare qualsiasi cosa con il contenuto del file JSON, ad esempio passarlo al processore audio
    }
    else {
        DBG("Errore: il file JSON non esiste: " + jsonFile.getFullPathName());
    }

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

void RolandCubeAudioProcessorEditor::resetImages()
{
    repaint();
}
