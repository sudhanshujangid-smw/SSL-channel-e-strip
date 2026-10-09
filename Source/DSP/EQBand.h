#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace SJAudio
{

enum class EQFilterType
{
    Bell,
    LowShelf,
    HighShelf
};

class EQBand
{
public:
    EQBand();
    ~EQBand() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void setParameters(bool enabled, float freqHz, float gainDb, float q, EQFilterType type);

    void process(juce::AudioBuffer<float>& buffer);

    /** Exact mathematical magnitude response at frequency */
    float getMagnitudeForFrequency(float freqHz) const;

    float getTargetFreq() const noexcept   { return targetFreq; }
    float getTargetGainDb() const noexcept { return targetGainDb; }
    float getTargetQ() const noexcept      { return targetQ; }
    bool  isEnabled() const noexcept       { return bandEnabled; }
    EQFilterType getType() const noexcept  { return filterType; }

private:
    double currentSampleRate = 44100.0;
    int currentNumChannels = 2;

    bool bandEnabled = true;
    float targetFreq = 1000.0f;
    float targetGainDb = 0.0f;
    float targetQ = 1.0f;
    EQFilterType filterType = EQFilterType::Bell;

    // Smoothing for zipper-free frequency, gain, and Q changes
    juce::SmoothedValue<float> smoothedFreq;
    juce::SmoothedValue<float> smoothedGainDb;
    juce::SmoothedValue<float> smoothedQ;
    juce::SmoothedValue<float> bandFade;

    float currentCoeffFreq = -1.0f;
    float currentCoeffGainDb = 0.0f;
    float currentCoeffQ = -1.0f;
    EQFilterType currentCoeffType = EQFilterType::Bell;

    static constexpr int maxChannels = 2;
    juce::dsp::IIR::Filter<float> filters[maxChannels];
    juce::dsp::IIR::Coefficients<float>::Ptr coeffs;

    void updateCoefficients(float freq, float gainDb, float q, EQFilterType type);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQBand)
};

} // namespace SJAudio
