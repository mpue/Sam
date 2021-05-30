/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.3

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "GraphicalEnvelope.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
GraphicalEnvelope::GraphicalEnvelope()
{
	//[Constructor_pre] You can add your own custom stuff here..

	//[/Constructor_pre]


	//[UserPreSize]
	//[/UserPreSize]

	setSize(600, 400);


	//[Constructor] You can add your own custom stuff here..
	addMouseListener(this, true);
	attackHandler = new juce::Rectangle<float>(10, 10, 10, 10);
	decayHandler = new juce::Rectangle<float>(100, 50, 10, 10);
	sustainHandler = new juce::Rectangle<float>(200, 50, 10, 10);
	releaseHandler = new juce::Rectangle<float>(390, 80, 10, 10);
	dashes = new float[2]{ 5, 5 };
	//[/Constructor]
}

GraphicalEnvelope::~GraphicalEnvelope()
{
	//[Destructor_pre]. You can add your own custom destruction code here..
	//[/Destructor_pre]



	//[Destructor]. You can add your own custom destruction code here..
	delete attackHandler;
	delete decayHandler;
	delete sustainHandler;
	delete releaseHandler;
	delete dashes;
	//[/Destructor]
}

//==============================================================================
void GraphicalEnvelope::paint(juce::Graphics& g)
{
	//[UserPrePaint] Add your own custom painting code here..
	//[/UserPrePaint]

	g.fillAll(juce::Colour(0xff616161));

	//[UserPaint] Add your own custom painting code here..


	g.setColour(juce::Colours::grey);
	juce::Line<float> a = juce::Line<float>(attackHandler->getPosition().x + 5, 0, attackHandler->getPosition().x + 5, getHeight());
	juce::Line<float> d = juce::Line<float>(decayHandler->getPosition().x + 5, 0, decayHandler->getPosition().x + 5, getHeight());
	juce::Line<float> s = juce::Line<float>(sustainHandler->getPosition().x + 5, 0, sustainHandler->getPosition().x + 5, getHeight());
	juce::Line<float> r = juce::Line<float>(releaseHandler->getPosition().x + 5, 0, releaseHandler->getPosition().x + 5, getHeight());

	juce::Line<float> level = juce::Line<float>(0, sustainHandler->getPosition().y + 5, getWidth(), sustainHandler->getPosition().y + 5);

	g.drawDashedLine(a, dashes, 2, 1.0f);
	g.drawDashedLine(d, dashes, 2, 1.0f);
	g.drawDashedLine(s, dashes, 2, 1.0f);
	g.drawDashedLine(r, dashes, 2, 1.0f);
	g.drawDashedLine(level, dashes, 2, 1.0f);
	g.setColour(juce::Colours::darkgrey.darker());
	g.drawLine(15, getHeight(), attackHandler->getPosition().x + 5, attackHandler->getPosition().y + 5, 2.0f);
	g.drawLine(attackHandler->getPosition().x + 5, attackHandler->getPosition().y + 5, decayHandler->getPosition().x + 5, decayHandler->getPosition().y + 5, 2.0f);
	g.drawLine(decayHandler->getPosition().x + 5, decayHandler->getPosition().y + 5, sustainHandler->getPosition().x + 5, sustainHandler->getPosition().y + 5, 2.0f);
	g.drawLine(sustainHandler->getPosition().x + 5, sustainHandler->getPosition().y + 5, releaseHandler->getPosition().x + 5, releaseHandler->getPosition().y + 5, 2.0f);

	if (isInsideAttackHandler) {
		g.setColour(juce::Colours::yellow);
		g.drawText("attack " + juce::String(attack), 10, getHeight() - 20, 100, 25,juce::Justification::left);
	}
	else {
		g.setColour(juce::Colours::white);
	}

	g.drawRect(*attackHandler, 2.0f);

	if (isInsideDecayHandler) {
		g.drawText("decay " + juce::String(decay), 10, getHeight() - 20, 100, 25, juce::Justification::left);
		g.setColour(juce::Colours::yellow);
	}
	else {
		g.setColour(juce::Colours::white);
	}

	g.drawRect(*decayHandler, 2.0f);

	if (isInsideSustainHandler) {
		g.drawText("sustain " + juce::String(sustain), 10, getHeight() - 40, 100, 25, juce::Justification::left);
		g.drawText("release " + juce::String(release), 10, getHeight() - 20, 100, 25, juce::Justification::left);
		g.setColour(juce::Colours::yellow);
	}
	else {
		g.setColour(juce::Colours::white);
	}

	g.drawRect(*sustainHandler, 2.0f);

	if (isInsideReleaseHandler) {
		g.drawText("sustain " + juce::String(sustain), 10, getHeight() - 40, 100, 25, juce::Justification::left);
		g.drawText("release " + juce::String(release), 10, getHeight() - 20, 100, 25, juce::Justification::left);

		g.setColour(juce::Colours::yellow);
	}
	else {
		g.setColour(juce::Colours::white);
	}

	g.drawRect(*releaseHandler, 2.0f);

	g.setColour(juce::Colours::darkgrey.darker());
	g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 5, 2.0f);


	//[/UserPaint]
}

void GraphicalEnvelope::resized()
{
	//[UserPreResize] Add your own custom resize code here..
	//[/UserPreResize]

	if (releaseHandler != nullptr) {
		releaseHandler->setPosition(releaseHandler->getPosition().x, getHeight() - 10);
	}

	//[UserResized] Add your own custom resize handling here..
	//[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void GraphicalEnvelope::mouseDown(const juce::MouseEvent& event) {

}

void GraphicalEnvelope::mouseUp(const juce::MouseEvent& event) {

}

void GraphicalEnvelope::mouseDrag(const juce::MouseEvent& event) {
	juce::Point <float> pos = juce::Point<float>(event.getPosition().x - 5, event.getPosition().y - 5);

	if (pos.x < 10)
		pos.x = 10;
	if (pos.x > getWidth() - 10)
		pos.x = getWidth() - 10;
	if (pos.y < 10)
		pos.y = 10;
	if (pos.y > getHeight() - 10)
		pos.y = getHeight() - 10;

	if (isInsideAttackHandler) {

		if (pos.y > 10)
			pos.y = 10;

		pos.y = attackHandler->getPosition().y;

		if (pos.x + 10 >= decayHandler->getPosition().x) {
			pos.x = decayHandler->getPosition().x - 10;			
		}
		attackHandler->setPosition(pos);
	}
	if (isInsideDecayHandler) {
		if (pos.x + 10 >= sustainHandler->getPosition().x) {
			pos.x = sustainHandler->getPosition().x - 10;
		}
		if (pos.x - 10 <= attackHandler->getPosition().x) {
			pos.x = attackHandler->getPosition().x + 10;
		}
		pos.y = decayHandler->getPosition().y;
		decayHandler->setPosition(pos);
		
	}
	if (isInsideSustainHandler) {

		if (pos.x + 10 >= releaseHandler->getPosition().x) {
			pos.x = releaseHandler->getPosition().x - 10;
		}
		if (pos.x - 10 <= decayHandler->getPosition().x) {
			pos.x = decayHandler->getPosition().x + 10;
		}

		decayHandler->setPosition(decayHandler->getPosition().x, pos.y);
		sustainHandler->setPosition(pos);
		
	}
	if (isInsideReleaseHandler) {

		if (pos.x - 10 <= sustainHandler->getPosition().x) {
			pos.x = sustainHandler->getPosition().x + 10;
		}
		if (pos.y < getHeight() - 10)
			pos.y = getHeight() - 10;

		releaseHandler->setPosition(pos);
	}

	repaint();

	attack = (getWidth() / 400) * (attackHandler->getPosition().x) / 100;
	decay =  (getWidth() / 400) * abs(decayHandler->getPosition().x - attackHandler->getPosition().x) / 100;
	sustain = ((getHeight() - sustainHandler->getPosition().y) / getHeight());
	release =  (getWidth() / 400) * abs(sustainHandler->getPosition().x - releaseHandler->getPosition().x) / 100;

	updateModel();

}

void GraphicalEnvelope::mouseMove(const juce::MouseEvent& event) {

	juce::Point <float> pos = juce::Point<float>(event.getPosition().x, event.getPosition().y);
	isInsideAttackHandler = attackHandler->contains(pos);
	isInsideDecayHandler = decayHandler->contains(pos);
	isInsideSustainHandler = sustainHandler->contains(pos);
	isInsideReleaseHandler = releaseHandler->contains(pos);
	repaint();
}

void GraphicalEnvelope::updateModel() {
	if (adsr != nullptr) {

		juce::ADSR::Parameters params;
		params.attack = attack;
		params.decay = decay;
		params.sustain = sustain;
		params.release = release;
		adsr->setParameters(params);

	}
	sendChangeMessage();

}

void GraphicalEnvelope::setAttack(float attack) {
	this->attack = attack;
	float x = 0;
	if (attack >= 0) {
		x = attack * (getWidth() / 4);
		if (x < 10) {
			x = 10;
		}
	}

	attackHandler->setPosition(x, 10);
}
void GraphicalEnvelope::setDecay(float decay) {
	this->decay = decay;
	float x = 0;
	float y = 0;

	if (decay >= 0) {
		x = decay * (getWidth() / 4) + attackHandler->getPosition().x;
	}
	decayHandler->setPosition(x, sustainHandler->getPosition().y);

}

void GraphicalEnvelope::setSustain(float sustain) {
	this->sustain = sustain;
	float y = getHeight() - 10;
	float x = 0;
	if (release >= 0) {
		x = getWidth() / 4- (release * (getWidth() / 4));
	}

	if (sustain >= 0) {
		y = getHeight() - (float)getHeight() * sustain;
		if (y >= getHeight()) {
			y = getHeight() - 10;
		}
	}
	sustainHandler->setPosition(sustainHandler->getPosition().x, y);
}

void GraphicalEnvelope::setRelease(float release) {
	this->release = release;
	setSustain(getSustain());
}

void GraphicalEnvelope::setADSR(float a, float d, float s, float r)
{
	this->attack = a;
	this->decay = d;
	this->sustain = s;
	this->release = r;

	float x = 0;
	float y = 0;
	float decay_x = 0;

	if (attack >= 0) {
		x = attack * (getWidth() / 4);
		if (x < 10) {
			x = 10;
		}
	}
	
	attackHandler->setPosition(x, 10);

	if (decay >= 0) {
		decay_x = decay * (getWidth() / 4) + attackHandler->getPosition().x;
	}

	y = getHeight() - 10;

	if (release >= 0) {
		x = getWidth() / 4 - (release * (getWidth() / 4));
	}

	if (sustain >= 0) {
		y = getHeight() - (float)getHeight() * sustain;
		if (y >= getHeight()) {
			y = getHeight() - 10;
		}
	}
	
	sustainHandler->setPosition(sustainHandler->getPosition().x, y);
	decayHandler->setPosition(decay_x, sustainHandler->getPosition().y);


}

float GraphicalEnvelope::getAttack() {
	return attack;
}

float GraphicalEnvelope::getDecay() {
	return decay;
}

float GraphicalEnvelope::getSustain() {
	return sustain;
}

float GraphicalEnvelope::getRelease() {
	return release;
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

	This is where the Projucer stores the metadata that describe this GUI layout, so
	make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="GraphicalEnvelope" componentName=""
				 parentClasses="public Component, public ChangeBroadcaster" constructorParams="Model* model"
				 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
				 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff616161"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

