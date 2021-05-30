/*
  ==============================================================================

    Arpeggiator.cpp
    Created: 28 Mar 2020 2:41:26pm
    Author:  mpue

  ==============================================================================
*/

#include "Arpeggiator.h"

Arpeggiator::Arpeggiator()
 {
	notes.clear();
}

  Arpeggiator::~Arpeggiator()
  {
  }

void Arpeggiator::prepareToPlay(double sampleRate, int samplesPerBlock) 
{

	octaves = 1;
	notes.clear();                          
	currentNote = 0;                        
	lastNoteValue = -1;                     
	time = 0.0;                             
	this->sampleRate = static_cast<float> (sampleRate); 
 }

void Arpeggiator::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer & midi)
{
	if (!enabled) {
		return;
	}
	// however we use the buffer to get timing information
	auto numSamples = buffer.getNumSamples();                                                    

	// get note duration
	auto noteDuration = static_cast<int> (std::ceil(sampleRate * 0.25f * (0.1f + (1.0f - (speed))))); 

	juce::MidiMessage msg;
	int ignore;

	for (juce::MidiBuffer::Iterator it(midi); it.getNextEvent(msg, ignore);)                        
	{
		if (msg.isNoteOn())  notes.add(msg.getNoteNumber());
		else if (msg.isNoteOff()) notes.removeValue(msg.getNoteNumber());
	}

	midi.clear();

	if ((time + numSamples) >= noteDuration)                                                      
	{
		auto offset = juce::jmax(0, juce::jmin((int)(noteDuration - time), numSamples - 1));               

		if (lastNoteValue > 0)                                                                 
		{
			midi.addEvent(juce::MidiMessage::noteOff(1, lastNoteValue + 12*currentOctave), offset);
			lastNoteValue = -1;
		}

		if (notes.size() > 0)                                                                     
		{
			currentNote = (currentNote + 1) % notes.size();
			lastNoteValue = notes[currentNote];
			midi.addEvent(juce::MidiMessage::noteOn(1, lastNoteValue + 12 * currentOctave, (juce::uint8)127), offset);
		}

	}

	time = (time + numSamples) % noteDuration;
	
	if (octaves > 0)
		currentOctave = (currentOctave + 1) % octaves;
}

