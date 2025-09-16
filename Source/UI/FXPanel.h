/*
  ==============================================================================

    FXPanel.h
    Created: $(Date)
    Author:  GitHub Copilot

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
    FX Panel component for reverb, chorus, and stereo delay effects
*/
class FXPanel : public juce::Component, 
                public juce::Slider::Listener
{
public:
    //==============================================================================
    FXPanel();
    ~FXPanel() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

    //==============================================================================
    // Reverb controls
    juce::Slider* getReverbRoomSizeSlider() { return reverbRoomSizeSlider.get(); }
    juce::Slider* getReverbDampingSlider() { return reverbDampingSlider.get(); }
    juce::Slider* getReverbWetLevelSlider() { return reverbWetLevelSlider.get(); }
    juce::Slider* getReverbDryLevelSlider() { return reverbDryLevelSlider.get(); }
    juce::Slider* getReverbWidthSlider() { return reverbWidthSlider.get(); }

    // Chorus controls
    juce::Slider* getChorusRateSlider() { return chorusRateSlider.get(); }
    juce::Slider* getChorusDepthSlider() { return chorusDepthSlider.get(); }
    juce::Slider* getChorusFeedbackSlider() { return chorusFeedbackSlider.get(); }
    juce::Slider* getChorusMixSlider() { return chorusMixSlider.get(); }

    // Delay controls
    juce::Slider* getDelayLeftTimeSlider() { return delayLeftTimeSlider.get(); }
    juce::Slider* getDelayRightTimeSlider() { return delayRightTimeSlider.get(); }
    juce::Slider* getDelayLeftFeedbackSlider() { return delayLeftFeedbackSlider.get(); }
    juce::Slider* getDelayRightFeedbackSlider() { return delayRightFeedbackSlider.get(); }
    juce::Slider* getDelayMixSlider() { return delayMixSlider.get(); }

    // Enable/disable toggles
    juce::ToggleButton* getReverbEnabledButton() { return reverbEnabledButton.get(); }
    juce::ToggleButton* getChorusEnabledButton() { return chorusEnabledButton.get(); }
    juce::ToggleButton* getDelayEnabledButton() { return delayEnabledButton.get(); }

private:
    //==============================================================================
    // Reverb controls
    std::unique_ptr<juce::Slider> reverbRoomSizeSlider;
    std::unique_ptr<juce::Slider> reverbDampingSlider;
    std::unique_ptr<juce::Slider> reverbWetLevelSlider;
    std::unique_ptr<juce::Slider> reverbDryLevelSlider;
    std::unique_ptr<juce::Slider> reverbWidthSlider;
    std::unique_ptr<juce::ToggleButton> reverbEnabledButton;

    // Chorus controls
    std::unique_ptr<juce::Slider> chorusRateSlider;
    std::unique_ptr<juce::Slider> chorusDepthSlider;
    std::unique_ptr<juce::Slider> chorusFeedbackSlider;
    std::unique_ptr<juce::Slider> chorusMixSlider;
    std::unique_ptr<juce::ToggleButton> chorusEnabledButton;

    // Delay controls
    std::unique_ptr<juce::Slider> delayLeftTimeSlider;
    std::unique_ptr<juce::Slider> delayRightTimeSlider;
    std::unique_ptr<juce::Slider> delayLeftFeedbackSlider;
    std::unique_ptr<juce::Slider> delayRightFeedbackSlider;
    std::unique_ptr<juce::Slider> delayMixSlider;
    std::unique_ptr<juce::ToggleButton> delayEnabledButton;

    // Labels
    std::unique_ptr<juce::Label> reverbLabel;
    std::unique_ptr<juce::Label> chorusLabel;
    std::unique_ptr<juce::Label> delayLabel;

    std::unique_ptr<juce::Label> reverbRoomSizeLabel;
    std::unique_ptr<juce::Label> reverbDampingLabel;
    std::unique_ptr<juce::Label> reverbWetLevelLabel;
    std::unique_ptr<juce::Label> reverbDryLevelLabel;
    std::unique_ptr<juce::Label> reverbWidthLabel;

    std::unique_ptr<juce::Label> chorusRateLabel;
    std::unique_ptr<juce::Label> chorusDepthLabel;
    std::unique_ptr<juce::Label> chorusFeedbackLabel;
    std::unique_ptr<juce::Label> chorusMixLabel;

    std::unique_ptr<juce::Label> delayLeftTimeLabel;
    std::unique_ptr<juce::Label> delayRightTimeLabel;
    std::unique_ptr<juce::Label> delayLeftFeedbackLabel;
    std::unique_ptr<juce::Label> delayRightFeedbackLabel;
    std::unique_ptr<juce::Label> delayMixLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FXPanel)
};