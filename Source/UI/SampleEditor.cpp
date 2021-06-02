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
	addMouseListener(this, true);
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
		float scaledSamplePosX = samplePosX / zoom - offset;
		float scaledSampleStartPosX = sampleStartPosX / zoom - offset;
		float scaledSampleEndPosX = sampleEndPosX / zoom - offset;

		juce::Line<float> samplePosIndicator = juce::Line<float>(scaledSamplePosX, 0, scaledSamplePosX, getHeight());
		juce::Line<float> startPosIndicator = juce::Line<float>(scaledSampleStartPosX, 0, scaledSampleStartPosX, getHeight());
		juce::Line<float> endPosIndicator = juce::Line<float>(scaledSampleEndPosX, 0, scaledSampleEndPosX, getHeight());
		juce::Line<float> startPosIndicatorArrow = juce::Line<float>(scaledSampleStartPosX, 0, scaledSampleStartPosX, 12);
		juce::Line<float> endPosIndicatorArrow = juce::Line<float>(scaledSampleEndPosX, 0, scaledSampleEndPosX, 12);
		float scaledOffset = (offset * ((float)sampler->getSampleLength()) / sampler->getSampleRate()) / getWidth();
		this->thumbnail->drawChannels(g, tb, scaledOffset * zoom, (sampleLength + scaledOffset) * zoom, 1);
		g.setColour(juce::Colour::fromFloatRGBA(1.0f, 1.0f, 1.0f, 0.3f));
		//		g.fillRect(0.0f, 0.0f, (sampleStartPosX - offset ) * zoom , (float)getHeight());
				//g.fillRect((sampleEndPosX - offset) * zoom , 0.0f, (float)getWidth(), (float)getHeight());
		g.setColour(juce::Colours::white);
		g.drawLine(samplePosIndicator, 3);
		g.setColour(juce::Colours::green);
		g.drawLine(startPosIndicator, 3);
		g.drawArrow(startPosIndicatorArrow, 3, 16, 16);
		g.setColour(juce::Colours::red);
		g.drawArrow(endPosIndicatorArrow, 3, 16, 16);
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
		samplePosX = ((float)(sampler->getCurrentPosition() / (float)sampler->getSampleLength()) * getWidth());

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
	stopTimer();
	if (event.mods.isLeftButtonDown()) {

		juce::Point<float> p = event.position;// getLocalPoint(this, event.position);
		if (abs(p.x - (sampleEndPosX / zoom - offset)) < 5) {
			draggingEnd = true;
			dragStartX = p.x;
			deltaX = 0;
			originX = sampleEndPosX;
		}
		else if (abs(p.x - (sampleStartPosX / zoom - offset)) < 5) {
			draggingStart = true;
			dragStartX = p.x;
			deltaX = 0;
			originX = sampleStartPosX;
		}

		else {
			setMouseCursor(juce::MouseCursor::NormalCursor);
		}
	}

	zoomStart = zoom;
	offsetStart = offset;
}

void SampleEditor::mouseUp(const juce::MouseEvent& event)
{
	draggingEnd = false;
	draggingStart = false;
	startTimerHz(50);
}

void SampleEditor::mouseDrag(const juce::MouseEvent& event)
{
	juce::Point<float> p = event.position;//  getLocalPoint(this, event.position);

	if (event.mods.isRightButtonDown()) {
		zoom = zoomStart - (float)event.getDistanceFromDragStartX() / 100.0f;
		repaint();

	}
	else if (event.mods.isMiddleButtonDown()) {
		offset = offsetStart - (float)event.getDistanceFromDragStartX();
		repaint();
	}

	else {
		if (draggingEnd) {
			deltaX = p.x - dragStartX;
			sampleEndPosX = originX + deltaX * zoom;
			if (sampleEndPosX > sampler->getSampleLength()) {
				sampleEndPosX = sampler->getSampleLength();
			}
			sampler->setEndPosition(sampleEndPosX * sampler->getSampleLength() / getWidth());
			repaint();
		}
		if (draggingStart) {
			deltaX = p.x - dragStartX;
			sampleStartPosX = originX + deltaX * zoom;
			if (sampleStartPosX < 0) {
				sampleStartPosX = 0;
			}
			sampler->setStartPosition(sampleStartPosX * sampler->getSampleLength() / getWidth());
			repaint();
		}
	}


}

void SampleEditor::mouseMove(const juce::MouseEvent& event)
{
	juce::Point<float> p = event.position;// getLocalPoint(this, event.position);

	juce::Logger::getCurrentLogger()->writeToLog(juce::String(abs(p.x - (sampleEndPosX / zoom - offset))));

	if (abs(p.x - (sampleStartPosX / zoom - offset)) < 5 || abs(p.x - (sampleEndPosX / zoom - offset)) < 5) {
		setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
	}
	else {
		setMouseCursor(juce::MouseCursor::NormalCursor);
	}

}
