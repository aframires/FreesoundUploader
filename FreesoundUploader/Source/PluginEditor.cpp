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
	positionOverlay(processor.transportSource),
	authorization("qtRxJcdBeEqAPPymT71w","xlMDWbwEp65jNneniFiwNe3u7aKyxBPKrxug05KC")
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
	fsImg = ImageCache::getFromMemory(BinaryData::Freesound_project_website_logo_png, BinaryData::Freesound_project_website_logo_pngSize);
	freesoundLogo.setImages(false,true,true, fsImg, 1.0f, Colour(),fsImg,0.9f, Colour(), fsImg, 0.9f, Colour());
	freesoundLogo.onClick = [this] {freesoundButtonClicked(); };
	freesoundLogo.setEnabled(true);

	addAndMakeVisible(&uploadButton);
	uploadButton.setButtonText("Upload");
	uploadButton.onClick = [this] {uploadButtonClicked(); };
	playButton.setColour(TextButton::buttonColourId, Colours::green);
	uploadButton.setEnabled(false);

	addAndMakeVisible(&license);
	license.setText("License: ", dontSendNotification);
	license.setFont(Font(20.0f, Font::plain).withTypefaceStyle("Regular"));
	license.setJustificationType(Justification::left);
	license.setEditable(false, false, false);
	//license.setColour(TextEditor::textColourId, Colours::black);
	license.setColour(TextEditor::backgroundColourId, Colour(0x00000000));

	addAndMakeVisible(&cc0Button);
	cc0Button.setButtonText("CC0");
	//cc0Button.setEnabled(true);
	cc0Button.setClickingTogglesState(true);
	cc0Button.setRadioGroupId(12345);
	cc0Button.setToggleState(true, dontSendNotification);


	addAndMakeVisible(&attribNCButton);
	attribNCButton.setButtonText("Attribution Non Commercial");
	//attribNCButton.setEnabled(false);
	attribNCButton.setClickingTogglesState(true);
	attribNCButton.setRadioGroupId(12345);

	addAndMakeVisible(&attribButton);
	attribButton.setButtonText("Attribution");
	//attribButton.setEnabled(false);
	attribButton.setClickingTogglesState(true);
	attribButton.setRadioGroupId(12345);

	addAndMakeVisible(&nameText);
	nameText.setMultiLine(true);
	nameText.setReturnKeyStartsNewLine(false);
	nameText.setReadOnly(false);
	nameText.setScrollbarsShown(false);
	nameText.setCaretVisible(true);
	nameText.setPopupMenuEnabled(true);
	nameText.setFont(Font(20.0f, Font::plain).withTypefaceStyle("Regular"));
	nameText.setTextToShowWhenEmpty("Name", Colour(Colours::whitesmoke));
	nameText.onTextChange = [this] {checkIfReadyForUpload(); };

	addAndMakeVisible(&tagsText);
	tagsText.setMultiLine(true);
	tagsText.setReturnKeyStartsNewLine(false);
	tagsText.setReadOnly(false);
	tagsText.setScrollbarsShown(false);
	tagsText.setCaretVisible(true);
	tagsText.setPopupMenuEnabled(true);
	tagsText.setFont(Font(17.0f, Font::plain).withTypefaceStyle("Regular"));
	tagsText.setTextToShowWhenEmpty("Tags",Colour(Colours::whitesmoke));
	tagsText.onTextChange = [this] {checkIfReadyForUpload(); };


	addAndMakeVisible(&descriptionText);
	descriptionText.setMultiLine(true);
	descriptionText.setReturnKeyStartsNewLine(true);
	descriptionText.setReadOnly(false);
	descriptionText.setScrollbarsShown(true);
	descriptionText.setCaretVisible(true);
	descriptionText.setPopupMenuEnabled(true);
	descriptionText.setFont(Font(17.0f, Font::plain).withTypefaceStyle("Regular"));
	descriptionText.setTextToShowWhenEmpty("Description of the Sound", Colour(Colours::whitesmoke));
	descriptionText.onTextChange = [this] {checkIfReadyForUpload(); };



	positionOverlay.setOnDropCallback([this](File inputFile) {fileDropped(inputFile); checkIfReadyForUpload(); });
	authorization.setAuthCallback([this] {authFinished(); });

	addAndMakeVisible(&thumbnailComp);
	addAndMakeVisible(&positionOverlay);

	setSize(600, 600);
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

	playButton.setBounds(10, 570, getWidth()/3 - 10, 20);
	stopButton.setBounds(205, 570, getWidth()/3 - 10, 20);
	uploadButton.setBounds(400, 570, getWidth() / 3 - 10, 20);

	license.setBounds(6, getHeight() / 2 + 58, getWidth() / 4 - 10, 20);
	cc0Button.setBounds(85, getHeight() / 2 + 60, getWidth() / 4 + 15, 20);
	cc0Button.setConnectedEdges(Button::ConnectedOnRight);
	attribButton.setBounds(85 + getWidth() / 4 + 15, getHeight() / 2 + 60, getWidth() / 4 + 15, 20);
	attribButton.setConnectedEdges(Button::ConnectedOnLeft + Button::ConnectedOnRight);
	attribNCButton.setBounds(85 + 2 * (getWidth() / 4 + 15), getHeight() / 2 + 60, getWidth() / 4 + 15, 20);
	attribNCButton.setConnectedEdges(Button::ConnectedOnLeft);

	descriptionText.setBounds(10, 200, getWidth() - 20, getHeight() / 4);

	tagsText.setBounds(10, 150, getWidth() - 20, 40);
	
	nameText.setBounds(10, 105, getWidth() - 20, 35);

	freesoundLogo.setBounds(10 + getWidth() / 2 - 158, 5, getWidth() / 2, 100);

	Rectangle<int> thumbnailBounds(10, getHeight()/2 + 90, getWidth() - 20, getHeight()/2 - 130);
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
	
	if(hasAudio){
		if (state == Paused)
			changeState(Stopped);
		else
			changeState(Stopping);
	}

	audioDropped();
}