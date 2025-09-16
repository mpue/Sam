/*
  ==============================================================================

    EffectsProcessor.cpp
    Created: $(Date)
    Author:  GitHub Copilot

  ==============================================================================
*/

#include "EffectsProcessor.h"

//==============================================================================
EffectsProcessor::EffectsProcessor()
{
    // Effects will be created in prepareToPlay when we have sample rate info
}

EffectsProcessor::~EffectsProcessor()
{
}

//==============================================================================
void EffectsProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentSamplesPerBlock = samplesPerBlock;

    // Create and initialize effects
    reverb = std::make_unique<StereoReverb>();
    reverb->reset();

    chorus = std::make_unique<StereoChorus>(static_cast<float>(sampleRate), samplesPerBlock);
    
    delay = std::make_unique<StereoDelay>();
    delay->resetDelay();

    // Set initial parameters
    setReverbRoomSize(reverbRoomSize);
    setReverbDamping(reverbDamping);
    setReverbWetLevel(reverbWetLevel);
    setReverbDryLevel(reverbDryLevel);
    setReverbWidth(reverbWidth);
    setReverbEnabled(reverbEnabled);

    setChorusRate(chorusRate);
    setChorusDepth(chorusDepth);
    setChorusFeedback(chorusFeedback);
    setChorusMix(chorusMix);
    setChorusEnabled(chorusEnabled);

    setDelayLeftTime(delayLeftTime);
    setDelayRightTime(delayRightTime);
    setDelayLeftFeedback(delayLeftFeedback);
    setDelayRightFeedback(delayRightFeedback);
    setDelayMix(delayMix);
    setDelayEnabled(delayEnabled);
}

void EffectsProcessor::processBlock(juce::AudioBuffer<float>& buffer, int numChannels, int numSamples)
{
    if (numChannels < 2)
        return; // Only process stereo

    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);

    // Process chorus first (modulation effect)
    if (chorus && chorusEnabled)
    {
        chorus->processStereo(leftChannel, rightChannel, numSamples);
    }

    // Process delay (time-based effect)
    if (delay && delayEnabled)
    {
        delay->processStereo(leftChannel, rightChannel, numSamples);
    }

    // Process reverb last (ambience effect)
    if (reverb && reverbEnabled)
    {
        reverb->processStereo(leftChannel, rightChannel, numSamples);
    }
}

void EffectsProcessor::reset()
{
    if (reverb)
        reverb->reset();
    
    if (delay)
        delay->resetDelay();
}

//==============================================================================
// Reverb methods
void EffectsProcessor::setReverbEnabled(bool enabled)
{
    reverbEnabled = enabled;
    if (reverb)
        reverb->setEnabled(enabled);
}

void EffectsProcessor::setReverbRoomSize(float roomSize)
{
    reverbRoomSize = juce::jlimit(0.0f, 1.0f, roomSize);
    if (reverb)
    {
        juce::Reverb::Parameters params = reverb->getParameters();
        params.roomSize = reverbRoomSize;
        reverb->setParameters(params);
    }
}

void EffectsProcessor::setReverbDamping(float damping)
{
    reverbDamping = juce::jlimit(0.0f, 1.0f, damping);
    if (reverb)
    {
        juce::Reverb::Parameters params = reverb->getParameters();
        params.damping = reverbDamping;
        reverb->setParameters(params);
    }
}

void EffectsProcessor::setReverbWetLevel(float wetLevel)
{
    reverbWetLevel = juce::jlimit(0.0f, 1.0f, wetLevel);
    if (reverb)
    {
        juce::Reverb::Parameters params = reverb->getParameters();
        params.wetLevel = reverbWetLevel;
        reverb->setParameters(params);
    }
}

void EffectsProcessor::setReverbDryLevel(float dryLevel)
{
    reverbDryLevel = juce::jlimit(0.0f, 1.0f, dryLevel);
    if (reverb)
    {
        juce::Reverb::Parameters params = reverb->getParameters();
        params.dryLevel = reverbDryLevel;
        reverb->setParameters(params);
    }
}

void EffectsProcessor::setReverbWidth(float width)
{
    reverbWidth = juce::jlimit(0.0f, 1.0f, width);
    if (reverb)
    {
        juce::Reverb::Parameters params = reverb->getParameters();
        params.width = reverbWidth;
        reverb->setParameters(params);
    }
}

//==============================================================================
// Chorus methods
void EffectsProcessor::setChorusEnabled(bool enabled)
{
    chorusEnabled = enabled;
    if (chorus)
        chorus->setEnabled(enabled);
}

void EffectsProcessor::setChorusRate(float rate)
{
    chorusRate = juce::jlimit(0.1f, 10.0f, rate);
    if (chorus)
    {
        // Note: The StereoChorus class seems to have different parameter names
        // We'll need to check the actual implementation and adjust accordingly
        // For now, using the public member variables
        chorus->modulation = chorusRate;
    }
}

void EffectsProcessor::setChorusDepth(float depth)
{
    chorusDepth = juce::jlimit(0.0f, 1.0f, depth);
    if (chorus)
    {
        // Map depth to delay modulation amount
        chorus->delay = chorusDepth * 0.02f; // Scale to reasonable delay range
    }
}

void EffectsProcessor::setChorusFeedback(float feedback)
{
    chorusFeedback = juce::jlimit(-1.0f, 1.0f, feedback);
    if (chorus)
    {
        chorus->feedback = chorusFeedback;
    }
}

void EffectsProcessor::setChorusMix(float mix)
{
    chorusMix = juce::jlimit(0.0f, 1.0f, mix);
    if (chorus)
    {
        chorus->mix = chorusMix;
    }
}

//==============================================================================
// Delay methods
void EffectsProcessor::setDelayEnabled(bool enabled)
{
    delayEnabled = enabled;
    if (delay)
        delay->setEnabled(enabled);
}

void EffectsProcessor::setDelayLeftTime(float timeMs)
{
    delayLeftTime = juce::jlimit(0.0f, 2000.0f, timeMs);
    if (delay)
        delay->setDelay(StereoDelay::LEFT, delayLeftTime);
}

void EffectsProcessor::setDelayRightTime(float timeMs)
{
    delayRightTime = juce::jlimit(0.0f, 2000.0f, timeMs);
    if (delay)
        delay->setDelay(StereoDelay::RIGHT, delayRightTime);
}

void EffectsProcessor::setDelayLeftFeedback(float feedback)
{
    delayLeftFeedback = juce::jlimit(0.0f, 0.95f, feedback);
    if (delay)
        delay->setFeedback(StereoDelay::LEFT, delayLeftFeedback);
}

void EffectsProcessor::setDelayRightFeedback(float feedback)
{
    delayRightFeedback = juce::jlimit(0.0f, 0.95f, feedback);
    if (delay)
        delay->setFeedback(StereoDelay::RIGHT, delayRightFeedback);
}

void EffectsProcessor::setDelayMix(float mix)
{
    delayMix = juce::jlimit(0.0f, 1.0f, mix);
    if (delay)
    {
        delay->setMix(StereoDelay::LEFT, delayMix);
        delay->setMix(StereoDelay::RIGHT, delayMix);
    }
}