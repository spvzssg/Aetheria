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

    AetheriaProcessor& audioProcessor;
    std::array<juce::Slider, 7> sliders;
    std::array<juce::Label, 7> labels;
    std::array<std::unique_ptr<SliderAttachment>, 7> sliderAttachments;
    std::array<juce::ComboBox, 4> sourceSelectors;
    std::array<std::unique_ptr<ComboAttachment>, 4> sourceAttachments;

    const std::array<juce::String, 7> parameterIds {
        "layerA", "layerB", "layerC", "layerD", "master", "attack", "release"
    };
    const std::array<juce::String, 7> displayNames {
        "A  SUB / BODY", "B  MID / HARMONY", "C  ATTACK", "D  AIR / TEXTURE",
        "MASTER", "MASTER ATTACK", "MASTER RELEASE"
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
