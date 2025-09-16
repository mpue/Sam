/*
  ==============================================================================

    EffectsProcessor.h
    Created: $(Date)
    Author:  GitHub Copilot

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StereoReverb.h"
#include "StereoChorus.h"
#include "StereoDelay.h"

//==============================================================================
/**
    Effects processor managing reverb, chorus, and stereo delay
*/
class EffectsProcessor
{
public:
    //==============================================================================
    EffectsProcessor();
    ~EffectsProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer, int numChannels, int numSamples);
    void reset();

    //==============================================================================
    // Reverb parameters
    void setReverbEnabled(bool enabled);
    void setReverbRoomSize(float roomSize);
    void setReverbDamping(float damping);
    void setReverbWetLevel(float wetLevel);
    void setReverbDryLevel(float dryLevel);
    void setReverbWidth(float width);

    bool getReverbEnabled() const { return reverbEnabled; }
    float getReverbRoomSize() const { return reverbRoomSize; }
    float getReverbDamping() const { return reverbDamping; }
    float getReverbWetLevel() const { return reverbWetLevel; }
    float getReverbDryLevel() const { return reverbDryLevel; }
    float getReverbWidth() const { return reverbWidth; }

    //==============================================================================
    // Chorus parameters
    void setChorusEnabled(bool enabled);
    void setChorusRate(float rate);
    void setChorusDepth(float depth);
    void setChorusFeedback(float feedback);
    void setChorusMix(float mix);

    bool getChorusEnabled() const { return chorusEnabled; }
    float getChorusRate() const { return chorusRate; }
    float getChorusDepth() const { return chorusDepth; }
    float getChorusFeedback() const { return chorusFeedback; }
    float getChorusMix() const { return chorusMix; }

    //==============================================================================
    // Delay parameters
    void setDelayEnabled(bool enabled);
    void setDelayLeftTime(float timeMs);
    void setDelayRightTime(float timeMs);
    void setDelayLeftFeedback(float feedback);
    void setDelayRightFeedback(float feedback);
    void setDelayMix(float mix);

    bool getDelayEnabled() const { return delayEnabled; }
    float getDelayLeftTime() const { return delayLeftTime; }
    float getDelayRightTime() const { return delayRightTime; }
    float getDelayLeftFeedback() const { return delayLeftFeedback; }
    float getDelayRightFeedback() const { return delayRightFeedback; }
    float getDelayMix() const { return delayMix; }

private:
    //==============================================================================
    std::unique_ptr<StereoReverb> reverb;
    std::unique_ptr<StereoChorus> chorus;
    std::unique_ptr<StereoDelay> delay;

    // Reverb parameters
    bool reverbEnabled = false;
    float reverbRoomSize = 0.5f;
    float reverbDamping = 0.5f;
    float reverbWetLevel = 0.33f;
    float reverbDryLevel = 0.4f;
    float reverbWidth = 1.0f;

    // Chorus parameters
    bool chorusEnabled = false;
    float chorusRate = 1.0f;
    float chorusDepth = 0.25f;
    float chorusFeedback = 0.0f;
    float chorusMix = 0.5f;

    // Delay parameters
    bool delayEnabled = false;
    float delayLeftTime = 250.0f;
    float delayRightTime = 375.0f;
    float delayLeftFeedback = 0.3f;
    float delayRightFeedback = 0.3f;
    float delayMix = 0.3f;

    double currentSampleRate = 44100.0;
    int currentSamplesPerBlock = 512;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectsProcessor)
};