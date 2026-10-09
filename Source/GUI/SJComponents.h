#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "SJLookAndFeel.h"
#include "../PresetManager.h"

namespace SJAudio
{

/**
 * @brief Professional Channel-Strip Rotary Knob component.
 */
class SJKnob : public juce::Slider
{
public:
    SJKnob(const juce::String& parameterName = {}, juce::Colour accentColor = juce::Colour(SJLookAndFeel::colAccentComp));
    ~SJKnob() override = default;

    void setAccentColor(juce::Colour color);

    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;
    void mouseDoubleClick(const juce::MouseEvent& e) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SJKnob)
};

/**
 * @brief Professional Hardware-style Push/Toggle Button.
 */
class SJToggleButton : public juce::ToggleButton
{
public:
    SJToggleButton(const juce::String& text = {}, juce::Colour ledColor = juce::Colour(SJLookAndFeel::colAccentComp));
    ~SJToggleButton() override = default;

    void setLedColor(juce::Colour color);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SJToggleButton)
};

/**
 * @brief Technical Channel-Strip Parameter Label.
 */
class SJLabel : public juce::Label
{
public:
    explicit SJLabel(const juce::String& text = {});
    ~SJLabel() override = default;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SJLabel)
};

/**
 * @brief Top Console Header Bar with Branding & Preset Manager.
 */
class SJHeader : public juce::Component
{
public:
    SJHeader(juce::AudioProcessorValueTreeState& apvts, PresetManager& presetMgr);
    ~SJHeader() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void refreshPresetList();

private:
    juce::AudioProcessorValueTreeState& apvtsRef;
    PresetManager& presetManager;

    juce::ComboBox presetBox;
    juce::TextButton prevButton { "<" };
    juce::TextButton nextButton { ">" };
    juce::TextButton saveButton { "SAVE" };
    juce::TextButton saveAsButton { "SAVE AS" };
    juce::TextButton deleteButton { "DEL" };

    // Oversampling Controls (Phase 13)
    juce::Label oversamplingLabel;
    juce::ComboBox oversamplingBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oversamplingAttachment;

    void promptSaveAs();
    void promptDelete();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SJHeader)
};

} // namespace SJAudio
