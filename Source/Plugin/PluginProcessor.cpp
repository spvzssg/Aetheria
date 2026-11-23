#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>

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
        : masterAttack(owner.parameters.getRawParameterValue("attack")),
          masterRelease(owner.parameters.getRawParameterValue("release")),
          layerLevels {
              owner.parameters.getRawParameterValue("layerA"),
              owner.parameters.getRawParameterValue("layerB"),
              owner.parameters.getRawParameterValue("layerC"),
              owner.parameters.getRawParameterValue("layerD")
          },
          sourceParameters {
              owner.parameters.getRawParameterValue("sourceA"),
              owner.parameters.getRawParameterValue("sourceB"),
              owner.parameters.getRawParameterValue("sourceC"),
              owner.parameters.getRawParameterValue("sourceD")
          },
          panParameters {
              owner.parameters.getRawParameterValue("panA"), owner.parameters.getRawParameterValue("panB"),
              owner.parameters.getRawParameterValue("panC"), owner.parameters.getRawParameterValue("panD")
          },
          octaveParameters {
              owner.parameters.getRawParameterValue("octaveA"), owner.parameters.getRawParameterValue("octaveB"),
              owner.parameters.getRawParameterValue("octaveC"), owner.parameters.getRawParameterValue("octaveD")
          },
          tuneParameters {
              owner.parameters.getRawParameterValue("tuneA"), owner.parameters.getRawParameterValue("tuneB"),
              owner.parameters.getRawParameterValue("tuneC"), owner.parameters.getRawParameterValue("tuneD")
          },
          cutoffParameters {
              owner.parameters.getRawParameterValue("cutoffA"), owner.parameters.getRawParameterValue("cutoffB"),
              owner.parameters.getRawParameterValue("cutoffC"), owner.parameters.getRawParameterValue("cutoffD")
          },
          resonanceParameters {
              owner.parameters.getRawParameterValue("resA"), owner.parameters.getRawParameterValue("resB"),
              owner.parameters.getRawParameterValue("resC"), owner.parameters.getRawParameterValue("resD")
          },
          soloParameters {
              owner.parameters.getRawParameterValue("soloA"), owner.parameters.getRawParameterValue("soloB"),
              owner.parameters.getRawParameterValue("soloC"), owner.parameters.getRawParameterValue("soloD")
          },
          muteParameters {
              owner.parameters.getRawParameterValue("muteA"), owner.parameters.getRawParameterValue("muteB"),
              owner.parameters.getRawParameterValue("muteC"), owner.parameters.getRawParameterValue("muteD")
          },
          envelopeParameters {{
              {{ owner.parameters.getRawParameterValue("A_Attack"), owner.parameters.getRawParameterValue("A_Decay"),
                 owner.parameters.getRawParameterValue("A_Sustain"), owner.parameters.getRawParameterValue("A_Release") }},
              {{ owner.parameters.getRawParameterValue("B_Attack"), owner.parameters.getRawParameterValue("B_Decay"),
                 owner.parameters.getRawParameterValue("B_Sustain"), owner.parameters.getRawParameterValue("B_Release") }},
              {{ owner.parameters.getRawParameterValue("C_Attack"), owner.parameters.getRawParameterValue("C_Decay"),
                 owner.parameters.getRawParameterValue("C_Sustain"), owner.parameters.getRawParameterValue("C_Release") }},
              {{ owner.parameters.getRawParameterValue("D_Attack"), owner.parameters.getRawParameterValue("D_Decay"),
                 owner.parameters.getRawParameterValue("D_Sustain"), owner.parameters.getRawParameterValue("D_Release") }}
          }}
    {
    }

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<AetheriaSound*>(sound) != nullptr;
    }

    void startNote(int midiNote, float velocity, juce::SynthesiserSound*, int) override
    {
        frequency = juce::MidiMessage::getMidiNoteInHertz(midiNote);
        level = velocity;
        elapsedSeconds = 0.0;
        phase.fill(0.0);
        detunePhase.fill(0.0);
        for (auto& filter : layerFilters)
            filter.reset();
        pinkState.fill(0.0f);

        juce::ADSR::Parameters masterEnvelopeParameters;
        masterEnvelopeParameters.attack = masterAttack->load();
        masterEnvelopeParameters.decay = 0.12f;
        masterEnvelopeParameters.sustain = 0.82f;
        masterEnvelopeParameters.release = masterRelease->load();
        masterEnvelope.setParameters(masterEnvelopeParameters);
        masterEnvelope.noteOn();

        for (size_t layer = 0; layer < layerEnvelopes.size(); ++layer)
        {
            juce::ADSR::Parameters p;
            p.attack = envelopeParameters[layer][0]->load();
            p.decay = envelopeParameters[layer][1]->load();
            p.sustain = envelopeParameters[layer][2]->load();
            p.release = envelopeParameters[layer][3]->load();
            layerEnvelopes[layer].setParameters(p);
            layerEnvelopes[layer].noteOn();
            randomState[layer] = static_cast<uint32_t>(midiNote * 747796405u)
                               ^ static_cast<uint32_t>((layer + 1) * 2891336453u)
                               ^ 0x9E3779B9u;
            if (randomState[layer] == 0)
                randomState[layer] = 1;
        }
    }

    void stopNote(float, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            masterEnvelope.noteOff();
            for (auto& envelope : layerEnvelopes)
                envelope.noteOff();
        }
        else
        {
            masterEnvelope.reset();
            for (auto& envelope : layerEnvelopes)
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

        if (preparedSampleRate != sampleRate)
        {
            juce::dsp::ProcessSpec spec { sampleRate, 1, 1 };
            for (auto& filter : layerFilters)
            {
                filter.prepare(spec);
                filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
                filter.reset();
            }
            preparedSampleRate = sampleRate;
        }

        std::array<float, 4> gains {};
        std::array<int, 4> sources {};
        std::array<double, 4> phaseIncrements {}, panLeft {}, panRight {};
        std::array<bool, 4> enabled {};
        bool anySolo = false;
        for (size_t layer = 0; layer < gains.size(); ++layer)
            anySolo = anySolo || soloParameters[layer]->load() > 0.5f;
        for (size_t layer = 0; layer < gains.size(); ++layer)
        {
            gains[layer] = layerLevels[layer]->load();
            sources[layer] = sourceParameters[layer] != nullptr ? juce::jlimit(0, 4, juce::roundToInt(sourceParameters[layer]->load())) : 0;
            const double semitones = octaveParameters[layer]->load() * 12.0 + tuneParameters[layer]->load();
            phaseIncrements[layer] = twoPi * frequency * std::pow(2.0, semitones / 12.0) / sampleRate;
            const double panAngle = (juce::jlimit(-1.0f, 1.0f, panParameters[layer]->load()) + 1.0)
                                    * juce::MathConstants<double>::pi * 0.25;
            panLeft[layer] = std::cos(panAngle);
            panRight[layer] = std::sin(panAngle);
            const bool solo = soloParameters[layer]->load() > 0.5f;
            const bool muted = muteParameters[layer]->load() > 0.5f;
            enabled[layer] = !muted && (!anySolo || solo);
            layerFilters[layer].setCutoffFrequency(cutoffParameters[layer]->load());
            layerFilters[layer].setResonance(resonanceParameters[layer]->load());
        }

        const int channels = output.getNumChannels();
        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float masterAmp = masterEnvelope.getNextSample() * level;
            double mixedLeft = 0.0;
            double mixedRight = 0.0;

            for (size_t layer = 0; layer < 4; ++layer)
            {
                const double baseIncrement = phaseIncrements[layer];
                phase[layer] += baseIncrement;
                if (phase[layer] >= twoPi)
                    phase[layer] -= twoPi;

                const double p = phase[layer];
                const double normalizedPhase = p / twoPi;
                const double sine = std::sin(p);
                const double triangle = (2.0 / juce::MathConstants<double>::pi) * std::asin(sine);
                const double saw = (2.0 * normalizedPhase) - 1.0;
                const double square = p < juce::MathConstants<double>::pi ? 1.0 : -1.0;
                double sourceSample = 0.0;

                if (layer == 0) // A: SUB / BODY
                {
                    switch (sources[layer])
                    {
                        case 0: sourceSample = sine; break;
                        case 1: sourceSample = triangle; break;
                        case 2: sourceSample = saw; break;
                        case 3: sourceSample = square; break;
                        default: sourceSample = std::sin(p * 0.5); break;
                    }
                }
                else if (layer == 1) // B: MID / HARMONY
                {
                    switch (sources[layer])
                    {
                        case 0: sourceSample = (saw * 0.72) + (std::sin(p * 2.0) * 0.18); break;
                        case 1:
                        {
                            detunePhase[layer] += baseIncrement * 1.006;
                            if (detunePhase[layer] >= twoPi)
                                detunePhase[layer] -= twoPi;
                            sourceSample = (saw + (2.0 * detunePhase[layer] / twoPi - 1.0)) * 0.5;
                            break;
                        }
                        case 2: sourceSample = square * 0.72 + sine * 0.12; break;
                        case 3: sourceSample = std::sin(p + std::sin(p * 2.0) * 1.4); break;
                        default: sourceSample = sine; break;
                    }
                }
                else if (layer == 2) // C: ATTACK / TRANSIENT
                {
                    const double transientDecay = std::exp(-elapsedSeconds * 8.0);
                    switch (sources[layer])
                    {
                        case 0: sourceSample = sine * transientDecay; break; // Pluck
                        case 1: sourceSample = std::sin(p + std::sin(p * 2.0) * 2.8 * transientDecay); break; // FM bell
                        case 2: sourceSample = (sine + 0.28 * std::sin(p * 2.76)) * transientDecay; break; // Mallet
                        case 3: sourceSample = nextNoise(layer) * transientDecay; break;
                        default: sourceSample = nextNoise(layer) * std::exp(-elapsedSeconds * 65.0); break; // Click
                    }
                }
                else // D: AIR / TEXTURE
                {
                    const float white = nextNoise(layer);
                    pinkState[layer] += 0.025f * (white - pinkState[layer]);
                    switch (sources[layer])
                    {
                        case 0: sourceSample = white; break;
                        case 1: sourceSample = pinkState[layer] * 4.0f; break;
                        case 2: sourceSample = (white - pinkState[layer]) * 1.8f; break;
                        case 3: sourceSample = std::sin(p * 1.997) * 0.35 + white * 0.08; break;
                        default: sourceSample = (white > 0.94f ? white : 0.0f) * 2.5f; break; // Crackle
                    }
                }

                sourceSample = layerFilters[layer].processSample(0, static_cast<float>(sourceSample));
                const float layerAmp = layerEnvelopes[layer].getNextSample();
                if (enabled[layer])
                {
                    const double layerValue = sourceSample * static_cast<double>(gains[layer] * layerAmp);
                    mixedLeft += layerValue * panLeft[layer];
                    mixedRight += layerValue * panRight[layer];
                }
            }

            const float leftValue = static_cast<float>(mixedLeft * 0.12) * masterAmp;
            const float rightValue = static_cast<float>(mixedRight * 0.12) * masterAmp;
            if (channels > 0) output.addSample(0, startSample + sample, leftValue);
            if (channels > 1) output.addSample(1, startSample + sample, rightValue);

            elapsedSeconds += 1.0 / sampleRate;
        }

        const bool allLayersFinished = std::all_of(layerEnvelopes.begin(), layerEnvelopes.end(),
                                                   [](const juce::ADSR& env) { return !env.isActive(); });
        if (!masterEnvelope.isActive() && allLayersFinished)
            clearCurrentNote();
    }

private:
    float nextNoise(size_t layer)
    {
        auto& state = randomState[layer];
        state ^= state >> 16;
        state *= 0x7feb352du;
        state ^= state >> 15;
        state *= 0x846ca68bu;
        state ^= state >> 16;
        return static_cast<float>(state & 0x00FFFFFFu) / 8388607.5f - 1.0f;
    }

    std::atomic<float>* masterAttack = nullptr;
    std::atomic<float>* masterRelease = nullptr;
    std::array<std::atomic<float>*, 4> layerLevels {};
    std::array<std::atomic<float>*, 4> sourceParameters {};
    std::array<std::atomic<float>*, 4> panParameters {}, octaveParameters {}, tuneParameters {};
    std::array<std::atomic<float>*, 4> cutoffParameters {}, resonanceParameters {}, soloParameters {}, muteParameters {};
    std::array<juce::dsp::StateVariableTPTFilter<float>, 4> layerFilters;
    double preparedSampleRate = 0.0;
    std::array<std::array<std::atomic<float>*, 4>, 4> envelopeParameters {};
    juce::ADSR masterEnvelope;
    std::array<juce::ADSR, 4> layerEnvelopes;
    std::array<double, 4> phase {};
    std::array<double, 4> detunePhase {};
    std::array<float, 4> pinkState {};
    std::array<uint32_t, 4> randomState {};
    double frequency = 440.0;
    double elapsedSeconds = 0.0;
    float level = 0.0f;
};
} // namespace

juce::AudioProcessorValueTreeState::ParameterLayout AetheriaProcessor::createParameterLayout()
{
    using Float = juce::AudioParameterFloat;
    using Choice = juce::AudioParameterChoice;
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<Float>(juce::ParameterID { "master", 1 }, "Master", 0.0f, 1.0f, 0.75f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "attack", 1 }, "Master Attack",
                                       juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.35f), 0.02f));
    layout.add(std::make_unique<Float>(juce::ParameterID { "release", 1 }, "Master Release",
                                       juce::NormalisableRange<float>(0.01f, 8.0f, 0.001f, 0.35f), 0.65f));

    const std::array<juce::StringArray, 4> sourceChoices {{
        { "Sine Sub", "Triangle", "Saw", "Square", "Deep Sub" },
        { "Warm Saw", "Detuned Saw", "Pulse", "FM Keys", "Sine" },
        { "Pluck", "FM Bell", "Mallet", "Noise Hit", "Click" },
        { "White Noise", "Pink Noise", "Air", "Sine Cloud", "Crackle" }
    }};
    const std::array<juce::String, 4> sourceIds { "sourceA", "sourceB", "sourceC", "sourceD" };
    const std::array<juce::String, 4> sourceNames { "A Source", "B Source", "C Source", "D Source" };
    const std::array<juce::String, 4> levelIds { "layerA", "layerB", "layerC", "layerD" };
    const std::array<juce::String, 4> levelNames { "A Sub / Body", "B Mid / Harmony", "C Attack", "D Air / Texture" };
    const std::array<juce::String, 4> layerLetters { "A", "B", "C", "D" };

    for (size_t layer = 0; layer < 4; ++layer)
    {
        layout.add(std::make_unique<Choice>(juce::ParameterID { sourceIds[layer], 1 },
                                            sourceNames[layer], sourceChoices[layer], 0));
        layout.add(std::make_unique<Float>(juce::ParameterID { levelIds[layer], 1 },
                                           levelNames[layer], 0.0f, 1.0f,
                                           layer == 0 ? 0.85f : (layer == 1 ? 0.55f : (layer == 2 ? 0.30f : 0.20f))));

        const juce::String prefix = layerLetters[layer];
        layout.add(std::make_unique<Float>(juce::ParameterID { "pan" + prefix, 1 }, prefix + " Pan", -1.0f, 1.0f, 0.0f));
        layout.add(std::make_unique<Float>(juce::ParameterID { "octave" + prefix, 1 }, prefix + " Octave", -2.0f, 2.0f, 0.0f));
        layout.add(std::make_unique<Float>(juce::ParameterID { "tune" + prefix, 1 }, prefix + " Fine Tune", -100.0f, 100.0f, 0.0f));
        layout.add(std::make_unique<Float>(juce::ParameterID { "cutoff" + prefix, 1 }, prefix + " Filter Cutoff",
                                           juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f), 18000.0f));
        layout.add(std::make_unique<Float>(juce::ParameterID { "res" + prefix, 1 }, prefix + " Filter Resonance", 0.1f, 0.95f, 0.2f));
        layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID { "solo" + prefix, 1 }, prefix + " Solo", false));
        layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID { "mute" + prefix, 1 }, prefix + " Mute", false));
        const std::array<juce::String, 4> suffixes { "Attack", "Decay", "Sustain", "Release" };
        const std::array<float, 4> defaults {{
            layer == 0 ? 0.005f : (layer == 1 ? 0.02f : (layer == 2 ? 0.001f : 0.08f)),
            layer == 0 ? 0.25f : (layer == 1 ? 0.40f : (layer == 2 ? 0.15f : 0.80f)),
            layer == 0 ? 0.90f : (layer == 1 ? 0.70f : (layer == 2 ? 0.15f : 0.70f)),
            layer == 0 ? 0.25f : (layer == 1 ? 0.60f : (layer == 2 ? 0.12f : 1.50f))
        }};
        for (size_t stage = 0; stage < suffixes.size(); ++stage)
        {
            const bool sustain = stage == 2;
            const float minimum = sustain ? 0.0f : 0.001f;
            const float maximum = sustain ? 1.0f : 8.0f;
            layout.add(std::make_unique<Float>(juce::ParameterID { prefix + "_" + suffixes[stage], 1 },
                                               prefix + " " + suffixes[stage],
                                               minimum, maximum, defaults[stage]));
        }
    }
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
