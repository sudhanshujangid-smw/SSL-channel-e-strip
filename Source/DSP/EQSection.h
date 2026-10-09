#pragma once

#include "EQBand.h"

namespace SJAudio
{

class EQSection
{
public:
    EQSection();
    ~EQSection() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void setMasterEnabled(bool enabled);

    // Band setters
    void setLowBand(bool enabled, float freqHz, float gainDb, float q, bool isShelf);
    void setLowMidBand(bool enabled, float freqHz, float gainDb, float q);
    void setHighMidBand(bool enabled, float freqHz, float gainDb, float q);
    void setHighBand(bool enabled, float freqHz, float gainDb, float q, bool isShelf);

    void process(juce::AudioBuffer<float>& buffer);

    float getCombinedMagnitudeForFrequency(float freqHz) const;

    const EQBand& getLowBand() const noexcept     { return lowBand; }
    const EQBand& getLowMidBand() const noexcept  { return lowMidBand; }
    const EQBand& getHighMidBand() const noexcept { return highMidBand; }
    const EQBand& getHighBand() const noexcept    { return highBand; }

    bool isMasterEnabled() const noexcept { return masterEnabled; }

private:
    double currentSampleRate = 44100.0;
    bool masterEnabled = true;

    juce::SmoothedValue<float> masterFade;

    EQBand lowBand;
    EQBand lowMidBand;
    EQBand highMidBand;
    EQBand highBand;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQSection)
};

} // namespace SJAudio
