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
    roland_logo = roland_logo.rescaled(roland_logo.getWidth() - 150, roland_logo.getHeight() - 35);
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

    gaintypeSelectorButtonAttach = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, TYPE_ID, gainType_Button);
	gainType_Button.addMouseListener(this, false);//il false dice che non lo estendo ai figli.
    gainType_Button.setColour(ToggleButton::ColourIds::textColourId, Colours::red);
    gainType_Button.setColour(ToggleButton::ColourIds::tickColourId, Colours::red);
    gainType_Button.setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::red);
	addAndMakeVisible(gainType_Button);

    // Size of plugin GUI
    setSize(background.getWidth(), background.getHeight());
    loadJsonFiles();
    startTimer(400);
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
    g.setOpacity(0.9);
    g.drawImageAt(roland_logo, getWidth() / 2.0 - (roland_logo.getWidth()/2.0) + 8, getHeight() - roland_logo.getHeight()-4);
    g.setOpacity(1.0);
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
    g.drawImage(background, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());
    g.drawImage(roland_logo, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());
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
    gainType_Button.setBounds(background.getWidth()/2.0, background.getHeight()/2.0, 100, 30);
    gainKnob.setBounds(959, heightLead, knobWidth, knobHeight);
    volumeKnob.setBounds(1073, heightLead, knobWidth, knobHeight);

    ampBassKnob.setBounds(275, height, knobWidth, knobHeight);
    ampMidKnob.setBounds(388, height, knobWidth, knobHeight);
    ampTrebleKnob.setBounds(501, height, knobWidth, knobHeight);
}

void RolandCubeAudioProcessorEditor::loadJsonFiles() {
    // Ottieni il percorso dell'eseguibile corrente
    File executableFile = File::getSpecialLocation(File::currentExecutableFile);

    // Risali fino alla cartella principale del progetto `RolandCube_Amplifier`
    File projectRoot = executableFile;
    while (!projectRoot.isRoot() && !projectRoot.getFileName().equalsIgnoreCase("RolandCube_Amplifier")) {
        projectRoot = projectRoot.getParentDirectory();
    }

    // Definisci le directory per i file JSON
    StringArray jsonDirectories = { "gainStable", "parametrizedGain" };

    // Carica i file JSON da entrambe le directory
    for (const auto& directory : jsonDirectories) {
        File jsonDirectory = projectRoot.getChildFile("train/models/" + directory);

        // Controlla se la directory esiste
        if (jsonDirectory.isDirectory()) {
            // Ottieni un array di file nella directory
            Array<File> jsonFiles = jsonDirectory.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.json");

            // Aggiungi i file JSON al vettore corrispondente
            for (const auto& file : jsonFiles) {
                if (directory.equalsIgnoreCase("gainStable"))
                    audioProcessor.jsonFilesGainStable.push_back(file);
                else if (directory.equalsIgnoreCase("parametrizedGain"))
                    audioProcessor.jsonFilesParametrizedGain.push_back(file);
            }

            // Stampa i nomi dei file JSON caricati a scopo di debug
            for (const auto& file : jsonFiles) {
                DBG("File JSON trovato (" + directory + "): " + file.getFullPathName());
            }
        }
        else {
            DBG("Errore: la directory " + directory + " non esiste: " + jsonDirectory.getFullPathName());
        }
    }
    orderJsonFiles(audioProcessor.jsonFilesGainStable);
    orderJsonFiles(audioProcessor.jsonFilesParametrizedGain);
    audioProcessor.initializeModelTypeAndLoadModel();
}

void RolandCubeAudioProcessorEditor::orderJsonFiles(std::vector<File>& jsonFiles) {
    // Crea un vettore temporaneo per memorizzare i file ordinati
    std::vector<File> orderedFiles(jsonFiles.size());

    // Definisci l'ordine delle caratteristiche
    std::vector<String> model = { "acoustic", "blackPanel", "britishCombo", "tweed", "classic", "metal", "rFier", "extreme", "dynamicAmp" };

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

void RolandCubeAudioProcessorEditor::timerCallback()
{
    stopTimer();
}
