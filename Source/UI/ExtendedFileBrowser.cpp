//
//  ExtendedFileBrowser.cpp
//  Synthlab - App
//
//  Created by Matthias Pueski on 30.04.18.
//  Copyright © 2018 Pueski. All rights reserved.
//

#include "ExtendedFileBrowser.h"
#include "../JuceLibraryCode/JuceHeader.h"


using juce::File;
using juce::WildcardFileFilter;
using juce::TableListBox;
using juce::Viewport;
using juce::Colour;
using juce::String;
using juce::DirectoryContentsList;
using juce::ChangeBroadcaster;
using juce::Graphics;
using juce::ValueTree;
using juce::OutputStream;
using juce::XmlElement;
using juce::XmlDocument;
using juce::ScopedPointer;
using juce::TextButton;

ExtendedFileBrowser::ExtendedFileBrowser(const File& initialFileOrDirectory, const WildcardFileFilter* fileFilter, FileBrowserModel* model, Sampler* sampler) : initialDir(initialFileOrDirectory) {

	// addMouseListener(this, true);
	table = new TableListBox();
	
	table->getHeader().addColumn("Name", 1, 300);
	table->getHeader().addColumn("Size", 2, 100);
	table->setAutoSizeMenuOptionShown(true);
	table->getHeader().setStretchToFitActive(true);
	table->setModel(model);
	this->model = model;
	view = new Viewport();	
	view->setViewedComponent(table);
	addAndMakeVisible(view);
	

	this->sampler = sampler;

	table->addMouseListener(this, true);

	loadState();

	model->update();

	juce::Array<juce::File> drives;

	File::findFileSystemRoots(drives);

	for (int i = 0; i < drives.size(); i++) {
		juce::File f = drives.getReference(i);
		TextButton* button = new TextButton(f.getFileName());
		button->setSize(30, 20);
		button->setTopLeftPosition(i * 35, 0);
		driveButtons.push_back(button);
		addAndMakeVisible(button);
		button->addListener(this);
	}

	repaint();
}

ExtendedFileBrowser::~ExtendedFileBrowser() {
	saveState();
	delete table;
	delete view;
	delete model;
	if (sampler != nullptr) {
		sampler->stop();
		delete sampler;
	}
	for (int i = 0; i < driveButtons.size(); i++) {
		delete driveButtons.at(i);
	}
}

void ExtendedFileBrowser::paint(juce::Graphics& g) {
	g.fillAll(Colour(0xff222222));
	g.fillRect(getLocalBounds());
	// Component::paint(g);
}

void ExtendedFileBrowser::resized() {
	if (getParentComponent() != nullptr) {
		setSize(getParentWidth(), getParentHeight());
		view->setSize(getWidth(), getHeight());
		table->setSize(getWidth(), getHeight());
	}
}

void ExtendedFileBrowser::changeListenerCallback(ChangeBroadcaster* source) {
	table->updateContent();
}

void ExtendedFileBrowser::mouseDown(const juce::MouseEvent& event) {


}

void ExtendedFileBrowser::buttonClicked(Button* button)
{
	juce::File* file = new juce::File(button->getButtonText() + "\\");
	model->setCurrentDir(file);
}

void ExtendedFileBrowser::mouseDoubleClick(const juce::MouseEvent& event) {

	if (table->getSelectedRow() > 0) {
		File* f = new File(model->getDirectoryList()->getFile(table->getSelectedRow()));
		if (f->exists()) {
			if (f->isDirectory()) {
				model->setCurrentDir(f);
			}
			else {
				if (table->getSelectedRow() > 0) {
					playFile(table->getSelectedRow());
				}
			}
		}
	}
	else {
		File* current = new File(model->getCurrentDir());
		File* parent = new File(current->getParentDirectory());
		model->setCurrentDir(parent);
		delete current;
	}

}

void ExtendedFileBrowser::selectNextFile() {
	int row = table->getSelectedRow();

	if (row < table->getNumRows() - 1) {
		row++;
		table->selectRow(row);
		playFile(row);
	}
}

void ExtendedFileBrowser::selectPreviousFile() {
	int row = table->getSelectedRow();

	if (row > 0) {
		row--;
		table->selectRow(row);
		playFile(row);
	}
}

void ExtendedFileBrowser::timerCallback() {

}

void ExtendedFileBrowser::playFile(int row) {
	File f = File(model->getDirectoryList()->getFile(row));
	if (f.exists()) {
		if (!f.isDirectory()) {
			if (f.getFileExtension().toLowerCase().contains("wav") ||
				f.getFileExtension().toLowerCase().contains("aif") ||
				f.getFileExtension().toLowerCase().contains("aiff") ||
				f.getFileExtension().toLowerCase().contains("mp3") ||
				f.getFileExtension().toLowerCase().contains("sam") ||
				f.getFileExtension().toLowerCase().contains("ogg")) {
				if (sampler != nullptr) {
					sampler->stop();
					sampler->loadSample(f);
					// sampler->setPitch(44100.0f/48000.0f);
					sampler->play();
				}
				selectedFile = f;
				sendChangeMessage();
			}
		}

	}
}

//===========================================================================
// Model
//===========================================================================

FileBrowserModel::FileBrowserModel(DirectoryContentsList* directoryList, File& initalDir) {
	this->directoryList = directoryList;
	this->currentDirectory = initalDir.getFullPathName();
	directoryList->setDirectory(initalDir, true, true);
}

int FileBrowserModel::getNumRows() {
	return directoryList->getNumFiles() + 1;
}
void FileBrowserModel::paintCell(Graphics& g,
	int rowNumber,
	int columnId,
	int width, int height,
	bool rowIsSelected) {

	g.setColour(juce::Colours::black);

	String text = "";

	if (columnId == 1) {

		if (rowNumber > 0) {
			text = directoryList->getFile(rowNumber).getFileName();
		}
		else {
			text = "[up]";
		}
		g.setColour(juce::Colours::white);
		g.drawText(text, 0, 0, width, height, juce::Justification::centredLeft);

	}
	else if (columnId == 2) {
		if (rowNumber > 0) {
			text = String(directoryList->getFile(rowNumber).getSize() / 1024) + "kB";
		}
		else {
			text = "";
		}

		g.setColour(juce::Colours::white);
		g.drawText(text, 0, 0, width, height, juce::Justification::right);
	}

}

void FileBrowserModel::paintRowBackground(Graphics& g,
	int rowNumber,
	int width, int height,
	bool rowIsSelected) {


	if (rowIsSelected) {
		g.setColour(juce::Colours::orange);
	}
	else {
		g.setColour(Colour(0xff222222));
	}

	g.fillRect(0, 0, width, height);
}

void FileBrowserModel::setCurrentDir(juce::File* dir) {
	this->currentDirectory = dir->getFullPathName();
	directoryList->setDirectory(*dir, true, true);
}

void FileBrowserModel::update() {
	directoryList->refresh();
}

DirectoryContentsList* FileBrowserModel::getDirectoryList() {
	return directoryList;
}

void ExtendedFileBrowser::saveState() {
	
}

void ExtendedFileBrowser::loadState() {
	
}


File ExtendedFileBrowser::getSelectedFile() {
	return selectedFile;
}
