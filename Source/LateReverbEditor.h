/*
  ==============================================================================

    LateReverbEditor.h
    Created: 7 Dec 2021 2:12:19pm
    Author:  eri24816

  ==============================================================================
*/

#pragma once
/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LateReverbProcessor.h"
#include "PluginProcessor.h"
#include <string>

class RotarySlider : public juce::Slider {
private:
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

public:
    juce::Label nameLabel;

    RotarySlider(std::string name);
    RotarySlider() {};
    void resized() override;
};

//==============================================================================
/**
*/
class LateReverbEditor : public juce::Component
{
public:
    LateReverbEditor(LateReverbProcessor&);
    ~LateReverbEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    RotarySlider  dryWet;

    RotarySlider roomSize, roomShape, decay, damping, modulationDepth;

    juce::TextButton impulseButton,chooseFileButton;
    juce::ToggleButton toggleConvolution, toggleIIR;
    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LateReverbProcessor& audioProcessor;
    void ChooseFile();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LateReverbEditor)
};
