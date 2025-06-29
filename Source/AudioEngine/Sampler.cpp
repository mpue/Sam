/*
  ==============================================================================

	Sampler.cpp
	Created: 17 Apr 2018 1:12:15pm
	Author:  Matthias Pueski

  ==============================================================================
*/

#include "Sampler.h"


using juce::CatmullRomInterpolator;
using juce::AudioSampleBuffer;
using juce::File;
using juce::AudioFormatReader;
using juce::ScopedPointer;
using juce::AudioFormatReaderSource;
using juce::InputStream;

Sampler::Sampler(float sampleRate, int bufferSize) {

	juce::Logger::getCurrentLogger()->writeToLog("Creating sample player with sample rate of " + juce::String(sampleRate) + " kHz");
	this->sampleRate = sampleRate;
	this->bufferSize = bufferSize;

	this->interpolatorLeft = new CatmullRomInterpolator();
	this->interpolatorRight = new CatmullRomInterpolator();
	this->sampleBuffer = new AudioSampleBuffer(2, 1024 * 1024);
	afm = new  juce::AudioFormatManager();
	afm->registerBasicFormats();
	envelope = new juce::ADSR();
	envelope->setSampleRate(sampleRate*5);
	
}

Sampler::~Sampler() {
	if (sampleBuffer != nullptr) {
		delete sampleBuffer;
	}
	if (tempBufferLeft != nullptr)
		delete this->tempBufferLeft;
	if (tempBufferRight != nullptr)
		delete this->tempBufferRight;


	delete interpolatorLeft;
	delete interpolatorRight;
	delete afm;
	delete envelope;
}

float Sampler::process() {

	
	nextSample();
	return getCurrentSample(0) + getCurrentSample(1);
}

void Sampler::nextSample() {
	if (!isDone()) {
		playbackPosition += pitch;

		if (isLoop() && playbackPosition >= endPosition) {
			playbackPosition = startPosition;
		}
	}

	if (!envelope->isActive()) {
		playing = false;
	}
}

void Sampler::nextBlock()
{

}

void Sampler::play() 
{
	playbackPosition = startPosition;
	playing = true;
}

void Sampler::stop() {

	playing = false;
}
float Sampler::getCurrentSample(int channel) {
	if (!sampleBuffer || !loaded || isDone())
		return 0.0f;

	auto* in = sampleBuffer->getReadPointer(channel);
	int total = sampleBuffer->getNumSamples();

	if (playbackPosition >= total - 1)
		return 0.0f;

	// Lineare Interpolation
	int index = static_cast<int>(playbackPosition);
	float frac = static_cast<float>(playbackPosition - index);

	float sample1 = in[index];
	float sample2 = (index + 1 < total) ? in[index + 1] : 0.0f;

	return (sample1 + frac * (sample2 - sample1)) * volume;
}


float Sampler::getSampleAt(int channel, long pos) {
	return sampleBuffer->getSample(channel, static_cast<int>(pos)) * volume;
}

void Sampler::loadSample(File file) {
	
	if (!file.getFileExtension().toLowerCase().contains("wav") &&
		!file.getFileExtension().toLowerCase().contains("aif") &&
		!file.getFileExtension().toLowerCase().contains("aiff") &&
		!file.getFileExtension().toLowerCase().contains("mp3") &&	
		!file.getFileExtension().toLowerCase().contains("ogg")) {
		return;
	}

	AudioFormatReader* reader = afm->createReaderFor(file);

	sampleLocation = file.getFullPathName();

	if (reader == nullptr) {
		return;
	}

	ScopedPointer<AudioFormatReaderSource> afr = new AudioFormatReaderSource(reader, true);

	if (sampleBuffer != nullptr) {
		delete sampleBuffer;
	}
	if (tempBufferLeft != nullptr) {
		delete tempBufferLeft;
	}
	if (tempBufferRight != nullptr) {
		delete tempBufferRight;
	}

	sampleBuffer = new AudioSampleBuffer(2, static_cast<int>(reader->lengthInSamples));
	this->tempBufferLeft = new float[reader->lengthInSamples * 2];
	this->tempBufferRight = new float[reader->lengthInSamples * 2];
	reader->read(sampleBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
	juce::Logger::getCurrentLogger()->writeToLog("Sample rate : " + juce::String(reader->sampleRate));
	sampleLength = reader->lengthInSamples;
	endPosition = sampleLength / 2;
	startPosition = 0;
	currentSample = 0;
	loaded = true;
	// setPitch(reader->sampleRate / sampleRate);
	sampleRate = reader->sampleRate;
	reader = nullptr;
	
}

void Sampler::loadSample(InputStream* input) {

	if (sampleBuffer != nullptr) {
		delete sampleBuffer;
	}
	if (tempBufferLeft != nullptr) {
		delete tempBufferLeft;
	}
	if (tempBufferRight != nullptr) {
		delete tempBufferRight;
	}
	juce::WavAudioFormat waf;
	AudioFormatReader* reader = waf.createReaderFor(input, true);
	ScopedPointer<AudioFormatReaderSource> afr = new AudioFormatReaderSource(reader, true);
	sampleBuffer = new AudioSampleBuffer(2, static_cast<int>(reader->lengthInSamples));
	this->tempBufferLeft = new float[reader->lengthInSamples * 2];
	this->tempBufferRight = new float[reader->lengthInSamples * 2];

	reader->read(sampleBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
	sampleRate = reader->sampleRate;
	sampleLength = reader->lengthInSamples;
	endPosition = sampleLength;
	startPosition = 0;
	loaded = true;
	reader = nullptr;
}


void Sampler::setStartPosition(long start) {
	this->startPosition = start;
	this->currentSample = startPosition;
}

long Sampler::getStartPosition() {
	return startPosition;
}

void Sampler::setEndPosition(long end) {
	this->endPosition = end;
}

long Sampler::getEndPosition() {
	return this->endPosition;
}

void Sampler::setSampleLength(long length) {
	this->sampleLength = length;
}

long Sampler::getSampleLength() {
	return this->sampleLength;
}

void Sampler::setLoop(bool loop) {
	this->loop = loop;
}

void Sampler::setPitch(float newPitch) {
	if (newPitch < 0.25f)
		newPitch = 0.25f;
	if (newPitch > 2.0f)
		newPitch = 2.0f;

	pitch = newPitch;
}


bool Sampler::isLoop() {
	return this->loop;
}

void Sampler::setVolume(float volume) {
	this->volume = volume;
}

AudioSampleBuffer* Sampler::getSampleBuffer() {
	return sampleBuffer;
}

bool Sampler::hasSample() {
	return loaded;
}

void Sampler::skipSamples(int num) {
	if (currentSample + num < getSampleLength()) {
		currentSample += num;
	}
}

void Sampler::rewindSamples(int num) {
	if (currentSample - num >= 0) {
		currentSample -= num;
	}
}

float Sampler::getPlaybackPercent() const {
	if (sampleLength <= 0)
		return 0.0f;
	return static_cast<float>(playbackPosition) / static_cast<float>(sampleLength);
}

double Sampler::getPlaybackPosition() const {
	return playbackPosition;
}
