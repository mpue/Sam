/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AudioEngine/Sampler.h"
#include "AudioEngine/ADSR.h"
#include "Sequencer.h"
#include "AudioEngine/MultimodeFilter.h"
#include "ControllerMappings.h"
#include "Event.h"
#include "AudioEngine/AudioRecorder.h"
#include <stack>
class SamAudioProcessorEditor;
//==============================================================================
/**
*/
class SamAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    SamAudioProcessor();
    ~SamAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    std::unique_ptr<juce::AudioFormatManager> fmtMgr = nullptr;
    int currentSampleIndex = 0;
    int bufferSize;
    double sampleRate;    
    bool voices[128];
    int numVoices = 0;

    std::unique_ptr<MultimodeFilter> lpfLeftStage1 = nullptr;
    std::unique_ptr<MultimodeFilter> lpfRightStage1 = nullptr;

    std::unique_ptr <juce::CatmullRomInterpolator> interpolatorLeft = nullptr;
    std::unique_ptr <juce::CatmullRomInterpolator> interpolatorRight = nullptr;

    float cutoff = 22000.0f;
    float resonance = 0.1f;
    float amount = 1.0f;
    float magnitude = 0;
    long currentSample = 0;

    bool learn = false;
    juce::Component* learningControl = nullptr;

    std::unique_ptr<Sampler> defaultSampler = nullptr;
    juce::MidiKeyboardState state;
    ControllerMappings mappings;
    std::stack<Event*> events;
    Sequencer* sequencer = nullptr;

    AudioRecorder recorder;
    bool isRecording = false;

    void saveSettings(juce::String currentDirectory);
    juce::String loadSettings();
    void loadFile(juce::File file);
    bool initialized = false;
    bool loaded = false;
    std::unique_ptr<Sampler> samplers[128] = { nullptr };

private:
    juce::File currentFile;
    float envValue = 0;
    juce::AudioSampleBuffer* tempBuffer = nullptr;
    SamAudioProcessorEditor* editor = nullptr;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamAudioProcessor)
};
