/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>
#include "PanProcessor.h"
#include "LateReverbProcessor.h"
#include "MultiBusProcessor.h"

#define MAX_SOURCE_NUM 4

//==============================================================================
/**
*/
class PanAudioProcessor  : public MultiBusProcessor
{
public:
    //==============================================================================
    PanAudioProcessor();
    ~PanAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    PanProcessor* pan();
    LateReverbProcessor* reverb();
private:
    std::unique_ptr<juce::AudioProcessorGraph> _mainProcessor;
    juce::AudioProcessorGraph::Node::Ptr _audioInputNode;
    juce::AudioProcessorGraph::Node::Ptr _audioOutputNode;
    juce::AudioProcessorGraph::Node::Ptr _midiInputNode;
    juce::AudioProcessorGraph::Node::Ptr _midiOutputNode;
    juce::AudioProcessorGraph::Node::Ptr _panNode;
    juce::AudioProcessorGraph::Node::Ptr _reverbNode;

    void initializeGraph();
    void connectAudioNodes();
    void connectMidiNodes();
    void updateGraph();
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanAudioProcessor)
};
