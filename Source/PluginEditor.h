/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "pan.h"

//==============================================================================
/**
*/
class PanAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PanAudioProcessorEditor (PanAudioProcessor&);
    ~PanAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PanAudioProcessor& audioProcessor;
    pan::Panner& _panner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanAudioProcessorEditor)
};
