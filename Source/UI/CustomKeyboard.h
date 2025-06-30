#pragma once

#include <JuceHeader.h>
#include <map>

class CustomKeyboard : public juce::MidiKeyboardComponent
{
public:
    CustomKeyboard(juce::MidiKeyboardState& state, juce::MidiKeyboardComponent::Orientation orientation)
        : juce::MidiKeyboardComponent(state, orientation)
    {}

    void setNoteColour(int midiNoteNumber, juce::Colour colour)
    {
        noteColours[midiNoteNumber] = colour;
        repaint();
    }

    void clearNoteColour(int midiNoteNumber)
    {
        noteColours.erase(midiNoteNumber);
        repaint();
    }

    void clearAllColours()
    {
        noteColours.clear();
        repaint();
    }

protected:
    virtual void drawWhiteNote(int midiNoteNumber,
        Graphics& g, Rectangle<float> area,
        bool isDown, bool isOver,
        Colour lineColour, Colour textColour)
    {
        juce::Colour fill = noteColours.count(midiNoteNumber) > 0
            ? noteColours[midiNoteNumber]
            : juce::Colours::white;

		if (isDown) 
            fill = fill.darker(); // Slightly darker when pressed

        g.setColour(fill);
        g.fillRect(area);

        g.setColour(lineColour);
        g.drawRect(area, 1.0f);
    }

    virtual void drawBlackNote(int midiNoteNumber,
        Graphics& g, Rectangle<float> area,
        bool isDown, bool isOver,
        Colour noteFillColour)
    {
        juce::Colour fill = noteColours.count(midiNoteNumber) > 0
            ? noteColours[midiNoteNumber]
            : juce::Colours::black;

        if (isDown)
			fill = fill.withAlpha(0.8f); // Slightly transparent when pressed

        g.setColour(fill);
        g.fillRect(area);
    }

private:
    std::map<int, juce::Colour> noteColours;
};
