/*
  ==============================================================================

    MultiBusProcessor.h
    Created: 29 Dec 2021 1:36:05am
    Author:  tanch

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class MultiBusProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MultiBusProcessor(int num_ibus) : AudioProcessor(getBusesProperties(num_ibus)) {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override { return false; }
    
private:
    static juce::AudioProcessor::BusesProperties getBusesProperties(int num_ibus)
    {
        juce::AudioProcessor::BusesProperties buses;
        buses.addBus (false, "Output", juce::AudioChannelSet::stereo());

        if (num_ibus < 0) {
            buses.addBus (true, "Input", juce::AudioChannelSet::stereo());
        }
        else {
            for(int i=0; i<num_ibus; i++) {
                buses.addBus (true, "Input " + std::to_string(i), juce::AudioChannelSet::mono());
            }
        }

        return buses;
    }
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiBusProcessor)
};
