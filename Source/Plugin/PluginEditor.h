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
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    AetheriaProcessor& audioProcessor;
    std::array<juce::Slider, 7> sliders;
    std::array<juce::Label, 7> labels;
    std::array<std::unique_ptr<Attachment>, 7> attachments;

    const std::array<juce::String, 7> parameterIds {
        "layerA", "layerB", "layerC", "layerD", "master", "attack", "release"
    };
    const std::array<juce::String, 7> displayNames {
        "A  SUB / BODY", "B  MID / HARMONY", "C  ATTACK", "D  AIR / TEXTURE",
        "MASTER", "ATTACK", "RELEASE"
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AetheriaEditor)
};
} // namespace Aetheria
