/*
  ==============================================================================

    HardLimiter.h
    Created: 14 Sep 2025 9:10:33pm
    Author:  mpue

  ==============================================================================
*/

#pragma once
#include <algorithm>
#include <cmath>

class HardLimiter
{
public:
    HardLimiter() : threshold(0.95f), enabled(true) {}

    ~HardLimiter() = default;

    void setThreshold(float newThreshold)
    {
        threshold = std::clamp(newThreshold, 0.1f, 1.0f);
    }

    float getThreshold() const
    {
        return threshold;
    }

    void setEnabled(bool isEnabled)
    {
        enabled = isEnabled;
    }

    bool isEnabled() const
    {
        return enabled;
    }

    // Process single sample
    float processSample(float sample)
    {
        if (!enabled)
            return sample;

        // Hard clipping
        if (sample > threshold)
            return threshold;
        else if (sample < -threshold)
            return -threshold;
        else
            return sample;
    }

    // Process audio buffer
    void processBlock(float* buffer, int numSamples)
    {
        if (!enabled)
            return;

        for (int i = 0; i < numSamples; ++i)
        {
            buffer[i] = processSample(buffer[i]);
        }
    }

    // Process stereo buffer
    void processBlock(float* leftBuffer, float* rightBuffer, int numSamples)
    {
        if (!enabled)
            return;

        for (int i = 0; i < numSamples; ++i)
        {
            leftBuffer[i] = processSample(leftBuffer[i]);
            rightBuffer[i] = processSample(rightBuffer[i]);
        }
    }

    // Process interleaved stereo buffer
    void processBlockInterleaved(float* buffer, int numSamples)
    {
        if (!enabled)
            return;

        for (int i = 0; i < numSamples * 2; i += 2)
        {
            buffer[i] = processSample(buffer[i]);         // Left
            buffer[i + 1] = processSample(buffer[i + 1]); // Right
        }
    }

private:
    float threshold;
    bool enabled;
};