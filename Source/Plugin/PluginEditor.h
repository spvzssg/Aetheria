#pragma once

#include <JuceHeader.h>
#include <array>
#include <memory>

#include "PluginProcessor.h"

namespace Aetheria
{
class AetheriaEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AetheriaEditor(AetheriaProcessor& processor);
    void paint(juce::Graphics& graphics) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    AetheriaProcessor& audioProcessor;
    std::array<juce::Slider, 27> sliders;
    std::array<juce::Label, 27> labels;
    std::array<std::unique_ptr<SliderAttachment>, 27> sliderAttachments;
    std::array<juce::ComboBox, 4> sourceSelectors;
    std::array<std::unique_ptr<ComboAttachment>, 4> sourceAttachments;
    std::array<juce::ToggleButton, 8> layerButtons;
    std::array<std::unique_ptr<ButtonAttachment>, 8> buttonAttachments;

    const std::array<juce::String, 27> parameterIds {
        "layerA", "panA", "octaveA", "tuneA", "cutoffA", "resA",
        "layerB", "panB", "octaveB", "tuneB", "cutoffB", "resB",
        "layerC", "panC", "octaveC", "tuneC", "cutoffC", "resC",
        "layerD", "panD", "octaveD", "tuneD", "cutoffD", "resD",
        "master", "attack", "release"
    };
    const std::array<juce::String, 27> displayNames {
        "LEVEL", "PAN", "OCTAVE", "FINE TUNE", "FILTER", "RESONANCE",
        "LEVEL", "PAN", "OCTAVE", "FINE TUNE", "FILTER", "RESONANCE",
        "LEVEL", "PAN", "OCTAVE", "FINE TUNE", "FILTER", "RESONANCE",
        "LEVEL", "PAN", "OCTAVE", "FINE TUNE", "FILTER", "RESONANCE",
        "MASTER", "MASTER ATTACK", "MASTER RELEASE"
    };
    const std::array<juce::String, 8> buttonParameterIds {
        "soloA", "muteA", "soloB", "muteB", "soloC", "muteC", "soloD", "muteD"
    };
    const std::array<juce::String, 4> sourceParameterIds {
        "sourceA", "sourceB", "sourceC", "sourceD"
    };
    const std::array<juce::StringArray, 4> sourceNames {{
        { "Sine Sub", "Triangle", "Saw", "Square", "Deep Sub" },
        { "Warm Saw", "Detuned Saw", "Pulse", "FM Keys", "Sine" },
        { "Pluck", "FM Bell", "Mallet", "Noise Hit", "Click" },
        { "White Noise", "Pink Noise", "Air", "Sine Cloud", "Crackle" }
    }};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AetheriaEditor)
};
} // namespace Aetheria
