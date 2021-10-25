/*
  ==============================================================================

    pan.h
    Created: 25 Oct 2021 10:50:30am
    Author:  tanch

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "geometry.h"
#include <vector>
#include <memory>
#include "hrir.h"

#define OUTPUT_VOICE_NUM 2

namespace pan
{

class Room;
class Panner;

class RoomObject : public juce::Component
{
public:
    RoomObject(Panner &panner, Room &room, float w_x, float w_y, int vid);
    ~RoomObject();
    void paint(juce::Graphics &g) override;
    void setSize(int size);
    void setColor(juce::Colour color);
    int vid;
    
    geo::vec2 coord;
private:
    Panner &_panner;
    Room &_room;
    juce::Point<int> _lastPos;
    int _size;
    juce::Colour _color;
    
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;
};

/* a rectangle room*/
class Room: public juce::Component {
public:
    Room(Panner& panner, int numVoice, float width, float height);
    ~Room();
    void paint(juce::Graphics &g) override;
    void resized() override;
    void updatePosition(RoomObject& obj);

    std::unique_ptr<RoomObject> head;
    std::vector<std::unique_ptr<RoomObject>> voices;
private:
    Panner& _panner;
    int _obj_size;
    float _width, _height;
    
    geo::vec2 getUiCoord(geo::vec2 &worldCoord);
    geo::vec2 getWordCoord(geo::vec2 &uiCoord);
};

class Panner
{
public:
    Panner(int numVoice, float roomWidth, float roomHeight);
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void updateFilter();
    void processBlock (juce::AudioBuffer<float>& buffer);
    
    Room room;
private:
    juce::dsp::ProcessSpec _spec;
    juce::CriticalSection _cs_filter;
    int _cur_filter = 0, _next_filter = 0;
    std::vector<juce::dsp::FIR::Filter<float> > _hrir_filter_l[2];
    std::vector<juce::dsp::FIR::Filter<float> > _hrir_filter_r[2];
    
    std::vector<float> getCoefficeints(float azimuth, float elevation, float distance_2, float hrir_coef[][HRIR_ELEVATION_NUM][HRIR_COEF_NUM]);
    void processBlock(int voice, juce::dsp::AudioBlock<float> &block_l, juce::dsp::AudioBlock<float> &block_r);
};

}
