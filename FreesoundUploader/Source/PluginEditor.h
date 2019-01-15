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
		g.fillAll(Colours::darkgrey);
		g.setColour(Colours::darkgrey);

		//Just works for debug and for displaying errors maybe
		if(textToDisplay.isNotEmpty()){
			g.drawFittedText(textToDisplay, getLocalBounds(), Justification::centred, 1.0f);
		}
		if (textToDisplay.isEmpty()) {
			g.drawFittedText("Drop Audio File Here!", getLocalBounds(), Justification::centred, 1.0f);
		}
	}

	void paintIfFileLoaded(Graphics& g)
	{
		g.fillAll(Colours::darkgrey);

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

	void audioDropped()
	{

		if (true) //Verify here the tipe of file added
		{
			File file(droppedFile);
			auto* reader = formatManager.createReaderFor(file);
			if (auto* reader = formatManager.createReaderFor(file))
			{
				std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
				transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
				playButton.setEnabled(true);
				thumbnailComp.setFile(file);
				readerSource.reset(newSource.release());
			}
		}
	}

	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	FreesoundUploaderAudioProcessor& processor;


	TextButton playButton;
	TextButton stopButton;

	AudioFormatManager formatManager;
	std::unique_ptr<AudioFormatReaderSource> readerSource;
	AudioTransportSource transportSource;
	TransportState state;
	File droppedFile;
	AudioThumbnailCache thumbnailCache;
	SimpleThumbnailComponent thumbnailComp;
	SimplePositionOverlay positionOverlay;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FreesoundUploaderAudioProcessorEditor)
};
