/*
  ==============================================================================

	SampleEditor.cpp
	Created: 31 May 2021 4:45:05pm
	Author:  mpue

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SampleEditor.h"

//==============================================================================
SampleEditor::SampleEditor(juce::AudioFormatManager* fmtMgr)
{
	// In your constructor, you should add any child components, and
	// initialise any special settings that your component needs.
	cache = std::make_unique<juce::AudioThumbnailCache>(1);
	thumbnail = std::make_unique<juce::AudioThumbnail>(4, *fmtMgr, *cache.get());
	addMouseListener(this,true);
}

SampleEditor::~SampleEditor()
{
	cache = nullptr;
	thumbnail = nullptr;
}

void SampleEditor::paint(juce::Graphics& g)
{
	juce::Rectangle<int> tb = juce::Rectangle<int>(0, 0, getWidth(), getHeight());
	g.setColour(juce::Colours::darkgrey.darker());
	g.fillRect(tb);
	g.setColour(juce::Colours::orange);
	if (sampler != nullptr) {
		juce::Line<float> samplePosIndicator = juce::Line<float>(samplePosX, 0, samplePosX, getHeight());
		juce::Line<float> startPosIndicator = juce::Line<float>(sampleStartPosX, 0, sampleStartPosX, getHeight());
		juce::Line<float> endPosIndicator = juce::Line<float>(sampleEndPosX, 0, sampleEndPosX, getHeight());
		this->thumbnail->drawChannels(g, tb, 0, sampleLength, 1);		
		g.setColour(juce::Colours::white);
		g.drawLine(samplePosIndicator, 3);
		g.setColour(juce::Colours::green);
		g.drawLine(startPosIndicator, 3);
		g.setColour(juce::Colours::green);
		g.drawLine(endPosIndicator, 3);
	}
	else {
		g.drawText("No sample assigned.", getWidth() / 2 - g.getCurrentFont().getStringWidth("No sample assigned.") / 2, getHeight() / 2, 150, 50, juce::Justification::centred);
	}
}

void SampleEditor::resized()
{
}

void SampleEditor::reset(double sampleRate)
{
	this->thumbnail->reset(2, sampleRate);
}

void SampleEditor::timerCallback()
{
	if (sampler != nullptr) {
		samplePosX = (float)(sampler->getCurrentPosition() / (float)sampler->getSampleLength()) * getWidth();
		
		sampleStartPosX = (float)(sampler->getStartPosition() / (float)sampler->getSampleLength()) * getWidth();
		sampleEndPosX = (float)(sampler->getEndPosition() / (float)sampler->getSampleLength()) * getWidth();
	}
}

void SampleEditor::setSampler(Sampler* sampler)
{
	this->sampler = sampler;

	if (sampler != nullptr) {
		reset(sampler->getSampleRate());
		sampleLength = (float)sampler->getSampleLength() / sampler->getSampleRate();
		thumbnail->addBlock(0, *sampler->getSampleBuffer(), 0, static_cast<int>(sampler->getSampleLength()));
	}
}

void SampleEditor::mouseDown(const juce::MouseEvent& event)
{
	juce::Point<float> p = event.position;// getLocalPoint(this, event.position);
	stopTimer();
	if (abs(p.x - sampleEndPosX) < 5) {		
		draggingEnd = true;
		dragStartX = p.x;
		deltaX = 0;
		originX = sampleEndPosX;
	}
	else {
		setMouseCursor(juce::MouseCursor::NormalCursor);
	}
}

void SampleEditor::mouseUp(const juce::MouseEvent& event)
{
	if (draggingEnd) {
		sampler->setEndPosition(sampleEndPosX * sampler->getSampleLength() / getWidth());
	}

	draggingEnd = false;
	draggingStart = false;
	startTimerHz(50);
}

void SampleEditor::mouseDrag(const juce::MouseEvent& event)
{
	juce::Point<float> p = event.position;//  getLocalPoint(this, event.position);

	if (draggingEnd) {
		deltaX = p.x - dragStartX;
		sampleEndPosX = originX + deltaX;
		repaint();
	}
}

void SampleEditor::mouseMove(const juce::MouseEvent& event)
{
	juce::Point<float> p = event.position;// getLocalPoint(this, event.position);

	if (abs(p.x - sampleEndPosX) < 5) {
		setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
	}
	else {
		setMouseCursor(juce::MouseCursor::NormalCursor);
	}

}
