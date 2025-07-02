#include "../AudioEngine/Sampler.h"
#include "PropertyView.h"

using juce::TabbedButtonBar;
using juce::TabbedComponent;
using juce::TextEditor;
using juce::WildcardFileFilter;
using juce::DirectoryContentsList;
using juce::File;
using juce::PropertyPanel;
using juce::Graphics;
using juce::Colour;

PropertyView::PropertyView (Sampler* sampler) : TimeSliceThread("PropertyWatcher")
{
    setSize (600, 400);
    addTimeSliceClient(this);
    setSize(getParentWidth(), getParentHeight());    
    filter = std::make_unique<WildcardFileFilter>("*.wav;*.aif;*.aiff;*.mp3;*.sam","*","*");
        directoryContents = std::make_unique<DirectoryContentsList>(filter.get(), *this);
    directoryContents->setIgnoresHiddenFiles(false);
    File initialDir = File::getSpecialLocation(File::userHomeDirectory);
    model = std::make_unique<FileBrowserModel>(directoryContents.get(), initialDir);
    browser = std::make_unique<ExtendedFileBrowser>(File::getSpecialLocation(File::userHomeDirectory),filter.get(), model.get(), sampler);
    directoryContents->addChangeListener(browser.get());
    addAndMakeVisible(browser.get());
    startThread();
    browser->resized();
}

PropertyView::~PropertyView()
{
    removeTimeSliceClient(this);
    filter = nullptr;
    model = nullptr;
    browser = nullptr;
    stopThread(100);
    directoryContents = nullptr;
}

void PropertyView::paint(Graphics& g)
{
    g.fillAll(Colour(0xff222222));
}

void PropertyView::resized()
{
    if (browser != nullptr) {
        browser->resized();
    }
}

int PropertyView::useTimeSlice() {
    return 1000;
}

ExtendedFileBrowser* PropertyView::getBrowser() {
    return browser.get();
}
