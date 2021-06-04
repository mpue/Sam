/*
  ==============================================================================

    ControllerMappings.cpp
    Created: 4 Jun 2021 3:24:39pm
    Author:  mpue

  ==============================================================================
*/

#include "ControllerMappings.h"

ControllerMappings::ControllerMappings()
{
}

ControllerMappings::~ControllerMappings()
{
}

void ControllerMappings::addMapping(int controller, juce::Component* component)
{
    mappings.insert(std::make_pair(controller, component));
}

void ControllerMappings::removeMapping(int controller)
{
    std::map<int, juce::Component*>::iterator iter = mappings.find(1);
    if (iter != mappings.end())
        mappings.erase(iter);
}

void ControllerMappings::removeMapping(juce::String controllerName)
{
    std::map<int, juce::Component*>::iterator iter = mappings.begin();

    while (iter != mappings.end()) {
        
        if (iter->second->getName() == controllerName) {
            iter = mappings.erase(iter++);
        }
        else {
            ++iter;
        }

    }
}

juce::Component* ControllerMappings::getMapping(int controller)
{
    if (mappings.find(controller) != mappings.end()) {
        return mappings[controller];
    }
    else {
        return nullptr;
    }    
}
