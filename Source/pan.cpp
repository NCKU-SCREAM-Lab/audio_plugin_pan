/*
  ==============================================================================

    pan_room.cpp
    Created: 25 Oct 2021 11:42:01am
    Author:  tanch

  ==============================================================================
*/

#include "pan.h"
#include "hrir.h"
#include <cmath>

#define PI 3.14159265

namespace pan
{

/*--------------------RoomObject------------------------*/
RoomObject::RoomObject(Panner &panner, Room &room, float w_x, float w_y, int vid)
: vid(vid), coord{w_x, w_y}, _panner{panner}, _room{room}
{}

RoomObject::~RoomObject() {}

void RoomObject::paint(juce::Graphics &g)
{
    g.setColour(_color);
    g.drawEllipse (2, 2, _size/2-4, _size/2-4, 2);
}

void RoomObject::setColor(juce::Colour color) { _color = color; }

void RoomObject::setSize(int size) { _size = size; }

void RoomObject::mouseDown (const juce::MouseEvent& event)
{
    _lastPos = getPosition();
}

void RoomObject::mouseDrag (const juce::MouseEvent& event)
{
    setTopLeftPosition(_lastPos.x + event.getScreenX() - event.getMouseDownScreenX(),
                       _lastPos.y + event.getScreenY() - event.getMouseDownScreenY());
    _room.updatePosition(*this);
    _panner.updateFilter();
}

void RoomObject::mouseUp (const juce::MouseEvent& event)
{
//    _panner.updateFilter();
}

/*-----------------------Room------------------------*/
Room::Room(Panner& p, int numVoice, float width, float height)
    : _panner{p}, _obj_size{40}, _width{width}, _height(height)
{
    head = std::make_unique<RoomObject>(_panner, *this, 0, 0, -1);
    head->setSize(_obj_size*2);
    head->setColor(juce::Colours::grey);
    addAndMakeVisible(*head);
    for (int i=0; i<numVoice; i++) {
        float vx = -_width/2.0 + _width/(2.0*numVoice) + i*_width/float(numVoice);
        float vy = _height/4.0;
        std::unique_ptr<RoomObject> v(new RoomObject(_panner, *this, vx, vy, i));
        v->setSize(_obj_size);
        v->setColor(juce::Colours::white);
        addAndMakeVisible(*v);
        voices.push_back(std::move(v));
    }
}

Room::~Room() {}

void Room::paint(juce::Graphics &g)
{
    g.fillAll (juce::Colour(50, 50, 50));
}

void Room::resized()
{
    auto uiCoord = getUiCoord(head->coord);
    head->setBounds(uiCoord.x, uiCoord.y, _obj_size, _obj_size);
    for (int i=0; i<voices.size(); i++) {
        auto uiCoord = getUiCoord(voices[i]->coord);
        voices[i]->setBounds(uiCoord.x, uiCoord.y, _obj_size, _obj_size);
    }
}

void Room::updatePosition(RoomObject& obj)
{
    geo::vec2 uiCoord{obj.getX(), obj.getY()};
    obj.coord = getWordCoord(uiCoord);
}

geo::vec2 Room::getUiCoord(geo::vec2 &worldCoord)
{
    auto bounds = getLocalBounds();
    auto ui_w = bounds.getWidth();
    auto ui_h = bounds.getHeight();
    
    /* origin is at the centre of the UI component */
    geo::vec2 coord{ui_w /2.f, ui_h/2.f};
    
    coord.x += worldCoord.x * (ui_w / _width);
    coord.y -= worldCoord.y * (ui_h / _height);
    
    return coord;
}

geo::vec2 Room::getWordCoord(geo::vec2 &uiCoord)
{
    auto bounds = getLocalBounds();
    auto ui_w = bounds.getWidth();
    auto ui_h = bounds.getHeight();
    
    /* origin is at the centre of the UI component */
    geo::vec2 coord{-_width/2, _height/2};
    
    coord.x += uiCoord.x * (_width / ui_w);
    coord.y -= uiCoord.y * (_height / ui_h);
    
    return coord;
}

/*-----------------------Panner----------------------*/
Panner::Panner(int numVoice, float roomWidth, float roomHeight)
    :room(*this, numVoice, roomWidth, roomHeight)
{
    for (int i=0; i<numVoice; i++) {
        for (int j=0; j<2; j++) {
            _hrir_filter_l[j].push_back(juce::dsp::FIR::Filter<float>());
            _hrir_filter_r[j].push_back(juce::dsp::FIR::Filter<float>());
        }
    }
}

void Panner::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    _spec.sampleRate = sampleRate;
    _spec.maximumBlockSize = samplesPerBlock;
    _spec.numChannels = 1;
    for (int i=0; i<2; i++) {
        for (auto &filter : _hrir_filter_l[i]) {
            filter.prepare(_spec);
            filter.reset();
        }
        for (auto &filter : _hrir_filter_r[i]) {
            filter.prepare(_spec);
            filter.reset();
        }
    }
    updateFilter();
}

float getAzimuth(float x1, float y1, float x2, float y2) // (x2, y2) is the reference
{
    if (x1 == x2) {
        return y1 > y2 ? 90 : 270;
    }
    else {
        float deg = atan((y1-y2) / (x1-x2)) * 180 / PI;
        if (x1 < x2) {
            return 180 + deg;
        }
        return deg < 0 ? deg + 360 : deg;
    }
}

float interpolate(float t3, float v1, float t1, float v2, float t2) {
    if (t1 == t2) {
        return (v1 + v2) / 2;
    }
    else {
        return v1 + (v2 - v1) * (t3 - t1) / (t2 - t1);
    }
}

std::vector<float> Panner::getCoefficeints(float azimuth, float elevation, float distance_2, float hrir_coef[][HRIR_ELEVATION_NUM][HRIR_COEF_NUM])
{
    /* transform the original azimuth to the one of HRIR data. */
    if (azimuth > 180.f) {
        azimuth = 360.f - azimuth;
    }
    azimuth = 90 - azimuth; // 90 is the zero of HRIR data.
    
    /* find the indices of left and right bounds */
    int azi_bound_l = 0, azi_bound_r = HRIR_AZIMUTH_NUM-1;
    int ele_bound_l = 0, ele_bound_r = HRIR_ELEVATION_NUM-1;
    
    for (int i=0, j=HRIR_AZIMUTH_NUM-1; i<HRIR_AZIMUTH_NUM; i++, j--) {
        if (hrir::azi_angle[i] <= azimuth) {
            azi_bound_l = i;
        }
        if (hrir::azi_angle[j] >= azimuth) {
            azi_bound_r = j;
        }
    }
    
    for (int i=0, j=HRIR_ELEVATION_NUM-1; i<HRIR_ELEVATION_NUM; i++, j--) {
        if (hrir::ele_angle[i] <= elevation) {
            ele_bound_l = i;
        }
        if (hrir::ele_angle[j] >= elevation) {
            ele_bound_r = j;
        }
    }
    // DBG(azi_bound_l << "," << azi_bound_r << "," << ele_bound_l << "," << ele_bound_r);
    
    /* get the coefficients from interpolation*/
    std::vector<float> coef(HRIR_COEF_NUM);
    for (int i=0; i<HRIR_COEF_NUM; i++) {
        float t1 = interpolate(azimuth,
                               hrir_coef[azi_bound_l][ele_bound_l][i], hrir::azi_angle[azi_bound_l],
                               hrir_coef[azi_bound_r][ele_bound_l][i], hrir::azi_angle[azi_bound_r]);
        float t2 = interpolate(azimuth,
                               hrir_coef[azi_bound_l][ele_bound_r][i], hrir::azi_angle[azi_bound_l],
                               hrir_coef[azi_bound_r][ele_bound_r][i], hrir::azi_angle[azi_bound_r]);
        coef[i] = interpolate(elevation,
                              t1, hrir::ele_angle[ele_bound_l],
                              t2, hrir::ele_angle[ele_bound_r]);
        if (distance_2 > 0) {
            coef[i] *= juce::jmin(1.0f, 2/distance_2);
            // DBG(2/distance_2);
        }
    }
    
    return coef;
}

void Panner::updateFilter()
{
    _cs_filter.enter();

    _next_filter = 1 - _next_filter;
    auto &head = room.head;
    for (auto &voice : room.voices) {
        float azimuth = getAzimuth(voice->coord.x, voice->coord.y, head->coord.x, head->coord.y);
        float dist_2 = (voice->coord.x - room.head->coord.x)*(voice->coord.x - room.head->coord.x) + (voice->coord.y - room.head->coord.y)*(voice->coord.y - room.head->coord.y);
        auto coef_l = getCoefficeints(azimuth, 0, dist_2, hrir::coef_l);
        auto coef_r = getCoefficeints(azimuth, 0, dist_2, hrir::coef_r);
        *(_hrir_filter_l[_next_filter][voice->vid].coefficients) = juce::dsp::FIR::Coefficients<float>(coef_l.data(), coef_l.size());
        *(_hrir_filter_r[_next_filter][voice->vid].coefficients) = juce::dsp::FIR::Coefficients<float>(coef_r.data(), coef_r.size());
    }
    
    _cs_filter.exit();
}

void Panner::processBlock(juce::AudioBuffer<float>& buffer)
{
    juce::AudioBuffer<float> buffer_l, buffer_r;
    buffer_l.makeCopyOf(buffer);
    buffer_r.makeCopyOf(buffer);
    
    for (int i=0; i<OUTPUT_VOICE_NUM; i++) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    if (_cs_filter.tryEnter()) {
        _cur_filter = _next_filter;
        _cs_filter.exit();
    }
    
    for (auto &voice : room.voices) {
        auto p_l = buffer_l.getWritePointer(voice->vid);
        auto p_r = buffer_r.getWritePointer(voice->vid);
        auto block_l = juce::dsp::AudioBlock<float>(&p_l, 1, buffer_l.getNumSamples());
        auto block_r = juce::dsp::AudioBlock<float>(&p_r, 1, buffer_r.getNumSamples());
        processBlock(voice->vid, block_l, block_r);
        buffer.addFrom(0, 0, buffer_l, voice->vid, 0, buffer.getNumSamples());
        buffer.addFrom(1, 0, buffer_r, voice->vid, 0, buffer.getNumSamples());
    }
    buffer.applyGain(1/(float)room.voices.size());
}

void Panner::processBlock(int voice, juce::dsp::AudioBlock<float> &block_l, juce::dsp::AudioBlock<float> &block_r)
{
    std::vector<float> vec_l, vec_r;
    vec_l.resize(block_l.getNumSamples());
    vec_r.resize(block_r.getNumSamples());
    auto p_l = vec_l.data();
    auto p_r = vec_r.data();
    
    juce::dsp::AudioBlock<float> copy_l(&p_l, block_l.getNumChannels(), vec_l.size());
    juce::dsp::AudioBlock<float> copy_r(&p_r, block_r.getNumChannels(), vec_r.size());
    copy_l.copyFrom(block_l);
    copy_r.copyFrom(block_r);
    
    /* let the unused filter process the input audio to avoid crack sound */
    _hrir_filter_l[_cur_filter][voice].process(juce::dsp::ProcessContextReplacing<float>(block_l));
    _hrir_filter_r[_cur_filter][voice].process(juce::dsp::ProcessContextReplacing<float>(block_r));
    _hrir_filter_l[1-_cur_filter][voice].process(juce::dsp::ProcessContextReplacing<float>(copy_l));
    _hrir_filter_r[1-_cur_filter][voice].process(juce::dsp::ProcessContextReplacing<float>(copy_r));
    
}

}
