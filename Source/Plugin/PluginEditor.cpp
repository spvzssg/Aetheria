#include "PluginEditor.h"

namespace Aetheria
{
AetheriaEditor::AetheriaEditor(AetheriaProcessor& processor)
    : juce::AudioProcessorEditor(&processor), audioProcessor(processor)
{
    setSize(1200, 720);
    setResizable(true, true);
    setResizeLimits(900, 600, 1600, 1000);

    for (size_t i = 0; i < sliders.size(); ++i)
    {
        auto& slider = sliders[i];
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 76, 22);
        slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xFF00DDF5));
        slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xFFFF791D));
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xFFD9DDE2));
        slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF15191F));
        addAndMakeVisible(slider);

        labels[i].setText(displayNames[i], juce::dontSendNotification);
        labels[i].setJustificationType(juce::Justification::centred);
        labels[i].setColour(juce::Label::textColourId, juce::Colour(0xFFD9DDE2));
        labels[i].setFont(juce::Font(13.0f, juce::Font::bold));
        addAndMakeVisible(labels[i]);

        attachments[i] = std::make_unique<Attachment>(audioProcessor.parameters,
                                                       parameterIds[i], slider);
    }
}

void AetheriaEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF08090C));

    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xFF20262E));
    g.drawRoundedRectangle(bounds.reduced(12.0f), 12.0f, 1.0f);

    g.setColour(juce::Colour(0xFF00DDF5));
    g.setFont(juce::Font(34.0f, juce::Font::bold));
    g.drawText("AETHERIA", 28, 20, getWidth() - 56, 48, juce::Justification::centred);

    g.setColour(juce::Colour(0xFFD9DDE2));
    g.setFont(juce::Font(13.0f));
    g.drawText("BY SPAZ SSG  |  GENERATIVE HYBRID INSTRUMENT",
               28, 65, getWidth() - 56, 24, juce::Justification::centred);

    const int panelTop = 112;
    const int panelHeight = juce::jmax(280, getHeight() - 270);
    const int panelGap = 12;
    const int panelWidth = (getWidth() - 56 - panelGap * 3) / 4;

    for (int i = 0; i < 4; ++i)
    {
        auto panel = juce::Rectangle<int>(28 + i * (panelWidth + panelGap),
                                           panelTop, panelWidth, panelHeight);
        g.setColour(juce::Colour(0xFF11161D));
        g.fillRoundedRectangle(panel.toFloat(), 9.0f);
        g.setColour(juce::Colour(0xFF303A45));
        g.drawRoundedRectangle(panel.toFloat(), 9.0f, 1.0f);
        g.setColour(i == 0 ? juce::Colour(0xFFFF791D) : juce::Colour(0xFF00DDF5));
        g.fillRoundedRectangle(panel.removeFromTop(3).toFloat(), 1.5f);
    }

    auto footer = juce::Rectangle<int>(28, getHeight() - 132, getWidth() - 56, 100);
    g.setColour(juce::Colour(0xFF11161D));
    g.fillRoundedRectangle(footer.toFloat(), 9.0f);
    g.setColour(juce::Colour(0xFF303A45));
    g.drawRoundedRectangle(footer.toFloat(), 9.0f, 1.0f);
    g.setColour(juce::Colour(0xFF8C98A5));
    g.setFont(11.0f);
    g.drawText("FOUNDATION BUILD  •  FOUR-LAYER POLYPHONIC ENGINE",
               footer.reduced(10).toFloat(), juce::Justification::centredBottom);
}

void AetheriaEditor::resized()
{
    const int top = 118;
    const int panelGap = 12;
    const int panelWidth = (getWidth() - 56 - panelGap * 3) / 4;
    const int panelHeight = juce::jmax(280, getHeight() - 270);

    for (int i = 0; i < 4; ++i)
    {
        const int x = 28 + i * (panelWidth + panelGap);
        labels[static_cast<size_t>(i)].setBounds(x + 8, top + 26, panelWidth - 16, 28);
        sliders[static_cast<size_t>(i)].setBounds(x + 8, top + 62, panelWidth - 16,
                                                   juce::jmin(190, panelHeight - 75));
    }

    const int footerY = getHeight() - 132;
    const int footerWidth = (getWidth() - 56) / 7;
    for (int i = 4; i < 7; ++i)
    {
        const int x = 28 + (i - 4) * footerWidth;
        labels[static_cast<size_t>(i)].setBounds(x, footerY + 8, footerWidth, 20);
        sliders[static_cast<size_t>(i)].setBounds(x + 4, footerY + 25,
                                                  footerWidth - 8, 68);
    }
}
} // namespace Aetheria
