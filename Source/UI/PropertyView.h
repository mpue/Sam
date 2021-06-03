/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.2.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"

#include "ExtendedFileBrowser.h"
//[/Headers]

//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class Sampler;
class PropertyView  : public juce::Component,
                      public juce::TimeSliceThread,
                      public juce::TimeSliceClient
            
{
public:
    //==============================================================================
    PropertyView (Sampler* sampler);
    ~PropertyView();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

    virtual int useTimeSlice() override;
    ExtendedFileBrowser* getBrowser();
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    ExtendedFileBrowser* browser = nullptr;

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

    juce::PropertyPanel* propertyPanel = nullptr;
    juce::ScopedPointer<juce::TextEditor> descriptionEditor;
    juce::DirectoryContentsList* directoryContents = nullptr;
    juce::WildcardFileFilter* filter = nullptr;
    //[/UserVariables]

    //==============================================================================

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertyView)
};

//[EndFile] You can add extra defines here...
//[/EndFile]
