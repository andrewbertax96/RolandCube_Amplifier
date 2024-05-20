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
class RolandCubeAudioProcessorEditor  : public juce::AudioProcessorEditor                                
{
public:
    RolandCubeAudioProcessorEditor (RolandCubeAudioProcessor&);
    ~RolandCubeAudioProcessorEditor() override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void loadJsonFiles();
    void loadJson();
    bool isValidFormat(File configFile);

    void resetImages();

    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RolandCubeAudioProcessor& audioProcessor;
    Atomic<bool> typeParam = { true };

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
      
    // LookandFeels of the knobs 
    myLookAndFeel knobLookAndFeel;
    myLookAndFeel knobLead_LookAndFeel;

    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> bassSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> midSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> trebleSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> gainSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> volumeSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> modelSelectorSliderAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RolandCubeAudioProcessorEditor)
};
