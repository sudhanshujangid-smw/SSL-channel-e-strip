#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace SJAudio
{

/**
 * @brief Original Hardware Console LookAndFeel for SJ Audio.
 *
 * Implements:
 * - Machined anodized aluminum rotary knobs with concentric bevels,
 *   subtle drop shadows, parameter arc tracks, and precision indicator notches.
 * - Hardware push-button switches with recessed bezels and realistic jewel LED indicators.
 * - Dark graphite console panel aesthetics with subtle metallic highlights.
 * - Technical typography and high-contrast parameter readouts.
 */
class SJLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SJLookAndFeel();
    ~SJLookAndFeel() override = default;

    // Rotary Knobs
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;

    // Toggle Buttons / Switches
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    // Combo Box / Preset Dropdown
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted,
                           bool isTicked, bool hasSubMenu, const juce::String& text,
                           const juce::String& shortcutKeyText,
                           const juce::Drawable* icon, const juce::Colour* textColour) override;

    // Label Font
    juce::Font getLabelFont(juce::Label& label) override;

    // Reusable color palette
    static constexpr uint32_t colPanelBase       = 0xff16181e;
    static constexpr uint32_t colPanelCard       = 0xff1c2028;
    static constexpr uint32_t colPanelCardHeader = 0xff232733;
    static constexpr uint32_t colBorder          = 0xff2a303e;
    static constexpr uint32_t colBorderHighlight = 0xff3b4356;
    static constexpr uint32_t colTextPrimary     = 0xffeceff4;
    static constexpr uint32_t colTextSecondary   = 0xff98a2b3;
    static constexpr uint32_t colTextMuted       = 0xff5c6370;

    // Section Accent Colors
    static constexpr uint32_t colAccentInput     = 0xffe5a93c; // Warm gold
    static constexpr uint32_t colAccentFilter    = 0xff2ac3a2; // Emerald teal
    static constexpr uint32_t colAccentGate      = 0xff4ea8de; // Ice blue
    static constexpr uint32_t colAccentEQ        = 0xff9d4edd; // Royal violet
    static constexpr uint32_t colAccentComp      = 0xff28a745; // Studio green
    static constexpr uint32_t colAccentColor     = 0xfff39c12; // Amber fire
    static constexpr uint32_t colAccentOutput    = 0xffe74c3c; // Crimson red
};

} // namespace SJAudio
