#pragma once
#include <JuceHeader.h>
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
    AetheriaProcessor& audioProcessor;
    const juce::Colour nearBlack { 0xFF08090C };
    const juce::Colour electricCyan { 0xFF00DDF5 };
    const juce::Colour warmOrange { 0xFFFF791D };
    const juce::Colour silver { 0xFFD9DDE2 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AetheriaEditor)
};
} // namespace Aetheria
