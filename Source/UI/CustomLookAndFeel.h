/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 25 Nov 2016 5:14:44pm
    Author:  Matthias Pueski

  ==============================================================================
*/

#ifndef CUSTOMLOOKANDFEEL_H_INCLUDED
#define CUSTOMLOOKANDFEEL_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
    
public:
    
    CustomLookAndFeel() {
        setColourScheme(LookAndFeel_V4::getGreyColourScheme());
    
        gradientVertical = juce::ColourGradient(juce::Colours::red, 0, 0, juce::Colours::chartreuse, 5, 128, false);
        gradientVertical.addColour(0.2 , juce::Colours::grey);
        
        gradientHorizontal = juce::ColourGradient(juce::Colours::chartreuse, 0, 0, juce::Colours::red, 128, 5, false);
        gradientHorizontal.addColour(0.8 , juce::Colours::grey);
        
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::grey.darker());
        setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::thumbColourId, juce::Colours::transparentBlack);
        setColour(juce::DialogWindow::backgroundColourId, juce::Colour (0xff222222));
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour (0xff222222));
        setColour(juce::AlertWindow::backgroundColourId ,juce::Colour (0xff222222));
        setColour(juce::AlertWindow::outlineColourId ,juce::Colours::grey);
        setColour(juce::Label::textColourId, juce::Colours::white);
        setColour(juce::ComboBox::backgroundColourId, juce::Colour (0xff222222));
        setColour(juce::PopupMenu::backgroundColourId, juce::Colour (0xff2d2d2d));
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colours::grey.brighter());
        setColour(juce::PopupMenu::textColourId, juce::Colours::grey.brighter());
        setColour(juce::ComboBox::textColourId, juce::Colours::grey.brighter());
        setColour(juce::ComboBox::outlineColourId, juce::Colours::grey.brighter());
        setColour(juce::ComboBox::arrowColourId, juce::Colours::grey.brighter());
        setColour(juce::ComboBox::buttonColourId, juce::Colours::grey.brighter());
        setColour(juce::ListBox::backgroundColourId, juce::Colour (0xff222222));
        setColour(juce::ListBox::textColourId, juce::Colours::grey.brighter());
        setColour(juce::TextButton::buttonColourId, juce::Colours::grey.darker());
        setColour(juce::TextButton::buttonOnColourId, juce::Colours::grey.darker());
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        setColour(juce::TextEditor::outlineColourId, juce::Colours::grey.darker());
        setColour(juce::TextEditor::backgroundColourId, juce::Colour (0xff222222));
        setColour(juce::TextEditor::highlightColourId, juce::Colours::grey.darker());
        setColour(juce::TextEditor::highlightedTextColourId, juce::Colours::black);
        setColour(juce::TextEditor::textColourId, juce::Colours::grey.brighter());
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        setColour(juce::CaretComponent::caretColourId, juce::Colours::grey);
        
        setColour (juce::ScrollBar::thumbColourId,juce::Colours::grey.darker());
        setColour (juce::ScrollBar::backgroundColourId, juce::Colour::fromRGBA(0x5a,0x5a,0x5a,0xff));
        
        setColour (juce::Slider::thumbColourId,juce::Colours::grey);
        setColour (juce::Slider::trackColourId, juce::Colour::fromRGBA(0x5a,0x5a,0x5a,0xff));
    
    }
    
    void drawLinearSlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle, juce::Slider&) override;
    
    void drawRotarySlider(juce::Graphics& g,
                                                 int     x,
                                                 int     y,
                                                 int     width,
                                                 int     height,
                                                 float     sliderPosProportional,
                                                 float     rotaryStartAngle,
                                                 float     rotaryEndAngle,
                              juce::Slider &     slider ) override;
    void drawButtonBackground(juce::Graphics& g,
        juce::Button& button,
        const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;
private:
    juce::ColourGradient gradientVertical;
    juce::ColourGradient gradientHorizontal;
    juce::ComponentAnimator animator;
    
    long triggerTime = 0;
    int maxY = 0xFFFF;
    int maxX = 0;
};



#endif  // CUSTOMLOOKANDFEEL_H_INCLUDED
