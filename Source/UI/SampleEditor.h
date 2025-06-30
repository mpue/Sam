/*
  ==============================================================================

    SampleEditor.h
    Created: 31 May 2021 4:45:05pm
    Author:  mpue

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../AudioEngine/Sampler.h"
//==============================================================================
/*
*/
class SampleEditor : public juce::Component, public juce::Timer
{
public:
    SampleEditor(juce::AudioFormatManager* fmtMgr);
    ~SampleEditor();

    void paint (juce::Graphics&) override;
    void resized() override;
    void reset(double sampleRate);
    std::unique_ptr<juce::AudioThumbnailCache> cache = nullptr;
    std::unique_ptr<juce::AudioThumbnail> thumbnail = nullptr;
    double sampleLength = 0;
    void timerCallback() override;
    void setSampler(Sampler* sampler);
    void mouseDown(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& e,
        const juce::MouseWheelDetails& wheel) override;

    // helpers =================================================================
    float sampleToScreen(float sampleIndex) const;           // samples ➜ px
    float screenToSample(float screenX) const;		   // px ➜ samples
private:
    Sampler* sampler = nullptr;
    float samplePosX = 0;
    float sampleStartPosX = 0;
    float zoom = 1;
    float zoomStart = 1;
    float offset = 0;
    float offsetStart = 0;
    float sampleEndPosX = 0;
    bool draggingStart = false;
    bool draggingEnd = false;
    float deltaX = 0;
    float dragStartX = 0;
    float originX = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleEditor)

};
