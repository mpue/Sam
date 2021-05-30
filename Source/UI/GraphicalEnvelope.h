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

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "../AudioEngine/ADSR.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class GraphicalEnvelope  : public juce::Component,
					public juce::ChangeBroadcaster
{
public:
    //==============================================================================
    GraphicalEnvelope ();
    ~GraphicalEnvelope();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void mouseDown(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;

	void setAttack(float attack);
	void setDecay(float decay);
	void setSustain(float sustain);
	void setRelease(float release);

	void setADSR(float a, float d, float s, float r);

	float getAttack();
	float getDecay();
	float getSustain();
	float getRelease();

	void updateModel();

    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;

	juce::ADSR* adsr = nullptr;

private:
    //[UserVariables]   -- You can add your own custom variables in this section.



	float attack;
	float decay;
	float sustain;
	float release;

	juce::Rectangle<float>* attackHandler = nullptr;
	bool isInsideAttackHandler = false;
	juce::Rectangle<float>* decayHandler = nullptr;
	bool isInsideDecayHandler = false;
	juce::Rectangle<float>* sustainHandler = nullptr;
	bool isInsideSustainHandler = false;
	juce::Rectangle<float>* releaseHandler = nullptr;
	bool isInsideReleaseHandler = false;

	float* dashes;
    //[/UserVariables]

    //==============================================================================


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphicalEnvelope)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

