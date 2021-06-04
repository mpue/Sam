/*
  ==============================================================================

    ControllerMappings.h
    Created: 4 Jun 2021 3:24:39pm
    Author:  mpue

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <map>
class ControllerMappings {

public:
    ControllerMappings();
    ~ControllerMappings();

    void addMapping(int controller, juce::Component* component);
    void removeMapping(int controller);
    void removeMapping(juce::String controllerName);
    juce::Component* getMapping(int controller);

private:
    std::map<int, juce::Component*> mappings;

};