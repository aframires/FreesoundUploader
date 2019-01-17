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
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

	formatManager.registerBasicFormats();
}

FreesoundUploaderAudioProcessor::~FreesoundUploaderAudioProcessor()
{
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
}

const String FreesoundUploaderAudioProcessor::getProgramName (int index)
{
    return {};
}

void FreesoundUploaderAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void FreesoundUploaderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
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
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

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
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
		if (readerSource.get() == nullptr)
		{
			buffer.clear(channel, 0, buffer.getNumSamples());
			return;
		}


        auto* channelData = buffer.getWritePointer (channel);

		//COPY THE BUFFER HERE
		//channelData = transportSource.buffer
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
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FreesoundUploaderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

int FreesoundUploaderAudioProcessor::audioDropped(File droppedAudio)
{
	File file(droppedAudio);
	auto* reader = formatManager.createReaderFor(file);
	if (auto* reader = formatManager.createReaderFor(file))
	{
		std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
		transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
		readerSource.reset(newSource.release());
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
