/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "FreesoundUpload.h"

//==============================================================================

typedef std::function<void (File)> DroppedComponentCallback;

class SimpleThumbnailComponent : public Component,
	private ChangeListener
{
public:
	SimpleThumbnailComponent(int sourceSamplesPerThumbnailSample,
		AudioFormatManager& formatManager,
		AudioThumbnailCache& cache)
		:	thumbnail(sourceSamplesPerThumbnailSample, formatManager, cache),
		textToDisplay()
	{
		thumbnail.addChangeListener(this);
	}

	void setFile(const File& file)
	{
		thumbnail.setSource(new FileInputSource(file));
	}

	void paint(Graphics& g) override
	{
		if (thumbnail.getNumChannels() == 0)
			paintIfNoFileLoaded(g);
		else
			paintIfFileLoaded(g);
	}

	void paintIfNoFileLoaded(Graphics& g)
	{
		g.setColour(Colours::white);
		g.setFont(17.0f);
		g.drawFittedText("Drop Audio File Here", getLocalBounds(), Justification::centred, 1.0f);

	}

	void paintIfFileLoaded(Graphics& g)
	{
		//g.fillAll(Colours::darkgrey);

		g.setColour(Colours::lightgrey);
		thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);

	}

	void changeListenerCallback(ChangeBroadcaster* source) override
	{
		if (source == &thumbnail)
			thumbnailChanged();
	}

	void setText(String newText) { textToDisplay = newText; }



private:
	void thumbnailChanged()
	{
		repaint();
	}

	AudioThumbnail thumbnail;
	String textToDisplay;



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleThumbnailComponent)
};

class SimplePositionOverlay :	public Component,
								private Timer,
								public FileDragAndDropTarget

{
public:
	SimplePositionOverlay(AudioTransportSource& transportSourceToUse)
		:	transportSource(transportSourceToUse)
	{
		startTimer(40);	
	}

	void paint(Graphics& g) override
	{
		auto duration = transportSource.getLengthInSeconds();
		
		if (somethingIsBeingDraggedOver)
		{
			g.setColour(Colours::red);
			g.drawRect(getLocalBounds(), 3);
		}

		if (duration > 0.0)
		{
			auto audioPosition = transportSource.getCurrentPosition();
			auto drawPosition = (audioPosition / duration) * getWidth();

			g.setColour(Colours::green);
			g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 2.0f);
		}
	}

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

	bool isInterestedInFileDrag(const StringArray& files) override
	{

		if(files.size() == 1){
			if(files[0].endsWith(".mp3") || files[0].endsWith(".flac") || files[0].endsWith(".fla") || files[0].endsWith(".ogg") || files[0].endsWith(".aiff") || files[0].endsWith(".aif") || files[0].endsWith(".wav")){
				return true;
			}
		}
		return false;
	}

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

	void filesDropped(const StringArray& files, int /*x*/, int /*y*/) override
	{

		File droppedFile = files[0];
		onDropCallback(droppedFile);

		somethingIsBeingDraggedOver = false;
		repaint();
	}

	void setOnDropCallback(DroppedComponentCallback cb) { onDropCallback = cb; }

private:
	void timerCallback() override
	{
		repaint();
	}

	AudioTransportSource& transportSource;
	DroppedComponentCallback onDropCallback;
	bool somethingIsBeingDraggedOver = false;



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimplePositionOverlay)
};

class FreesoundUploaderAudioProcessorEditor : public AudioProcessorEditor,
	public ChangeListener
{
public:

	FreesoundUploaderAudioProcessorEditor(FreesoundUploaderAudioProcessor&);
	~FreesoundUploaderAudioProcessorEditor();

	//==============================================================================
	void paint(Graphics&) override;
	void resized() override;
	void changeListenerCallback(ChangeBroadcaster*) override;
	void fileDropped(File newDroppedFile);


private:

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

	enum TransportState
	{
		Stopped,
		Starting,
		Playing,
		Pausing,
		Paused,
		Stopping
	};

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

	void transportSourceChanged()
	{
		if (processor.transportSource.isPlaying())
			changeState(Playing);
		else
			changeState(Stopped);
	}

	void playButtonClicked()
	{
		if ((state == Stopped) || (state == Paused))
			changeState(Starting);
		else if (state == Playing)
			changeState(Pausing);
	}

	void stopButtonClicked()
	{
		if (state == Paused)
			changeState(Stopped);
		else
			changeState(Stopping);
	}

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
	
	void uploadButtonClicked() {
		
		//Fazer set dos parameters usando a funçao
		uploadButton.setEnabled(false);

		status.setText("Uploading...",dontSendNotification);

		String selLicense;
		if (cc0Button.getToggleState()) { selLicense = "Creative Commons 0"; }
		if (attribNCButton.getToggleState()) { selLicense = "Attribution Noncommercial"; }
		if (attribButton.getToggleState()) { selLicense = "Attribution"; }
		
		URL url = "https://freesound.org/apiv2/sounds/upload/";

		//url = url.withPOSTData("name=\"" + nameText.getText() + "\"&tags=\"" + tagsText.getText() + "\"&description=\"" + descriptionText.getText() + "\"&license=\"" + license + "\"");
		
		url = url.withParameter("name", nameText.getText());
		url = url.withParameter("tags", tagsText.getText());
		url = url.withParameter("description", descriptionText.getText());
		url = url.withParameter("license", selLicense);

		
		url = url.withFileToUpload("audiofile", droppedFile, "audio/*");


		uploadSound.reset(new FreesoundRequest(url, String(), authorization.getAccessToken()));
/*
		Response incoming = uploadSound.makeRequest();
		if (incoming.first >= 200 && incoming.first < 300) {
			var answer = JSON::fromString(incoming.second);
			uploadButton.setColour(TextButton::buttonColourId, Colours::green);
		}
*/
		uploadSound->setResponseCallback([this] (Response inResponse) {getResponse(inResponse); });
		//uploadSound->setProgressCallback([this] (int progress) {progressCallback(progress); });
		uploadSound->startThread();

		return;
	}

	void progressCallback(int progress) {
		uploadButton.setButtonText("Upload (" + String(progress) + "%)");
	}

	void getResponse(Response inResponse) {
		response = inResponse;
		if (response.first >= 200 && response.first < 300) {
			uploadButton.setEnabled(true);
		}

		var answer = JSON::fromString(response.second);
		
			
		status.setText(answer["detail"],dontSendNotification);

	}

	void freesoundButtonClicked() {

		//Abrir uma nova janela
		//authorization.addToDesktop(ComponentPeer::windowIsTemporary);

		RectanglePlacement placement(RectanglePlacement::xLeft
			| RectanglePlacement::yBottom
			| RectanglePlacement::doNotResize);


		authorization.setBounds(getLocalBounds());
		authorization.startLogin();
		addAndMakeVisible(authorization);

		return;
	}

	void authFinished() {

		authorization.setVisible(false);

		int statusCode = authorization.codeExchange();
		if (statusCode == 200) { authFlag = true; }
		checkIfReadyForUpload();

	}


	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
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
