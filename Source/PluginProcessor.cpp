/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================
SamAudioProcessor::SamAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Stereo In", juce::AudioChannelSet::stereo(), true)
		.withInput("Mono In", juce::AudioChannelSet::mono(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
{
	fmtMgr = std::make_unique<juce::AudioFormatManager>();
	fmtMgr->registerBasicFormats();
	mappings = ControllerMappings();
	sequencer = new Sequencer();
}

SamAudioProcessor::~SamAudioProcessor()
{
	for (int i = 0; i < 128; i++) {
		delete samplers[i];
	}

	defaultSampler = nullptr;
	lpfLeftStage1 = nullptr;
	lpfRightStage1 = nullptr;
	delete tempBuffer;
	fmtMgr = nullptr;
	interpolatorLeft = nullptr;
	interpolatorRight = nullptr;
	delete sequencer;
}

//==============================================================================
const juce::String SamAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool SamAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool SamAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool SamAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double SamAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int SamAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int SamAudioProcessor::getCurrentProgram()
{
	return 0;
}

void SamAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String SamAudioProcessor::getProgramName(int index)
{
	return {};
}

void SamAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void SamAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
	bufferSize = samplesPerBlock;
	this->sampleRate = sampleRate;

	for (int i = 0; i < 128; i++) {
		samplers[i] = nullptr;
		voices[i] = false;
	}
	tempBuffer = new juce::AudioSampleBuffer(2, samplesPerBlock);

	interpolatorLeft = std::make_unique<CatmullRomInterpolator>();
	interpolatorRight = std::make_unique<CatmullRomInterpolator>();

	lpfLeftStage1 = std::make_unique<MultimodeFilter>();
	lpfRightStage1 = std::make_unique<MultimodeFilter>();


	lpfLeftStage1->coefficients(sampleRate, cutoff, resonance);
	lpfRightStage1->coefficients(sampleRate, cutoff, resonance);
	filterEnvelope = std::make_unique<juce::ADSR>();
	filterEnvelope->setSampleRate(sampleRate/256);
	defaultSampler = std::make_unique<Sampler>(sampleRate, bufferSize);
}

void SamAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SamAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void SamAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	if (isRecording) {
		recorder.writeAudioData(buffer);
	}
	else {
		// In case we have more outputs than inputs, this code clears any output
		// channels that didn't contain input data, (because these aren't
		// guaranteed to be empty - they may contain garbage).
		// This is here to avoid people getting screaming feedback
		// when they first compile a plugin, but obviously you don't need to keep
		// this code if your algorithm always overwrites all the output channels.
		for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
			buffer.clear(i, 0, buffer.getNumSamples());

		float* leftOut = buffer.getWritePointer(0);
		float* rightOut = buffer.getWritePointer(1);
		const float* leftIn = buffer.getReadPointer(0);
		const float* righIn = buffer.getReadPointer(1);

		for (int j = 0; j < 128; j++) {
			if (samplers[j] != nullptr) {

				for (int i = 0; i < bufferSize; i++) {

					envValue = samplers[j]->envelope->getNextSample();
					samplers[j]->nextSample();

					float left = samplers[j]->getCurrentSample(0) * envValue;
					float right = samplers[j]->getCurrentSample(1) * envValue;

					buffer.addSample(0, i, left);
					buffer.addSample(1, i, right);

				}

			}

		}

		if (filterEnvelope != nullptr) {
			float f = cutoff + (filterEnvelope->getNextSample() * amount * (22000 - cutoff));
			if (f < 0) {
				f = 0;
			}
			lpfLeftStage1->coefficients(sampleRate, f, resonance);
		}

		currentSample = (currentSample + bufferSize) % buffer.getNumSamples();
		magnitude = buffer.getMagnitude(currentSample, bufferSize);

		lpfLeftStage1->processStereo(leftOut, rightOut, buffer.getNumSamples());

		if (!events.empty()) {
			Event* e = events.top();
			events.pop();

			if (e != nullptr && e->getType() == Event::GATE) {

				if (e->getValue() > 0) {
					midiMessages.addEvent(MidiMessage::noteOn(1, e->getNote(), (juce::uint8)e->getValue()), currentSample);
				}
				else {
					midiMessages.addEvent(MidiMessage::noteOff(1, e->getNote(), (juce::uint8)e->getValue()), currentSample);
				}
				delete e;
			}
		}

		juce::MidiMessage m;
		int time;

		for (juce::MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);)
		{
			if (m.isNoteOn())
			{

				if (numVoices == 0) {
					filterEnvelope->noteOn();
				}

				numVoices++;

				if (samplers[m.getNoteNumber()] != nullptr) {
					samplers[m.getNoteNumber()]->envelope->noteOn(); //(m.getVelocity());
					samplers[m.getNoteNumber()]->setCurrentSample(samplers[m.getNoteNumber()]->getStartPosition());
					samplers[m.getNoteNumber()]->play();
					voices[m.getNoteNumber()] = true;
				}
				state.noteOn(m.getChannel(), m.getNoteNumber(), m.getVelocity() / 128);
			}
			if (m.isNoteOff())
			{
				if (numVoices > 0) {
					numVoices--;
				}
				else {
					filterEnvelope->noteOff();
				}

				if (samplers[m.getNoteNumber()] != nullptr) {
					//samplers[m.getNoteNumber()]->stop();

					samplers[m.getNoteNumber()]->envelope->noteOff();
					voices[m.getNoteNumber()] = false;
				}

				state.noteOff(m.getChannel(), m.getNoteNumber(), m.getVelocity() / 128);

			}
			if (m.isAftertouch())
			{
			}
			if (m.isPitchWheel())
			{
			}
			if (m.isController()) {
				if (learn) {
					mappings.addMapping(m.getControllerNumber(), learningControl);
					learn = false;
				}
				else {

					juce::Logger::getCurrentLogger()->writeToLog("controller " + juce::String(m.getControllerNumber()) + " value " + juce::String(m.getControllerValue()));

				juce:Component* c = mappings.getMapping(m.getControllerNumber());

					if (c != nullptr) {
						if (c->getName() == "Cutoff") {
							cutoff = (20000.0f / 127.0f) * m.getControllerValue();
							lpfLeftStage1->coefficients(sampleRate, cutoff, resonance);
						}
						else if (c->getName() == "Resonance") {
							resonance = (5.0f / 127.0f) * m.getControllerValue();
							lpfLeftStage1->coefficients(sampleRate, cutoff, resonance);
						}
						else if (c->getName() == "Amount") {
							amount = (1.0f / 127.0f) * m.getControllerValue();
							lpfLeftStage1->coefficients(sampleRate, cutoff, resonance);
						}

					}

					// Modulation wheel
					if (m.getControllerNumber() == 1) {

					}
				}
			}
			else {
				//( Logger::getCurrentLogger()->writeToLog("Other message : " + String(m.getTimeStamp()));
			}
		}
	}
}

//==============================================================================
bool SamAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SamAudioProcessor::createEditor()
{
	return new SamAudioProcessorEditor(*this);
}

//==============================================================================
void SamAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void SamAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SamAudioProcessor();
}
