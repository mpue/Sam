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


//[/Headers]
using namespace juce;


//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class EnvelopePanel  : public Component,
                       public ChangeBroadcaster,
                       public Slider::Listener
{
public:
    //==============================================================================
    EnvelopePanel ();
    ~EnvelopePanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void initAttachments();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;

    void setEnvelope(juce::ADSR* adsr) {
        this->adsr = adsr;
    }

    void setADSR(float a, float d, float s, float r) {
        this->a = a;
        this->d = d;
        this->s = s;
        this->r = r;
        amp_attack->setValue(a);
        amp_decay->setValue(d);
        amp_sustain->setValue(s);
        amp_release->setValue(r);
    }

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Slider> amp_attack;
    std::unique_ptr<Slider> amp_decay;
    std::unique_ptr<Slider> amp_sustain;
    std::unique_ptr<Slider> amp_release;
    std::unique_ptr<Label> attack;
    std::unique_ptr<Label> decay;
    std::unique_ptr<Label> sustain;
    std::unique_ptr<Label> Release;

    juce::ADSR* adsr = nullptr;
    float a, d, s, r;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopePanel)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

