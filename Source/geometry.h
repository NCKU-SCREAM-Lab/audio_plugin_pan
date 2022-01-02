/*
  ==============================================================================

    geometry.h
    Created: 25 Oct 2021 1:39:48pm
    Author:  tanch

  ==============================================================================
*/

#pragma once

#include <array>

namespace geo
{

class vec2//: public std::array<float, 2>
{
public:
    //vec2(float v0, float v1) : std::array<float, 2>{v0, v1}, x{this->at(0)}, y{this->at(1)} {}
    //vec2(int v0, int v1) : std::array<float, 2>{(float)v0, (float)v1}, x{this->at(0)}, y{this->at(1)} {}
    vec2(float v0, float v1) : x{v0}, y{v1} {}
    vec2(int v0, int v1) : x{(float)v0}, y{(float)v1} {}
    vec2& operator= (vec2 other)
    {
        //this->at(0) = other[0];
        //this->at(1) = other[1];
        //std::array<float, 2>::operator=(other);
        //x = this->at(0);
        //y = this->at(1);
        x = other.x;
        y = other.y;
        return *this;
    }
    
    float x;
    float y;
};

class vec3: public std::array<float, 3>
{
public:
    vec3(float v0, float v1, float v2)
        :std::array<float, 3>{v0, v1, v2}, x{this->at(0)}, y{this->at(1)}, z{this->at(2)}
    {}
    
    float &x;
    float &y;
    float &z;
};

}
