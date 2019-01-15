/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FreesoundUploaderAudioProcessorEditor::FreesoundUploaderAudioProcessorEditor(FreesoundUploaderAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p),
	state(Stopped),
	thumbnailCache(5),
	thumbnailComp(512, formatManager, thumbnailCache),
	positionOverlay(transportSource)
{
	addAndMakeVisible(&playButton);
	playButton.setButtonText("Play");
	playButton.onClick = [this] { playButtonClicked(); };
	playButton.setColour(TextButton::buttonColourId, Colours::green);
	playButton.setEnabled(false);
	//e preciso por funcoes de listener

	addAndMakeVisible(&stopButton);
	stopButton.setButtonText("Stop");
	stopButton.onClick = [this] { stopButtonClicked(); };
	stopButton.setColour(TextButton::buttonColourId, Colours::red);
	stopButton.setEnabled(false);

	positionOverlay.setOnDropCallback([this](File inputFile) {fileDropped(inputFile); });

	addAndMakeVisible(&thumbnailComp);
	addAndMakeVisible(&positionOverlay);

	setSize(600, 400);

	formatManager.registerBasicFormats();
	transportSource.addChangeListener(this);

	//setAudioChannels(2, 2);
}

FreesoundUploaderAudioProcessorEditor::~FreesoundUploaderAudioProcessorEditor()
{
}
//==============================================================================
void FreesoundUploaderAudioProcessorEditor::paint(Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

	g.setColour(Colours::white);
}

void FreesoundUploaderAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..

	playButton.setBounds(10, 40, getWidth() - 20, 20);
	stopButton.setBounds(10, 70, getWidth() - 20, 20);

	Rectangle<int> thumbnailBounds(10, 100, getWidth() - 20, getHeight() - 120);
	thumbnailComp.setBounds(thumbnailBounds);
	positionOverlay.setBounds(thumbnailBounds);
}

void FreesoundUploaderAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == &transportSource)
		transportSourceChanged();
}

void FreesoundUploaderAudioProcessorEditor::fileDropped(File newDroppedFile) {
	droppedFile = newDroppedFile;
	audioDropped();
}