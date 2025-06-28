#include <iomanip>
#include "SequenceEditor.h"
#include "../Event.h"

using juce::String;
using juce::TextButton;
using juce::Slider;
using juce::Viewport;
using juce::Graphics;
using juce::Button;
using juce::MouseEvent;
using juce::KeyPress;
using juce::ModifierKeys;
using juce::MidiMessage;
using juce::MidiInput;
using juce::Logger;
using juce::uint8;

const String SequenceEditor::notes[128] = { "C","C#", "D", "D#", "E", "F", "F#", "G", "G#" , "A" ,"A#", "B" };
int SequenceEditor::selectedCol = 0;
int SequenceEditor::selectedRow = 0;
bool SequenceEditor::optionPressed = false;
bool SequenceEditor::showVelocity = false;

SequenceEditor::SequenceEditor(SamAudioProcessor* p, Sequencer* s)
{

	this->sequencer = s;

	addAndMakeVisible(playButton = new TextButton("Play"));
	playButton->setButtonText(TRANS("Play"));
	playButton->addListener(this);

	playButton->setBounds(10, 10, 150, 24);

	addAndMakeVisible(recordButton = new TextButton("Record"));
	recordButton->setButtonText(TRANS("Record"));
	recordButton->addListener(this);

	recordButton->setBounds(170, 10, 150, 24);

	addAndMakeVisible(clearButton = new TextButton("Clear"));
	clearButton->setButtonText(TRANS("Clear"));
	clearButton->addListener(this);

	clearButton->setBounds(330, 10, 150, 24);

	addAndMakeVisible(lengthSlider = new Slider("lengthSlider"));
	lengthSlider->setRange(1, 64, 1);
	lengthSlider->setSliderStyle(Slider::LinearHorizontal);
	lengthSlider->setTextBoxStyle(Slider::TextBoxLeft, false, 80, 20);
	lengthSlider->addListener(this);
	lengthSlider->setBounds(500, 10, 150, 24);

	setSize(800, 768);

	content = new SequencePanel(sequencer);

	view = new Viewport();
	view->setViewedComponent(content);

	setSize(gridWidth * cellSize, gridHeight * cellSize + 100);
	setBounds(0, 0, gridWidth * cellSize, gridHeight * cellSize + 100);

	for (int x = 0; x < gridWidth; x++) {
		for (int y = 0; y < gridHeight; y++) {
			Cell c = Cell();
			c.setEnabled(false);
			c.setNote(48);
			c.setVelocity(64);
			sequencer->grid[x][y] = c;
		}
	}

	selectedCol = 0;
	selectedRow = 0;
	addAndMakeVisible(view);

	setWantsKeyboardFocus(true);
	setInterceptsMouseClicks(true, true);
	addMouseListener(content, true);

}

SequenceEditor::~SequenceEditor()
{
	playButton = nullptr;
	recordButton = nullptr;
	clearButton = nullptr;
	lengthSlider = nullptr;
	delete view;
}

//==============================================================================
void SequenceEditor::paint(Graphics& g)
{

}

void SequenceEditor::resized()
{
	if (content != nullptr && view != nullptr) {
		view->setTopLeftPosition(0, 50);
		view->setSize(getWidth(), cellSize * 7);
		content->setSize(cellSize * 32, cellSize * 6);
	}
}

void SequenceEditor::buttonClicked(Button* buttonThatWasClicked)
{

	if (buttonThatWasClicked == playButton)
	{
		sequencer->setRunning(true);

	}
	else if (buttonThatWasClicked == recordButton)
	{
		sequencer->setRunning(false);
	}
	else if (buttonThatWasClicked == clearButton) {
		clearSequence();
	}
}

void SequenceEditor::sliderValueChanged(juce::Slider* slider) {
	if (slider == lengthSlider) {
		gridWidth = slider->getValue();
		repaint();
	}
}

void SequenceEditor::mouseMove(const MouseEvent& e)
{
	mouseX = e.getPosition().getX();
	mouseY = e.getPosition().getY();

	std::function<void(void)> changeLambda =
		[=]() {  content->repaint(); };
	juce::MessageManager::callAsync(changeLambda);


}

void SequenceEditor::mouseDown(const MouseEvent& e)
{

	// toggle cell
	sequencer->grid[e.getPosition().getX() / cellSize][e.getPosition().getY() / cellSize].setEnabled(!sequencer->grid[e.getPosition().getX() / cellSize][e.getPosition().getY() / cellSize].isEnabled());
}

void SequenceEditor::mouseDrag(const MouseEvent& e)
{

	int delta = -e.getDistanceFromDragStartY() / 20;

	if (!showVelocity) {

		int note = sequencer->grid[e.getMouseDownX() / cellSize][e.getMouseDownY() / cellSize].getNote();
		if (note + delta > 127) {
			note = 127;
		}
		else if (note + delta < 0) {
			note = 0;
		}
		else
		{
			note += delta;
		}

		sequencer->grid[e.getMouseDownX() / cellSize][e.getMouseDownY() / cellSize].setNote(note);

	}
	else {
		int velocity = sequencer->grid[e.getMouseDownX() / cellSize][e.getMouseDownY() / cellSize].getVelocity();
		if (velocity + delta > 127) {
			velocity = 127;
		}
		else if (velocity + delta < 0) {
			velocity = 0;
		}
		else
		{
			velocity += delta;
		}

		sequencer->grid[e.getMouseDownX() / cellSize][e.getMouseDownY() / cellSize].setVelocity(velocity);
	}

	std::function<void(void)> changeLambda =
		[=]() {  content->repaint(); };
	juce::MessageManager::callAsync(changeLambda);


}

void SequenceEditor::mouseUp(const MouseEvent& e)
{

}

bool SequenceEditor::keyPressed(const KeyPress& key)
{

	if (key.getKeyCode() == KeyPress::spaceKey) {
		if (!sequencer->isRunning()) {
			sequencer->setRunning(true);
		}
		else {
			sequencer->setRunning(false);
		}
	}
	if (key.getKeyCode() == KeyPress::returnKey) {
		sequencer->grid[selectedCol][selectedRow].setEnabled(!sequencer->grid[selectedCol][selectedRow].isEnabled());
	}

	int velocity = sequencer->grid[selectedCol][selectedRow].getVelocity();
	int note = sequencer->grid[selectedCol][selectedRow].getNote();

	if (key.getKeyCode() == KeyPress::upKey) {
		if (showVelocity) {

			if (velocity < 127) {
				velocity++;
			}
			sequencer->grid[selectedCol][selectedRow].setVelocity(velocity);
		}
		else if (optionPressed) {
			note = note + 1 % 127;
			sequencer->grid[selectedCol][selectedRow].setNote(note);
		}
		else {
			if (selectedRow > 0) {
				selectedRow--;
			}
		}

	}
	if (key.getKeyCode() == KeyPress::downKey) {

		if (showVelocity) {

			if (velocity > 0) {
				velocity--;
			}
			sequencer->grid[selectedCol][selectedRow].setVelocity(velocity);
		}
		else if (optionPressed) {

			if (note > 0)
				note = note - 1 % 127;

			sequencer->grid[selectedCol][selectedRow].setNote(note);
		}
		else {
			if (selectedRow < 5) {
				selectedRow++;
			}
		}

	}
	if (key.getKeyCode() == KeyPress::leftKey) {
		if (selectedCol > 0) {
			selectedCol--;
		}
	}
	if (key.getKeyCode() == KeyPress::rightKey) {
		if (selectedCol < 63) {
			selectedCol++;
		}
	}
	
	repaint();
	content->repaint();

	return true;

}

void SequenceEditor::modifierKeysChanged(const ModifierKeys& modifiers)
{
	optionPressed = modifiers.isAltDown();
	showVelocity = modifiers.isShiftDown();
	repaint();
}

void SequenceEditor::clearSequence() {
	for (int x = 0; x < gridWidth; x++) {
		for (int y = 0; y < gridHeight; y++) {
			sequencer->grid[x][y].setLength(0);
			sequencer->grid[x][y].setEnabled(false);
			sequencer->grid[x][y].setNote(36);
			sequencer->grid[x][y].setVelocity(64);

		}
	}
	repaint();
}

void SequenceEditor::handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) {


	if (message.isNoteOn() || message.isNoteOff()) {
		Logger::writeToLog("received message " + message.getDescription());
		if (message.isNoteOn()) {
			sequencer->grid[selectedCol][selectedRow].setNote(message.getNoteNumber());
			sequencer->grid[selectedCol][selectedRow].setVelocity(message.getVelocity());
			sequencer->grid[selectedCol][selectedRow].setEnabled(true);
			std::function<void(void)> changeLambda =
				[this]() { repaint(); };
			juce::MessageManager::callAsync(changeLambda);
		}
	}
}



void SequenceEditor::timerCallback() {
	repaint();
}




