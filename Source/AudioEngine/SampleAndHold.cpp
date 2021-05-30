/*
  ==============================================================================

	SampleAndHold.cpp
	Created: 3 Jun 2016 9:05:33pm
	Author:  Matthias Pueski

  ==============================================================================
*/

#include "SampleAndHold.h"

using juce::Random;

SampleAndHold::SampleAndHold(double sampleRate, int buffersize) : Oszillator(sampleRate) {
	this->volume = 1.0f;
	random = new Random();
	this->fine = 0.0f;
	this->buffersize = buffersize;
	currentTime = juce::Time::currentTimeMillis();

}

SampleAndHold::~SampleAndHold() {
	random = nullptr;
}

void SampleAndHold::reset() {
}

float SampleAndHold::process() {
	
	if (juce::Time::currentTimeMillis() - currentTime > (1/frequency)*1000) {
		currentTime = juce::Time::currentTimeMillis();
		value = (random->nextFloat()) * 1 * 10;
	}

	return value;

}

float SampleAndHold::getOutput() {
	return this->process();
}

void SampleAndHold::setFine(float fine) {
	this->fine = fine;
}

float SampleAndHold::getFine() const {
	return this->fine;
}
