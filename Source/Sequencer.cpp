/*
  ==============================================================================

    Sequencer.cpp
    Created: 7 Jun 2021 9:01:35am
    Author:  mpue

  ==============================================================================
*/

#include "Sequencer.h"




Sequencer::Sequencer()
{
}

Sequencer::~Sequencer()
{
}

void Sequencer::setRunning(bool running)
{
    this->running = running;
}


juce::MidiMessage Sequencer::getNextMessage()
{

	if (clockEventNum % 1 == 0) {
		currentStep = (currentStep + 1) % steps;

		for (int y = 0; y < lanes; y++) {

			if (grid[currentStep][y].isEnabled()) {
				return juce::MidiMessage::noteOn(1, grid[currentStep][y].getNote(), (juce::uint8)grid[currentStep][y].getVelocity());
			}

			if (currentStep > 0) {

			}
			else {

			}

			// send event


		}
	}
	clockEventNum++;

	return juce::MidiMessage::allNotesOff(1);

}

void Sequencer::reset()
{
}

juce::uint8* Sequencer::getConfiguration() {

	juce::uint8* data = new juce::uint8[steps * lanes * 3 + 2];

	// store the dimensions of the sequence in the first two bytes

	data[0] = steps;
	data[1] = lanes;

	int index = 2;

	// now serialize the grid each note is a triple containing length, note and velocity
	// if the note id not enabled we assume that the length is zero.

	for (int x = 0; x < steps; x++) {
		for (int y = 0; y < lanes; y++) {

			if (grid[x][y].isEnabled())
				data[index] = 1;
			else
				data[index] = 0;

			data[index + 1] = grid[x][y].getNote();
			data[index + 2] = grid[x][y].getVelocity();
			index += 3;

		}
	}

	return data;
}

void Sequencer::setConfiguration(juce::uint8* data) {

	// sequence size is stored in the first two bytes

	juce::uint8 steps = data[0];
	juce::uint8 lanes = data[1];

	int index = 2;

	for (int x = 0; x < steps; x++) {
		for (int y = 0; y < lanes; y++) {
			grid[x][y].setEnabled(data[index] > 0);
			grid[x][y].setLength(data[index]);
			grid[x][y].setNote(data[index + 1]);
			grid[x][y].setVelocity(data[index + 2]);
			index += 3;

		}
	}

	this->steps = steps;
	this->lanes = lanes;
}