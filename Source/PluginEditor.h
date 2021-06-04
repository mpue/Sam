/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/EnvelopePanel.h"
#include "UI/CustomLookAndFeel.h"
#include "UI/VUMeter.h"
#include "UI/PropertyView.h"
#include "UI/SampleEditor.h"
//==============================================================================
/**
*/
class SamAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                 public juce::MidiKeyboardStateListener, 
                                 public juce::Button::Listener, 
                                 public juce::Slider::Listener,
                                 public juce::Timer,
                                 public juce::ChangeListener,
                                 public juce::MouseListener
                                 
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
    void changeListenerCallback(ChangeBroadcaster* source) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void saveSettings();
    void loadSettings();
    void loadFile(juce::File file);
    void mouseDown(const MouseEvent& event) override;
    std::unique_ptr<juce::MidiKeyboardComponent> keyboard;
    std::unique_ptr<EnvelopePanel> ampEnvelope = nullptr;
    std::unique_ptr<EnvelopePanel> filterEnvelope = nullptr;
    std::unique_ptr <juce::Button> loadButton = nullptr;
    std::unique_ptr <juce::Button> loadSetButton = nullptr;
    std::unique_ptr <juce::Button> saveSetButton = nullptr;
    std::unique_ptr <juce::Button> newSetButton = nullptr;
    std::unique_ptr <juce::Label> noteLabel = nullptr;
    std::unique_ptr <juce::ToggleButton> loopButton = nullptr;
    std::unique_ptr <juce::Slider> cutoffSlider = nullptr;
    std::unique_ptr <juce::Slider> resoSlider = nullptr;
    std::unique_ptr <juce::Slider> amtSlider = nullptr;
    std::unique_ptr <juce::Slider> driveSlider = nullptr;
    std::unique_ptr <juce::Label> cutoffLabel = nullptr;
    std::unique_ptr <juce::Label> resoLabel = nullptr;
    std::unique_ptr <juce::Label> amtLabel = nullptr;
    std::unique_ptr <juce::Label> driveLabel = nullptr;
    std::unique_ptr <VUMeter> vuMeter = nullptr;
    std::unique_ptr <PropertyView> propertyViewLeft = nullptr;
    std::unique_ptr <PropertyView> propertyViewRight = nullptr;
    std::unique_ptr <SampleEditor> sampleEditor = nullptr;
    std::unique_ptr <juce::Slider> pitchSlider = nullptr;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CustomLookAndFeel tlf;
    SamAudioProcessor& audioProcessor;
    juce::String currentFile;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamAudioProcessorEditor)
};
