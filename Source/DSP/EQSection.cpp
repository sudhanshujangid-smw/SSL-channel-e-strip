#include "EQSection.h"

namespace SJAudio
{

EQSection::EQSection()
{
    masterFade.setCurrentAndTargetValue(1.0f);
}

void EQSection::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;

    masterFade.reset(currentSampleRate, 0.015); // 15 ms clickless master EQ bypass

    lowBand.prepare(spec);
    lowMidBand.prepare(spec);
    highMidBand.prepare(spec);
    highBand.prepare(spec);

    reset();
}

void EQSection::reset()
{
    lowBand.reset();
    lowMidBand.reset();
    highMidBand.reset();
    highBand.reset();
}

void EQSection::setMasterEnabled(bool enabled)
{
    masterEnabled = enabled;
    masterFade.setTargetValue(enabled ? 1.0f : 0.0f);
}

void EQSection::setLowBand(bool enabled, float freqHz, float gainDb, float q, bool isShelf)
{
    lowBand.setParameters(enabled, freqHz, gainDb, q, isShelf ? EQFilterType::LowShelf : EQFilterType::Bell);
}

void EQSection::setLowMidBand(bool enabled, float freqHz, float gainDb, float q)
{
    lowMidBand.setParameters(enabled, freqHz, gainDb, q, EQFilterType::Bell);
}

void EQSection::setHighMidBand(bool enabled, float freqHz, float gainDb, float q)
{
    highMidBand.setParameters(enabled, freqHz, gainDb, q, EQFilterType::Bell);
}

void EQSection::setHighBand(bool enabled, float freqHz, float gainDb, float q, bool isShelf)
{
    highBand.setParameters(enabled, freqHz, gainDb, q, isShelf ? EQFilterType::HighShelf : EQFilterType::Bell);
}

void EQSection::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    if (numChannels == 0 || numSamples == 0)
        return;

    const bool isFading = masterFade.isSmoothing();
    const bool isAudiblyActive = masterEnabled || isFading || (masterFade.getTargetValue() > 0.0f);

    if (!isAudiblyActive)
        return;

    // Process all 4 bands in series
    lowBand.process(buffer);
    lowMidBand.process(buffer);
    highMidBand.process(buffer);
    highBand.process(buffer);
}

float EQSection::getCombinedMagnitudeForFrequency(float freqHz) const
{
    if (!masterEnabled)
        return 1.0f;

    float mag = 1.0f;
    mag *= lowBand.getMagnitudeForFrequency(freqHz);
    mag *= lowMidBand.getMagnitudeForFrequency(freqHz);
    mag *= highMidBand.getMagnitudeForFrequency(freqHz);
    mag *= highBand.getMagnitudeForFrequency(freqHz);

    return mag;
}

} // namespace SJAudio
