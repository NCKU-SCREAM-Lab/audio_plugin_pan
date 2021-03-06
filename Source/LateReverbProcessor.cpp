/*
  ==============================================================================

    LateReverbProcessor.cpp
    Created: 6 Dec 2021 9:51:44pm
    Author:  eri24816

  ==============================================================================
*/

#include "LateReverbProcessor.h"

LateReverbProcessor::LateReverbProcessor()
    : ProcessorBase(-1)
    , allpass(new float[] {0.8, 0.8}, new float[] {1, 1})
{
}

LateReverbProcessor::~LateReverbProcessor()
{
}

//==============================================================================
const juce::String LateReverbProcessor::getName() const
{
    return "Late Reverb";
}

//==============================================================================
void LateReverbProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    convSpec.sampleRate = sampleRate;
    convSpec.maximumBlockSize = samplesPerBlock;
    convSpec.numChannels = getTotalNumOutputChannels();
    conv.prepare(convSpec);
    PrepareConv();
}

void LateReverbProcessor::PrepareConv() {
    conv.reset();
    //conv.prepare(convSpec);
}

void LateReverbProcessor::LoadIR(const juce::File& file) {
    IrToLoad = file;
    hasIrToLoad = true;
}

void LateReverbProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void LateReverbProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());




    int numSamples = buffer.getNumSamples();
    float systemInput[2];// 2 channels
    for (int sample = 0; sample < numSamples; ++sample) {

        // Collect input data for the IIR system
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            systemInput[channel] = *buffer.getWritePointer(channel, sample);
            if (addInpulseNextSample) {
                systemInput[channel] += 1;

            }
        }
        addInpulseNextSample = false;
        // Update the system
        float* systemOutput = reverb.update(systemInput);
        //float* systemOutput = comb.update(systemInput);
        //float* systemOutput = systemInput;

        // Write the system's output back to the AudioBuffer
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            *buffer.getWritePointer(channel, sample) = systemOutput[channel];
        }
        
    }
    
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    conv.process(context);
    
    if (hasIrToLoad) {
        conv.reset();
        conv.loadImpulseResponse(IrToLoad,
            juce::dsp::Convolution::Stereo::yes
            , juce::dsp::Convolution::Trim::yes, 0);
        hasIrToLoad = false;
        PrepareConv();
    }
}
