#pragma once


#include "../JuceLibraryCode/JuceHeader.h"
#include "Cell.h"
#include "../Sequencer.h"
#include "../PluginProcessor.h"

class SequenceEditor : public juce::Component,
    public juce::AudioProcessorPlayer,
    public juce::Timer,
    public juce::KeyListener,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:

    
    SequenceEditor(SamAudioProcessor* p, Sequencer* s);
    ~SequenceEditor();

    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message);// override;

    void timerCallback();// override;
    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* buttonThatWasClicked) override;
    void mouseMove (const juce::MouseEvent& e) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;
    bool keyPressed (const juce::KeyPress& key) override;
    void modifierKeysChanged (const juce::ModifierKeys& modifiers) override;
    void clearSequence();
    void sliderValueChanged(juce::Slider* slider) override;
    
    
    class SequencePanel : public juce::Component {
        
    public:
        Sequencer* sequencer;
        
        SequencePanel(Sequencer* sequencer) {
            this->sequencer = sequencer;
        }

        void paint (juce::Graphics& g) override {
            
            g.fillAll (juce::Colour (0xff323e44));
            
            g.setColour(juce::Colours::green);
            g.fillRect(selectedCol * cellSize, selectedRow*cellSize,cellSize, cellSize);
            
            for (int x = 0; x < gridWidth;x++) {
                for (int y = 0; y < gridHeight; y++) {
                    
                    int velocity = sequencer->grid[x][y].getVelocity();
                    
                    if (sequencer->currentStep == x) {
                         g.setColour(juce::Colours::orange);
                    }
                    else {
                        if (sequencer->grid[x][y].isEnabled()) {
                            
                            g.setColour(juce::Colour::fromRGB(velocity*2,165,0));
                        }
                        else {
                            g.setColour(juce::Colours::darkgrey);
                        }
                    }
                    
                    g.fillRect(x*cellSize+2, y*cellSize+2,cellSize-4 ,cellSize-4);
                    
                    g.setColour(juce::Colours::white);
                    
                    int octave = sequencer->grid[x][y].getNote() / 12 - 1;
                    
                    if (showVelocity) {
                        
                        g.drawText(juce::String(velocity),x*cellSize+1, y*cellSize+1, cellSize,cellSize,juce::Justification::centred);
                    }
                    else {
                        g.drawText(notes[sequencer->grid[x][y].getNote() % 12]+juce::String(octave),x*cellSize+1, y*cellSize+1, cellSize,cellSize,juce::Justification::centred);
                    }
                }
            }
        }
    };

private:

    int mouseX;
    int mouseY;

    static int selectedCol;
    static int selectedRow;

    static int gridWidth;
    static int gridHeight;
    const static int cellSize = 32;

    static const juce::String notes[128];

    static bool showVelocity;
    static bool optionPressed;

    juce::ScopedPointer<juce::TextButton> playButton;
    juce::ScopedPointer<juce::TextButton> recordButton;
    juce::ScopedPointer<juce::TextButton> clearButton;
    juce::ScopedPointer<juce::Slider> lengthSlider;
    
    juce::Viewport* view = nullptr;
    SequencePanel* content = nullptr;
    Sequencer* sequencer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequenceEditor)
};


