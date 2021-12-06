/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PanAudioProcessorEditor::PanAudioProcessorEditor (PanAudioProcessor& p)
    : AudioProcessorEditor(&p), _audioProcessor(p)//, _panner(NULL)
{
    setSize (800, 600);
    addAndMakeVisible(p.pan()->_panner.room);
}

PanAudioProcessorEditor::~PanAudioProcessorEditor()
{
}

//==============================================================================
void PanAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PanAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    juce::FlexBox flexBox;
    flexBox.items.add(juce::FlexItem(_audioProcessor.pan()->_panner.room).withFlex(1.0));
    flexBox.performLayout(getLocalBounds().reduced(50));
}
