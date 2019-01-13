/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/

class SimpleThumbnailComponent : public Component,
	private ChangeListener
{
public:
	SimpleThumbnailComponent(int sourceSamplesPerThumbnailSample,
		AudioFormatManager& formatManager,
		AudioThumbnailCache& cache,
		std::shared_ptr<String> incomingTextToDisplay)
		:	thumbnail(sourceSamplesPerThumbnailSample, formatManager, cache),
			textToDisplay(move(incomingTextToDisplay))
	{
		//textToDisplay = NULL;
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
		g.fillAll(Colours::white);
		g.setColour(Colours::darkgrey);
		if(textToDisplay != NULL)
		{
		g.drawFittedText(*textToDisplay, getLocalBounds(), Justification::centred, 1.0f);
		}
	}

	void paintIfFileLoaded(Graphics& g)
	{
		g.fillAll(Colours::white);

		g.setColour(Colours::red);
		thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);

	}

	void changeListenerCallback(ChangeBroadcaster* source) override
	{
		if (source == &thumbnail)
			thumbnailChanged();
	}


private:
	void thumbnailChanged()
	{
		repaint();
	}

	AudioThumbnail thumbnail;
	std::shared_ptr<String> textToDisplay;



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleThumbnailComponent)
};

class SimplePositionOverlay :	public Component,
								private Timer,
								public FileDragAndDropTarget

{
public:
	SimplePositionOverlay(AudioTransportSource& transportSourceToUse, std::shared_ptr<String> textDisplayed)
		:	transportSource(transportSourceToUse),
			textEdit(move(textDisplayed))
	{
		//textEdit = textDisplayed;
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
			//Confirm here that the type of file is the desired one. 
			//See how to send the error messages to the thumbnail viewer
			return true;
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
		*textEdit = files[0];

		somethingIsBeingDraggedOver = false;
		repaint();
	}

private:
	void timerCallback() override
	{
		repaint();
	}

	AudioTransportSource& transportSource;
	bool somethingIsBeingDraggedOver = false;
	std::shared_ptr<String> textEdit;



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



private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.


	enum TransportState
	{
		Stopped,
		Starting,
		Playing,
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
				stopButton.setEnabled(false);
				playButton.setEnabled(true);
				transportSource.setPosition(0.0);
				break;

			case Starting:
				playButton.setEnabled(false);
				transportSource.start();
				break;

			case Playing:
				stopButton.setEnabled(true);
				break;

			case Stopping:
				transportSource.stop();
				break;

			default:
				jassertfalse;
				break;
			}
		}
	}

	void transportSourceChanged()
	{
		if (transportSource.isPlaying())
			changeState(Playing);
		else
			changeState(Stopped);
	}

	void playButtonClicked()
	{
		changeState(Starting);
	}

	void stopButtonClicked()
	{
		changeState(Stopping);
	}



	FreesoundUploaderAudioProcessor& processor;


	TextButton playButton;
	TextButton stopButton;

	AudioFormatManager formatManager;
	std::unique_ptr<AudioFormatReaderSource> readerSource;
	AudioTransportSource transportSource;
	TransportState state;
	std::shared_ptr<String> textToDisplay;
	AudioThumbnailCache thumbnailCache;
	SimpleThumbnailComponent thumbnailComp;
	SimplePositionOverlay positionOverlay;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FreesoundUploaderAudioProcessorEditor)
};
