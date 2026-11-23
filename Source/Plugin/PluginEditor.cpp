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
        labels[i].setFont(juce::Font(juce::FontOptions(13.0f, juce::Font::bold)));
        addAndMakeVisible(labels[i]);

        sliderAttachments[i] = std::make_unique<SliderAttachment>(audioProcessor.parameters,
                                                                   parameterIds[i], slider);
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
}

void AetheriaEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF08090C));

    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xFF20262E));
    g.drawRoundedRectangle(bounds.reduced(12.0f), 12.0f, 1.0f);

    g.setColour(juce::Colour(0xFF00DDF5));
    g.setFont(juce::Font(juce::FontOptions(34.0f, juce::Font::bold)));
    g.drawText("AETHERIA", 28, 20, getWidth() - 56, 48, juce::Justification::centred);

    g.setColour(juce::Colour(0xFFD9DDE2));
    g.setFont(juce::Font(juce::FontOptions(13.0f)));
    g.drawText("BY SPAZ SSG  |  FOUR INDEPENDENT SOUND LAYERS",
               28, 65, getWidth() - 56, 24, juce::Justification::centred);

    const int panelTop = 112;
    const int panelHeight = juce::jmax(280, getHeight() - 270);
    const int panelGap = 12;
    const int panelWidth = (getWidth() - 56 - panelGap * 3) / 4;
    const std::array<juce::Colour, 4> layerColours {{
        juce::Colour(0xFF00DDF5), juce::Colour(0xFFFFA52B),
        juce::Colour(0xFFC65BFF), juce::Colour(0xFF687DFF)
    }};

    const std::array<juce::String, 4> roles {
        "A  SUB / BODY", "B  MID / HARMONY", "C  ATTACK / TRANSIENT", "D  AIR / TEXTURE"
    };

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
        g.setFont(juce::Font(juce::FontOptions(13.0f, juce::Font::bold)));
        g.drawText(roles[static_cast<size_t>(i)], 36 + i * (panelWidth + panelGap),
                   panelTop + 5, panelWidth - 16, 22, juce::Justification::centred);
    }

    auto footer = juce::Rectangle<int>(28, getHeight() - 132, getWidth() - 56, 100);
    g.setColour(juce::Colour(0xFF11161D));
    g.fillRoundedRectangle(footer.toFloat(), 9.0f);
    g.setColour(juce::Colour(0xFF303A45));
    g.drawRoundedRectangle(footer.toFloat(), 9.0f, 1.0f);
    g.setColour(juce::Colour(0xFF8C98A5));
    g.setFont(juce::Font(juce::FontOptions(11.0f)));
    g.drawText("MASTER OUTPUT  •  GLOBAL ENVELOPE  •  LAYER-SPECIFIC SOURCES AND ENVELOPES",
               footer.reduced(10).toFloat(), juce::Justification::centredBottom);
}

void AetheriaEditor::resized()
{
    const int top = 112;
    const int panelGap = 12;
    const int panelWidth = (getWidth() - 56 - panelGap * 3) / 4;
    const int panelHeight = juce::jmax(280, getHeight() - 270);

    for (int i = 0; i < 4; ++i)
    {
        const int x = 28 + i * (panelWidth + panelGap);
        const auto index = static_cast<size_t>(i);
        labels[index].setBounds(x + 8, top + 27, panelWidth - 16, 24);
        sourceSelectors[index].setBounds(x + 12, top + 60, panelWidth - 24, 30);
        sliders[index].setBounds(x + 8, top + 102, panelWidth - 16,
                                 juce::jmin(175, panelHeight - 112));
    }

    const int footerY = getHeight() - 132;
    const int footerWidth = (getWidth() - 56) / 3;
    for (int i = 4; i < 7; ++i)
    {
        const int x = 28 + (i - 4) * footerWidth;
        const auto index = static_cast<size_t>(i);
        labels[index].setBounds(x, footerY + 5, footerWidth, 20);
        sliders[index].setBounds(x + 4, footerY + 24, footerWidth - 8, 68);
    }
}
} // namespace Aetheria
