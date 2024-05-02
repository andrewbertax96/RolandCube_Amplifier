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

    AudioProcessorValueTreeState& treeState;

    virtual void sliderValueChanged(Slider* slider) override;
    bool isValidFormat(File configFile);


    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> bassSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> midSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> trebleSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> gainSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> volumeSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> modelSelectorSliderAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RolandCubeAudioProcessorEditor)
};
