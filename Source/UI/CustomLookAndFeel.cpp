/*
  ==============================================================================

    CustomLookAndFeel.cpp
    Created: 25 Nov 2016 5:14:44pm
    Author:  Matthias Pueski

  ==============================================================================
*/

#include "CustomLookAndFeel.h"

using juce::Graphics;
using juce::Slider;
using juce::Time;
using juce::ImageCache;
using juce::Image;

void CustomLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                        float sliderPos, float minSliderPos, float maxSliderPos,
                                          const Slider::SliderStyle style, Slider& slider) {
     g.fillAll (slider.findColour (Slider::backgroundColourId));
    
    if (style == Slider::SliderStyle::LinearBarVertical) {
        
        float from  = (sliderPos / maxSliderPos) * height;
        float to = height;
        
        g.setGradientFill(gradientVertical);

        if (Time::getMillisecondCounter() - triggerTime > 2000){
            triggerTime = Time::getMillisecondCounter();
            maxY = 0xFFFF;
        }
        
        if (from < maxY) {
            maxY = from;

        }
        
        g.fillRect((float)x,(float)maxY,(float)width,1.0f);
        g.fillRect((float)x,from,(float)width,to);

    }
    else if (style == Slider::SliderStyle::LinearBar) {
        
        // float to  = (sliderPos / maxSliderPos) * width;
        //float to = width;
        
        g.setGradientFill(gradientHorizontal);
        //g.fillRect((float)x,(float)y,(float)to,(float)height);
        
        if (Time::getMillisecondCounter() - triggerTime > 2000){
            triggerTime = Time::getMillisecondCounter();
            maxX = 0;
        }
        
        if (sliderPos > maxX) {
            maxX = sliderPos;

        }
        
        g.fillRect ((float)maxX, (float)y, 2.0f, (float)height);
        g.fillRect (0.0f, (float)y, (float)(sliderPos), (float)height);
    }
    
    
    else if (style == Slider::SliderStyle::LinearVertical) {
        Image fader = ImageCache::getFromMemory(BinaryData::Fader_png,BinaryData::Fader_pngSize);
        

        float from  = (sliderPos / maxSliderPos) * height;
        LookAndFeel_V3::drawLinearSliderBackground(g, x, y + 10, width, height + 10, sliderPos, minSliderPos, maxSliderPos, style, slider);
        
        
        g.setColour(juce::Colours::white);
        g.drawImageAt(fader.rescaled(fader.getWidth(), fader.getHeight()/2),0, from - fader.getHeight()/4 + 10);
        
    }
    else {
        LookAndFeel_V3::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
    
    
}

void CustomLookAndFeel::drawRotarySlider    (    Graphics &     g,
                                           int     x,
                                           int     y,
                                           int     width,
                                           int     height,
                                           float     sliderPosProportional,
                                           float     rotaryStartAngle,
                                           float     rotaryEndAngle,
                                           Slider &     slider )
{
    //LookAndFeel_V2::drawRotarySlider(g, x, y, width, height, sliderPosProportional, rotaryStartAngle, rotaryEndAngle, slider);
    
    // This is the binary image data that uses very little CPU when rotating
    Image myStrip = ImageCache::getFromMemory (BinaryData::Knob_64_png, BinaryData::Knob_64_pngSize);
    
    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    const float radius = juce::jmin (width / 2.0f, height / 2.0f) ;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float rx = centreX - radius - 1.0f;
    const float ry = centreY - radius - 1.0f;
    const float rw = radius * 2.0f;
    const float thickness = 0.9f;
    
    // const double fractRotation = (slider.getValue() - slider.getMinimum())  /
    //(slider.getMaximum() - slider.getMinimum()); //value between 0 and 1 for current amount of rotation
    
    const int nFrames = myStrip.getHeight()/myStrip.getWidth(); // number of frames for vertical film strip
    const int frameIdx = (int)ceil(sliderPosProportional  * ((double)nFrames-1.0) ); // current index from 0 --> nFrames-1
    
    // Logger::getCurrentLogger()->writeToLog("==========" +String(sliderPosProportional));
    
    g.setColour(juce::Colours::darkorange);
    {
        juce::Path filledArc;

        float dist = angle - rotaryStartAngle;
        
        for (float a = rotaryStartAngle; a < dist + rotaryStartAngle;a+= 0.15) {
            filledArc.addPieSegment (rx+1, ry+1, rw-0.5, rw-0.5, a, a+0.1, thickness);
        }
        
        // filledArc.addPieSegment (rx+1, ry+1, rw-0.5, rw-0.5, rotaryStartAngle, angle, thickness);
        
        g.fillPath (filledArc);
    }
    
    g.drawImage(myStrip,
                (int)rx,
                (int)ry,
                2*(int)radius,
                2*(int)radius,   //Dest
                0,
                frameIdx*myStrip.getWidth(),
                myStrip.getWidth(),
                myStrip.getWidth()); //Source
    
}

void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g,
    juce::Button& button,
    const juce::Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    auto cornerSize = 1.0f;
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

    auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
        .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

    if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
        baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);

    g.setColour(baseColour);

    auto flatOnLeft = button.isConnectedOnLeft();
    auto flatOnRight = button.isConnectedOnRight();
    auto flatOnTop = button.isConnectedOnTop();
    auto flatOnBottom = button.isConnectedOnBottom();

    if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
    {
        juce::Path path;
        path.addRoundedRectangle(bounds.getX(), bounds.getY(),
            bounds.getWidth(), bounds.getHeight(),
            cornerSize, cornerSize,
            !(flatOnLeft || flatOnTop),
            !(flatOnRight || flatOnTop),
            !(flatOnLeft || flatOnBottom),
            !(flatOnRight || flatOnBottom));

        g.fillPath(path);

        g.setColour(button.findColour(juce::ComboBox::outlineColourId));
        g.strokePath(path, juce::PathStrokeType(1.0f));
    }
    else
    {
        g.fillRoundedRectangle(bounds, cornerSize);

        g.setColour(button.findColour(juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
    }
}
