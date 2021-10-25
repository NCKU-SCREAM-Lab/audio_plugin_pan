/*
  ==============================================================================

    pan_room.cpp
    Created: 25 Oct 2021 11:42:01am
    Author:  tanch

  ==============================================================================
*/

#include "pan.h"

namespace pan
{

/*--------------------RoomObject------------------------*/

RoomObject::RoomObject(Room &room, float x, float y)
: coord{x, y}, _room{room}
{}

void RoomObject::paint(juce::Graphics &g)
{
    g.setColour(juce::Colours::white);
    g.drawEllipse (0, 0, 50, 50, 3);
}

void RoomObject::mouseDown (const juce::MouseEvent& event)
{
    _lastPos = getPosition();
}

void RoomObject::mouseDrag (const juce::MouseEvent& event)
{
    setTopLeftPosition(_lastPos.x + event.getScreenX() - event.getMouseDownScreenX(),
                       _lastPos.y + event.getScreenY() - event.getMouseDownScreenY());
}

void RoomObject::mouseUp (const juce::MouseEvent& event)
{
}

/*-----------------------Room------------------------*/

Room::Room(float width, float height, int numVoice)
: _head{*this, 0, 0}, _width{width}, _height(height)
{
    addAndMakeVisible(_head);
}

void Room::paint(juce::Graphics &g)
{
    g.fillAll (juce::Colour(50, 50, 50));
}

void Room::resized()
{
    _head.setBounds(60, 60, 100, 100);
}

geo::vec2 Room::getUiCoord(geo::vec2 &worldCoord)
{
    auto bounds = getLocalBounds();
    auto ui_w = bounds.getWidth();
    auto ui_h = bounds.getHeight();
    
    /* origin is at the centre of the UI component */
    geo::vec2 coord{ui_w /2.f, ui_h/2.f};
    
    coord.x += worldCoord.x * (ui_w / _width);
    coord.y += worldCoord.y * (ui_h / _height);
    
    return coord;
}


}
