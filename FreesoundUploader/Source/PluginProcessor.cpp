/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FreesoundUploaderAudioProcessor::FreesoundUploaderAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (	BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
						logger(File(FileLogger::getSystemLogFileFolder().getFullPathName() + File::getSeparatorChar() + "FSUPAPlog.txt"), "Freesound Uploader log file, FSR:\n")

#endif
{
	//Allows the formatManager to understand the allowed filetipes
	formatManager.registerBasicFormats();
}

FreesoundUploaderAudioProcessor::~FreesoundUploaderAudioProcessor()
{
	//Delete the reader source
	if(readerSource != nullptr) 
		readerSource->releaseResources();
}

//==============================================================================
const String FreesoundUploaderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FreesoundUploaderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FreesoundUploaderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FreesoundUploaderAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FreesoundUploaderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FreesoundUploaderAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FreesoundUploaderAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FreesoundUploaderAudioProcessor::setCurrentProgram (int index)
{
	index;
}

const String FreesoundUploaderAudioProcessor::getProgramName (int index)
{
	index;
    return {};
}

void FreesoundUploaderAudioProcessor::changeProgramName (int index, const String& newName)
{
	newName;
	index;
}

//==============================================================================
void FreesoundUploaderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	logger.logMessage("Prepare to play");
	transportSource.prepareToPlay(samplesPerBlock, sampleRate);
}

void FreesoundUploaderAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
	transportSource.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FreesoundUploaderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo()){
	//		logger.logMessage("OutputChannels not mono or stereo");
			return false;
	}
    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif



void FreesoundUploaderAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	midiMessages;
	int totalNumInputChannels = 0;
	if (readerSource.get() != nullptr) { totalNumInputChannels = 2; }
    //Get the next audioblock from the transportSource
	transportSource.getNextAudioBlock(AudioSourceChannelInfo(buffer));

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
		if (readerSource.get() == nullptr)
		{
			buffer.clear(channel, 0, buffer.getNumSamples());
			return;
		}
	}
}

//==============================================================================
bool FreesoundUploaderAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FreesoundUploaderAudioProcessor::createEditor()
{
    return new FreesoundUploaderAudioProcessorEditor (*this);
}

//==============================================================================
void FreesoundUploaderAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	destData;
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FreesoundUploaderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	sizeInBytes;
	data;
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//When new audio is dropped, this sets the file to the transportSource
int FreesoundUploaderAudioProcessor::audioDropped(File droppedAudio)
{
	logger.logMessage("AudioDropped: " + droppedAudio.getFullPathName());
	File file(droppedAudio);
	if (auto* reader = formatManager.createReaderFor(file))
	{
		std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource(reader, true));
		transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
		readerSource.reset(newSource.release());
		logger.logMessage("Audio loaded");

		return (1);
	}
	else { return(0); }

}



//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FreesoundUploaderAudioProcessor();
}
