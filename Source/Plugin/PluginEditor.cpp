#include "PluginEditor.h"

namespace Aetheria
{
AetheriaEditor::AetheriaEditor(AetheriaProcessor& processor)
    : juce::AudioProcessorEditor(&processor), audioProcessor(processor)
{
    setSize(1200, 700);
}

void AetheriaEditor::paint(juce::Graphics& g)
{
    juce::ignoreUnused(audioProcessor);
    g.fillAll(nearBlack);

    auto area = getLocalBounds().reduced(32);
    g.setColour(electricCyan);
    g.setFont(juce::Font(48.0f, juce::Font::bold));
    g.drawText("AETHERIA", area.removeFromTop(100), juce::Justification::centred);

    g.setColour(silver);
    g.setFont(20.0f);
    g.drawText("by Spaz SsG", area.removeFromTop(36), juce::Justification::centred);

    g.setColour(warmOrange);
    g.setFont(16.0f);
    g.drawText("PHASE 0 BUILD VERIFICATION", area.removeFromTop(32), juce::Justification::centred);

    g.setColour(silver.withAlpha(0.75f));
    g.drawText("Plugin shell only — audio output is intentionally silent at this stage.",
               area.removeFromTop(28), juce::Justification::centred);
}

void AetheriaEditor::resized()
{
}
} // namespace Aetheria
