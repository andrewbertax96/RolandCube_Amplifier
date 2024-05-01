/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class RolandCubeAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        private Button::Listener,
                                        private Slider::Listener
{
public:
    RolandCubeAudioProcessorEditor (RolandCubeAudioProcessor&, AudioProcessorValueTreeState&);
    ~RolandCubeAudioProcessorEditor() override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void resetImages();
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RolandCubeAudioProcessor& audioProcessor;
    
    //Inserisci le immagini
    Image background = ImageCache::getFromMemory(BinaryData::backgroungCube_png, BinaryData::backgroungCube_pngSize);
    Image logo_Eq = ImageCache::getFromMemory(BinaryData::logoAndEQ_Cube_png, BinaryData::logoAndEQ_Cube_pngSize);
    Image lead = ImageCache::getFromMemory(BinaryData::leadChannelCube_png, BinaryData::leadChannelCube_pngSize);

    // Global Widgets
    Label modelLabel;
    Label versionLabel;

    //Amp Widgets
    Slider ampBassKnob;
    Slider ampMidKnob;
    Slider ampTrebleKnob;
    Slider odDriveKnob;
    Slider odLevelKnob;
    Slider modelSelectorKnob;
 
    // LookandFeels of the knobs 
    myLookAndFeel knob;

    //AudioProcessorParameter* getParameter(const String& paramId);
    AudioProcessorValueTreeState& treeState;

    bool model_loaded = false;

    virtual void sliderValueChanged(Slider* slider) override;
    bool isValidFormat(File configFile);


    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> bassSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> midSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> trebleSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> driveSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> masterSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> modelSelectorSliderAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RolandCubeAudioProcessorEditor)
};
