/*
  ==============================================================================

    Sampler.h
    Created: 17 Apr 2018 1:12:15pm
    Author:  Matthias Pueski

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Oszillator.h"
#include "ADSR.h"
#include "MultimodeFilter.h"

class Sampler : public Oszillator {

public:
    
    Sampler(float sampleRate, int bufferSize);
    ~Sampler();

    void play();
    void stop();
    float getSampleAt(int channel, long position);
    void loadSample(juce::File file);
    void loadSample(juce::InputStream* input);
    void setStartPosition(long start);
    long getStartPosition();
    void setEndPosition(long end);
    long getEndPosition();
    void setSampleLength(long length);
    long getSampleLength();
    void setLoop(bool loop);
    void skipSamples(int num);
    void rewindSamples(int num);
    bool isLoop();
    void setVolume(float volume);
    float getVolume();
    juce::AudioSampleBuffer* getSampleBuffer();
    bool hasSample();
    void nextSample();
    void nextBlock();
	virtual float process() override;
    float getCurrentSample(int channel);
    
    void setReverse(bool shouldReverse) { reverse = shouldReverse; }
    bool isReverse() const { return reverse; }

    long getCurrentPosition() {
        return currentSample;
    }
    
    float getSampleRate() {
        return sampleRate;
    }
    
    void setCurrentSample(long num) {
        currentSample = num;
    }
    
    void reset(){
        currentSample = startPosition;
    }
    
    bool isDone() {
        return !loop && currentSample >= sampleLength - 1;
    }
    
    void setLoaded(bool loaded) {
        this->loaded = loaded;
    }
    
    void setDirty(bool dirty){
        this->dirty = dirty;
    }
    
    bool isDirty() {
        return dirty;
    }
       
    virtual void setPitch(float pitch);
    
    float getPitchAsFloat() {
        return pitch;
    }

    bool isPlaying() {
        return playing;
    }

    float getOutput(int channel) {
		return getCurrentSample(channel);
    }
    
    float getMagnitude(int channel) {
        if (currentSample < sampleLength)
            return sampleBuffer->getMagnitude(channel, currentSample, bufferSize / 4);
        else
            return 0;
    }

    juce::String getSampleLocation() {
        return sampleLocation;
    }

    juce::ADSR* getAmpEnvelope() {
        return ampEnvelope.get();
	}
    juce::ADSR* getFilterEnvelope() {
        return filterEnvelope.get();
    }

    float getPlaybackPercent() const;
    double getPlaybackPosition() const;

private:
    
    double playbackPosition = 0.0;          // Gleitkomma-Position im Sample
    double pitch = 1.0;                     // Pitch-Faktor (1.0 = normal)

    std::unique_ptr <juce::ADSR> ampEnvelope = nullptr;
    std::unique_ptr <juce::ADSR> filterEnvelope = nullptr;

    float sampleRate;
    int bufferSize;
    
    float volume = 0.5;
    
    juce::AudioSampleBuffer *sampleBuffer = nullptr;
    juce::AudioFormatManager* afm = nullptr;

    long currentSample = 0;
    
    long startPosition = 0;
    long endPosition = 0;
    long sampleLength = 0;
    
    bool loop = false;
    bool loaded = false;    
    bool playing = false;    
    bool dirty = false;
    bool reverse = false; // true = reverse playback
    
    float* tempBufferLeft = nullptr;
    float* tempBufferRight = nullptr;
    
    juce::String sampleLocation = "";

    juce::CatmullRomInterpolator* interpolatorLeft;
    juce::CatmullRomInterpolator* interpolatorRight;
    
    std::unique_ptr<MultimodeFilter> lpfLeftStage1 = nullptr;
    std::unique_ptr<MultimodeFilter> lpfRightStage1 = nullptr;

};
