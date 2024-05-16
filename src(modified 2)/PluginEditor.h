/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "myLookAndFeel.h"

//==============================================================================
/**
*/
class RolandCubeAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        private Button::Listener,
                                        private Slider::Listener
{
public:
    RolandCubeAudioProcessorEditor (RolandCubeAudioProcessor&);
    ~RolandCubeAudioProcessorEditor() override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    std::unique_ptr<FileChooser> myChooser;

    void loadJsonFiles();
    void loadJson();
    bool isValidFormat(File configFile);

    void loadButtonClicked();

    void loadFromFolder();
    void resetImages();

    //void buttonClicked(juce::Button* button);
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RolandCubeAudioProcessor& audioProcessor;

    TextButton loadButton;
    virtual void buttonClicked(Button* button) override;

    virtual void sliderValueChanged(Slider* slider) override;
    void modelSelectChanged();
    // Global Widgets
    Label modelLabel;
    Label versionLabel;

    ComboBox modelSelect;
    //Inserisci le immagini
    Image background = ImageCache::getFromMemory(BinaryData::backgroundCube_png, BinaryData::backgroundCube_pngSize);
    Image logo_Eq = ImageCache::getFromMemory(BinaryData::logoAndEq_Cube_png, BinaryData::logoAndEq_Cube_pngSize);
    Image lead = ImageCache::getFromMemory(BinaryData::leadChannelCube_png, BinaryData::leadChannelCube_pngSize);

    //Amp Widgets
    Slider ampBassKnob;
    Slider ampMidKnob;
    Slider ampTrebleKnob;
    Slider gainKnob;
    Slider volumeKnob;
    Slider modelSelectorKnob;
    
    ToggleButton typeSelector;
    
    // LookandFeels of the knobs 
    myLookAndFeel knobLookAndFeel;
    myLookAndFeel knobLead_LookAndFeel;

    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> bassSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> midSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> trebleSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> gainSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> volumeSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> modelSelectorSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> typeButtonAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RolandCubeAudioProcessorEditor)
};
