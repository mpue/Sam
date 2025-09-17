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

	.withInput("Stereo In", juce::AudioChannelSet::stereo(), true)
		.withInput("Mono In", juce::AudioChannelSet::mono(), true)

		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
{
	fmtMgr = std::make_unique<juce::AudioFormatManager>();
	fmtMgr->registerBasicFormats();
	mappings = ControllerMappings();
	sequencer = std::make_unique<Sequencer>();
	state.addListener(this);
}

SamAudioProcessor::~SamAudioProcessor()
{
	for (int i = 0; i < 128; i++) {
		samplers[i] = nullptr;
	}

	defaultSampler = nullptr;
	lpfLeftStage1 = nullptr;
	lpfRightStage1 = nullptr;
	tempBuffer = nullptr;
	fmtMgr = nullptr;
	interpolatorLeft = nullptr;
	interpolatorRight = nullptr;
	sequencer = nullptr;
}

//==============================================================================
const juce::String SamAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

void SamAudioProcessor::handleNoteOn(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
	currentTimeStamp++;

	// Count active voices
	int activeVoices = 0;
	for (int i = 0; i < 128; i++) {
		if (voiceInfo[i].isActive) {
			activeVoices++;
		}
	}

	// Voice stealing if too many voices
	if (activeVoices >= maxPolyphony) {
		int voiceToSteal = findOldestVoice();
		if (voiceToSteal >= 0) {
			stealVoice(voiceToSteal, midiNoteNumber, velocity);
			return;
		}
	}

	// Apply velocity sensitivity
	float adjustedVelocity = std::pow(velocity, velocitySensitivity.load());
	adjustedVelocity = juce::jlimit(0.0f, 1.0f, adjustedVelocity);

	// Update voice info
	voiceInfo[midiNoteNumber] = { midiNoteNumber, adjustedVelocity, currentTimeStamp, true };

	if (samplers[midiNoteNumber] != nullptr) {
		if (numVoices == 0) {
			samplers[midiNoteNumber]->getFilterEnvelope()->noteOn();
		}

		numVoices++;
		samplers[midiNoteNumber]->getAmpEnvelope()->noteOn();
		samplers[midiNoteNumber]->setVolume(adjustedVelocity);
		samplers[midiNoteNumber]->setCurrentSample(samplers[midiNoteNumber]->getStartPosition());
		samplers[midiNoteNumber]->play();
		voices[midiNoteNumber] = true;
	}
	else {
		int note = findZoneForNoteAndVelocity(midiNoteNumber, (int)(adjustedVelocity * 127));
		Logger::getCurrentLogger()->writeToLog("NoteOn : Zone for note " + String(midiNoteNumber) + " : " + String(note));

		if (note >= 0) {
			SampleZone* zone = getZone(note);
			Sampler* sampler = zone->sampler.get();
			sampler->getFilterEnvelope()->noteOn();
			sampler->getAmpEnvelope()->noteOn();
			sampler->setVolume(adjustedVelocity);
			sampler->setCurrentSample(0);
			sampler->play();
			voices[midiNoteNumber] = true;
		}
	}

	numVoices++;
}

void SamAudioProcessor::handleNoteOff(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
	// Update voice info
	voiceInfo[midiNoteNumber].isActive = false;

	if (samplers[midiNoteNumber] != nullptr) {
		samplers[midiNoteNumber]->getFilterEnvelope()->noteOff();
		samplers[midiNoteNumber]->getAmpEnvelope()->noteOff();
		voices[midiNoteNumber] = false;
	}
	else {

		std::vector<int> zoneIndices = findAllZonesForNote(midiNoteNumber);

		for (int i = 0; i < zoneIndices.size(); i++) {
			int zoneIndex = zoneIndices[i];
			SampleZone* zone = getZone(zoneIndex);
			Sampler* sampler = zone->sampler.get();
			sampler->getFilterEnvelope()->noteOff();
			sampler->getAmpEnvelope()->noteOff();
		}
	}

	if (numVoices > 0) {
		numVoices--;
	}
	voices[midiNoteNumber] = false;
}

int SamAudioProcessor::findZoneForNote(int midiNote) const
{
	for (int i = 0; i < static_cast<int>(zones.size()); ++i)
	{
		const auto& zone = zones[static_cast<size_t>(i)];
		if (isValidZone(zone) &&
			midiNote >= zone.startNote && midiNote <= zone.endNote)
		{
			return i;
		}
	}
	return -1;
}

int SamAudioProcessor::findZoneForNoteAndVelocity(int midiNote, int velocity) const
{
	for (int i = 0; i < static_cast<int>(zones.size()); ++i)
	{
		const auto& zone = zones[static_cast<size_t>(i)];
		if (isValidZone(zone) &&
			midiNote >= zone.startNote && midiNote <= zone.endNote &&
			velocity >= zone.velLow && velocity <= zone.velHigh)
		{
			return i;
		}
	}
	return -1;
}

std::vector<int> SamAudioProcessor::findAllZonesForNote(int midiNote) const
{
	std::vector<int> matchingZones;
	for (int i = 0; i < static_cast<int>(zones.size()); ++i)
	{
		const auto& zone = zones[static_cast<size_t>(i)];
		if (isValidZone(zone) &&
			midiNote >= zone.startNote && midiNote <= zone.endNote)
		{
			matchingZones.push_back(i);
		}
	}
	return matchingZones;
}

std::vector<int> SamAudioProcessor::findAllZonesForNoteAndVelocity(int midiNote, int velocity) const
{
	std::vector<int> matchingZones;
	for (int i = 0; i < static_cast<int>(zones.size()); ++i)
	{
		const auto& zone = zones[static_cast<size_t>(i)];
		if (isValidZone(zone) &&
			midiNote >= zone.startNote && midiNote <= zone.endNote &&
			velocity >= zone.velLow && velocity <= zone.velHigh)
		{
			matchingZones.push_back(i);
		}
	}
	return matchingZones;
}

int SamAudioProcessor::findBestZoneForNoteAndVelocity(int midiNote, int velocity) const
{
	int bestZone = -1;
	int smallestVelRange = 128;

	for (int i = 0; i < static_cast<int>(zones.size()); ++i)
	{
		const auto& zone = zones[static_cast<size_t>(i)];
		if (isValidZone(zone) &&
			midiNote >= zone.startNote && midiNote <= zone.endNote &&
			velocity >= zone.velLow && velocity <= zone.velHigh)
		{
			const int velRange = zone.velHigh - zone.velLow + 1;
			if (velRange < smallestVelRange)
			{
				smallestVelRange = velRange;
				bestZone = i;
			}
		}
	}
	return bestZone;
}

bool SamAudioProcessor::isValidZone(const SampleZone& z) noexcept
{
	return z.startNote >= 0 && z.endNote <= 127 && z.startNote <= z.endNote &&
		z.velLow >= 1 && z.velHigh <= 127 && z.velLow <= z.velHigh;
}

SampleZone* SamAudioProcessor::getZone(int index) 
{
	if (index >= 0 && index < static_cast<int>(zones.size()))
		return &zones[static_cast<size_t>(index)];
	return nullptr;
}


int SamAudioProcessor::getNumZones() const noexcept
{
	return static_cast<int>(zones.size());
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
	bufferSize = samplesPerBlock;
	this->sampleRate = sampleRate;

	for (int i = 0; i < 128; i++) {
		voices[i] = false;
		voiceInfo[i] = { -1, 0.0f, 0, false };
	}

	tempBuffer = std::make_unique<juce::AudioSampleBuffer>(2, samplesPerBlock);

	interpolatorLeft = std::make_unique<CatmullRomInterpolator>();
	interpolatorRight = std::make_unique<CatmullRomInterpolator>();

	lpfLeftStage1 = std::make_unique<MultimodeFilter>();
	lpfRightStage1 = std::make_unique<MultimodeFilter>();

	lpfLeftStage1->coefficients(sampleRate, cutoff, resonance);
	lpfRightStage1->coefficients(sampleRate, cutoff, resonance);

	defaultSampler = std::make_unique<Sampler>(sampleRate, bufferSize);

	// Initialize compressor
	compressor = std::make_unique<juce::dsp::Compressor<float>>();
	compressor->setRatio(4.0f);
	compressor->setThreshold(-12.0f);
	compressor->setAttack(1.0f);
	compressor->setRelease(50.0f);

	// Initialize limiter
	limiter = std::make_unique<juce::dsp::Limiter<float>>();
	limiter->setThreshold(-0.1f);
	limiter->setRelease(5.0f);

	// Prepare DSP components
	juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = 2;

	compressor->prepare(spec);
	limiter->prepare(spec);

	// Initialize effects processor
	effectsProcessor = std::make_unique<EffectsProcessor>();
	effectsProcessor->prepareToPlay(sampleRate, samplesPerBlock);

	if (currentFile.existsAsFile()) {
		loadFile(currentFile);	
		getAndClearLoadedZones();
		loaded = true;
	}

	hardLimiter = std::make_unique<HardLimiter>();
	hardLimiter->setThreshold(0.85f); // Lower threshold for safety
}

// Helper methods for voice management
int SamAudioProcessor::findOldestVoice()
{
	int oldestVoice = -1;
	int64_t oldestTime = std::numeric_limits<int64_t>::max();

	for (int i = 0; i < 128; i++) {
		if (voiceInfo[i].isActive && voiceInfo[i].startTime < oldestTime) {
			oldestTime = voiceInfo[i].startTime;
			oldestVoice = i;
		}
	}

	return oldestVoice;
}

void SamAudioProcessor::stealVoice(int noteToSteal, int newNote, float velocity)
{
	// Stop the old voice
	handleNoteOff(nullptr, 1, noteToSteal, 0.0f);

	// Start the new voice
	handleNoteOn(nullptr, 1, newNote, velocity);
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
		// Clear output channels
		for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
			buffer.clear(i, 0, buffer.getNumSamples());

		float* leftOut = buffer.getWritePointer(0);
		float* rightOut = buffer.getWritePointer(1);
		const float* leftIn = buffer.getReadPointer(0);
		const float* righIn = buffer.getReadPointer(1);

		// Count active voices for automatic gain reduction
		int activeVoices = 0;
		for (int j = 0; j < 128; j++) {
			if (samplers[j] != nullptr && voices[j]) {
				activeVoices++;
			}
		}


		for (int j = 0; j < getNumZones(); j++) {
			SampleZone* zone = getZone(j);
			if (zone->sampler != nullptr && zone->sampler->isPlaying()) {
				activeVoices++;
			}
		}


		// Calculate automatic gain reduction based on active voices
		float voiceGainReduction = 1.0f;
		if (activeVoices > 1) {
			// Reduce gain logarithmically with more voices
			voiceGainReduction = 1.0f / std::sqrt(static_cast<float>(activeVoices));
			// Cap minimum gain to prevent complete silence
			voiceGainReduction = std::max(voiceGainReduction, 0.1f);
		}

		// Master volume control - using the actual master volume setting
		float masterVolumeValue = masterVolume.load();
		float finalGain = masterVolumeValue; // * voiceGainReduction;

		// Process legacy samplers
		for (int j = 0; j < 128; j++) {
			if (samplers[j] != nullptr && voices[j]) {
				for (int i = 0; i < bufferSize; i++) {
					envValue = samplers[j]->getAmpEnvelope()->getNextSample();
					samplers[j]->nextSample();

					float left = samplers[j]->getCurrentSample(0) * envValue * finalGain;
					float right = samplers[j]->getCurrentSample(1) * envValue * finalGain;

					//// Soft clipping to prevent hard clipping
					//left = std::tanh(left * 0.8f);
					//right = std::tanh(right * 0.8f);

					buffer.addSample(0, i, left);
					buffer.addSample(1, i, right);
				}

				if (samplers[j]->getFilterEnvelope() != nullptr) {
					float f = cutoff + (samplers[j]->getFilterEnvelope()->getNextSample() * amount * (22000 - cutoff));
					f = std::max(0.0f, std::min(f, 22000.0f)); // Clamp frequency
					lpfLeftStage1->coefficients(sampleRate, f, resonance);
				}
			}
		}

		// Process zone-based samplers

		for (int j = 0; j < getNumZones(); j++) {
			SampleZone* zone = getZone(j);
			if (zone->sampler != nullptr && zone->sampler->isPlaying()) {
				for (int i = 0; i < bufferSize; i++) {
					envValue = zone->sampler->getAmpEnvelope()->getNextSample();
					zone->sampler->nextSample();

					float left = zone->sampler->getCurrentSample(0) * envValue * finalGain;
					float right = zone->sampler->getCurrentSample(1) * envValue * finalGain;

					//// Soft clipping
					//left = std::tanh(left * 0.8f);
					//right = std::tanh(right * 0.8f);

					buffer.addSample(0, i, left);
					buffer.addSample(1, i, right);
				}

				if (zone->sampler->getFilterEnvelope() != nullptr) {
					float f = cutoff + (zone->sampler->getFilterEnvelope()->getNextSample() * amount * (22000 - cutoff));
					f = std::max(0.0f, std::min(f, 22000.0f));
					lpfLeftStage1->coefficients(sampleRate, f, resonance);
				}
			}
		}

		currentSample = (currentSample + bufferSize) % buffer.getNumSamples();
		magnitude = buffer.getMagnitude(currentSample, bufferSize);

		// Apply filter
		lpfLeftStage1->processStereo(leftOut, rightOut, buffer.getNumSamples());

		// Process effects
		if (effectsProcessor)
		{
			effectsProcessor->processBlock(buffer, 2, buffer.getNumSamples());
		}

		//// Final limiting with adjusted threshold
		//hardLimiter->setThreshold(0.9f); // Increase threshold slightly
		//hardLimiter->processBlock(leftOut, buffer.getNumSamples());
		//hardLimiter->processBlock(rightOut, buffer.getNumSamples()); // Process right channel too!

		// Peak limiting as safety net
		//for (int i = 0; i < buffer.getNumSamples(); i++) {
		//	leftOut[i] = std::max(-0.95f, std::min(0.95f, leftOut[i]));
		//	rightOut[i] = std::max(-0.95f, std::min(0.95f, rightOut[i]));
		//}

		// MIDI processing (unchanged)
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

		for (juce::MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);) {
			if (m.isNoteOn()) {
				state.noteOn(m.getChannel(), m.getNoteNumber(), m.getVelocity() / 128.0f);
			}
			if (m.isNoteOff()) {
				state.noteOff(m.getChannel(), m.getNoteNumber(), 0);
			}
			if (m.isAftertouch()) {
			}
			if (m.isPitchWheel()) {
			}
			if (m.isController()) {
				if (learn) {
					mappings.addMapping(m.getControllerNumber(), learningControl);
					learn = false;
				}
				else {
					juce::Logger::getCurrentLogger()->writeToLog("controller " + juce::String(m.getControllerNumber()) + " value " + juce::String(m.getControllerValue()));

					juce::Component* c = mappings.getMapping(m.getControllerNumber());

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
	SamAudioProcessorEditor* editor = new SamAudioProcessorEditor(*this);
	this->editor = editor;
	return editor;
}

//==============================================================================
void SamAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	juce::XmlElement root("SamAudioProcessorState");
	root.setAttribute("samplesetPath", currentFile.getFullPathName());
	saveSettings(currentFile.getParentDirectory().getFullPathName());	
	copyXmlToBinary(root, destData);
}

void SamAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	loadSettings();
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState && xmlState->hasTagName("SamAudioProcessorState"))
	{
		currentFile = juce::File(xmlState->getStringAttribute("samplesetPath", ""));			
	}
}

void SamAudioProcessor::setKeyboardEditor(KeyboardMappingEditor* editor)
{
	this->keyEditor = editor;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SamAudioProcessor();
}

void SamAudioProcessor::saveSettings(juce::String currentDirectory)
{
	String userHome = File::getSpecialLocation(File::userHomeDirectory).getFullPathName();
	File appDir = File(userHome + "/.Sam");

	if (!appDir.exists()) {
		appDir.createDirectory();
	}

	File configFile = File(userHome + "/.Sam/settings.xml");

	if (!configFile.exists()) {
		configFile.create();
	}
	else {
		configFile.deleteFile();
		configFile = File(userHome + "/.Sam/settings.xml");
		configFile.create();
	}

	ValueTree* v = new ValueTree("Settings");

	juce::String leftDir = juce::File(currentDirectory).getFullPathName();
	

	v->setProperty("currentFile", currentFile.getFullPathName(), nullptr);
	v->setProperty("leftCurrentDir",leftDir , nullptr);
	v->setProperty("rightCurrentDir", leftDir, nullptr);
	v->setProperty("sampleRate", sampleRate, nullptr);
	v->setProperty("bufferSize", bufferSize, nullptr);
	std::unique_ptr<XmlElement> xml = v->createXml();
	xml->writeToFile(configFile, "");

	xml = nullptr;
	delete v;
}

juce::String SamAudioProcessor::loadSettings()
{
	String userHome = File::getSpecialLocation(File::userHomeDirectory).getFullPathName();

	File appDir = File(userHome + "/.Sam");

	if (!appDir.exists()) {
		appDir.createDirectory();
	}

	File configFile = File(userHome + "/.Sam/settings.xml");

	if (configFile.exists()) {
		std::unique_ptr<XmlElement> xml = XmlDocument(configFile).getDocumentElement();
		ValueTree v = ValueTree::fromXml(*xml.get());
		String path = v.getProperty("currentFile");
		String sLeftDir = v.getProperty("leftCurrentDir");
		String sRightDir = v.getProperty("rightCurrentDir");
		sampleRate = v.getProperty("sampleRate");
		bufferSize = v.getProperty("bufferSize");

		juce::File* leftDir = new juce::File(sLeftDir);
		juce::File* rightDir = new juce::File(sRightDir);

		//if (path.length() > 0 && sampleRate > 0) {
		//	loadFile(juce::File(path));
		//}

		xml = nullptr;

		return leftDir->getFullPathName();
	}

	return File::getCurrentWorkingDirectory().getFileName();
}

void SamAudioProcessor::loadFile(juce::File file)
{
	if (!file.exists()) {
		return;
	}

	std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument(file).getDocumentElement();
	juce::ValueTree v = juce::ValueTree::fromXml(*xml.get());
	xml = nullptr;

	// Clear existing samplers
	for (int i = 0; i < 128; i++) {
		if (samplers[i] != nullptr) {
			samplers[i] = nullptr;
		}
	}
	
	int count = 0;
	bool hasZones = false;
	
	// Check if this file has zone data (new format) by looking for a "Zones" child
	for (int i = 0; i < v.getNumChildren(); i++) {
		if (v.getChild(i).getType().toString() == "Zones") {
			hasZones = true;
			break;
		}
	}

	// Load sampler data (both legacy and new format have this)
	for (int i = 0; i < v.getNumChildren(); i++) {
		juce::ValueTree child = v.getChild(i);
		
		// Skip zones data in this pass - we'll handle it separately
		if (child.getType().toString() == "Zones") {
			continue;
		}
		
		// Load sample data
		if (child.getType().toString() == "Sample") {
			std::unique_ptr<Sampler> s = std::make_unique<Sampler>(sampleRate, bufferSize);
			s->loadSample(juce::File(child.getProperty("sample").toString()));

			juce::String sLoop = child.getProperty("loop").toString();
			s->setLoop(sLoop == "true");
			
			s->setStartPosition(child.getProperty("loopStart").toString().getLargeIntValue());
			s->setEndPosition(child.getProperty("loopEnd").toString().getLargeIntValue());
			s->setPitch(child.getProperty("pitch").toString().getFloatValue());
			s->setReverse(child.getProperty("reverse").toString() == "true");

			juce::ADSR::Parameters params;
			params.attack = child.getProperty("amp_attack").toString().getFloatValue();
			params.decay = child.getProperty("amp_decay").toString().getFloatValue();
			params.sustain = child.getProperty("amp_sustain").toString().getFloatValue();
			params.release = child.getProperty("amp_release").toString().getFloatValue();
			s->getAmpEnvelope()->setParameters(params);
			
			juce::ADSR::Parameters filterParams;
			filterParams.attack = child.getProperty("filter_attack").toString().getFloatValue();
			filterParams.decay = child.getProperty("filter_decay").toString().getFloatValue();
			filterParams.sustain = child.getProperty("filter_sustain").toString().getFloatValue();
			filterParams.release = child.getProperty("filter_release").toString().getFloatValue();
			s.get()->getFilterEnvelope()->setParameters(filterParams);

			s->play();
			int index = child.getProperty("note").toString().getIntValue();
					
			samplers[index] = std::move(s);
			count++;
		}
		
	}

	juce::Logger::writeToLog("Loaded " + juce::String(count) + " samplers.");

	// Store zone data for the editor to load later
	loadedZones.clear();
	
	if (hasZones) {
		// Find and parse zones data
		for (int i = 0; i < v.getNumChildren(); i++) {
			juce::ValueTree child = v.getChild(i);
			if (child.getType().toString() == "Zones") {
				// Process each zone
				for (int j = 0; j < child.getNumChildren(); j++) {
					juce::ValueTree zoneNode = child.getChild(j);
					if (zoneNode.getType().toString() == "Zone") {
						SampleZone zone;
						zone.startNote = zoneNode.getProperty("startNote", 60);
						zone.endNote = zoneNode.getProperty("endNote", 60);
						zone.velLow = zoneNode.getProperty("velLow", 1);
						zone.velHigh = zoneNode.getProperty("velHigh", 127);
						zone.note = zoneNode.getProperty("note", 60);
						zone.name = zoneNode.getProperty("name", "").toString();
						zone.audioFile = juce::File(zoneNode.getProperty("audioFile", "").toString());
						
						bool hasAudio = zoneNode.getProperty("hasAudio", false);
						if (hasAudio && zone.audioFile.exists()) {
							// Create and configure sampler
							zone.sampler = std::make_unique<Sampler>(sampleRate, bufferSize);
							zone.sampler->loadSample(zone.audioFile);
							zone.sampler->setLoop(zoneNode.getProperty("loop", true));
							zone.sampler->setReverse(zoneNode.getProperty("reverse", false));
							zone.sampler->setPitch(zoneNode.getProperty("pitch", 1.0f));
							zone.sampler->setStartPosition(static_cast<int64_t>(zoneNode.getProperty("loopStart", 0)));
							zone.sampler->setEndPosition(static_cast<int64_t>(zoneNode.getProperty("loopEnd", 0)));
							
							// Set amplitude envelope parameters
							juce::ADSR::Parameters ampParams;
							ampParams.attack = zoneNode.getProperty("amp_attack", 0.01f);
							ampParams.decay = zoneNode.getProperty("amp_decay", 0.1f);
							ampParams.sustain = zoneNode.getProperty("amp_sustain", 1.0f);
							ampParams.release = zoneNode.getProperty("amp_release", 0.1f);
							zone.sampler->getAmpEnvelope()->setParameters(ampParams);
							
							// Set filter envelope parameters  
							juce::ADSR::Parameters filterParams;
							filterParams.attack = zoneNode.getProperty("filter_attack", 0.01f);
							filterParams.decay = zoneNode.getProperty("filter_decay", 0.1f);
							filterParams.sustain = zoneNode.getProperty("filter_sustain", 1.0f);
							filterParams.release = zoneNode.getProperty("filter_release", 0.1f);
							zone.sampler->getFilterEnvelope()->setParameters(filterParams);
							
							zone.sampler->play();
						}
						
						loadedZones.push_back(std::move(zone));
					}
				}
				break; // Found zones, no need to continue
			}
		}
		
		juce::Logger::writeToLog("Loaded " + juce::String(loadedZones.size()) + " zones.");
	}

	currentFile = file;
	hasLoadedZoneData = hasZones;
	
	saveSettings(currentFile.getParentDirectory().getFullPathName());
}