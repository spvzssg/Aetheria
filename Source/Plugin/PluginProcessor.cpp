#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <array>
#include <atomic>
#include <cmath>

namespace Aetheria
{
namespace
{
constexpr double twoPi = juce::MathConstants<double>::twoPi;

class AetheriaSound final : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

class AetheriaVoice final : public juce::SynthesiserVoice
{
public:
    explicit AetheriaVoice(AetheriaProcessor& owner)
        : attackParameter(owner.parameters.getRawParameterValue("attack")),
          releaseParameter(owner.parameters.getRawParameterValue("release")),
          layerParameters {
              owner.parameters.getRawParameterValue("layerA"),
              owner.parameters.getRawParameterValue("layerB"),
              owner.parameters.getRawParameterValue("layerC"),
              owner.parameters.getRawParameterValue("layerD")
          }
    {
    }

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<AetheriaSound*>(sound) != nullptr;
    }

    void startNote(int midiNote, float velocity,
                   juce::SynthesiserSound*, int) override
    {
        frequency = juce::MidiMessage::getMidiNoteInHertz(midiNote);
        level = velocity;
        phase.fill(0.0);
        juce::ADSR::Parameters envelopeParameters;
        envelopeParameters.attack = attackParameter->load();
        envelopeParameters.decay = 0.12f;
        envelopeParameters.sustain = 0.82f;
        envelopeParameters.release = releaseParameter->load();
        envelope.setParameters(envelopeParameters);
        envelope.noteOn();
    }

    void stopNote(float, bool allowTailOff) override
    {
        if (allowTailOff)
            envelope.noteOff();
        else
        {
            envelope.reset();
            clearCurrentNote();
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void renderNextBlock(juce::AudioBuffer<float>& output, int startSample, int numSamples) override
    {
        if (!isVoiceActive())
            return;

        const double sampleRate = getSampleRate();
        if (sampleRate <= 0.0)
            return;

        std::array<float, 4> layerLevels {};
        for (size_t layer = 0; layer < layerLevels.size(); ++layer)
            layerLevels[layer] = layerParameters[layer]->load();

        const std::array<double, 4> ratios { 0.5, 1.0, 2.0, 4.0 };
        const int channels = output.getNumChannels();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float amp = envelope.getNextSample() * level;
            double mixed = 0.0;

            for (size_t layer = 0; layer < ratios.size(); ++layer)
            {
                const double increment = twoPi * frequency * ratios[layer] / sampleRate;
                phase[layer] += increment;
                if (phase[layer] >= twoPi)
                    phase[layer] -= twoPi;

                // Four simple oscillator layers establish the playable engine:
                // sub/body, mid/harmony, attack, and air/texture.
                const double fundamental = std::sin(phase[layer]);
                const double harmonic = std::sin(phase[layer] * 2.0) * (layer == 2 ? 0.22 : 0.08);
                mixed += (fundamental + harmonic) * static_cast<double>(layerLevels[layer]);
            }

            const float value = static_cast<float>(mixed * 0.02) * amp;
            for (int channel = 0; channel < channels; ++channel)
                output.addSample(channel, startSample + sample, value);
        }

        if (!envelope.isActive())
            clearCurrentNote();
    }

private:
    std::atomic<float>* attackParameter = nullptr;
    std::atomic<float>* releaseParameter = nullptr;
    std::array<std::atomic<float>*, 4> layerParameters {};
    juce::ADSR envelope;
    std::array<double, 4> phase {};
    double frequency = 440.0;
    float level = 0.0f;
};
} // namespace

juce::AudioProcessorValueTreeState::ParameterLayout AetheriaProcessor::createParameterLayout()
{
    using Float = juce::AudioParameterFloat;
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<Float>(juce::ParameterID { "master", 1 }, "Master",
                                       juce::NormalisableRange<float>(0.0f, 1.0f), 0.75f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "attack", 1 }, "Attack",
                                       juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.35f), 0.02f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "release", 1 }, "Release",
                                       juce::NormalisableRange<float>(0.01f, 8.0f, 0.001f, 0.35f), 0.65f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "layerA", 1 }, "A Sub / Body", 0.0f, 1.0f, 0.85f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "layerB", 1 }, "B Mid / Harmony", 0.0f, 1.0f, 0.55f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "layerC", 1 }, "C Attack", 0.0f, 1.0f, 0.30f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "layerD", 1 }, "D Air / Texture", 0.0f, 1.0f, 0.20f));
    return layout;
}

AetheriaProcessor::AetheriaProcessor()
    : juce::AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "AETHERIA_STATE", createParameterLayout())
{
    for (int i = 0; i < 32; ++i)
        synthesiser.addVoice(new AetheriaVoice(*this));
    synthesiser.addSound(new AetheriaSound());
}

void AetheriaProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    synthesiser.setCurrentPlaybackSampleRate(sampleRate);
}

void AetheriaProcessor::releaseResources()
{
    synthesiser.allNotesOff(0, false);
}

bool AetheriaProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AetheriaProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    synthesiser.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());
    buffer.applyGain(parameters.getRawParameterValue("master")->load());
}

juce::AudioProcessorEditor* AetheriaProcessor::createEditor()
{
    return new AetheriaEditor(*this);
}

void AetheriaProcessor::getStateInformation(juce::MemoryBlock& destination)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destination);
}

void AetheriaProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
} // namespace Aetheria

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Aetheria::AetheriaProcessor();
}
