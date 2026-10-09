#include "SJComponents.h"
#include "../Parameters.h"

namespace SJAudio
{

//==============================================================================
// SJKnob
//==============================================================================
SJKnob::SJKnob(const juce::String& /*parameterName*/, juce::Colour accentColor)
{
    setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 15);
    setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
                        juce::MathConstants<float>::pi * 2.75f,
                        true);
    setScrollWheelEnabled(true);
    setDoubleClickReturnValue(true, 0.0);
    setAccentColor(accentColor);
}

void SJKnob::setAccentColor(juce::Colour color)
{
    getProperties().set("accentColor", static_cast<int64_t>(color.getARGB()));
    setColour(juce::Slider::rotarySliderFillColourId, color);
    repaint();
}

void SJKnob::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    if (isEnabled())
    {
        const double delta = (getMaximum() - getMinimum()) * (wheel.deltaY > 0.0f ? 0.02 : -0.02);
        setValue(getValue() + delta, juce::sendNotificationAsync);
    }
    else
    {
        juce::Slider::mouseWheelMove(e, wheel);
    }
}

void SJKnob::mouseDoubleClick(const juce::MouseEvent& /*e*/)
{
    if (isEnabled())
    {
        setValue(getDoubleClickReturnValue(), juce::sendNotificationAsync);
    }
}

//==============================================================================
// SJToggleButton
//==============================================================================
SJToggleButton::SJToggleButton(const juce::String& text, juce::Colour ledColor)
    : juce::ToggleButton(text)
{
    setLedColor(ledColor);
}

void SJToggleButton::setLedColor(juce::Colour color)
{
    setColour(juce::ToggleButton::tickColourId, color);
    repaint();
}

//==============================================================================
// SJLabel
//==============================================================================
SJLabel::SJLabel(const juce::String& text)
    : juce::Label({}, text)
{
    setFont(juce::FontOptions(9.0f, juce::Font::bold));
    setJustificationType(juce::Justification::centred);
    setColour(juce::Label::textColourId, juce::Colour(SJLookAndFeel::colTextSecondary));
}

//==============================================================================
// SJHeader
//==============================================================================
SJHeader::SJHeader(juce::AudioProcessorValueTreeState& apvts, PresetManager& presetMgr)
    : apvtsRef(apvts), presetManager(presetMgr)
{
    // Preset ComboBox
    refreshPresetList();
    presetBox.setSelectedId(1, juce::dontSendNotification);
    presetBox.setTooltip("Preset Selector: Choose from 19 factory presets or user-saved presets");
    presetBox.onChange = [this]()
    {
        const int selectedId = presetBox.getSelectedId();
        if (selectedId > 0)
        {
            presetManager.applyPreset(selectedId - 1, apvtsRef);
        }
    };
    addAndMakeVisible(presetBox);

    // Prev Button
    prevButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1f232d));
    prevButton.setColour(juce::TextButton::textColourOffId, juce::Colour(SJLookAndFeel::colTextSecondary));
    prevButton.setTooltip("Previous Preset");
    prevButton.onClick = [this]()
    {
        if (presetManager.applyPreviousPreset(apvtsRef))
            presetBox.setSelectedId(presetManager.getCurrentIndex() + 1, juce::dontSendNotification);
    };
    addAndMakeVisible(prevButton);

    // Next Button
    nextButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1f232d));
    nextButton.setColour(juce::TextButton::textColourOffId, juce::Colour(SJLookAndFeel::colTextSecondary));
    nextButton.setTooltip("Next Preset");
    nextButton.onClick = [this]()
    {
        if (presetManager.applyNextPreset(apvtsRef))
            presetBox.setSelectedId(presetManager.getCurrentIndex() + 1, juce::dontSendNotification);
    };
    addAndMakeVisible(nextButton);

    // Save Button
    saveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff222734));
    saveButton.setColour(juce::TextButton::textColourOffId, juce::Colour(SJLookAndFeel::colTextPrimary));
    saveButton.setTooltip("Save changes to current user preset (Factory presets prompt Save As)");
    saveButton.onClick = [this]()
    {
        if (presetManager.isCurrentPresetFactory())
        {
            promptSaveAs();
        }
        else
        {
            if (presetManager.saveCurrentPreset(apvtsRef))
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::MessageBoxIconType::InfoIcon,
                    "PRESET SAVED",
                    "User preset '" + presetManager.getPresetName(presetManager.getCurrentIndex()) + "' saved successfully.",
                    "OK"
                );
            }
        }
    };
    addAndMakeVisible(saveButton);

    // Save As Button
    saveAsButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff222734));
    saveAsButton.setColour(juce::TextButton::textColourOffId, juce::Colour(SJLookAndFeel::colTextPrimary));
    saveAsButton.setTooltip("Save current settings as a new user preset");
    saveAsButton.onClick = [this]()
    {
        promptSaveAs();
    };
    addAndMakeVisible(saveAsButton);

    // Delete Button
    deleteButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2a1b1e));
    deleteButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffe74c3c));
    deleteButton.setTooltip("Delete current user preset (Factory presets protected)");
    deleteButton.onClick = [this]()
    {
        promptDelete();
    };
    addAndMakeVisible(deleteButton);

    // Oversampling Controls (Phase 13)
    oversamplingLabel.setText("HQ / OS", juce::dontSendNotification);
    oversamplingLabel.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    oversamplingLabel.setColour(juce::Label::textColourId, juce::Colour(SJLookAndFeel::colTextSecondary));
    oversamplingLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(oversamplingLabel);

    oversamplingBox.addItem("1x", 1);
    oversamplingBox.addItem("2x", 2);
    oversamplingBox.addItem("4x", 3);
    oversamplingBox.setTooltip("Nonlinear Saturation Oversampling (1x: 0 smp latency, 2x: High Quality, 4x: Mastering Quality)");
    addAndMakeVisible(oversamplingBox);

    oversamplingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvtsRef, ParameterIDs::oversampling, oversamplingBox);
}

void SJHeader::refreshPresetList()
{
    presetBox.clear(juce::dontSendNotification);

    bool addedUserHeading = false;
    for (int i = 0; i < presetManager.getNumPresets(); ++i)
    {
        if (!presetManager.getPresets()[i].isFactory && !addedUserHeading)
        {
            presetBox.addSeparator();
            presetBox.addSectionHeading("USER PRESETS");
            addedUserHeading = true;
        }

        presetBox.addItem(presetManager.getPresetName(i), i + 1);
    }
}

void SJHeader::promptSaveAs()
{
    auto* aw = new juce::AlertWindow("SAVE PRESET AS", "Enter new preset name:", juce::MessageBoxIconType::QuestionIcon);
    aw->addTextEditor("presetName", "My Custom Strip", "Name:");
    aw->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
    aw->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    aw->enterModalState(true, juce::ModalCallbackFunction::create([this, aw](int result) {
        if (result == 1)
        {
            auto name = aw->getTextEditorContents("presetName").trim();
            if (name.isNotEmpty())
            {
                if (presetManager.savePresetAs(name, apvtsRef))
                {
                    refreshPresetList();
                    presetBox.setSelectedId(presetManager.getCurrentIndex() + 1, juce::dontSendNotification);
                }
            }
        }
        delete aw;
    }), true);
}

void SJHeader::promptDelete()
{
    if (presetManager.isCurrentPresetFactory())
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::MessageBoxIconType::InfoIcon,
            "PROTECTED PRESET",
            "Factory presets are protected and cannot be deleted.",
            "OK"
        );
        return;
    }

    auto name = presetManager.getPresetName(presetManager.getCurrentIndex());
    juce::AlertWindow::showOkCancelBox(
        juce::MessageBoxIconType::WarningIcon,
        "DELETE PRESET",
        "Are you sure you want to delete user preset '" + name + "'?",
        "Delete", "Cancel", nullptr,
        juce::ModalCallbackFunction::create([this](int result) {
            if (result == 1)
            {
                presetManager.deleteCurrentPreset(apvtsRef);
                refreshPresetList();
                presetBox.setSelectedId(presetManager.getCurrentIndex() + 1, juce::dontSendNotification);
            }
        })
    );
}

void SJHeader::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Dark chassis header background
    juce::ColourGradient bgGrad(
        juce::Colour(0xff171920), 0.0f, 0.0f,
        juce::Colour(0xff121318), 0.0f, bounds.getBottom(),
        false
    );
    g.setGradientFill(bgGrad);
    g.fillRect(bounds);

    // Metallic bottom accent divider
    g.setColour(juce::Colour(0xff292e3c));
    g.drawHorizontalLine(static_cast<int>(bounds.getBottom() - 1.0f), 0.0f, bounds.getRight());
    g.setColour(juce::Colour(0xff101115));
    g.drawHorizontalLine(static_cast<int>(bounds.getBottom()), 0.0f, bounds.getRight());

    // Left: SJ Audio Emblem & Wordmark
    auto logoArea = bounds.removeFromLeft(130.0f).reduced(10.0f, 14.0f);
    g.setColour(juce::Colour(0xffe5a93c));
    g.drawRoundedRectangle(logoArea, 3.5f, 1.4f);
    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    g.drawText("SJ AUDIO", logoArea, juce::Justification::centred);

    // Center: Plugin Title
    auto titleArea = bounds.removeFromLeft(380.0f).reduced(12.0f, 8.0f);
    g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    g.setColour(juce::Colour(SJLookAndFeel::colTextPrimary));
    g.drawText("SJ E CHANNEL STRIP", titleArea.withTrimmedBottom(14.0f), juce::Justification::left);

    g.setFont(juce::FontOptions(10.5f, juce::Font::plain));
    g.setColour(juce::Colour(SJLookAndFeel::colTextSecondary));
    g.drawText("ANALOG CONSOLE DYNAMICS & PARAMETRIC EQ", titleArea.withTrimmedTop(18.0f), juce::Justification::left);

    // Right: Version badge
    auto verArea = bounds.removeFromRight(70.0f).reduced(8.0f, 18.0f);
    g.setColour(juce::Colour(0xff1c202a));
    g.fillRoundedRectangle(verArea, 3.0f);
    g.setColour(juce::Colour(0xff2c3240));
    g.drawRoundedRectangle(verArea, 3.0f, 1.0f);
    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    g.setColour(juce::Colour(SJLookAndFeel::colTextMuted));
    g.drawText("v1.0.0", verArea, juce::Justification::centred);
}

void SJHeader::resized()
{
    auto bounds = getLocalBounds().reduced(8, 12);
    bounds.removeFromLeft(480); // Logo + Title
    bounds.removeFromRight(70); // Version

    // Preset Controls Area: [ < ] [ Preset Box ] [ > ] [ SAVE ] [ SAVE AS ] [ DEL ]
    auto presetArea = bounds.removeFromRight(440);

    deleteButton.setBounds(presetArea.removeFromRight(38).reduced(1, 3));
    presetArea.removeFromRight(3);

    saveAsButton.setBounds(presetArea.removeFromRight(65).reduced(1, 3));
    presetArea.removeFromRight(3);

    saveButton.setBounds(presetArea.removeFromRight(46).reduced(1, 3));
    presetArea.removeFromRight(4);

    nextButton.setBounds(presetArea.removeFromRight(26).reduced(1, 3));
    presetArea.removeFromRight(2);

    prevButton.setBounds(presetArea.removeFromLeft(26).reduced(1, 3));
    presetArea.removeFromLeft(4);

    presetBox.setBounds(presetArea.reduced(1, 3));

    // Middle area: Oversampling Controls (Phase 13) [ HQ / OS ] [ 1x | 2x | 4x ]
    bounds.removeFromRight(12);
    auto osArea = bounds.removeFromRight(125);
    oversamplingBox.setBounds(osArea.removeFromRight(56).reduced(1, 3));
    osArea.removeFromRight(4);
    oversamplingLabel.setBounds(osArea);
}

} // namespace SJAudio
