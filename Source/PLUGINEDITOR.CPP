/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SamAudioProcessorEditor::SamAudioProcessorEditor(SamAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.

	setLookAndFeel(&tlf);
	setSize(1260, 1000);


	keyboard = std::make_unique<juce::MidiKeyboardComponent>(audioProcessor.state, juce::MidiKeyboardComponent::Orientation::horizontalKeyboard);
	keyboard->setSize(1193, 150);
	keyboard->setTopLeftPosition(30, 810);
	audioProcessor.state.addListener(this);
	addAndMakeVisible(keyboard.get());

	ampEnvelope = std::make_unique<EnvelopePanel>();
	ampEnvelope->setSize(500, 195);
	ampEnvelope->setTopLeftPosition(32, 720);
	addAndMakeVisible(ampEnvelope.get());

	filterEnvelope = std::make_unique<EnvelopePanel>();
	filterEnvelope->setSize(500, 195);
	filterEnvelope->setTopLeftPosition(722, 720);
	addAndMakeVisible(filterEnvelope.get());

	loadButton = std::make_unique<juce::TextButton>("Load");
	loadButton->setSize(100, 20);
	loadButton->setTopLeftPosition(32, 5);
	addAndMakeVisible(loadButton.get());

	loadSetButton = std::make_unique<juce::TextButton>("Load set");
	loadSetButton->setSize(100, 20);
	loadSetButton->setTopLeftPosition(140, 5);
	addAndMakeVisible(loadSetButton.get());

	saveSetButton = std::make_unique<juce::TextButton>("Save set");
	saveSetButton->setSize(100, 20);
	saveSetButton->setTopLeftPosition(248, 5);
	addAndMakeVisible(saveSetButton.get());

	newSetButton = std::make_unique<juce::TextButton>("New set");
	newSetButton->setSize(100, 20);
	newSetButton->setTopLeftPosition(366, 5);
	addAndMakeVisible(newSetButton.get());

	loopButton = std::make_unique<juce::ToggleButton>("Loop");
	loopButton->setSize(100, 20);
	loopButton->setTopLeftPosition(42, 300);
	addAndMakeVisible(loopButton.get());

	pitchSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextEntryBoxPosition::TextBoxRight);
	pitchSlider->setSize(400, 24);
	pitchSlider->setTopLeftPosition(100, 300);
	pitchSlider->setRange(0.25, 2);
	pitchSlider->setOpaque(false);
	pitchSlider->setValue(1);
	addAndMakeVisible(pitchSlider.get());

	noteLabel = std::make_unique<juce::Label>("None");
	noteLabel->setSize(100, 25);
	noteLabel->setTopLeftPosition(10, 255);
	addAndMakeVisible(noteLabel.get());

	cutoffSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox);
	cutoffSlider->setSize(64, 64);
	cutoffSlider->setTopLeftPosition(725, 650);
	cutoffSlider->setRange(0, 22000);
	addAndMakeVisible(cutoffSlider.get());

	cutoffLabel = std::make_unique<juce::Label>("Cutoff", "Cutoff");
	cutoffLabel->setSize(100, 25);
	cutoffLabel->setTopLeftPosition(730, 710);
	addAndMakeVisible(cutoffLabel.get());

	resoSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox);
	resoSlider->setSize(64, 64);
	resoSlider->setTopLeftPosition(825, 650);
	resoSlider->setRange(0, 5);
	addAndMakeVisible(resoSlider.get());

	resoLabel = std::make_unique<juce::Label>("Resonance", "Resonance");
	resoLabel->setSize(100, 25);
	resoLabel->setTopLeftPosition(817, 710);
	addAndMakeVisible(resoLabel.get());

	amtSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox);
	amtSlider->setSize(64, 64);
	amtSlider->setTopLeftPosition(925, 650);
	amtSlider->setRange(0, 1);
	addAndMakeVisible(amtSlider.get());

	amtLabel = std::make_unique<juce::Label>("Amount", "Amount");
	amtLabel->setSize(100, 25);
	amtLabel->setTopLeftPosition(925, 710);
	addAndMakeVisible(amtLabel.get());

	vuMeter = std::make_unique<VUMeter>();
	vuMeter->setSize(128, 128);
	vuMeter->setTopLeftPosition(567, 665);
	addAndMakeVisible(vuMeter.get());

	sampleEditor = std::make_unique<SampleEditor>(audioProcessor.fmtMgr.get());
	sampleEditor->setTopLeftPosition(34, 34);
	sampleEditor->setSize(1190, 254);
	addAndMakeVisible(sampleEditor.get());

	propertyViewLeft = std::make_unique<PropertyView>(nullptr);
	propertyViewLeft->setTopLeftPosition(34, 330);
	propertyViewLeft->setSize(506, 200);
	addAndMakeVisible(propertyViewLeft.get());

	propertyViewLeft->browser->addChangeListener(this);

	propertyViewRight = std::make_unique<PropertyView>(nullptr);
	propertyViewRight->setTopLeftPosition(723,330);
	propertyViewRight->setSize(506, 200);
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
	cutoffSlider->addListener(this);
	resoSlider->addListener(this);
	amtSlider->addListener(this);
	pitchSlider->addListener(this);
	// driveSlider->addListener(this);
	loadSettings();

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
}

//==============================================================================
void SamAudioProcessorEditor::paint(juce::Graphics& g)
{
	/**/
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.drawImageAt(juce::ImageCache::getFromMemory(BinaryData::sam_png, BinaryData::sam_pngSize), 0, 0);

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
			ampEnvelope->setEnvelope(audioProcessor.samplers[midiNoteNumber]->envelope);
			filterEnvelope->setEnvelope(audioProcessor.filterEnvelope);
			this->loopButton->setToggleState(audioProcessor.samplers[midiNoteNumber]->isLoop(), juce::NotificationType::dontSendNotification);
			if (audioProcessor.samplers[midiNoteNumber]->hasSample()) {
				sampleEditor->setSampler(audioProcessor.samplers[midiNoteNumber]);
			}
			float a = audioProcessor.samplers[midiNoteNumber]->envelope->getParameters().attack;
			float d = audioProcessor.samplers[midiNoteNumber]->envelope->getParameters().decay;
			float s = audioProcessor.samplers[midiNoteNumber]->envelope->getParameters().sustain;
			float r = audioProcessor.samplers[midiNoteNumber]->envelope->getParameters().release;
			pitchSlider->setValue(audioProcessor.samplers[midiNoteNumber]->getPitchAsFloat());
			ampEnvelope->setADSR(a, d, s, r);
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
		stopTimer();

		// audioProcessor.samplers[midiNoteNumber]->stop();

	}
}

void SamAudioProcessorEditor::buttonClicked(juce::Button* button)
{

	if (button == loadButton.get()) {

		juce::FileChooser chooser("Select sample to add", juce::File("d:\\samples\\drums"), "*");

		if (chooser.browseForFileToOpen()) {

			juce::File file = chooser.getResult();

			Sampler* s = new Sampler(audioProcessor.sampleRate, audioProcessor.bufferSize);
			s->loadSample(file);
			s->setLoop(true);
			s->play();

			if (audioProcessor.samplers[audioProcessor.currentSampleIndex] != nullptr) {
				audioProcessor.samplers[audioProcessor.currentSampleIndex] = nullptr;
			}
			audioProcessor.samplers[audioProcessor.currentSampleIndex] = s;
		}
	}
	else if (button == loadSetButton.get()) {

		juce::FileChooser chooser("Select set to open", juce::File("d:\\samples\\drums"), "*");

		if (chooser.browseForFileToOpen()) {

			juce::File file = chooser.getResult();
			loadFile(file);

		}
	}
	else if (button == saveSetButton.get()) {


		juce::FileChooser chooser("Select target file", juce::File("d:\\samples\\drums"), "*");

		if (chooser.browseForFileToSave(true)) {

			juce::File file = chooser.getResult();

			juce::ValueTree audioFiles = juce::ValueTree("Samples");

			for (int i = 0; i < 128; i++) {
				if (audioProcessor.samplers[i] != nullptr) {
					juce::ValueTree file = juce::ValueTree("Sample");
					file.setProperty("note", i, nullptr);
					file.setProperty("loop", audioProcessor.samplers[i]->isLoop(), nullptr);
					file.setProperty("sample", audioProcessor.samplers[i]->getSampleLocation(), nullptr);
					file.setProperty("amp_attack", audioProcessor.samplers[i]->envelope->getParameters().attack, nullptr);
					file.setProperty("amp_decay", audioProcessor.samplers[i]->envelope->getParameters().decay, nullptr);
					file.setProperty("amp_sustain", audioProcessor.samplers[i]->envelope->getParameters().sustain, nullptr);
					file.setProperty("amp_release", audioProcessor.samplers[i]->envelope->getParameters().release, nullptr);
					file.setProperty("loopStart", audioProcessor.samplers[i]->getStartPosition(), nullptr);
					file.setProperty("loopEnd", audioProcessor.samplers[i]->getEndPosition(), nullptr);
					file.setProperty("pitch", audioProcessor.samplers[i]->getPitchAsFloat(), nullptr);

					audioFiles.addChild(file, -1, nullptr);
				}

			}

			std::unique_ptr < juce::XmlElement > xml = audioFiles.createXml();
			xml->writeToFile(file, "");
			xml = nullptr;
			saveSettings();
		}
	}
	else if (button = newSetButton.get()) {
		for (int i = 0; i < 128; i++) {
			if (audioProcessor.samplers[i] != nullptr) {
				delete audioProcessor.samplers[i];
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
}

void SamAudioProcessorEditor::timerCallback()
{
	if (audioProcessor.samplers[audioProcessor.currentSampleIndex] == nullptr) {
		return;
	}
	cutoffSlider->setValue(audioProcessor.cutoff);
	vuMeter->magnitude = audioProcessor.magnitude;
	repaint();

}

void SamAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster* source)
{
	ExtendedFileBrowser* efb = dynamic_cast<ExtendedFileBrowser*>(source);
	Sampler* s = new Sampler(audioProcessor.sampleRate, audioProcessor.bufferSize);
	s->loadSample(efb->getSelectedFile());
	s->setLoop(true);
	s->play();

	if (audioProcessor.samplers[audioProcessor.currentSampleIndex] != nullptr) {
		audioProcessor.samplers[audioProcessor.currentSampleIndex] = nullptr;
	}
	audioProcessor.samplers[audioProcessor.currentSampleIndex] = s;
	if (s->hasSample()) {
		sampleEditor->setSampler(s);
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

	audioProcessor.lpfLeftStage1->coefficients(audioProcessor.sampleRate, audioProcessor.cutoff, audioProcessor.resonance);

}

void SamAudioProcessorEditor::saveSettings()
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

	v->setProperty("currentFile", currentFile, nullptr);

	std::unique_ptr<XmlElement> xml = v->createXml();
	xml->writeToFile(configFile, "");

	xml = nullptr;
	delete v;
}

void SamAudioProcessorEditor::loadSettings()
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

		if (path.length() > 0) {
			loadFile(juce::File(path));
		}

		xml = nullptr;
	}
}

void SamAudioProcessorEditor::loadFile(juce::File file)
{
	std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument(file).getDocumentElement();
	juce::ValueTree v = juce::ValueTree::fromXml(*xml.get());
	xml = nullptr;

	for (int i = 0; i < 128; i++) {
		if (audioProcessor.samplers[i] != nullptr) {
			delete audioProcessor.samplers[i];
			audioProcessor.samplers[i] = nullptr;
		}
	}

	for (int i = 0; i < v.getNumChildren(); i++) {
		Sampler* s = new Sampler(audioProcessor.sampleRate, audioProcessor.bufferSize);
		s->loadSample(juce::File(v.getChild(i).getProperty("sample").toString()));

		juce::String sLoop = v.getChild(i).getProperty("loop").toString();

		if (sLoop == "true") {
			s->setLoop(true);
		}
		else {
			s->setLoop(false);
		}
		s->setStartPosition(v.getChild(i).getProperty("loopStart").toString().getLargeIntValue());
		s->setEndPosition(v.getChild(i).getProperty("loopEnd").toString().getLargeIntValue());
		s->setPitch(v.getChild(i).getProperty("pitch").toString().getFloatValue());

		juce::ADSR::Parameters params;
		params.attack = v.getChild(i).getProperty("amp_attack").toString().getFloatValue();
		params.decay = v.getChild(i).getProperty("amp_decay").toString().getFloatValue();
		params.sustain = v.getChild(i).getProperty("amp_sustain").toString().getFloatValue();
		params.release = v.getChild(i).getProperty("amp_release").toString().getFloatValue();

		s->envelope->setParameters(params);

		s->play();
		int index = v.getChild(i).getProperty("note").toString().getIntValue();
		audioProcessor.samplers[index] = s;

	}

	currentFile = file.getFullPathName();
	saveSettings();
}


