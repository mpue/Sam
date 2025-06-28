/*
  ==============================================================================

    Sequencer.h
    Created: 7 Jun 2021 9:01:35am
    Author:  mpue

  ==============================================================================
*/

#pragma once
#include "UI/Cell.h"
#include "../JuceLibraryCode/JuceHeader.h"

class Sequencer {

public:
    enum SyncMode {
        INTERNAL,
        MIDI_CLOCK
    };

    void setRunning(bool running);

    juce::MidiMessage getNextMessage();

    void reset();

    void setTempo(float tempo) {
        this->tempo = tempo;
    }


    bool isRunning() {
        return running;
    }

    Sequencer();
    ~Sequencer();
    Cell grid[32][6];

    juce::uint8* getConfiguration();
    void setConfiguration(juce::uint8* data);

    int currentStep;

private:
    SyncMode syncMode = INTERNAL;
    int clockEventNum = 0;
    bool running = false;
    int lastTime = 0;
    float tempo = 120.0f;
    int steps = 16;
    int lanes = 6;
};