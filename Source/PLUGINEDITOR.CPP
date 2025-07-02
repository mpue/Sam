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

	for (int i = 60; i < 100; i++) {
		zones.push_back({ i, i, 1, 127,i }); // C4–C5, mittlere Velocity
	}


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

	mapViewButton = std::make_unique<juce::TextButton>("Mapping");
	mapViewButton->setSize(140, 20);
	mapViewButton->setTopLeftPosition(455, 345);
	addAndMakeVisible(mapViewButton.get());

	sequencerViewButton = std::make_unique<juce::TextButton>("Sequencer");
	sequencerViewButton->setSize(140, 20);
	sequencerViewButton->setTopLeftPosition(455, 370);
	addAndMakeVisible(sequencerViewButton.get());

	modViewButton = std::make_unique<juce::TextButton>("Modulation");
	modViewButton->setSize(140, 20);
	modViewButton->setTopLeftPosition(455, 395);
	addAndMakeVisible(modViewButton.get());

	fxViewButton = std::make_unique<juce::TextButton>("Effects");
	fxViewButton->setSize(140, 20);
	fxViewButton->setTopLeftPosition(455, 420);
	addAndMakeVisible(fxViewButton.get());

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
			sampleEditor->setSampler(nullptr);
			sampleEditor->repaint();
		}
		lock.exit();
	};
	juce::MessageManager::callAsync(changeLambda);

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
			for (int i = 0; i < 128; i++) {
				if (audioProcessor.samplers[i] != nullptr) {
					keyboard->setNoteColour(i, juce::Colours::green);
				}
			}
		}
	}
	else if (button == saveSetButton.get()) {


		juce::FileChooser chooser("Select target file", juce::File(userHome), "*");

		if (chooser.browseForFileToSave(true)) {

			juce::File file = chooser.getResult();

			juce::ValueTree audioFiles = juce::ValueTree("Samples");

			for (int i = 0; i < 128; i++) {
				if (audioProcessor.samplers[i] != nullptr) {
					juce::ValueTree file = juce::ValueTree("Sample");
					file.setProperty("note", i, nullptr);
					file.setProperty("loop", audioProcessor.samplers[i]->isLoop(), nullptr);
					file.setProperty("reverse", audioProcessor.samplers[i]->isReverse(), nullptr);
					file.setProperty("sample", audioProcessor.samplers[i]->getSampleLocation(), nullptr);
					file.setProperty("amp_attack", audioProcessor.samplers[i]->getAmpEnvelope()->getParameters().attack, nullptr);
					file.setProperty("amp_decay", audioProcessor.samplers[i]->getAmpEnvelope()->getParameters().decay, nullptr);
					file.setProperty("amp_sustain", audioProcessor.samplers[i]->getAmpEnvelope()->getParameters().sustain, nullptr);
					file.setProperty("amp_release", audioProcessor.samplers[i]->getAmpEnvelope()->getParameters().release, nullptr);
					file.setProperty("filter_attack",  audioProcessor.samplers[i]->getFilterEnvelope()->getParameters().attack, nullptr);
					file.setProperty("filter_decay",   audioProcessor.samplers[i]->getFilterEnvelope()->getParameters().decay, nullptr);
					file.setProperty("filter_sustain", audioProcessor.samplers[i]->getFilterEnvelope()->getParameters().sustain, nullptr);
					file.setProperty("filter_release", audioProcessor.samplers[i]->getFilterEnvelope()->getParameters().release, nullptr);

					file.setProperty("loopStart", audioProcessor.samplers[i]->getStartPosition(), nullptr);
					file.setProperty("loopEnd", audioProcessor.samplers[i]->getEndPosition(), nullptr);
					file.setProperty("pitch", audioProcessor.samplers[i]->getPitchAsFloat(), nullptr);

					audioFiles.addChild(file, -1, nullptr);
				}

			}

			std::unique_ptr < juce::XmlElement > xml = audioFiles.createXml();
			xml->writeToFile(file, "");
			xml = nullptr;
			audioProcessor.saveSettings(file.getParentDirectory().getFullPathName());
		}
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
		if (audioProcessor.samplers[audioProcessor.currentSampleIndex] != nullptr) {
			audioProcessor.samplers[audioProcessor.currentSampleIndex]->setLoop(loopButton->getToggleState());
		}
	}
	else if (button == reverseButton.get())
	{
		if (audioProcessor.samplers[audioProcessor.currentSampleIndex] != nullptr) {
			audioProcessor.samplers[audioProcessor.currentSampleIndex]->setReverse(reverseButton->getToggleState());
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
		}

		mappingEditor->setSize(keyboard->getWidth(), 250);
		mappingEditor->setTopLeftPosition(40, 620);
		mappingEditor->setVisible(true);
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
		if (audioProcessor.samplers[audioProcessor.currentSampleIndex] != nullptr) {
			audioProcessor.samplers[audioProcessor.currentSampleIndex]->setPitch(pitchSlider->getValue());
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


