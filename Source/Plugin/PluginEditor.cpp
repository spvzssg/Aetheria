#include "PluginEditor.h"

namespace Aetheria
{
AetheriaEditor::AetheriaEditor(AetheriaProcessor& processor)
    : juce::AudioProcessorEditor(&processor), audioProcessor(processor)
{
    setSize(1200, 760);
    setResizable(true, true);
    setResizeLimits(1000, 700, 1800, 1100);

    for (size_t i = 0; i < sliders.size(); ++i)
    {
        auto& slider = sliders[i];
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 58, 16);
        slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xFF00DDF5));
        slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xFFFF791D));
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xFFD9DDE2));
        slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF15191F));
        addAndMakeVisible(slider);

        labels[i].setText(displayNames[i], juce::dontSendNotification);
        labels[i].setJustificationType(juce::Justification::centred);
        labels[i].setColour(juce::Label::textColourId, juce::Colour(0xFFD9DDE2));
        labels[i].setFont(juce::Font(juce::FontOptions(10.5f, juce::Font::bold)));
        addAndMakeVisible(labels[i]);

        sliderAttachments[i] = std::make_unique<SliderAttachment>(
            audioProcessor.parameters, parameterIds[i], slider);
    }

    for (size_t i = 0; i < sourceSelectors.size(); ++i)
    {
        auto& selector = sourceSelectors[i];
        selector.addItemList(sourceNames[i], 1);
        selector.setSelectedItemIndex(0, juce::dontSendNotification);
        selector.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF101820));
        selector.setColour(juce::ComboBox::textColourId, juce::Colour(0xFFE0E8EF));
        selector.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF31505F));
        selector.setColour(juce::ComboBox::arrowColourId, juce::Colour(0xFF00DDF5));
        addAndMakeVisible(selector);
        sourceAttachments[i] = std::make_unique<ComboAttachment>(
            audioProcessor.parameters, sourceParameterIds[i], selector);
    }

    for (size_t i = 0; i < layerButtons.size(); ++i)
    {
        auto& button = layerButtons[i];
        button.setButtonText((i % 2 == 0) ? "SOLO" : "MUTE");
        button.setColour(juce::ToggleButton::textColourId, juce::Colour(0xFFD9DDE2));
        button.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xFF00DDF5));
        button.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(0xFF52606B));
        addAndMakeVisible(button);
        buttonAttachments[i] = std::make_unique<ButtonAttachment>(
            audioProcessor.parameters, buttonParameterIds[i], button);
    }
}

void AetheriaEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF08090C));

    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xFF20262E));
    g.drawRoundedRectangle(bounds.reduced(12.0f), 12.0f, 1.0f);

    g.setColour(juce::Colour(0xFF00DDF5));
    g.setFont(juce::Font(juce::FontOptions(34.0f, juce::Font::bold)));
    g.drawText("AETHERIA", 28, 18, getWidth() - 56, 44, juce::Justification::centred);

    g.setColour(juce::Colour(0xFFD9DDE2));
    g.setFont(juce::Font(juce::FontOptions(12.0f)));
    g.drawText("BY SPAZ SSG  |  FOUR INDEPENDENT SOUND LAYERS",
               28, 62, getWidth() - 56, 20, juce::Justification::centred);

    const int panelTop = 98;
    const int panelHeight = getHeight() - 222;
    const int panelGap = 12;
    const int panelWidth = (getWidth() - 56 - panelGap * 3) / 4;
    const std::array<juce::Colour, 4> layerColours {{
        juce::Colour(0xFF00DDF5), juce::Colour(0xFFFFA52B),
        juce::Colour(0xFFC65BFF), juce::Colour(0xFF687DFF)
    }};
    const std::array<juce::String, 4> layerNames {{
        "A  SUB / BODY", "B  MID / HARMONY", "C  ATTACK / TRANSIENT", "D  AIR / TEXTURE"
    }};

    for (int i = 0; i < 4; ++i)
    {
        auto panel = juce::Rectangle<int>(28 + i * (panelWidth + panelGap),
                                           panelTop, panelWidth, panelHeight);
        g.setColour(juce::Colour(0xFF11161D));
        g.fillRoundedRectangle(panel.toFloat(), 9.0f);
        g.setColour(layerColours[static_cast<size_t>(i)].withAlpha(0.75f));
        g.drawRoundedRectangle(panel.toFloat(), 9.0f, 1.2f);
        g.setColour(layerColours[static_cast<size_t>(i)]);
        g.fillRoundedRectangle(panel.removeFromTop(3).toFloat(), 1.5f);
        g.setColour(layerColours[static_cast<size_t>(i)]);
        g.setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::bold)));
        g.drawText(layerNames[static_cast<size_t>(i)],
                   28 + i * (panelWidth + panelGap) + 8, panelTop + 12,
                   panelWidth - 16, 22, juce::Justification::centred);
    }

    auto footer = juce::Rectangle<int>(28, getHeight() - 112, getWidth() - 56, 84);
    g.setColour(juce::Colour(0xFF11161D));
    g.fillRoundedRectangle(footer.toFloat(), 9.0f);
    g.setColour(juce::Colour(0xFF303A45));
    g.drawRoundedRectangle(footer.toFloat(), 9.0f, 1.0f);
}

void AetheriaEditor::resized()
{
    const int top = 98;
    const int panelGap = 12;
    const int panelWidth = (getWidth() - 56 - panelGap * 3) / 4;
    const int panelHeight = getHeight() - 222;

    for (int layer = 0; layer < 4; ++layer)
    {
        const int x = 28 + layer * (panelWidth + panelGap);
        sourceSelectors[static_cast<size_t>(layer)].setBounds(x + 10, top + 42, panelWidth - 20, 28);
        layerButtons[static_cast<size_t>(layer * 2)].setBounds(x + 16, top + 74, (panelWidth - 40) / 2, 24);
        layerButtons[static_cast<size_t>(layer * 2 + 1)].setBounds(
            x + 24 + (panelWidth - 40) / 2, top + 74, (panelWidth - 40) / 2, 24);

        const int cellWidth = (panelWidth - 16) / 2;
        const int rowHeight = (panelHeight - 118) / 3;
        for (int control = 0; control < 6; ++control)
        {
            const int row = control / 2;
            const int column = control % 2;
            const int index = layer * 6 + control;
            const int cellX = x + 8 + column * cellWidth;
            const int cellY = top + 104 + row * rowHeight;
            labels[static_cast<size_t>(index)].setBounds(cellX, cellY, cellWidth, 16);
            sliders[static_cast<size_t>(index)].setBounds(
                cellX + 2, cellY + 15, cellWidth - 4, juce::jmin(88, rowHeight - 16));
        }
    }

    const int footerY = getHeight() - 112;
    const int footerWidth = (getWidth() - 56) / 3;
    for (int i = 24; i < 27; ++i)
    {
        const int x = 28 + (i - 24) * footerWidth;
        labels[static_cast<size_t>(i)].setBounds(x, footerY + 3, footerWidth, 16);
        sliders[static_cast<size_t>(i)].setBounds(x + 8, footerY + 18, footerWidth - 16, 62);
    }
}
} // namespace Aetheria
