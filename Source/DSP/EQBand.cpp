#include "EQBand.h"
#include <algorithm>

namespace SJAudio
{

EQBand::EQBand()
{
    smoothedFreq.setCurrentAndTargetValue(1000.0f);
    smoothedGainDb.setCurrentAndTargetValue(0.0f);
    smoothedQ.setCurrentAndTargetValue(1.0f);
    bandFade.setCurrentAndTargetValue(1.0f);
}

void EQBand::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;
    currentNumChannels = std::min((int)spec.numChannels, maxChannels);

    // 25 ms smoothing for continuous adjustments
    smoothedFreq.reset(currentSampleRate, 0.025);
    smoothedGainDb.reset(currentSampleRate, 0.025);
    smoothedQ.reset(currentSampleRate, 0.025);

    // 15 ms ramp for clickless band enable/disable
    bandFade.reset(currentSampleRate, 0.015);

    reset();
}

void EQBand::reset()
{
    currentCoeffFreq = -1.0f;
    currentCoeffGainDb = -999.0f;
    currentCoeffQ = -1.0f;

    for (int ch = 0; ch < maxChannels; ++ch)
        filters[ch].reset();

    updateCoefficients(smoothedFreq.getTargetValue(), smoothedGainDb.getTargetValue(), smoothedQ.getTargetValue(), filterType);
}

void EQBand::setParameters(bool enabled, float freqHz, float gainDb, float q, EQFilterType type)
{
    bandEnabled = enabled;
    targetFreq = freqHz;
    targetGainDb = gainDb;
    targetQ = q;
    filterType = type;

    bandFade.setTargetValue(enabled ? 1.0f : 0.0f);
    smoothedFreq.setTargetValue(freqHz);
    smoothedGainDb.setTargetValue(gainDb);
    smoothedQ.setTargetValue(q);
}

void EQBand::updateCoefficients(float freq, float gainDb, float q, EQFilterType type)
{
    const float safeFreq = std::clamp(freq, 20.0f, static_cast<float>(currentSampleRate * 0.49));
    const float safeQ = std::clamp(q, 0.1f, 10.0f);
    const float linearGain = juce::Decibels::decibelsToGain(gainDb);

    if (type == EQFilterType::LowShelf)
    {
        coeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(currentSampleRate, safeFreq, safeQ, linearGain);
    }
    else if (type == EQFilterType::HighShelf)
    {
        coeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(currentSampleRate, safeFreq, safeQ, linearGain);
    }
    else // Bell / Peak
    {
        coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(currentSampleRate, safeFreq, safeQ, linearGain);
    }

    currentCoeffFreq = safeFreq;
    currentCoeffGainDb = gainDb;
    currentCoeffQ = safeQ;
    currentCoeffType = type;

    for (int ch = 0; ch < maxChannels; ++ch)
    {
        if (coeffs)
            *filters[ch].coefficients = *coeffs;
    }
}

void EQBand::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = std::min(buffer.getNumChannels(), maxChannels);
    const int numSamples  = buffer.getNumSamples();

    if (numChannels == 0 || numSamples == 0)
        return;

    const bool isFading = bandFade.isSmoothing();
    const bool isAudiblyActive = bandEnabled || isFading || (bandFade.getTargetValue() > 0.0f);

    if (!isAudiblyActive)
        return;

    // Fast-path: if gain is 0 dB and not smoothing, band is flat (unity)
    if (!isFading && bandEnabled && targetGainDb == 0.0f && !smoothedGainDb.isSmoothing())
        return;

    constexpr int subBlockSize = 32;

    for (int startSample = 0; startSample < numSamples; startSample += subBlockSize)
    {
        const int samplesThisBlock = std::min(subBlockSize, numSamples - startSample);

        float nextFreq   = smoothedFreq.getNextValue();
        float nextGainDb = smoothedGainDb.getNextValue();
        float nextQ      = smoothedQ.getNextValue();

        // Update coefficients if parameters shifted
        if (std::abs(nextFreq - currentCoeffFreq) > 0.1f ||
            std::abs(nextGainDb - currentCoeffGainDb) > 0.05f ||
            std::abs(nextQ - currentCoeffQ) > 0.02f ||
            filterType != currentCoeffType)
        {
            updateCoefficients(nextFreq, nextGainDb, nextQ, filterType);
        }

        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* channelData = buffer.getWritePointer(ch);

            for (int i = 0; i < samplesThisBlock; ++i)
            {
                const int sampleIdx = startSample + i;
                const float dry = channelData[sampleIdx];
                const float filtered = filters[ch].processSample(dry);

                if (isFading || bandFade.getTargetValue() < 1.0f)
                {
                    const float fade = bandFade.getNextValue();
                    channelData[sampleIdx] = dry + fade * (filtered - dry);
                }
                else
                {
                    channelData[sampleIdx] = filtered;
                }
            }
        }
    }
}

float EQBand::getMagnitudeForFrequency(float freqHz) const
{
    if (!bandEnabled)
        return 1.0f;

    if (coeffs && currentCoeffFreq > 0.0f)
    {
        return static_cast<float>(coeffs->getMagnitudeForFrequency(freqHz, currentSampleRate));
    }

    return 1.0f;
}

} // namespace SJAudio
