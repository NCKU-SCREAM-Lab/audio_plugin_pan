/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PanProcessor.h"
#include "LateReverbProcessor.h"

using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;

//==============================================================================
//PanAudioProcessor::PanAudioProcessor()
//#ifndef JucePlugin_PreferredChannelConfigurations
//     : AudioProcessor (BusesProperties()
//                     #if ! JucePlugin_IsMidiEffect
//                      #if ! JucePlugin_IsSynth
//                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
//                      #endif
//                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
//                     #endif
//                       ),
//#endif
//       _mainProcessor(new juce::AudioProcessorGraph())
//{
//}

PanAudioProcessor::PanAudioProcessor()
    : MultiBusProcessor(MAX_SOURCE_NUM),
      _mainProcessor(new juce::AudioProcessorGraph())
{}

PanProcessor* PanAudioProcessor::pan()
{
    return (PanProcessor*)_panNode->getProcessor();
}

LateReverbProcessor* PanAudioProcessor::reverb()
{
    return (LateReverbProcessor*)_reverbNode->getProcessor();
}

PanAudioProcessor::~PanAudioProcessor() {}

//==============================================================================
const juce::String PanAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PanAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PanAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PanAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PanAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PanAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PanAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PanAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PanAudioProcessor::getProgramName (int index)
{
    return {};
}

void PanAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PanAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    _mainProcessor->setPlayConfigDetails(MAX_SOURCE_NUM,
                                         getMainBusNumOutputChannels(),
                                         sampleRate, samplesPerBlock);
    _mainProcessor->prepareToPlay(sampleRate, samplesPerBlock);
    initializeGraph();
}

void PanAudioProcessor::initializeGraph()
{
    _mainProcessor->clear();
    
    _audioInputNode = _mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioInputNode));
    _audioOutputNode = _mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioOutputNode));
    _midiInputNode = _mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiInputNode));
    _midiOutputNode = _mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiOutputNode));
    _panNode = _mainProcessor->addNode(std::make_unique<PanProcessor>(MAX_SOURCE_NUM));
    _reverbNode = _mainProcessor->addNode(std::make_unique<LateReverbProcessor>());
    
    connectAudioNodes();
    connectMidiNodes();
}

void PanAudioProcessor::connectAudioNodes()
{
    for (int channel = 0; channel < MAX_SOURCE_NUM; ++channel) {
        _mainProcessor->addConnection({
            {_audioInputNode->nodeID, channel},
            {_panNode->nodeID, channel}});
    }
    for (int channel = 0; channel < 2; ++channel) {
        _mainProcessor->addConnection({
            {_panNode->nodeID, channel},
            {_reverbNode->nodeID, channel}});
        _mainProcessor->addConnection({
            {_reverbNode->nodeID, channel},
            {_audioOutputNode->nodeID, channel}});
    } 
}

void PanAudioProcessor::connectMidiNodes()
{
    _mainProcessor->addConnection ({
        {_midiInputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex},
        {_midiOutputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}});
}

void PanAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    _mainProcessor->releaseResources();
}

//#ifndef JucePlugin_PreferredChannelConfigurations
//bool PanAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
//{
//  #if JucePlugin_IsMidiEffect
//    juce::ignoreUnused (layouts);
//    return true;
//  #else
//    // This is the place where you check if the layout is supported.
//    // In this template code we only support mono or stereo.
//    // Some plugin hosts, such as certain GarageBand versions, will only
//    // load plugins that support stereo bus layouts.
//    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
//     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
//        return false;
//
//    // This checks if the input layout matches the output layout
//   #if ! JucePlugin_IsSynth
//    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
//        return false;
//   #endif
//
//    return true;
//  #endif
//}
//#endif

void PanAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    updateGraph();
    _mainProcessor->processBlock(buffer, midiMessages);
    
}

void PanAudioProcessor::updateGraph()
{
    
}

//==============================================================================
bool PanAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PanAudioProcessor::createEditor()
{
    return new PanAudioProcessorEditor (*this);
}

//==============================================================================
void PanAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PanAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PanAudioProcessor();
}
