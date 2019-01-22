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
	thumbnailComp(512, processor.formatManager, thumbnailCache),
	positionOverlay(processor.transportSource)
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

	addAndMakeVisible(&freesoundLogo);
	//freesoundLogo.setImages();
	freesoundLogo.onClick = [this] {freesoundButtonClicked(); };
	freesoundLogo.setEnabled(true);

	addAndMakeVisible(&uploadButton);
	uploadButton.setButtonText("Upload");
	uploadButton.onClick = [this] {uploadButtonClicked(); };
	playButton.setColour(TextButton::buttonColourId, Colours::green);
	uploadButton.setEnabled(false);

	addAndMakeVisible(&license);
	//license.setFont(Font(15.0f, Font::plain).withTypefaceStyle("Regular"));
	license.setJustificationType(Justification::centredLeft);
	license.setEditable(false, false, false);
	//license.setColour(TextEditor::textColourId, Colours::black);
	//license.setColour(TextEditor::backgroundColourId, Colour(0x00000000));

	addAndMakeVisible(&cc0Button);
	cc0Button.setButtonText("CC0");
	cc0Button.onClick = [this] {cc0ButtonClicked(); };
	cc0Button.setEnabled(true);

	addAndMakeVisible(&atribbNCButton);
	atribbNCButton.setButtonText("AtribNC");
	atribbNCButton.onClick = [this] {atribbNCButtonClicked(); };
	atribbNCButton.setEnabled(true);

	addAndMakeVisible(&attribButton);
	attribButton.setButtonText("Attribution");
	attribButton.onClick = [this] {attribButtonClicked(); };
	attribButton.setEnabled(true);

	addAndMakeVisible(&nameText);
	nameText.setMultiLine(false);
	nameText.setReturnKeyStartsNewLine(false);
	nameText.setReadOnly(false);
	nameText.setScrollbarsShown(true);
	nameText.setCaretVisible(true);
	nameText.setPopupMenuEnabled(true);
	nameText.setText("Name");

	tagsText.setText("Tags");
	
	descriptionText.setText("Description of the sound");



	positionOverlay.setOnDropCallback([this](File inputFile) {fileDropped(inputFile); });

	addAndMakeVisible(&thumbnailComp);
	addAndMakeVisible(&positionOverlay);

	setSize(600, 800);
	processor.transportSource.addChangeListener(this);



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
	if (source == &processor.transportSource) {

		if (processor.transportSource.isPlaying())
			changeState(Playing);
		else if ((state == Stopping) || (state == Playing))
			changeState(Stopped);
		else if (Pausing == state)
			changeState(Paused);

	}

}

void FreesoundUploaderAudioProcessorEditor::fileDropped(File newDroppedFile) {
	droppedFile = newDroppedFile;
	audioDropped();
}