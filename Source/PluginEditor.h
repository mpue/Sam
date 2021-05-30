/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/GraphicalEnvelope.h"
#include "UI/CustomLookAndFeel.h"
//==============================================================================
/**
*/
class SamAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                 public juce::MidiKeyboardStateListener, 
                                 public juce::Button::Listener, 
                                 public juce::Slider::Listener,
                                 public juce::Timer
                                 
{
public:
    SamAudioProcessorEditor (SamAudioProcessor&);
    ~SamAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void handleNoteOn(juce::MidiKeyboardState* source,
        int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState* source,
        int midiChannel, int midiNoteNumber, float velocity) override;

    void buttonClicked(juce::Button* button) override;    
    void timerCallback() override;
    void sliderValueChanged(juce::Slider* slider) override;

    std::unique_ptr<juce::MidiKeyboardComponent> keyboard;
    juce::MidiKeyboardState state;
    std::unique_ptr<juce::AudioThumbnailCache> cache = nullptr;
    std::unique_ptr<juce::AudioThumbnail> thumbnail = nullptr;
    std::unique_ptr<GraphicalEnvelope> ampEnvelope = nullptr;
    std::unique_ptr<GraphicalEnvelope> filterEnvelope = nullptr;
    std::unique_ptr <juce::Button> loadButton = nullptr;
    std::unique_ptr <juce::Button> loadSetButton = nullptr;
    std::unique_ptr <juce::Button> saveSetButton = nullptr;
    std::unique_ptr <juce::Label> noteLabel = nullptr;
    std::unique_ptr <juce::ToggleButton> loopButton = nullptr;
    std::unique_ptr <juce::Slider> cutoffSlider = nullptr;
    std::unique_ptr <juce::Slider> resoSlider = nullptr;
    std::unique_ptr <juce::Slider> amtSlider = nullptr;
    std::unique_ptr <juce::Slider> driveSlider = nullptr;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CustomLookAndFeel tlf;
    SamAudioProcessor& audioProcessor;
    float samplePosX = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamAudioProcessorEditor)
};
