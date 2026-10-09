#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>
#include <vector>

namespace SJAudio
{

struct Preset
{
    juce::String name;
    bool isFactory = true;
    juce::File file {};
    std::vector<std::pair<juce::String, float>> paramValues;
};

/**
 * @brief Professional Preset Manager for SJ E Channel Strip (Phase 12).
 *
 * Implements 19 factory presets, user preset scanning, loading, saving,
 * "Save As", deleting, and robust XML preset serialization with error safety.
 */
class PresetManager
{
public:
    PresetManager();
    ~PresetManager() = default;

    void scanUserPresets();

    const std::vector<Preset>& getPresets() const noexcept { return presets; }
    int getNumPresets() const noexcept { return static_cast<int>(presets.size()); }
    juce::String getPresetName(int index) const;
    bool isCurrentPresetFactory() const;

    int getCurrentIndex() const noexcept { return currentIndex; }
    void setCurrentIndex(int index) noexcept;

    bool applyPreset(int index, juce::AudioProcessorValueTreeState& apvts);
    bool applyNextPreset(juce::AudioProcessorValueTreeState& apvts);
    bool applyPreviousPreset(juce::AudioProcessorValueTreeState& apvts);

    bool saveCurrentPreset(juce::AudioProcessorValueTreeState& apvts);
    bool savePresetAs(const juce::String& presetName, juce::AudioProcessorValueTreeState& apvts);
    bool deleteCurrentPreset(juce::AudioProcessorValueTreeState& apvts);

    juce::File getUserPresetDirectory() const;

private:
    std::vector<Preset> presets;
    int currentIndex = 0;

    void initFactoryPresets();
    bool savePresetToFile(const juce::String& name, const juce::File& file, juce::AudioProcessorValueTreeState& apvts);
    bool parsePresetFile(const juce::File& file, Preset& outPreset);
};

} // namespace SJAudio
