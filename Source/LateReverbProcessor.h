/*
  ==============================================================================

    LateReverbProcessor.h
    Created: 6 Dec 2021 9:51:27pm
    Author:  eri24816

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ProcessorBase.h"
#include "IIR.h"
//==============================================================================
/**
*/
class LateReverbProcessor : public ProcessorBase
{
public:
    //==============================================================================
    LateReverbProcessor();
    ~LateReverbProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    const juce::String getName() const override;

    //==============================================================================
    void addInpulse() {
        addInpulseNextSample = true;
    }
    IIR::Reverb<2> reverb;
    IIR::Allpass2<2> allpass;
    juce::dsp::Convolution conv;

    juce::AudioSampleBuffer chooseFile() {

        juce::File filterFile("D:\\impulse_response\\WIDE HALL-1.wav");
        juce::AudioBuffer<float> sourceBuffer;
        //return sourceBuffer;
        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        auto* reader = formatManager.createReaderFor(filterFile);

        sourceBuffer.setSize(reader->numChannels, (int)reader->lengthInSamples);
        reader->read(&sourceBuffer, 0, (int)reader->lengthInSamples, 0, false, false);

        int len = sourceBuffer.getNumSamples();
        std::cout << "Length: " << len << std::endl;
        for (int i = 0; i < len; i++) {
            std::cout << *(sourceBuffer.getReadPointer(0) + i) << " ";
            if (i == 100)
                break;
        }
        return sourceBuffer;
    }
    void LoadIR(const juce::File& file);
private:
    bool hasIrToLoad = false;
    juce::File IrToLoad;
    void PrepareConv();
    int addInpulseNextSample = false;
    juce::dsp::ProcessSpec convSpec;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LateReverbProcessor)
};

