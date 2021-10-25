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

namespace pan
{

class Room;

class Panner{};

class RoomObject : public juce::Component
{
public:
    RoomObject(Room &room, float x, float y);
    void paint(juce::Graphics &g) override;
    
    geo::vec2 coord;
private:
    Room &_room;
    juce::Point<int> _lastPos;
    
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;
};

/* a rectangle room*/
class Room: public juce::Component {
public:
    Room(float width, float height, int numVoice);
    void paint(juce::Graphics &g) override;
    void resized() override;
private:
    RoomObject _head;
    std::vector<RoomObject> _voices;
    float _width, _height;
    
    geo::vec2 getUiCoord(geo::vec2 &worldCoord);
};



}
