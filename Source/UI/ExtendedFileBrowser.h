//
//  ExtendedFileBrowser.hpp
//  Synthlab - App
//
//  Created by Matthias Pueski on 30.04.18.
//  Copyright © 2018 Pueski. All rights reserved.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../AudioEngine/Sampler.h"
using juce::Button;
class Sampler;
class FileBrowserModel : public juce::TableListBoxModel {
public:
    
    FileBrowserModel(juce::DirectoryContentsList* directoryList, juce::File& initalDir);
    
    virtual void paintRowBackground (juce::Graphics& g,
                                     int rowNumber,
                                     int width, int height,
                                     bool rowIsSelected) override;
    virtual void paintCell (juce::Graphics& g,
                            int rowNumber,
                            int columnId,
                            int width, int height,
                            bool rowIsSelected) override;
    virtual int getNumRows() override;
    void setCurrentDir(juce::File* dir);
    void update();
    juce::DirectoryContentsList* getDirectoryList();
    
    juce::String& getCurrentDir() {
        return currentDirectory;
    }
    
    juce::var getDragSourceDescription (const juce::SparseSet<int>& currentlySelectedRows) override{
    //    
    //    if (!currentlySelectedRows.getTotalRange().isEmpty()) {
    //        juce::File file = directoryList->getFile(currentlySelectedRows.getTotalRange().getStart());
    //        return file.getFullPathName();
    //    }
    //    
    //    return juce::var();
        return {};
    }
    


private:
    juce::DirectoryContentsList* directoryList;
    juce::String currentDirectory;

};

class ExtendedFileBrowser : public juce::Component,  public juce::ChangeListener, public juce::Timer, public juce::ChangeBroadcaster, public Button::Listener
{
    
    
public:
    ExtendedFileBrowser(const juce::File& initialFileOrDirectory,const juce::WildcardFileFilter* fileFilter, FileBrowserModel* model, Sampler* sampler);
    ~ExtendedFileBrowser();
    
    void mouseDoubleClick(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void paint (juce::Graphics& g) override;
    void resized() override;
    virtual void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    juce::File getSelectedFile();
    void saveState();
    void loadState();
    void buttonClicked(Button* button) override;
    juce::TableListBox* getTable() {
        return table.get();
    };
    
    FileBrowserModel* getModel() {
        return model;
    }
    
    void selectNextFile();
    void selectPreviousFile();
    void playFile(int row);
    
    virtual void timerCallback() override;
    
    Sampler* getSampler() {
        return sampler;
    }
    
    FileBrowserModel* model = nullptr;
private:
    const juce::File& initialDir;
    const juce::WildcardFileFilter* filter;
    juce::File selectedFile;
    std::unique_ptr<juce::TableListBox> table = nullptr;
    std::unique_ptr <juce::Viewport> view = nullptr;
    Sampler* sampler;
    bool playing = false;
    std::vector<std::unique_ptr<Button>> driveButtons;

};

