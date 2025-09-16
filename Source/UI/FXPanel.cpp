/*
  ==============================================================================

    FXPanel.cpp
    Created: $(Date)
    Author:  GitHub Copilot

  ==============================================================================
*/

#include "FXPanel.h"

//==============================================================================
FXPanel::FXPanel()
{
    //==============================================================================
    // Reverb Section
    reverbLabel = std::make_unique<juce::Label>("Reverb", "REVERB");
    reverbLabel->setFont(juce::Font(16.0f, juce::Font::bold));
    reverbLabel->setColour(juce::Label::textColourId, juce::Colours::white);
    reverbLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbLabel.get());

    reverbEnabledButton = std::make_unique<juce::ToggleButton>("On");
    reverbEnabledButton->setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(reverbEnabledButton.get());

    // Room Size
    reverbRoomSizeSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    reverbRoomSizeSlider->setRange(0.0, 1.0, 0.01);
    reverbRoomSizeSlider->setValue(0.5);
    reverbRoomSizeSlider->addListener(this);
    addAndMakeVisible(reverbRoomSizeSlider.get());

    reverbRoomSizeLabel = std::make_unique<juce::Label>("Room Size", "Room");
    reverbRoomSizeLabel->attachToComponent(reverbRoomSizeSlider.get(), false);
    reverbRoomSizeLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbRoomSizeLabel.get());

    // Damping
    reverbDampingSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    reverbDampingSlider->setRange(0.0, 1.0, 0.01);
    reverbDampingSlider->setValue(0.5);
    reverbDampingSlider->addListener(this);
    addAndMakeVisible(reverbDampingSlider.get());

    reverbDampingLabel = std::make_unique<juce::Label>("Damping", "Damp");
    reverbDampingLabel->attachToComponent(reverbDampingSlider.get(), false);
    reverbDampingLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbDampingLabel.get());

    // Wet Level
    reverbWetLevelSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    reverbWetLevelSlider->setRange(0.0, 1.0, 0.01);
    reverbWetLevelSlider->setValue(0.33);
    reverbWetLevelSlider->addListener(this);
    addAndMakeVisible(reverbWetLevelSlider.get());

    reverbWetLevelLabel = std::make_unique<juce::Label>("Wet Level", "Wet");
    reverbWetLevelLabel->attachToComponent(reverbWetLevelSlider.get(), false);
    reverbWetLevelLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbWetLevelLabel.get());

    // Dry Level
    reverbDryLevelSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    reverbDryLevelSlider->setRange(0.0, 1.0, 0.01);
    reverbDryLevelSlider->setValue(0.4);
    reverbDryLevelSlider->addListener(this);
    addAndMakeVisible(reverbDryLevelSlider.get());

    reverbDryLevelLabel = std::make_unique<juce::Label>("Dry Level", "Dry");
    reverbDryLevelLabel->attachToComponent(reverbDryLevelSlider.get(), false);
    reverbDryLevelLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbDryLevelLabel.get());

    // Width
    reverbWidthSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    reverbWidthSlider->setRange(0.0, 1.0, 0.01);
    reverbWidthSlider->setValue(1.0);
    reverbWidthSlider->addListener(this);
    addAndMakeVisible(reverbWidthSlider.get());

    reverbWidthLabel = std::make_unique<juce::Label>("Width", "Width");
    reverbWidthLabel->attachToComponent(reverbWidthSlider.get(), false);
    reverbWidthLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbWidthLabel.get());

    //==============================================================================
    // Chorus Section
    chorusLabel = std::make_unique<juce::Label>("Chorus", "CHORUS");
    chorusLabel->setFont(juce::Font(16.0f, juce::Font::bold));
    chorusLabel->setColour(juce::Label::textColourId, juce::Colours::white);
    chorusLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusLabel.get());

    chorusEnabledButton = std::make_unique<juce::ToggleButton>("On");
    chorusEnabledButton->setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(chorusEnabledButton.get());

    // Rate
    chorusRateSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    chorusRateSlider->setRange(0.1, 10.0, 0.1);
    chorusRateSlider->setValue(1.0);
    chorusRateSlider->addListener(this);
    addAndMakeVisible(chorusRateSlider.get());

    chorusRateLabel = std::make_unique<juce::Label>("Rate", "Rate");
    chorusRateLabel->attachToComponent(chorusRateSlider.get(), false);
    chorusRateLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusRateLabel.get());

    // Depth
    chorusDepthSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    chorusDepthSlider->setRange(0.0, 1.0, 0.01);
    chorusDepthSlider->setValue(0.25);
    chorusDepthSlider->addListener(this);
    addAndMakeVisible(chorusDepthSlider.get());

    chorusDepthLabel = std::make_unique<juce::Label>("Depth", "Depth");
    chorusDepthLabel->attachToComponent(chorusDepthSlider.get(), false);
    chorusDepthLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusDepthLabel.get());

    // Feedback
    chorusFeedbackSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    chorusFeedbackSlider->setRange(-1.0, 1.0, 0.01);
    chorusFeedbackSlider->setValue(0.0);
    chorusFeedbackSlider->addListener(this);
    addAndMakeVisible(chorusFeedbackSlider.get());

    chorusFeedbackLabel = std::make_unique<juce::Label>("Feedback", "FB");
    chorusFeedbackLabel->attachToComponent(chorusFeedbackSlider.get(), false);
    chorusFeedbackLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusFeedbackLabel.get());

    // Mix
    chorusMixSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    chorusMixSlider->setRange(0.0, 1.0, 0.01);
    chorusMixSlider->setValue(0.5);
    chorusMixSlider->addListener(this);
    addAndMakeVisible(chorusMixSlider.get());

    chorusMixLabel = std::make_unique<juce::Label>("Mix", "Mix");
    chorusMixLabel->attachToComponent(chorusMixSlider.get(), false);
    chorusMixLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusMixLabel.get());

    //==============================================================================
    // Delay Section
    delayLabel = std::make_unique<juce::Label>("Delay", "STEREO DELAY");
    delayLabel->setFont(juce::Font(16.0f, juce::Font::bold));
    delayLabel->setColour(juce::Label::textColourId, juce::Colours::white);
    delayLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayLabel.get());

    delayEnabledButton = std::make_unique<juce::ToggleButton>("On");
    delayEnabledButton->setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(delayEnabledButton.get());

    // Left Time
    delayLeftTimeSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    delayLeftTimeSlider->setRange(0.0, 2000.0, 1.0);
    delayLeftTimeSlider->setValue(250.0);
    delayLeftTimeSlider->addListener(this);
    addAndMakeVisible(delayLeftTimeSlider.get());

    delayLeftTimeLabel = std::make_unique<juce::Label>("Left Time", "L Time");
    delayLeftTimeLabel->attachToComponent(delayLeftTimeSlider.get(), false);
    delayLeftTimeLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayLeftTimeLabel.get());

    // Right Time
    delayRightTimeSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    delayRightTimeSlider->setRange(0.0, 2000.0, 1.0);
    delayRightTimeSlider->setValue(375.0);
    delayRightTimeSlider->addListener(this);
    addAndMakeVisible(delayRightTimeSlider.get());

    delayRightTimeLabel = std::make_unique<juce::Label>("Right Time", "R Time");
    delayRightTimeLabel->attachToComponent(delayRightTimeSlider.get(), false);
    delayRightTimeLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayRightTimeLabel.get());

    // Left Feedback
    delayLeftFeedbackSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    delayLeftFeedbackSlider->setRange(0.0, 0.95, 0.01);
    delayLeftFeedbackSlider->setValue(0.3);
    delayLeftFeedbackSlider->addListener(this);
    addAndMakeVisible(delayLeftFeedbackSlider.get());

    delayLeftFeedbackLabel = std::make_unique<juce::Label>("Left FB", "L FB");
    delayLeftFeedbackLabel->attachToComponent(delayLeftFeedbackSlider.get(), false);
    delayLeftFeedbackLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayLeftFeedbackLabel.get());

    // Right Feedback
    delayRightFeedbackSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    delayRightFeedbackSlider->setRange(0.0, 0.95, 0.01);
    delayRightFeedbackSlider->setValue(0.3);
    delayRightFeedbackSlider->addListener(this);
    addAndMakeVisible(delayRightFeedbackSlider.get());

    delayRightFeedbackLabel = std::make_unique<juce::Label>("Right FB", "R FB");
    delayRightFeedbackLabel->attachToComponent(delayRightFeedbackSlider.get(), false);
    delayRightFeedbackLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayRightFeedbackLabel.get());

    // Mix
    delayMixSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    delayMixSlider->setRange(0.0, 1.0, 0.01);
    delayMixSlider->setValue(0.3);
    delayMixSlider->addListener(this);
    addAndMakeVisible(delayMixSlider.get());

    delayMixLabel = std::make_unique<juce::Label>("Mix", "Mix");
    delayMixLabel->attachToComponent(delayMixSlider.get(), false);
    delayMixLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayMixLabel.get());
}

FXPanel::~FXPanel()
{
}

//==============================================================================
void FXPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2f2f2f));
    
    // Draw section dividers
    g.setColour(juce::Colours::darkgrey);
    int sectionWidth = getWidth() / 3;
    g.drawVerticalLine(sectionWidth, 30, getHeight() - 10);
    g.drawVerticalLine(2 * sectionWidth, 30, getHeight() - 10);
}

void FXPanel::resized()
{
    auto area = getLocalBounds();
    const int margin = 10;
    const int sectionWidth = (area.getWidth() - 4 * margin) / 3;
    const int knobSize = 60;
    const int buttonHeight = 25;
    const int labelHeight = 20;

    //==============================================================================
    // Reverb Section
    auto reverbArea = area.removeFromLeft(sectionWidth);
    reverbArea.reduce(margin, margin);

    reverbLabel->setBounds(reverbArea.removeFromTop(labelHeight));
    reverbEnabledButton->setBounds(reverbArea.removeFromTop(buttonHeight + 10));
    reverbEnabledButton->setTopLeftPosition(reverbEnabledButton->getPosition().getX(), reverbEnabledButton->getPosition().getY() - 10);
    reverbArea.removeFromTop(5); // spacing

    auto reverbKnobArea = reverbArea.removeFromTop(knobSize + 20);
    auto knobsPerRow = 3;
    auto knobWidth = reverbKnobArea.getWidth() / knobsPerRow;

    reverbRoomSizeSlider->setBounds(reverbKnobArea.removeFromLeft(knobWidth).reduced(5));
    reverbDampingSlider->setBounds(reverbKnobArea.removeFromLeft(knobWidth).reduced(5));
    reverbWetLevelSlider->setBounds(reverbKnobArea.removeFromLeft(knobWidth).reduced(5));

    reverbKnobArea = reverbArea.removeFromTop(knobSize + 30);
    reverbKnobArea.setTop(reverbKnobArea.getTopLeft().getY() + 20);
    knobWidth = reverbKnobArea.getWidth() / 2;
    reverbDryLevelSlider->setBounds(reverbKnobArea.removeFromLeft(knobWidth).reduced(5));
    reverbWidthSlider->setBounds(reverbKnobArea.removeFromLeft(knobWidth).reduced(5));

    area.removeFromLeft(margin);

    //==============================================================================
    // Chorus Section
    auto chorusArea = area.removeFromLeft(sectionWidth);
    chorusArea.reduce(margin, margin);

    chorusLabel->setBounds(chorusArea.removeFromTop(labelHeight));
    chorusEnabledButton->setBounds(chorusArea.removeFromTop(buttonHeight + 10));
    chorusEnabledButton->setTopLeftPosition(chorusEnabledButton->getPosition().getX(), chorusEnabledButton->getPosition().getY() - 10);
    chorusArea.removeFromTop(5); // spacing

    auto chorusKnobArea = chorusArea.removeFromTop(knobSize + 20);
    knobWidth = chorusKnobArea.getWidth() / 2;

    chorusRateSlider->setBounds(chorusKnobArea.removeFromLeft(knobWidth).reduced(5));
    chorusDepthSlider->setBounds(chorusKnobArea.removeFromLeft(knobWidth).reduced(5));


    chorusKnobArea = chorusArea.removeFromTop(knobSize + 30);
    chorusKnobArea.setTop(chorusKnobArea.getTopLeft().getY() + 20);
    chorusFeedbackSlider->setBounds(chorusKnobArea.removeFromLeft(knobWidth).reduced(5));
    chorusMixSlider->setBounds(chorusKnobArea.removeFromLeft(knobWidth).reduced(5));

    area.removeFromLeft(margin);

    //==============================================================================
    // Delay Section
    auto delayArea = area;
    delayArea.reduce(margin, margin);

    delayLabel->setBounds(delayArea.removeFromTop(labelHeight));
    delayEnabledButton->setBounds(delayArea.removeFromTop(buttonHeight + 10));
    delayEnabledButton->setTopLeftPosition(delayEnabledButton->getPosition().getX(), delayEnabledButton->getPosition().getY() - 10);
    delayArea.removeFromTop(5); // spacing

    auto delayKnobArea = delayArea.removeFromTop(knobSize + 20);
    knobWidth = delayKnobArea.getWidth() / 3;

    delayLeftTimeSlider->setBounds(delayKnobArea.removeFromLeft(knobWidth).reduced(5));
    delayRightTimeSlider->setBounds(delayKnobArea.removeFromLeft(knobWidth).reduced(5));
    delayMixSlider->setBounds(delayKnobArea.removeFromLeft(knobWidth).reduced(5));
    
    delayKnobArea = delayArea.removeFromTop(knobSize + 30);
    knobWidth = delayKnobArea.getWidth() / 2;
    delayKnobArea.setTop(delayKnobArea.getTopLeft().getY() + 20);
    delayLeftFeedbackSlider->setBounds(delayKnobArea.removeFromLeft(knobWidth).reduced(5));
    delayRightFeedbackSlider->setBounds(delayKnobArea.removeFromLeft(knobWidth).reduced(5));
}

void FXPanel::sliderValueChanged(juce::Slider* slider)
{
    // The parameter changes will be handled by the editor
    // This is just for any local UI updates if needed
}