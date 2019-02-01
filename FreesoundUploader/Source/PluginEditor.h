/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "FreesoundUpload.h"

//==============================================================================

//typedef for the callback called when a file is dropped in the drag and drop component.
typedef std::function<void (File)> DroppedComponentCallback;

//This class contains the component which is responsible for handling all the audio
//thumbnail related issues. This code is based on the "Draw audio waveforms tutorial
//from JUCE (https://docs.juce.com/master/tutorial_audio_thumbnail.html)
class SimpleThumbnailComponent : public Component,
	private ChangeListener
{
public:

	//Constructor for the thumbnail component.
	SimpleThumbnailComponent(int sourceSamplesPerThumbnailSample,
		AudioFormatManager& formatManager,
		AudioThumbnailCache& cache)
		:	thumbnail(sourceSamplesPerThumbnailSample, formatManager, cache)
	{
		thumbnail.addChangeListener(this);
	}

	//Sets the file used as a source for the thumbnail
	void setFile(const File& file)
	{
		thumbnail.setSource(new FileInputSource(file));
	}

	//Function for painting the thumbnail component. Two different
	//functions can be called, depending if the file has or not been loaded.
	void paint(Graphics& g) override
	{
		if (thumbnail.getNumChannels() == 0)
			paintIfNoFileLoaded(g);
		else
			paintIfFileLoaded(g);
	}

	//If no file is loaded, just ask the user to drop a file
	void paintIfNoFileLoaded(Graphics& g)
	{
		g.setColour(Colours::white);
		g.setFont(17.0f);
		g.drawFittedText("Drop Audio File Here", getLocalBounds(), Justification::centred, 1.0f);
	}

	//If the file has been loaded, draw it.
	void paintIfFileLoaded(Graphics& g)
	{
		g.setColour(Colours::lightgrey);
		thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);
	}

	//Callback function called whenever a new file has been loaded.
	void changeListenerCallback(ChangeBroadcaster* source) override
	{
		if (source == &thumbnail)
			thumbnailChanged();
	}


private:
	//When the callback function is called, repaint the component
	void thumbnailChanged()
	{
		repaint();
	}
	AudioThumbnail thumbnail;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleThumbnailComponent)
};

//This class is responsible for handling both the drag and dropping behaviour
//as well as an overlay which indicates the current position and allows the user
//to change the playback position. This code is based on the "Draw audio waveforms tutorial
//from JUCE (https://docs.juce.com/master/tutorial_audio_thumbnail.html)
class SimplePositionOverlay :	public Component,
								private Timer,
								public FileDragAndDropTarget
{
public:

	//Constructor of the class, sets the timer for redrawing the position overlay.
	SimplePositionOverlay(AudioTransportSource& transportSourceToUse)
		:	transportSource(transportSourceToUse)
	{
		startTimer(40);	
	}

	//Paint function of the class
	void paint(Graphics& g) override
	{
		//Get the duration of the transportSource
		auto duration = transportSource.getLengthInSeconds();
		
		//If something acceptable is being dragged over, add a rectangle around the component.
		if (somethingIsBeingDraggedOver)
		{
			g.setColour(Colours::red);
			g.drawRect(getLocalBounds(), 3);
		}

		//If there is an audio file loaded,
		if (duration > 0.0)
		{
			//Get the current position and draw a vertical line displaying it
			auto audioPosition = transportSource.getCurrentPosition();
			auto drawPosition = (audioPosition / duration) * getWidth();

			g.setColour(Colours::green);
			g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 2.0f);
		}
	}

	//This function handles pressing the desired playback position and sets the playback to it.
	void mouseDown(const MouseEvent& event) override
	{
		auto duration = transportSource.getLengthInSeconds();

		if (duration > 0.0)
		{
			auto clickPosition = event.position.x;
			auto audioPosition = (clickPosition / getWidth()) * duration;

			transportSource.setPosition(audioPosition);
		}
	}

	//Verifies is the dropped files is only one and if it is supported by Freesound. Otherwise it won't
	//accept the file
	bool isInterestedInFileDrag(const StringArray& files) override
	{

		if(files.size() == 1){
			if(files[0].endsWith(".mp3") || files[0].endsWith(".flac") || files[0].endsWith(".fla") || files[0].endsWith(".ogg") || files[0].endsWith(".aiff") || files[0].endsWith(".aif") || files[0].endsWith(".wav")){
				return true;
			}
		}
		return false;
	}

	//Repaint whenever a file is being dragged over
	void fileDragEnter(const StringArray& /*files*/, int /*x*/, int /*y*/) override
	{
		somethingIsBeingDraggedOver = true;
		repaint();
	}

	void fileDragMove(const StringArray& /*files*/, int /*x*/, int /*y*/) override
	{
	}

	void fileDragExit(const StringArray& /*files*/) override
	{
		somethingIsBeingDraggedOver = false;
		repaint();
	}

	//Whenever files are dropped, call this function which calls the callback
	void filesDropped(const StringArray& files, int /*x*/, int /*y*/) override
	{

		File droppedFile = files[0];
		onDropCallback(droppedFile);

		somethingIsBeingDraggedOver = false;
		repaint();
	}

	//Function for setting the audio drop callback.
	void setOnDropCallback(DroppedComponentCallback cb) { onDropCallback = cb; }

private:

	//Repaint the vertical line every 20ms
	void timerCallback() override
	{
		repaint();
	}

	AudioTransportSource& transportSource;
	DroppedComponentCallback onDropCallback;
	bool somethingIsBeingDraggedOver = false;



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimplePositionOverlay)
};

//This function is responsible for managing all the UI and UX aspects of the software.
//It is the AudioProcessorEditor of this plugin
class FreesoundUploaderAudioProcessorEditor : public AudioProcessorEditor,
	public ChangeListener
{
public:

	//Constructor, takes information related to the all other subcomponents it contains,
	//such as the thumbnail component, the authorization object and position overlay.
	FreesoundUploaderAudioProcessorEditor(FreesoundUploaderAudioProcessor& p)
		: AudioProcessorEditor(&p), processor(p),
		state(Stopped),
		thumbnailCache(5),
		thumbnailComp(512, processor.formatManager, thumbnailCache),
		positionOverlay(processor.transportSource),
		authorization("qtRxJcdBeEqAPPymT71w", "xlMDWbwEp65jNneniFiwNe3u7aKyxBPKrxug05KC"),
		uploadSound(nullptr)
	{

		//These functions are related to the UI components:

		//Play button
		addAndMakeVisible(&playButton);
		playButton.setButtonText("Play");
		playButton.onClick = [this] { playButtonClicked(); };
		playButton.setColour(TextButton::buttonColourId, Colours::green);
		playButton.setEnabled(false);

		//Stop button
		addAndMakeVisible(&stopButton);
		stopButton.setButtonText("Stop");
		stopButton.onClick = [this] { stopButtonClicked(); };
		stopButton.setColour(TextButton::buttonColourId, Colours::red);
		stopButton.setEnabled(false);

		//Freesound Logo
		addAndMakeVisible(&freesoundLogo);
		fsImg = ImageCache::getFromMemory(BinaryData::Freesound_project_website_logo_png, BinaryData::Freesound_project_website_logo_pngSize);
		freesoundLogo.setImages(false, true, true, fsImg, 1.0f, Colour(), fsImg, 0.9f, Colour(), fsImg, 0.9f, Colour());
		freesoundLogo.onClick = [this] {freesoundButtonClicked(); };
		freesoundLogo.setEnabled(true);

		//Upload Button
		addAndMakeVisible(&uploadButton);
		uploadButton.setButtonText("Upload");
		uploadButton.onClick = [this] {uploadButtonClicked(); };
		playButton.setColour(TextButton::buttonColourId, Colours::green);
		uploadButton.setEnabled(false);

		//License selector buttons
		addAndMakeVisible(&license);
		license.setText("License: ", dontSendNotification);
		license.setFont(Font(20.0f, Font::plain).withTypefaceStyle("Regular"));
		license.setJustificationType(Justification::left);
		license.setEditable(false, false, false);
		license.setColour(TextEditor::backgroundColourId, Colour(0x00000000));

		addAndMakeVisible(&cc0Button);
		cc0Button.setButtonText("CC0");
		cc0Button.setClickingTogglesState(true);
		cc0Button.setRadioGroupId(12345);
		cc0Button.setToggleState(true, dontSendNotification);


		addAndMakeVisible(&attribNCButton);
		attribNCButton.setButtonText("Attribution Non Commercial");
		attribNCButton.setClickingTogglesState(true);
		attribNCButton.setRadioGroupId(12345);

		addAndMakeVisible(&attribButton);
		attribButton.setButtonText("Attribution");
		attribButton.setClickingTogglesState(true);
		attribButton.setRadioGroupId(12345);

		//Textbox for putting the name
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

		//Textbox for adding the tags
		addAndMakeVisible(&tagsText);
		tagsText.setMultiLine(true);
		tagsText.setReturnKeyStartsNewLine(false);
		tagsText.setReadOnly(false);
		tagsText.setScrollbarsShown(false);
		tagsText.setCaretVisible(true);
		tagsText.setPopupMenuEnabled(true);
		tagsText.setFont(Font(17.0f, Font::plain).withTypefaceStyle("Regular"));
		tagsText.setTextToShowWhenEmpty("Tags (at least 3!)", Colour(Colours::whitesmoke));
		tagsText.onTextChange = [this] {checkIfReadyForUpload(); };

		//Textbox for adding a description
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

		//Status text which allows to give information to the user
		addAndMakeVisible(&status);
		status.setText("Login by pressing the Freesound logo, set descriptions and drop audio", dontSendNotification);
		status.setJustificationType(Justification::centred);


		//Set callbacks
		positionOverlay.setOnDropCallback([this](File inputFile) {fileDropped(inputFile); checkIfReadyForUpload(); });
		authorization.setAuthCallback([this] {authFinished(); });

		//Make the thumbnail and position components visible
		addAndMakeVisible(&thumbnailComp);
		addAndMakeVisible(&positionOverlay);

		setSize(600, 620);

		//Make this class a listener to whenever the transport source is changed
		processor.transportSource.addChangeListener(this);



	}

	~FreesoundUploaderAudioProcessorEditor()
	{
	}

	//Fill the background of the UI
	void paint(Graphics& g)
	{
		g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
		g.setColour(Colours::white);
	}


	void resized()
	{
		// This is where you'll want to lay out the positions of any
		// subcomponents in your editor

		playButton.setBounds(10, 570, getWidth() / 3 - 10, 20);
		stopButton.setBounds(205, 570, getWidth() / 3 - 10, 20);
		uploadButton.setBounds(400, 570, getWidth() / 3 - 10, 20);

		license.setBounds(6, 600 / 2 + 58, getWidth() / 4 - 10, 20);
		cc0Button.setBounds(85, 600 / 2 + 60, getWidth() / 4 + 15, 20);
		cc0Button.setConnectedEdges(Button::ConnectedOnRight);
		attribButton.setBounds(85 + getWidth() / 4 + 15, 600 / 2 + 60, getWidth() / 4 + 15, 20);
		attribButton.setConnectedEdges(Button::ConnectedOnLeft + Button::ConnectedOnRight);
		attribNCButton.setBounds(85 + 2 * (getWidth() / 4 + 15), 600 / 2 + 60, getWidth() / 4 + 15, 20);
		attribNCButton.setConnectedEdges(Button::ConnectedOnLeft);

		descriptionText.setBounds(10, 200, getWidth() - 20, 600 / 4);

		tagsText.setBounds(10, 150, getWidth() - 20, 40);

		nameText.setBounds(10, 105, getWidth() - 20, 35);

		freesoundLogo.setBounds(10 + getWidth() / 2 - 158, 5, getWidth() / 2, 100);
		status.setBounds(10, 595, getWidth() - 20, 20);

		Rectangle<int> thumbnailBounds(10, 600 / 2 + 90, getWidth() - 20, 600 / 2 - 130);
		thumbnailComp.setBounds(thumbnailBounds);
		positionOverlay.setBounds(thumbnailBounds);
	}

	//Function which handles whenever the transportSource is changed
	void changeListenerCallback(ChangeBroadcaster* source)
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

	//This function handles when a new file is dropped to the componenet
	void fileDropped(File newDroppedFile) {
		droppedFile = newDroppedFile;

		if (hasAudio) {
			if (state == Paused)
				changeState(Stopped);
			else
				changeState(Stopping);
		}

		audioDropped();
	}

private:

	//enum for representing the possible states of the transportSource
	enum TransportState
	{
		Stopped,
		Starting,
		Playing,
		Pausing,
		Paused,
		Stopping
	};

	//This function checks if the user has filled every component for the upload
	void checkIfReadyForUpload() {

		String statusText = String();
		if (authFlag) { statusText += "You are logged in"; }
		if (!authFlag) { statusText += "Log in by pressing the Freesound logo"; }
		if (tagsText.isEmpty() || nameText.isEmpty() || descriptionText.isEmpty()) { statusText += ", set descriptions"; }
		else { statusText += ", descriptions are set"; }
		if (playButton.isEnabled()) { statusText += " and audio is loaded!"; }
		else { statusText += " and drop audio"; }

		status.setText(statusText, dontSendNotification);

		if (playButton.isEnabled() && //if File is dropped
			!tagsText.isEmpty() &&
			!nameText.isEmpty() &&
			!descriptionText.isEmpty() &&
			authFlag) 
		{
			uploadButton.setEnabled(true);
		}


	}

	//Function called whenever the state of the transport source is changed
	void changeState(TransportState newState)
	{
		if (state != newState)
		{
			state = newState;

			switch (state)
			{
			case Stopped:
				playButton.setButtonText("Play");
				stopButton.setButtonText("Stop");
				stopButton.setEnabled(false);
				processor.transportSource.setPosition(0.0);
				break;

			case Starting:
				processor.transportSource.start();
				break;

			case Playing:
				playButton.setButtonText("Pause");
				stopButton.setButtonText("Stop");
				stopButton.setEnabled(true);
				break;

			case Pausing:
				processor.transportSource.stop();
				break;
			case Paused:
				playButton.setButtonText("Resume");
				stopButton.setButtonText("Return to Zero");
				break;

			case Stopping:
				processor.transportSource.stop();
				break;

			default:
				jassertfalse;
				break;
			}
		}
	}

	//Function called when the transportSource changes
	void transportSourceChanged()
	{
		if (processor.transportSource.isPlaying())
			changeState(Playing);
		else
			changeState(Stopped);
	}

	//Function called when the playButton is clicked
	void playButtonClicked()
	{
		if ((state == Stopped) || (state == Paused))
			changeState(Starting);
		else if (state == Playing)
			changeState(Pausing);
	}

	//Function called when the stopButton is clicked
	void stopButtonClicked()
	{
		if (state == Paused)
			changeState(Stopped);
		else
			changeState(Stopping);
	}

	//Function called by FileDropped() when new audio is dropped
	void audioDropped()
	{
		int result = processor.audioDropped(droppedFile);
		if (result == 1) 
		{
			hasAudio = true;
			thumbnailComp.setFile(droppedFile);
			playButton.setEnabled(true);


		}
		else {} //In case the processor could not create the readstream

	}
	
	//Whenever the upload button is clicked,
	void uploadButtonClicked() {
		//Disable the button so the user cannot create a new request
		uploadButton.setEnabled(false);
		//Change the status
		status.setText("Uploading...",dontSendNotification);
		//Get the license from the radioButtons
		String selLicense;
		if (cc0Button.getToggleState()) { selLicense = "Creative Commons 0"; }
		if (attribNCButton.getToggleState()) { selLicense = "Attribution Noncommercial"; }
		if (attribButton.getToggleState()) { selLicense = "Attribution"; }
		
		//Create the URL and add the POST parameters (doing it like this is better than adding POST data in the FreesoundRequest
		URL url = "https://freesound.org/apiv2/sounds/upload/";		
		url = url.withParameter("name", nameText.getText());
		url = url.withParameter("tags", tagsText.getText());
		url = url.withParameter("description", descriptionText.getText());
		url = url.withParameter("license", selLicense);
		//Add the file to upload
		url = url.withFileToUpload("audiofile", droppedFile, "audio/*");
		//Store the FreesoundRequest objecto so that the pointer is not destroyed while the thread is still running
		uploadSound.reset(new FreesoundRequest(url, String(), authorization.getAccessToken()));
		//Set the responseCallback
		uploadSound->setResponseCallback([this] (Response inResponse) {getResponse(inResponse); });
		//And ask the object to run assynchronously
		uploadSound->startThread();

		return;
	}

	//Progress callback which is not supported atm
	void progressCallback(int progress) {
		uploadButton.setButtonText("Upload (" + String(progress) + "%)");
	}

	//Callback function called whenever the upload proccess has finished running
	void getResponse(Response inResponse) {

		//Get the response
		response = inResponse;
		//If everything went well,
		if (response.first >= 200 && response.first < 300) {
			uploadButton.setEnabled(true);
		}

		var answer = JSON::fromString(response.second);
		//Put the response text in the status label.
		status.setText(answer["detail"],dontSendNotification);

	}

	//Function called when the FS button is clicked
	void freesoundButtonClicked() {
		//Set the size of the component to fill all the window
		authorization.setBounds(getLocalBounds());
		//Start the login proccess
		authorization.startLogin();
		//Make the component visible
		addAndMakeVisible(authorization);

		return;
	}

	void authFinished() {
		//When the authorisation first step has finished,
		authorization.setVisible(false);
		//Exchange the authorizationCode for a authorizationToken
		int statusCode = authorization.codeExchange();
		//If everything went well set the app to authenticated
		if (statusCode == 200) { authFlag = true; }
		checkIfReadyForUpload();

	}

	FreesoundUploaderAudioProcessor& processor;
	FreesoundAuthorization authorization;
	std::unique_ptr<FreesoundRequest> uploadSound;
	Response response;

	bool authFlag = false;
	bool hasAudio = false;

	ImageButton freesoundLogo;

	TextButton playButton;
	TextButton stopButton;
	TextButton uploadButton;

	Label license;
	TextButton cc0Button;
	TextButton attribNCButton;
	TextButton attribButton;

	TextEditor nameText;
	TextEditor tagsText;
	TextEditor descriptionText;
	Label status;



	Image fsImg;
	TransportState state;
	File droppedFile;
	AudioThumbnailCache thumbnailCache;
	SimpleThumbnailComponent thumbnailComp;
	SimplePositionOverlay positionOverlay;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FreesoundUploaderAudioProcessorEditor)
};
