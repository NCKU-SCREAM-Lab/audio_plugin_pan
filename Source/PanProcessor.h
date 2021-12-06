/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ProcessorBase.h"
#include "pan.h"

#define MAX_SOURCE_NUM 1

//==============================================================================
/**
*/
class PanProcessor : public ProcessorBase
{
public:
    //==============================================================================
    PanProcessor();
    ~PanProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    const juce::String getName() const override;

    pan::Panner _panner;
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanProcessor)
};
