#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "UI/GraphicalEnvelope.h"
#include "UI/EnvelopePanel.h"
#include "UI/CustomLookAndFeel.h"
#include "UI/VUMeter.h"
#include "UI/PropertyView.h"
#include "UI/SampleEditor.h"
#include "UI/CustomKeyboard.h"
#include "UI/KeyboardMappingEditor.h"

//==============================================================================

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

    enum class UIMode { Main, Map, FX, Mod, Sequencer };

    

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
    void mouseDown(const MouseEvent& event) override;

    std::unique_ptr<CustomKeyboard> keyboard;
    std::unique_ptr <KeyboardMappingEditor> mappingEditor = nullptr;
    std::unique_ptr<EnvelopePanel> ampEnvelope = nullptr;
    std::unique_ptr<EnvelopePanel> filterEnvelope = nullptr;
    std::unique_ptr <juce::Button> loadButton = nullptr;
    std::unique_ptr <juce::Button> loadSetButton = nullptr;
    std::unique_ptr <juce::Button> saveSetButton = nullptr;
    std::unique_ptr <juce::Button> newSetButton = nullptr;
    std::unique_ptr <juce::Label> noteLabel = nullptr;
    std::unique_ptr <juce::ToggleButton> loopButton = nullptr;
    std::unique_ptr <juce::ToggleButton> reverseButton = nullptr;
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
    std::unique_ptr <juce::Button> mainViewButton = nullptr;    
    std::unique_ptr <juce::Button> mapViewButton = nullptr;
    std::unique_ptr <juce::Button> fxViewButton = nullptr;
    std::unique_ptr <juce::Button> modViewButton = nullptr;
    std::unique_ptr <juce::Button> sequencerViewButton = nullptr;
    std::unique_ptr <juce::Button> recordButton = nullptr;

    UIMode currentMode = UIMode::Main;
    std::vector<SampleZone> zones;

private:

    CustomLookAndFeel tlf;
    SamAudioProcessor& audioProcessor;

    juce::File currentSetFile;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamAudioProcessorEditor)
};

