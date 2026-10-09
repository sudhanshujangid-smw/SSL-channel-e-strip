#include "SJLookAndFeel.h"

namespace SJAudio
{

SJLookAndFeel::SJLookAndFeel()
{
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff4ea8de));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff15181f));
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(colTextPrimary));
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff121419));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff252a36));

    setColour(juce::ToggleButton::textColourId, juce::Colour(colTextSecondary));
    setColour(juce::ToggleButton::tickColourId, juce::Colour(colAccentComp));

    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff161920));
    setColour(juce::ComboBox::textColourId, juce::Colour(colTextPrimary));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff2d3342));
    setColour(juce::ComboBox::arrowColourId, juce::Colour(colTextSecondary));

    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff161920));
    setColour(juce::PopupMenu::textColourId, juce::Colour(colTextPrimary));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff262b38));
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colour(0xffffffff));
}

juce::Font SJLookAndFeel::getLabelFont(juce::Label& /*label*/)
{
    return juce::FontOptions(10.0f, juce::Font::bold);
}

void SJLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                     float sliderPosProportional, float rotaryStartAngle,
                                     float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                         static_cast<float>(width), static_cast<float>(height));

    const float cx = bounds.getCentreX();
    const float cy = bounds.getCentreY();
    const float radius = std::min(bounds.getWidth(), bounds.getHeight()) * 0.5f - 3.0f;

    if (radius < 4.0f)
        return;

    // Retrieve accent color for active arc and pointer
    juce::Colour accent = slider.findColour(juce::Slider::rotarySliderFillColourId);
    if (slider.getProperties().contains("accentColor"))
    {
        accent = juce::Colour(static_cast<juce::uint32>(static_cast<int64_t>(slider.getProperties()["accentColor"])));
    }

    // 1. Drop shadow behind the knob
    g.setColour(juce::Colour(0x66000000));
    g.fillEllipse(cx - radius, cy - radius + 2.0f, radius * 2.0f, radius * 2.0f);

    // 2. Outer machined bezel
    juce::ColourGradient bezelGrad(
        juce::Colour(0xff3c4354), cx - radius, cy - radius,
        juce::Colour(0xff1a1d24), cx + radius, cy + radius,
        false
    );
    g.setGradientFill(bezelGrad);
    g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

    // 3. Background parameter arc groove
    const float trackR = radius - 2.0f;
    juce::Path bgTrack;
    bgTrack.addCentredArc(cx, cy, trackR, trackR, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff121419));
    g.strokePath(bgTrack, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 4. Active parameter arc
    const float curAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    if (curAngle > rotaryStartAngle + 0.01f)
    {
        juce::Path activeTrack;
        activeTrack.addCentredArc(cx, cy, trackR, trackR, 0.0f, rotaryStartAngle, curAngle, true);
        g.setColour(accent.withAlpha(0.9f));
        g.strokePath(activeTrack, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // 5. Knob Cap Body
    const float bodyR = radius * 0.77f;
    juce::ColourGradient capGrad(
        juce::Colour(0xff2d323f), cx - bodyR * 0.5f, cy - bodyR * 0.5f,
        juce::Colour(0xff16181f), cx + bodyR * 0.5f, cy + bodyR * 0.5f,
        false
    );
    g.setGradientFill(capGrad);
    g.fillEllipse(cx - bodyR, cy - bodyR, bodyR * 2.0f, bodyR * 2.0f);

    // Inner rim stroke
    g.setColour(juce::Colour(0xff3e4556));
    g.drawEllipse(cx - bodyR, cy - bodyR, bodyR * 2.0f, bodyR * 2.0f, 1.0f);

    // Subtle concentric grip ring
    const float ringR = bodyR * 0.85f;
    g.setColour(juce::Colour(0x22ffffff));
    g.drawEllipse(cx - ringR, cy - ringR, ringR * 2.0f, ringR * 2.0f, 0.6f);

    // 6. Machined Indicator Pointer
    const float pLenInner = bodyR * 0.32f;
    const float pLenOuter = bodyR * 0.94f;
    const float sinA = std::sin(curAngle);
    const float cosA = -std::cos(curAngle);

    juce::Line<float> pointerLine(
        cx + pLenInner * sinA, cy + pLenInner * cosA,
        cx + pLenOuter * sinA, cy + pLenOuter * cosA
    );

    // Shadow below pointer
    juce::Line<float> shadowLine(
        pointerLine.getStartX() + 0.8f, pointerLine.getStartY() + 1.0f,
        pointerLine.getEndX() + 0.8f, pointerLine.getEndY() + 1.0f
    );
    g.setColour(juce::Colour(0x55000000));
    g.drawLine(shadowLine, 2.2f);

    // Luminous pointer
    g.setColour(accent.brighter(0.35f));
    g.drawLine(pointerLine, 2.0f);

    // 7. Central screw disc
    const float centerR = bodyR * 0.26f;
    juce::ColourGradient centerGrad(
        juce::Colour(0xff363c4a), cx - centerR, cy - centerR,
        juce::Colour(0xff13151b), cx + centerR, cy + centerR,
        false
    );
    g.setGradientFill(centerGrad);
    g.fillEllipse(cx - centerR, cy - centerR, centerR * 2.0f, centerR * 2.0f);
    g.setColour(juce::Colour(0xff22252f));
    g.drawEllipse(cx - centerR, cy - centerR, centerR * 2.0f, centerR * 2.0f, 0.8f);
}

void SJLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                     bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f, 1.0f);

    const bool isToggled = button.getToggleState();
    const juce::Colour ledColor = button.findColour(juce::ToggleButton::tickColourId);

    // 1. Tactile push-button body
    if (shouldDrawButtonAsDown)
    {
        g.setColour(juce::Colour(0xff14161c));
        g.fillRoundedRectangle(bounds, 3.5f);
        g.setColour(juce::Colour(0xff282e3b));
        g.drawRoundedRectangle(bounds, 3.5f, 1.0f);
    }
    else
    {
        juce::ColourGradient btnGrad(
            shouldDrawButtonAsHighlighted ? juce::Colour(0xff282d3a) : juce::Colour(0xff222631),
            bounds.getX(), bounds.getY(),
            juce::Colour(0xff181a22),
            bounds.getX(), bounds.getBottom(),
            false
        );
        g.setGradientFill(btnGrad);
        g.fillRoundedRectangle(bounds, 3.5f);

        g.setColour(shouldDrawButtonAsHighlighted ? juce::Colour(0xff3f475a) : juce::Colour(0xff303645));
        g.drawRoundedRectangle(bounds, 3.5f, 1.0f);
    }

    // 2. LED Jewel Indicator
    const float ledSize = 6.0f;
    const float ledX = bounds.getX() + 7.0f;
    const float ledY = bounds.getCentreY() - ledSize * 0.5f;

    // Chrome lens housing
    g.setColour(juce::Colour(0xff12141a));
    g.fillEllipse(ledX - 1.0f, ledY - 1.0f, ledSize + 2.0f, ledSize + 2.0f);
    g.setColour(juce::Colour(0xff383f4f));
    g.drawEllipse(ledX - 1.0f, ledY - 1.0f, ledSize + 2.0f, ledSize + 2.0f, 0.8f);

    if (isToggled)
    {
        // Glowing LED lens with soft bloom
        g.setColour(ledColor.withAlpha(0.35f));
        g.fillEllipse(ledX - 2.5f, ledY - 2.5f, ledSize + 5.0f, ledSize + 5.0f);

        juce::ColourGradient ledGrad(
            ledColor.brighter(0.5f), ledX + 1.5f, ledY + 1.5f,
            ledColor, ledX + ledSize, ledY + ledSize,
            true
        );
        g.setGradientFill(ledGrad);
        g.fillEllipse(ledX, ledY, ledSize, ledSize);

        // Specular dot
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.fillEllipse(ledX + 1.5f, ledY + 1.5f, 1.8f, 1.8f);
    }
    else
    {
        // Dark unlit lens
        g.setColour(juce::Colour(0xff1a1d24));
        g.fillEllipse(ledX, ledY, ledSize, ledSize);
    }

    // 3. Button Label Text
    auto textArea = bounds.withTrimmedLeft(ledSize + 12.0f).withTrimmedRight(4.0f);
    g.setFont(juce::FontOptions(9.5f, juce::Font::bold));
    g.setColour(isToggled ? juce::Colour(colTextPrimary) : juce::Colour(colTextSecondary));
    g.drawText(button.getButtonText(), textArea, juce::Justification::centredLeft, true);
}

void SJLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                 int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                 juce::ComboBox& /*box*/)
{
    auto bounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)).reduced(0.5f);

    g.setColour(juce::Colour(0xff161920));
    g.fillRoundedRectangle(bounds, 3.5f);
    g.setColour(juce::Colour(0xff2d3342));
    g.drawRoundedRectangle(bounds, 3.5f, 1.0f);

    // Chevron Arrow
    const float arrowX = bounds.getRight() - 14.0f;
    const float arrowY = bounds.getCentreY();
    juce::Path arrow;
    arrow.startNewSubPath(arrowX - 3.5f, arrowY - 2.0f);
    arrow.lineTo(arrowX, arrowY + 2.5f);
    arrow.lineTo(arrowX + 3.5f, arrowY - 2.0f);

    g.setColour(juce::Colour(colTextSecondary));
    g.strokePath(arrow, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void SJLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
    auto bounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    g.setColour(juce::Colour(0xff161920));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(juce::Colour(0xff2d3342));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
}

void SJLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                      bool isSeparator, bool isActive, bool isHighlighted,
                                      bool isTicked, bool /*hasSubMenu*/, const juce::String& text,
                                      const juce::String& /*shortcutKeyText*/,
                                      const juce::Drawable* /*icon*/, const juce::Colour* textColour)
{
    if (isSeparator)
    {
        auto r = area.reduced(4, 0);
        g.setColour(juce::Colour(0xff262b38));
        g.drawLine(static_cast<float>(r.getX()), static_cast<float>(r.getCentreY()),
                   static_cast<float>(r.getRight()), static_cast<float>(r.getCentreY()), 1.0f);
        return;
    }

    if (isHighlighted && isActive)
    {
        g.setColour(juce::Colour(0xff262b38));
        g.fillRoundedRectangle(area.reduced(2, 1).toFloat(), 3.0f);
    }

    g.setFont(juce::FontOptions(11.0f, isTicked ? juce::Font::bold : juce::Font::plain));
    g.setColour(isHighlighted ? juce::Colours::white : (textColour ? *textColour : juce::Colour(colTextPrimary)));

    auto textArea = area.reduced(8, 0);
    g.drawText(text, textArea, juce::Justification::centredLeft, true);

    if (isTicked)
    {
        g.setColour(juce::Colour(colAccentComp));
        g.fillEllipse(area.getRight() - 14.0f, area.getCentreY() - 3.0f, 6.0f, 6.0f);
    }
}

} // namespace SJAudio
