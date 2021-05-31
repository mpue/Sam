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
class VUMeter  : public juce::Component
{
public:
    VUMeter();
    ~VUMeter() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    float magnitude = 0;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VUMeter)
};
