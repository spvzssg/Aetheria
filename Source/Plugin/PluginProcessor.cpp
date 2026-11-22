#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace Aetheria
{
AetheriaProcessor::AetheriaProcessor()
    : juce::AudioProcessor(BusesProperties()
          .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

void AetheriaProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void AetheriaProcessor::releaseResources()
{
}

bool AetheriaProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AetheriaProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                     juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);
    buffer.clear();
}

juce::AudioProcessorEditor* AetheriaProcessor::createEditor()
{
    return new AetheriaEditor(*this);
}

void AetheriaProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    destData.reset();
}

void AetheriaProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::ignoreUnused(data, sizeInBytes);
}
} // namespace Aetheria

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Aetheria::AetheriaProcessor();
}
