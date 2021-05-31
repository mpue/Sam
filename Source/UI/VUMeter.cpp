/*
  ==============================================================================

    VUMeter.cpp
    Created: 31 May 2021 11:41:08am
    Author:  mpue

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VUMeter.h"
#define PI 3.14159265
//==============================================================================
VUMeter::VUMeter()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setOpaque(false);

}

VUMeter::~VUMeter()
{
}

void VUMeter::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */    
    float x = getWidth() / 2 + cos((210.0f + (90 * magnitude)) * PI/180) * 50.0f;
    float y = getHeight() / 2 + 10 + sin((210.0f + (90*magnitude)) * PI/180) * 50.0f;
    juce::Line<float> line = juce::Line<float>(getWidth() / 2, getHeight() / 2 +10, x,y);
    g.drawImageAt(juce::ImageCache::getFromMemory(BinaryData::vintage_vu_png, BinaryData::vintage_vu_pngSize),0,0);
    g.drawArrow(line, 3, 6, 6);

}

void VUMeter::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
