#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace SJAudio
{

class FilterSection
{
public:
    enum class Slope
    {
        Slope12Db = 0,
        Slope18Db = 1,
        Slope24Db = 2
    };

    FilterSection();
    ~FilterSection() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    /** Configure High Pass Filter */
    void setHighPass(bool enabled, float freqHz, int slopeIndex);

    /** Configure Low Pass Filter */
    void setLowPass(bool enabled, float freqHz, int slopeIndex);

    /** Process audio in-place (mono or stereo) */
    void process(juce::AudioBuffer<float>& buffer);

    /** Compute combined magnitude response at frequency for UI graph */
    float getMagnitudeForFrequency(float freqHz) const;

    /** Compute individual HPF magnitude response at frequency */
    float getHighPassMagnitudeForFrequency(float freqHz) const;

    /** Compute individual LPF magnitude response at frequency */
    float getLowPassMagnitudeForFrequency(float freqHz) const;

    bool isHighPassActive() const noexcept { return hpfEnabled; }
    bool isLowPassActive() const noexcept  { return lpfEnabled; }
    float getHighPassFreq() const noexcept { return smoothedHpFreq.getTargetValue(); }
    float getLowPassFreq() const noexcept  { return smoothedLpFreq.getTargetValue(); }

private:
    double currentSampleRate = 44100.0;
    int currentNumChannels = 2;

    // HPF State & Smoothing
    bool hpfEnabled = false;
    Slope hpfSlope = Slope::Slope12Db;
    juce::SmoothedValue<float> smoothedHpFreq;
    juce::SmoothedValue<float> hpfFade;
    float currentHpfCoeffFreq = -1.0f;
    Slope currentHpfCoeffSlope = Slope::Slope12Db;

    // LPF State & Smoothing
    bool lpfEnabled = false;
    Slope lpfSlope = Slope::Slope12Db;
    juce::SmoothedValue<float> smoothedLpFreq;
    juce::SmoothedValue<float> lpfFade;
    float currentLpfCoeffFreq = -1.0f;
    Slope currentLpfCoeffSlope = Slope::Slope12Db;

    static constexpr int maxStages = 2;
    static constexpr int maxChannels = 2;

    juce::dsp::IIR::Filter<float> hpfFilters[maxChannels][maxStages];
    juce::dsp::IIR::Filter<float> lpfFilters[maxChannels][maxStages];

    juce::dsp::IIR::Coefficients<float>::Ptr hpfCoeffs[maxStages];
    juce::dsp::IIR::Coefficients<float>::Ptr lpfCoeffs[maxStages];

    void updateHpfCoefficients(float freq, Slope slope);
    void updateLpfCoefficients(float freq, Slope slope);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterSection)
};

} // namespace SJAudio
