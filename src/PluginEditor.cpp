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
    loadJsonFiles();
    orderJsonFiles(audioProcessor.jsonFiles);
    //modelSelect();
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

void RolandCubeAudioProcessorEditor::loadJsonFiles() {
    // Verifica se typeParam è valido
    bool isGainStable = typeParam.get() == false;

    // Ottieni il percorso dell'eseguibile corrente
    File executableFile = File::getSpecialLocation(File::currentExecutableFile);

    // Stampa il percorso dell'eseguibile corrente per il debug
    DBG("Percorso dell'eseguibile corrente: " + executableFile.getFullPathName());

    // Risali fino alla cartella principale del progetto `RolandCube_Amplifier`
    File projectRoot = executableFile;
    while (!projectRoot.isRoot() && !projectRoot.getFileName().equalsIgnoreCase("RolandCube_Amplifier")) {
        projectRoot = projectRoot.getParentDirectory();
    }

    // Stampa il percorso della cartella principale del progetto per il debug
    DBG("Percorso della cartella principale del progetto: " + projectRoot.getFullPathName());

    // Definisci la directory in base al valore di typeParam
    File directory = isGainStable ? projectRoot.getChildFile("resources/jSonModels/gainStable")
        : projectRoot.getChildFile("resources/jSonModels/parametrizedGain");

    // Controlla se la directory esiste
    if (directory.isDirectory()) {
        // Ottieni un array di file nella directory
        Array<File> files = directory.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.json");

        // Aggiungi i file JSON al vettore
        for (const auto& file : files) {
            audioProcessor.jsonFiles.push_back(file);
        }

        // Stampa i nomi dei file JSON caricati a scopo di debug
        for (const auto& file : audioProcessor.jsonFiles) {
            DBG("File JSON trovato: " + file.getFullPathName());
        }

        // Se sono stati trovati file JSON, carica il primo per dimostrazione
        //if (!audioProcessor.jsonFiles.empty()) {
        //    String jsonContent = audioProcessor.jsonFiles[0].loadFileAsString();

        //    // Verifica se il caricamento del file JSON ha avuto successo
        //    if (!jsonContent.isEmpty()) {
        //        DBG("Contenuto del primo file JSON: " + jsonContent);

        //        audioProcessor.saved_model = audioProcessor.jsonFiles[0];
        //        audioProcessor.loadConfig(audioProcessor.saved_model);
        //    }
        //    else {
        //        DBG("Errore: il caricamento del contenuto del primo file JSON ha fallito.");
        //    }
        //}
        //else {
        //    DBG("Errore: Nessun file JSON trovato.");
        //}
    }
    else {
        DBG("Errore: la directory non esiste: " + directory.getFullPathName());
    }
}

void RolandCubeAudioProcessorEditor::orderJsonFiles(std::vector<File>& jsonFiles) {
    // Crea un vettore temporaneo per memorizzare i file ordinati
    std::vector<File> orderedFiles(jsonFiles.size());

    // Definisci l'ordine delle caratteristiche
    std::vector<String> model = { "acoustic", "black panel", "british", "tweed", "classic", "metal", "rfier", "extreme", "dynamic" };

    // Itera su ogni file JSON
    for (const auto& file : jsonFiles) {
        // Ottieni il nome del file senza estensione
        String fileName = file.getFileNameWithoutExtension();

        // Itera su ogni caratteristica per trovare la corrispondenza
        for (size_t i = 0; i < model.size(); ++i) {
            // Se il nome del file contiene la caratteristica, inseriscilo nella posizione corrispondente
            if (fileName.containsIgnoreCase(model[i])) {
                orderedFiles[i] = file;
                break;
            }
        }
    }

    // Sostituisci il vettore originale con il vettore ordinato
    jsonFiles = orderedFiles;
}

void RolandCubeAudioProcessorEditor::modelSelect()
{
    // Verifica se il valore selezionato è valido
    int selectedFileIndex = audioProcessor.modelParam.get();
    if (selectedFileIndex >= 0 && selectedFileIndex < audioProcessor.jsonFiles.size()) {
        // Carica il file JSON corrispondente
        if (audioProcessor.jsonFiles[selectedFileIndex].existsAsFile() && isValidFormat(audioProcessor.jsonFiles[selectedFileIndex])) {
            audioProcessor.loadConfig(audioProcessor.jsonFiles[selectedFileIndex]);
            audioProcessor.current_model_index = selectedFileIndex;
            audioProcessor.saved_model = audioProcessor.jsonFiles[selectedFileIndex];
        }
        else {
            DBG("Errore: Il file JSON selezionato non esiste o non è nel formato corretto.");
        }
    }
    else {
        DBG("Errore: Indice di modello non valido.");
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
