#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SamAudioProcessorEditor::SamAudioProcessorEditor(SamAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	setLookAndFeel(&tlf);
	setSize(1024, 1024);

	keyboard = std::make_unique<CustomKeyboard>(audioProcessor.state, juce::MidiKeyboardComponent::Orientation::horizontalKeyboard);
	keyboard->setSize(750, 100);
	keyboard->setTopLeftPosition(200, 880);
	keyboard->setWantsKeyboardFocus(false);
	keyboard->setInterceptsMouseClicks(true, true);

	audioProcessor.state.addListener(this);
	addAndMakeVisible(keyboard.get());

	loopButton = std::make_unique<juce::ToggleButton>("Loop");
	loopButton->setSize(100, 20);
	loopButton->setTooltip("Toggle loop for the current sample");
	loopButton->setAlwaysOnTop(true);
	loopButton->setTopLeftPosition(462, 273);
	addAndMakeVisible(loopButton.get());

	reverseButton = std::make_unique<juce::ToggleButton>("Reverse");
	reverseButton->setSize(100, 20);
	reverseButton->setTooltip("Toggle reverse playback for the current sample");
	reverseButton->setAlwaysOnTop(true);
	reverseButton->setTopLeftPosition(540, 273);
	addAndMakeVisible(reverseButton.get());


	recordButton = std::make_unique<juce::TextButton>("Record");
	recordButton->setSize(70, 20);
	recordButton->setTopLeftPosition(670, 273);
	addAndMakeVisible(recordButton.get());
	recordButton->addListener(this);

	ampEnvelope = std::make_unique<EnvelopePanel>();
	ampEnvelope->setSize(400, 195);
	ampEnvelope->setTopLeftPosition(30, 650);
	addAndMakeVisible(ampEnvelope.get());

	filterEnvelope = std::make_unique<EnvelopePanel>();
	filterEnvelope->setSize(400, 195);
	filterEnvelope->setTopLeftPosition(622, 650);
	addAndMakeVisible(filterEnvelope.get());

	loadButton = std::make_unique<juce::TextButton>("Load");
	loadButton->setSize(90, 20);
	loadButton->setTopLeftPosition(45,273);
	loadButton->setTooltip("Load a sample to the current note");
	loadButton->setAlwaysOnTop(true);

	addAndMakeVisible(loadButton.get());

	loadSetButton = std::make_unique<juce::TextButton>("Load set");
	loadSetButton->setSize(90, 20);
	loadSetButton->setTopLeftPosition(140, 273);
	addAndMakeVisible(loadSetButton.get());

	saveSetButton = std::make_unique<juce::TextButton>("Save set");
	saveSetButton->setSize(90, 20);
	saveSetButton->setTopLeftPosition(242, 273);
	addAndMakeVisible(saveSetButton.get());

	newSetButton = std::make_unique<juce::TextButton>("New set");
	newSetButton->setSize(90, 20);
	newSetButton->setTopLeftPosition(342, 273);
	addAndMakeVisible(newSetButton.get());

	mainViewButton = std::make_unique<juce::TextButton>("Main");
	mainViewButton->setSize(140, 20);
	mainViewButton->setTopLeftPosition(455, 320);
	addAndMakeVisible(mainViewButton.get());
	mainViewButton->setEnabled(true);

	mapViewButton = std::make_unique<juce::TextButton>("Mapping");
	mapViewButton->setSize(140, 20);
	mapViewButton->setTopLeftPosition(455, 345);
	addAndMakeVisible(mapViewButton.get());
	mapViewButton->setEnabled(true);

	sequencerViewButton = std::make_unique<juce::TextButton>("Sequencer");
	sequencerViewButton->setSize(140, 20);
	sequencerViewButton->setTopLeftPosition(455, 370);
	addAndMakeVisible(sequencerViewButton.get());
	sequencerViewButton->setEnabled(false);

	modViewButton = std::make_unique<juce::TextButton>("Modulation");
	modViewButton->setSize(140, 20);
	modViewButton->setTopLeftPosition(455, 395);
	addAndMakeVisible(modViewButton.get());
	modViewButton->setEnabled(false);

	fxViewButton = std::make_unique<juce::TextButton>("Effects");
	fxViewButton->setSize(140, 20);
	fxViewButton->setTopLeftPosition(455,420);
	addAndMakeVisible(fxViewButton.get());
	fxViewButton->setEnabled(false);

	pitchSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextEntryBoxPosition::TextBoxRight);
	pitchSlider->setSize(200, 24);
	pitchSlider->setTopLeftPosition(780, 273);
	pitchSlider->setRange(0.25, 1.99);
	pitchSlider->setOpaque(false);
	pitchSlider->setValue(1);
	addAndMakeVisible(pitchSlider.get());

	noteLabel = std::make_unique<juce::Label>("None");
	noteLabel->setSize(100, 25);
	noteLabel->setTopLeftPosition(10, 255);
	addAndMakeVisible(noteLabel.get());

	cutoffSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox);
	cutoffSlider->setSize(48, 48);
	cutoffSlider->setTopLeftPosition(715, 530);
	cutoffSlider->setRange(0, 22000);
	cutoffSlider->setName("Cutoff");
	addAndMakeVisible(cutoffSlider.get());

	cutoffLabel = std::make_unique<juce::Label>("Cutoff", "Cutoff");
	cutoffLabel->setSize(100, 25);
	cutoffLabel->setTopLeftPosition(710, 580); 
	addAndMakeVisible(cutoffLabel.get());

	resoSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox);
	resoSlider->setSize(48, 48);
	resoSlider->setTopLeftPosition(810, 530);
	resoSlider->setRange(0, 5);
	resoSlider->setName("Resonance");
	addAndMakeVisible(resoSlider.get());

	resoLabel = std::make_unique<juce::Label>("Resonance", "Resonance");
	resoLabel->setSize(100, 25);
	resoLabel->setTopLeftPosition(797, 580);
	addAndMakeVisible(resoLabel.get());

	amtSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox);
	amtSlider->setSize(48, 48);
	amtSlider->setTopLeftPosition(925, 530);
	amtSlider->setRange(0, 1);
	amtSlider->setName("Amount");
	addAndMakeVisible(amtSlider.get());

	amtLabel = std::make_unique<juce::Label>("Amount", "Amount");
	amtLabel->setSize(100, 25);
	amtLabel->setTopLeftPosition(915, 580);
	addAndMakeVisible(amtLabel.get());

	vuMeter = std::make_unique<VUMeter>();
	vuMeter->setSize(128, 128);
	vuMeter->setTopLeftPosition(461, 630);
	addAndMakeVisible(vuMeter.get());

	sampleEditor = std::make_unique<SampleEditor>(audioProcessor.fmtMgr.get());
	sampleEditor->setTopLeftPosition(24, 134);
	sampleEditor->setSize(850, 124);
	addAndMakeVisible(sampleEditor.get());

	propertyViewLeft = std::make_unique<PropertyView>(nullptr);
	propertyViewLeft->setTopLeftPosition(20, 310);
	propertyViewLeft->setSize(413, 200);
	addAndMakeVisible(propertyViewLeft.get());

	propertyViewLeft->browser->addChangeListener(this);

	propertyViewRight = std::make_unique<PropertyView>(nullptr);
	propertyViewRight->setTopLeftPosition(620,310);
	propertyViewRight->setSize(360, 200);
	addAndMakeVisible(propertyViewRight.get());

	propertyViewRight->browser->addChangeListener(this);


	/*
	driveSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox);
	driveSlider->setSize(64, 64);
	driveSlider->setTopLeftPosition(1025, 550);
	addAndMakeVisible(driveSlider.get());
	*/

	loadButton->addListener(this);
	loadSetButton->addListener(this);
	saveSetButton->addListener(this);
	newSetButton->addListener(this);
	loopButton->addListener(this);
	reverseButton->addListener(this);
	cutoffSlider->addListener(this);
	resoSlider->addListener(this);
	amtSlider->addListener(this);
	pitchSlider->addListener(this);
	modViewButton->addListener(this);
	fxViewButton->addListener(this);	
	sequencerViewButton->addListener(this);
	mainViewButton->addListener(this);
	mapViewButton->addListener(this);
	fxViewButton->addListener(this);


	// driveSlider->addListener(this);
	addMouseListener(this,true);

	String dir = audioProcessor.loadSettings();
	propertyViewLeft->getBrowser()->getModel()->setCurrentDir(new juce::File(dir));
	propertyViewRight->getBrowser()->getModel()->setCurrentDir(new juce::File(dir));
}

SamAudioProcessorEditor::~SamAudioProcessorEditor()
{	
	audioProcessor.state.removeListener(this);
	setLookAndFeel(nullptr);
	keyboard = nullptr;
	ampEnvelope = nullptr;
	filterEnvelope = nullptr;
	loadButton = nullptr;
	loadSetButton = nullptr;
	saveSetButton = nullptr;
	newSetButton = nullptr;
	noteLabel = nullptr;
	loopButton = nullptr;
	reverseButton = nullptr;
	cutoffSlider = nullptr;
	resoSlider = nullptr;
	amtSlider = nullptr;
	driveSlider = nullptr;
	cutoffLabel = nullptr;
	resoLabel = nullptr;
	amtLabel = nullptr;
	driveLabel = nullptr;
	vuMeter = nullptr;
	propertyViewLeft = nullptr;
	propertyViewRight = nullptr;
	sampleEditor = nullptr;
	pitchSlider = nullptr;
	mainViewButton = nullptr;
	sequencerViewButton = nullptr;
	recordButton = nullptr;
	modViewButton = nullptr;
	fxViewButton = nullptr;
	mapViewButton = nullptr;
}

//==============================================================================
void SamAudioProcessorEditor::paint(juce::Graphics& g)
{
	/**/
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	if (currentMode == UIMode::Sequencer) {
	}
	else if (currentMode == UIMode::Mod) {
	}
	else if (currentMode == UIMode::FX) {
	}
	else if (currentMode == UIMode::Map) {
		g.drawImageAt(juce::ImageCache::getFromMemory(BinaryData::sam2_mappings_png, BinaryData::sam2_mappings_pngSize), 0, 0);
	}
	else {
		g.drawImageAt(juce::ImageCache::getFromMemory(BinaryData::sam2_png, BinaryData::sam2_pngSize), 0, 0);
	}

	g.setFont(15.0f);

	g.setColour(juce::Colours::orange);


}

void SamAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
}

void SamAudioProcessorEditor::handleNoteOn(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
	if (!isTimerRunning()) {
		startTimerHz(50);
		sampleEditor->startTimerHz(50);
	}
	audioProcessor.currentSampleIndex = midiNoteNumber;
	std::function<void(void)> changeLambda =
		[=]() {
		juce::MessageManager::Lock lock;
		lock.enter();
		if (audioProcessor.samplers[midiNoteNumber] != nullptr) {

			noteLabel->setText("current : " + juce::String(midiNoteNumber), juce::NotificationType::dontSendNotification);
			// audioProcessor.samplers[midiNoteNumber]->play();
			// 

			ampEnvelope->setEnvelope(audioProcessor.samplers[midiNoteNumber]->getAmpEnvelope());

			float a = audioProcessor.samplers[midiNoteNumber]->getAmpEnvelope()->getParameters().attack;
			float d = audioProcessor.samplers[midiNoteNumber]->getAmpEnvelope()->getParameters().decay;
			float s = audioProcessor.samplers[midiNoteNumber]->getAmpEnvelope()->getParameters().sustain;
			float r = audioProcessor.samplers[midiNoteNumber]->getAmpEnvelope()->getParameters().release;
			//// 
			
			ampEnvelope->setADSR(a, d, s, r);
			
			this->loopButton->setToggleState(audioProcessor.samplers[midiNoteNumber]->isLoop(), juce::NotificationType::dontSendNotification);
			pitchSlider->setValue(audioProcessor.samplers[midiNoteNumber]->getPitchAsFloat());
			
			if (audioProcessor.samplers[midiNoteNumber]->hasSample()) {
				sampleEditor->setSampler(audioProcessor.samplers[midiNoteNumber].get());
			}
			filterEnvelope->setEnvelope(audioProcessor.samplers[midiNoteNumber]->getFilterEnvelope());

			a = audioProcessor.samplers[midiNoteNumber]->getFilterEnvelope()->getParameters().attack;
			d = audioProcessor.samplers[midiNoteNumber]->getFilterEnvelope()->getParameters().decay;
			s = audioProcessor.samplers[midiNoteNumber]->getFilterEnvelope()->getParameters().sustain;
			r = audioProcessor.samplers[midiNoteNumber]->getFilterEnvelope()->getParameters().release;
			
			filterEnvelope->setADSR(a, d, s, r);
			
			repaint();
			
			sampleEditor->repaint();
			ampEnvelope->repaint();			
		}
		else {
			if (mappingEditor == nullptr) {
				lock.exit();
				return;
			}

			int zone = mappingEditor->findZoneForNote(midiNoteNumber);
			if (zone >= 0) {
				selectZone(zone);
			}
			else {
				currentSelectedZoneIndex = -1;  // No zone selected
				sampleEditor->setSampler(nullptr);
				sampleEditor->repaint();
			}
			
		}
		lock.exit();
	};
	juce::MessageManager::callAsync(changeLambda);

}

void SamAudioProcessorEditor::selectZone(int zoneIndex)
{
	if (zoneIndex < 0) {
		currentSelectedZoneIndex = -1;
		juce::Logger::writeToLog("Deselected zone");
		return;
	}

	currentSelectedZoneIndex = zoneIndex;
	juce::Logger::writeToLog("Selected zone " + juce::String(zoneIndex));
	
	Sampler* sampler = zones[zoneIndex].sampler.get();

	if (sampler == nullptr) {
		juce::Logger::writeToLog("Warning: Zone " + juce::String(zoneIndex) + " has no sampler");
		return;
	}

	ampEnvelope->setEnvelope(sampler->getAmpEnvelope());

	float a = sampler->getAmpEnvelope()->getParameters().attack;
	float d = sampler->getAmpEnvelope()->getParameters().decay;
	float s = sampler->getAmpEnvelope()->getParameters().sustain;
	float r = sampler->getAmpEnvelope()->getParameters().release;

	ampEnvelope->setADSR(a, d, s, r);

	this->loopButton->setToggleState(sampler->isLoop(), juce::NotificationType::dontSendNotification);
	this->reverseButton->setToggleState(sampler->isReverse(), juce::NotificationType::dontSendNotification);
	pitchSlider->setValue(sampler->getPitchAsFloat());

	if (sampler->hasSample()) {
		sampleEditor->setSampler(sampler);
		juce::Logger::writeToLog("Set sample editor to zone " + juce::String(zoneIndex) + " sampler");
	}
	filterEnvelope->setEnvelope(sampler->getFilterEnvelope());

	a = sampler->getFilterEnvelope()->getParameters().attack;
	d = sampler->getFilterEnvelope()->getParameters().decay;
	s = sampler->getFilterEnvelope()->getParameters().sustain;
	r = sampler->getFilterEnvelope()->getParameters().release;

	filterEnvelope->setADSR(a, d, s, r);

	repaint();

	sampleEditor->repaint();
	ampEnvelope->repaint();
	
}

void SamAudioProcessorEditor::handleNoteOff(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
	audioProcessor.currentSampleIndex = midiNoteNumber;
	if (audioProcessor.samplers[midiNoteNumber] != nullptr) {
		audioProcessor.samplers[midiNoteNumber]->stop();
	}

	if (audioProcessor.numVoices == 0) {
		// stopTimer();

		// audioProcessor.samplers[midiNoteNumber]->stop();

	}
}

void SamAudioProcessorEditor::buttonClicked(juce::Button* button)
{

	String userHome = File::getSpecialLocation(File::userHomeDirectory).getFullPathName();

	if (button == loadButton.get()) {

		juce::FileChooser chooser("Select sample to add", juce::File(userHome), "*");

		if (chooser.browseForFileToOpen()) {

			juce::File file = chooser.getResult();

			std::unique_ptr<Sampler> s = std::make_unique<Sampler>(audioProcessor.sampleRate, audioProcessor.bufferSize);
			s->loadSample(file);
			s->setLoop(true);
			s->play();

			if (audioProcessor.samplers[audioProcessor.currentSampleIndex] != nullptr) {
				audioProcessor.samplers[audioProcessor.currentSampleIndex] = nullptr;
			}
			audioProcessor.samplers[audioProcessor.currentSampleIndex] = std::move(s);
		}
	}
	else if (button == loadSetButton.get()) {

		juce::FileChooser chooser("Select set to open", juce::File(userHome), "*");

		if (chooser.browseForFileToOpen()) {

			juce::File file = chooser.getResult();
			audioProcessor.loadFile(file);

			
			// Load zone data if available
			loadZonesFromProcessor();
			
			for (int i = 0; i < 128; i++) {
				if (audioProcessor.samplers[i] != nullptr) {
					keyboard->setNoteColour(i, juce::Colours::green);
				}
			}
		}
	}
	else if (button == saveSetButton.get()) {
		saveSet(userHome);
	}
	else if (button == newSetButton.get()) {
		for (int i = 0; i < 128; i++) {
			if (audioProcessor.samplers[i] != nullptr) {				
				audioProcessor.samplers[i] = nullptr;
			}
		}
		sampleEditor->setSampler(nullptr);
		sampleEditor->repaint();
	}
	else if (button == loopButton.get())
	{
		// Handle both zones and legacy samplers
		if (currentSelectedZoneIndex >= 0 && currentSelectedZoneIndex < static_cast<int>(zones.size())) {
			// Working with a zone
			if (zones[currentSelectedZoneIndex].sampler != nullptr) {
				zones[currentSelectedZoneIndex].sampler->setLoop(loopButton->getToggleState());
				juce::Logger::writeToLog("Set loop for zone " + juce::String(currentSelectedZoneIndex) + " to " + (loopButton->getToggleState() ? "ON" : "OFF"));
			}
		}
		else if (audioProcessor.samplers[audioProcessor.currentSampleIndex] != nullptr) {
			// Working with legacy sampler
			audioProcessor.samplers[audioProcessor.currentSampleIndex]->setLoop(loopButton->getToggleState());
			juce::Logger::writeToLog("Set loop for legacy sampler " + juce::String(audioProcessor.currentSampleIndex) + " to " + (loopButton->getToggleState() ? "ON" : "OFF"));
		}
	}
	else if (button == reverseButton.get())
	{
		// Handle both zones and legacy samplers
		if (currentSelectedZoneIndex >= 0 && currentSelectedZoneIndex < static_cast<int>(zones.size())) {
			// Working with a zone
			if (zones[currentSelectedZoneIndex].sampler != nullptr) {
				zones[currentSelectedZoneIndex].sampler->setReverse(reverseButton->getToggleState());
				juce::Logger::writeToLog("Set reverse for zone " + juce::String(currentSelectedZoneIndex) + " to " + (reverseButton->getToggleState() ? "ON" : "OFF"));
			}
		}
		else if (audioProcessor.samplers[audioProcessor.currentSampleIndex] != nullptr) {
			// Working with legacy sampler
			audioProcessor.samplers[audioProcessor.currentSampleIndex]->setReverse(reverseButton->getToggleState());
			juce::Logger::writeToLog("Set reverse for legacy sampler " + juce::String(audioProcessor.currentSampleIndex) + " to " + (reverseButton->getToggleState() ? "ON" : "OFF"));
		}
	}
	else if (button == sequencerViewButton.get()) {
		// sequenceEditor->setVisible(true);
		sampleEditor->setVisible(false);
		currentMode = UIMode::Sequencer;
		repaint();
	}
	else if (button == mainViewButton.get()) {
		//sequenceEditor->setVisible(false);		
		sampleEditor->setVisible(true);
		currentMode = UIMode::Main;
		currentSelectedZoneIndex = -1;  // Clear zone selection when switching to main view
		ampEnvelope->setVisible(true);
		filterEnvelope->setVisible(true);
		ampEnvelope->setVisible(true);
		filterEnvelope->setVisible(true);
		cutoffLabel->setVisible(true);
		cutoffSlider->setVisible(true);
		amtLabel->setVisible(true);
		amtSlider->setVisible(true);
		resoLabel->setVisible(true);
		resoSlider->setVisible(true);
		vuMeter->setVisible(true);
		keyboard->setSize(750, 100);
		keyboard->setTopLeftPosition(200, 880);
		mappingEditor->setVisible(false);
		repaint();
	}
	else if (button == modViewButton.get()) {
		currentMode = UIMode::Mod;
		sampleEditor->setVisible(false);
		repaint();
	}
	else if (button == fxViewButton.get()) {
		currentMode = UIMode::FX;
		sampleEditor->setVisible(false);
		repaint();
	}
	else if (button == mapViewButton.get()) {
		currentMode = UIMode::Map;		
		repaint();
		ampEnvelope->setVisible(false);
		filterEnvelope->setVisible(false);
		ampEnvelope->setVisible(false);
		filterEnvelope->setVisible(false);
		cutoffLabel->setVisible(false);
		cutoffSlider->setVisible(false);
		amtLabel->setVisible(false);
		amtSlider->setVisible(false);
		resoLabel->setVisible(false);
		resoSlider->setVisible(false);
		vuMeter->setVisible(false);
		keyboard->setSize(920, 100);
		keyboard->setTopLeftPosition(40, 880);

		if (mappingEditor == nullptr) {
			mappingEditor = std::make_unique<KeyboardMappingEditor>(*keyboard, zones);
			addAndMakeVisible(mappingEditor.get());		
			mappingEditor->toFront(true);
			mappingEditor->repaint();				

			mappingEditor->setZoneAddedCallback([this](int index, const SampleZone& zone) {
				onZoneAdded(index, zone);
				});

			mappingEditor->setZoneRemovedCallback([this](int index) {
				onZoneRemoved(index);
			});

			mappingEditor->setSelectionChangedCallback([this](int index) {
				onSelectionChanged(index);
			});
		}

		mappingEditor->setSize(keyboard->getWidth(), 250);
		mappingEditor->setTopLeftPosition(40, 620);
		mappingEditor->setVisible(true);
		audioProcessor.setKeyboardEditor(mappingEditor.get());
	}
	else if (button == recordButton.get())
	{
		if (!audioProcessor.isRecording)
		{
			juce::FileChooser chooser("Select a location to save the recording", {}, "*.wav");
			if (chooser.browseForFileToSave(true))
			{
				currentSetFile = chooser.getResult();
				audioProcessor.recorder.startRecording(currentSetFile, audioProcessor.getSampleRate(), 2);
				audioProcessor.isRecording = true;
				recordButton->setButtonText("Stop Recording");
			}
		}
		else
		{
			audioProcessor.recorder.stopRecording();
			audioProcessor.isRecording = false;

			std::unique_ptr<Sampler> s = std::make_unique<Sampler>(audioProcessor.sampleRate, audioProcessor.bufferSize);
			s->loadSample(currentSetFile);
			s->setLoop(true);
			s->play();

			if (audioProcessor.samplers[audioProcessor.currentSampleIndex] != nullptr) {
				audioProcessor.samplers[audioProcessor.currentSampleIndex] = nullptr;
			}
			audioProcessor.samplers[audioProcessor.currentSampleIndex] = std::move(s);
			recordButton->setButtonText("Record");
		}
	}
}

void SamAudioProcessorEditor::saveSet(juce::String& userHome)
{
	juce::FileChooser chooser("Select target file", juce::File(userHome), "*");

	if (chooser.browseForFileToSave(true)) {

		juce::File file = chooser.getResult();

		juce::ValueTree audioFiles = juce::ValueTree("Samples");

		// Save sampler data
		for (int i = 0; i < 128; i++) {
			if (audioProcessor.samplers[i] != nullptr) {
				juce::ValueTree sampleNode = juce::ValueTree("Sample");
				sampleNode.setProperty("note", i, nullptr);
				sampleNode.setProperty("loop", audioProcessor.samplers[i]->isLoop(), nullptr);
				sampleNode.setProperty("reverse", audioProcessor.samplers[i]->isReverse(), nullptr);
				sampleNode.setProperty("sample", audioProcessor.samplers[i]->getSampleLocation(), nullptr);
				sampleNode.setProperty("amp_attack", audioProcessor.samplers[i]->getAmpEnvelope()->getParameters().attack, nullptr);
				sampleNode.setProperty("amp_decay", audioProcessor.samplers[i]->getAmpEnvelope()->getParameters().decay, nullptr);
				sampleNode.setProperty("amp_sustain", audioProcessor.samplers[i]->getAmpEnvelope()->getParameters().sustain, nullptr);
				sampleNode.setProperty("amp_release", audioProcessor.samplers[i]->getAmpEnvelope()->getParameters().release, nullptr);
				sampleNode.setProperty("filter_attack", audioProcessor.samplers[i]->getFilterEnvelope()->getParameters().attack, nullptr);
				sampleNode.setProperty("filter_decay", audioProcessor.samplers[i]->getFilterEnvelope()->getParameters().decay, nullptr);
				sampleNode.setProperty("filter_sustain", audioProcessor.samplers[i]->getFilterEnvelope()->getParameters().sustain, nullptr);
				sampleNode.setProperty("filter_release", audioProcessor.samplers[i]->getFilterEnvelope()->getParameters().release, nullptr);
				sampleNode.setProperty("loopStart", audioProcessor.samplers[i]->getStartPosition(), nullptr);
				sampleNode.setProperty("loopEnd", audioProcessor.samplers[i]->getEndPosition(), nullptr);
				sampleNode.setProperty("pitch", audioProcessor.samplers[i]->getPitchAsFloat(), nullptr);

				audioFiles.addChild(sampleNode, -1, nullptr);
			}
		}

		// Save zone data
		juce::ValueTree zonesNode = juce::ValueTree("Zones");
		for (int i = 0; i < static_cast<int>(zones.size()); i++) {
			const SampleZone& zone = zones[i];
			
			juce::ValueTree zoneNode = juce::ValueTree("Zone");
			zoneNode.setProperty("index", i, nullptr);
			zoneNode.setProperty("startNote", zone.startNote, nullptr);
			zoneNode.setProperty("endNote", zone.endNote, nullptr);
			zoneNode.setProperty("velLow", zone.velLow, nullptr);
			zoneNode.setProperty("velHigh", zone.velHigh, nullptr);
			zoneNode.setProperty("note", zone.note, nullptr);
			zoneNode.setProperty("name", zone.name, nullptr);
			zoneNode.setProperty("audioFile", zone.audioFile.getFullPathName(), nullptr);
			
			// Save sampler properties if present
			if (zone.sampler != nullptr) {
				zoneNode.setProperty("hasAudio", true, nullptr);
				zoneNode.setProperty("loop", zone.sampler->isLoop(), nullptr);
				zoneNode.setProperty("reverse", zone.sampler->isReverse(), nullptr);
				zoneNode.setProperty("pitch", zone.sampler->getPitchAsFloat(), nullptr);
				zoneNode.setProperty("loopStart", zone.sampler->getStartPosition(), nullptr);
				zoneNode.setProperty("loopEnd", zone.sampler->getEndPosition(), nullptr);
				
				// Save envelope parameters
				zoneNode.setProperty("amp_attack", zone.sampler->getAmpEnvelope()->getParameters().attack, nullptr);
				zoneNode.setProperty("amp_decay", zone.sampler->getAmpEnvelope()->getParameters().decay, nullptr);
				zoneNode.setProperty("amp_sustain", zone.sampler->getAmpEnvelope()->getParameters().sustain, nullptr);
				zoneNode.setProperty("amp_release", zone.sampler->getAmpEnvelope()->getParameters().release, nullptr);
				zoneNode.setProperty("filter_attack", zone.sampler->getFilterEnvelope()->getParameters().attack, nullptr);
				zoneNode.setProperty("filter_decay", zone.sampler->getFilterEnvelope()->getParameters().decay, nullptr);
				zoneNode.setProperty("filter_sustain", zone.sampler->getFilterEnvelope()->getParameters().sustain, nullptr);
				zoneNode.setProperty("filter_release", zone.sampler->getFilterEnvelope()->getParameters().release, nullptr);
			} 
			else {
				zoneNode.setProperty("hasAudio", false, nullptr);
			}
			
			zonesNode.addChild(zoneNode, -1, nullptr);
		}
		
		audioFiles.addChild(zonesNode, -1, nullptr);

		std::unique_ptr<juce::XmlElement> xml = audioFiles.createXml();
		xml->writeToFile(file, "");
		xml = nullptr;
		audioProcessor.saveSettings(file.getParentDirectory().getFullPathName());
	}
}

void SamAudioProcessorEditor::onZoneAdded(int index, const SampleZone& zone)
{
	juce::Logger::writeToLog("Zone added at index " + juce::String(index));

	juce::File file = zones[index].audioFile;
	zones[index].sampler = std::make_unique<Sampler>(audioProcessor.sampleRate, audioProcessor.bufferSize);		
	zones[index].sampler->loadSample(file);
	zones[index].sampler->setLoop(true);
	zones[index].sampler->play();

	selectZone(index);
}

void SamAudioProcessorEditor::onZoneRemoved(int index)
{
	juce::Logger::writeToLog("Zone removed from index " + juce::String(index));

}

void SamAudioProcessorEditor::onSelectionChanged(int index)
{
	selectZone(index);
}

void SamAudioProcessorEditor::timerCallback()
{
	if (audioProcessor.samplers[audioProcessor.currentSampleIndex] == nullptr) {
		return;
	}
	cutoffSlider->setValue(audioProcessor.cutoff);
	resoSlider->setValue(audioProcessor.resonance);
	amtSlider->setValue(audioProcessor.amount);
	vuMeter->setMagnitude(audioProcessor.magnitude);;
	repaint();

}

void SamAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster* source)
{
	ExtendedFileBrowser* efb = dynamic_cast<ExtendedFileBrowser*>(source);

	if (efb != nullptr) {

		if (efb->getSelectedFile().getFileExtension() == ".sam") {
			sampleEditor->setSampler(nullptr);
			audioProcessor.loadFile(efb->getSelectedFile());
			
			// Load zone data if available
			loadZonesFromProcessor();

		}
		else {
			std::unique_ptr<Sampler> s = std::make_unique<Sampler>(audioProcessor.sampleRate, audioProcessor.bufferSize);
			s->loadSample(efb->getSelectedFile());
			s->setLoop(true);
			s->play();

			if (audioProcessor.samplers[audioProcessor.currentSampleIndex] != nullptr) {
				audioProcessor.samplers[audioProcessor.currentSampleIndex] = nullptr;
			}
			audioProcessor.samplers[audioProcessor.currentSampleIndex] = std::move(s);
			if (audioProcessor.samplers[audioProcessor.currentSampleIndex]->hasSample()) {
				sampleEditor->setSampler(audioProcessor.samplers[audioProcessor.currentSampleIndex].get());
			}

		}
	}

	for (int i = 0; i < 128; i++) {
		if (audioProcessor.samplers[i] != nullptr) {
			keyboard->setNoteColour(i, juce::Colours::green.brighter());
		}
		else {
			keyboard->clearNoteColour(i);
		}
	}

	sampleEditor->repaint();
}

void SamAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
	if (slider == cutoffSlider.get()) {
		audioProcessor.cutoff = cutoffSlider->getValue();
	}
	else if (slider == resoSlider.get()) {
		audioProcessor.resonance = resoSlider->getValue();
	}
	else if (slider == amtSlider.get()) {
		audioProcessor.amount = amtSlider->getValue();
	}
	else if (slider == pitchSlider.get()) {
		// Handle both zones and legacy samplers
		if (currentSelectedZoneIndex >= 0 && currentSelectedZoneIndex < static_cast<int>(zones.size())) {
			// Working with a zone
			if (zones[currentSelectedZoneIndex].sampler != nullptr) {
				zones[currentSelectedZoneIndex].sampler->setPitch(pitchSlider->getValue());
				juce::Logger::writeToLog("Set pitch for zone " + juce::String(currentSelectedZoneIndex) + " to " + juce::String(pitchSlider->getValue()));
			}
		}
		else if (audioProcessor.samplers[audioProcessor.currentSampleIndex] != nullptr) {
			// Working with legacy sampler
			audioProcessor.samplers[audioProcessor.currentSampleIndex]->setPitch(pitchSlider->getValue());
			juce::Logger::writeToLog("Set pitch for legacy sampler " + juce::String(audioProcessor.currentSampleIndex) + " to " + juce::String(pitchSlider->getValue()));
		}
	}
	if (audioProcessor.lpfLeftStage1 != nullptr)
	{
		audioProcessor.lpfLeftStage1->coefficients(audioProcessor.sampleRate, audioProcessor.cutoff, audioProcessor.resonance);
	}
}

void SamAudioProcessorEditor::mouseDown(const MouseEvent& event)
{
	if (event.mods.isRightButtonDown()) {
		PopupMenu m = PopupMenu();

		Component* c = event.source.getComponentUnderMouse();
		m.addItem(1, "MIDI Learn");
		m.addItem(2, "MIDI forget");

		const int result = m.show();

		if (result == 0)
		{
			// user dismissed the menu without picking anything
		}
		else if (result == 1 &&  c != nullptr) {
			audioProcessor.learn = true;
			audioProcessor.learningControl = c;
		}
		else if (result == 2) {
			audioProcessor.mappings.removeMapping(c->getName());
		}
		
	}
}

//==============================================================================
void SamAudioProcessorEditor::loadZonesFromProcessor()
{
	if (!audioProcessor.hasZoneDataToLoad()) {
		return;
	}
	
	// Clear existing zones
	zones.clear();
	
	// Get loaded zone data from processor - now returns SampleZone objects directly
	auto loadedZones = audioProcessor.getAndClearLoadedZones();
	
	// Move the zones directly into our zones vector
	for (auto& zone : loadedZones) {
		zones.push_back(std::move(zone));
	}
	
	// Update mapping editor if it exists
	if (mappingEditor != nullptr) {
		mappingEditor->repaint();
	}
	
	juce::Logger::writeToLog("Loaded " + juce::String(zones.size()) + " zones into editor.");
}

















