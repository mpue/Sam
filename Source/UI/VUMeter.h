/*
  ==============================================================================

    VUMeter.h
    Created: 31 May 2021 11:41:08am
    Author:  mpue

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class VUMeter  : public juce::Component, public juce::Timer
{
public:
    VUMeter();
    ~VUMeter() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void setMagnitude(float newMagnitude);
    void timerCallback() override;

private:
    float targetMagnitude = 0;
    float currentMagnitude = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VUMeter)
};
