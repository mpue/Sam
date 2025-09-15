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
#include "AudioEngine/HardLimiter.h"
#include <stack>
#include "UI/KeyboardMappingEditor.h"

class SamAudioProcessorEditor;

//==============================================================================
/**
*/
class SamAudioProcessor : public juce::AudioProcessor, public juce::MidiKeyboardStateListener
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

    void handleNoteOn(juce::MidiKeyboardState* source,
        int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState* source,
        int midiChannel, int midiNoteNumber, float velocity) override;

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

    void setKeyboardEditor(KeyboardMappingEditor* editor);

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
    std::unique_ptr<Sequencer> sequencer = nullptr;
	std::unique_ptr<HardLimiter> hardLimiter = nullptr;

    AudioRecorder recorder;
    bool isRecording = false;


    void saveSettings(juce::String currentDirectory);
    juce::String loadSettings();
    void loadFile(juce::File file);
    bool initialized = false;
    bool loaded = false;
    std::unique_ptr<Sampler> samplers[128] = { nullptr };
    KeyboardMappingEditor* keyEditor = nullptr;
    
    // Zone data handling - using SampleZone from KeyboardMappingEditor.h
    std::vector<SampleZone> loadedZones;
    bool hasLoadedZoneData = false;
    
    // Method for editor to get and clear zone data
    std::vector<SampleZone> getAndClearLoadedZones() {
        auto zones = std::move(loadedZones);
        loadedZones.clear();
        hasLoadedZoneData = false;
        return zones;
    }
    
    bool hasZoneDataToLoad() const { return hasLoadedZoneData; }

    void setMasterVolume(float volume) { masterVolume = juce::jlimit(0.0f, 1.0f, volume); }
    float getMasterVolume() const { return masterVolume.load(); }

    void setVelocitySensitivity(float sensitivity) { velocitySensitivity = juce::jlimit(0.0f, 2.0f, sensitivity); }
    float getVelocitySensitivity() const { return velocitySensitivity.load(); }

    void setMaxPolyphony(int voices) { maxPolyphony = juce::jlimit(1, 32, voices); }
    int getMaxPolyphony() const { return maxPolyphony; }

private:

    int findOldestVoice(); // Helper method for voice stealing
    void stealVoice(int noteToSteal, int newNote, float velocity);

    juce::File currentFile;
    float envValue = 0;
    std::unique_ptr<juce::AudioSampleBuffer> tempBuffer = nullptr;
    SamAudioProcessorEditor* editor = nullptr;
    //==============================================================================

    // Voice management
    int maxPolyphony = 16; // Maximum number of simultaneous voices
    std::atomic<float> masterVolume{ 0.7f };
    std::atomic<float> velocitySensitivity{ 1.0f };

    // Compressor/Limiter
    std::unique_ptr<juce::dsp::Compressor<float>> compressor;
    std::unique_ptr<juce::dsp::Limiter<float>> limiter;

    // Voice stealing for polyphony management
    struct VoiceInfo {
        int midiNote;
        float velocity;
        int64_t startTime;
        bool isActive;
    };

    std::array<VoiceInfo, 128> voiceInfo;
    int64_t currentTimeStamp = 0;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamAudioProcessor)
};
