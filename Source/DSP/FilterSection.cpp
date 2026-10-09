#include "FilterSection.h"
#include <algorithm>

namespace SJAudio
{

FilterSection::FilterSection()
{
    smoothedHpFreq.setCurrentAndTargetValue(20.0f);
    smoothedLpFreq.setCurrentAndTargetValue(20000.0f);
    hpfFade.setCurrentAndTargetValue(0.0f);
    lpfFade.setCurrentAndTargetValue(0.0f);
}

void FilterSection::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;
    currentNumChannels = std::min((int)spec.numChannels, maxChannels);

    // 25 ms smoothing for cutoff frequency sweeps to avoid zipper noise
    smoothedHpFreq.reset(currentSampleRate, 0.025);
    smoothedLpFreq.reset(currentSampleRate, 0.025);

    // 15 ms smooth crossfade for clickless filter bypass/engage
    hpfFade.reset(currentSampleRate, 0.015);
    lpfFade.reset(currentSampleRate, 0.015);

    reset();
}

void FilterSection::reset()
{
    currentHpfCoeffFreq = -1.0f;
    currentLpfCoeffFreq = -1.0f;

    for (int ch = 0; ch < maxChannels; ++ch)
    {
        for (int st = 0; st < maxStages; ++st)
        {
            hpfFilters[ch][st].reset();
            lpfFilters[ch][st].reset();
        }
    }

    updateHpfCoefficients(smoothedHpFreq.getTargetValue(), hpfSlope);
    updateLpfCoefficients(smoothedLpFreq.getTargetValue(), lpfSlope);
}

void FilterSection::setHighPass(bool enabled, float freqHz, int slopeIndex)
{
    hpfEnabled = enabled;
    hpfSlope = static_cast<Slope>(std::clamp(slopeIndex, 0, 2));

    hpfFade.setTargetValue(enabled ? 1.0f : 0.0f);
    smoothedHpFreq.setTargetValue(std::clamp(freqHz, 20.0f, 1000.0f));
}

void FilterSection::setLowPass(bool enabled, float freqHz, int slopeIndex)
{
    lpfEnabled = enabled;
    lpfSlope = static_cast<Slope>(std::clamp(slopeIndex, 0, 2));

    lpfFade.setTargetValue(enabled ? 1.0f : 0.0f);
    smoothedLpFreq.setTargetValue(std::clamp(freqHz, 1000.0f, 20000.0f));
}

void FilterSection::updateHpfCoefficients(float freq, Slope slope)
{
    const float safeFreq = std::clamp(freq, 20.0f, static_cast<float>(currentSampleRate * 0.49));

    if (slope == Slope::Slope12Db)
    {
        // 2nd order Butterworth (12 dB/oct)
        hpfCoeffs[0] = juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, safeFreq, 0.70710678f);
        hpfCoeffs[1] = nullptr;
    }
    else if (slope == Slope::Slope18Db)
    {
        // 3rd order Butterworth (18 dB/oct) = 1st-order + 2nd-order with Q = 1.0
        hpfCoeffs[0] = juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(currentSampleRate, safeFreq);
        hpfCoeffs[1] = juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, safeFreq, 1.0f);
    }
    else // Slope24Db
    {
        // 4th order Butterworth (24 dB/oct) = two cascaded 2nd-order with Q1=0.5412, Q2=1.3066
        hpfCoeffs[0] = juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, safeFreq, 0.5411961f);
        hpfCoeffs[1] = juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, safeFreq, 1.306563f);
    }

    currentHpfCoeffFreq = safeFreq;
    currentHpfCoeffSlope = slope;

    for (int ch = 0; ch < maxChannels; ++ch)
    {
        if (hpfCoeffs[0]) *hpfFilters[ch][0].coefficients = *hpfCoeffs[0];
        if (hpfCoeffs[1]) *hpfFilters[ch][1].coefficients = *hpfCoeffs[1];
    }
}

void FilterSection::updateLpfCoefficients(float freq, Slope slope)
{
    const float safeFreq = std::clamp(freq, 1000.0f, static_cast<float>(currentSampleRate * 0.49));

    if (slope == Slope::Slope12Db)
    {
        // 2nd order Butterworth (12 dB/oct)
        lpfCoeffs[0] = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, safeFreq, 0.70710678f);
        lpfCoeffs[1] = nullptr;
    }
    else if (slope == Slope::Slope18Db)
    {
        // 3rd order Butterworth (18 dB/oct) = 1st-order + 2nd-order with Q = 1.0
        lpfCoeffs[0] = juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(currentSampleRate, safeFreq);
        lpfCoeffs[1] = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, safeFreq, 1.0f);
    }
    else // Slope24Db
    {
        // 4th order Butterworth (24 dB/oct)
        lpfCoeffs[0] = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, safeFreq, 0.5411961f);
        lpfCoeffs[1] = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, safeFreq, 1.306563f);
    }

    currentLpfCoeffFreq = safeFreq;
    currentLpfCoeffSlope = slope;

    for (int ch = 0; ch < maxChannels; ++ch)
    {
        if (lpfCoeffs[0]) *lpfFilters[ch][0].coefficients = *lpfCoeffs[0];
        if (lpfCoeffs[1]) *lpfFilters[ch][1].coefficients = *lpfCoeffs[1];
    }
}

void FilterSection::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = std::min(buffer.getNumChannels(), maxChannels);
    const int numSamples  = buffer.getNumSamples();

    if (numChannels == 0 || numSamples == 0)
        return;

    // Fast-path bypass if both filters are fully disabled and not crossfading
    const bool hpfActive = hpfEnabled || hpfFade.isSmoothing() || (hpfFade.getTargetValue() > 0.0f);
    const bool lpfActive = lpfEnabled || lpfFade.isSmoothing() || (lpfFade.getTargetValue() > 0.0f);

    if (!hpfActive && !lpfActive)
        return;

    // Process in sub-blocks of up to 32 samples to smooth filter coefficients without excessive trig evaluations
    constexpr int subBlockSize = 32;

    for (int startSample = 0; startSample < numSamples; startSample += subBlockSize)
    {
        const int samplesThisBlock = std::min(subBlockSize, numSamples - startSample);

        // Advance smoothed frequency and update coefficients if needed
        float nextHpFreq = smoothedHpFreq.getNextValue();
        float nextLpFreq = smoothedLpFreq.getNextValue();

        if (hpfActive)
        {
            if (std::abs(nextHpFreq - currentHpfCoeffFreq) > 0.1f || hpfSlope != currentHpfCoeffSlope)
                updateHpfCoefficients(nextHpFreq, hpfSlope);
        }

        if (lpfActive)
        {
            if (std::abs(nextLpFreq - currentLpfCoeffFreq) > 0.5f || lpfSlope != currentLpfCoeffSlope)
                updateLpfCoefficients(nextLpFreq, lpfSlope);
        }

        // Process each channel
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* channelData = buffer.getWritePointer(ch);

            for (int i = 0; i < samplesThisBlock; ++i)
            {
                const int sampleIdx = startSample + i;
                const float dry = channelData[sampleIdx];
                float sample = dry;

                // 1. Process High Pass Filter
                if (hpfActive)
                {
                    float hpSample = sample;
                    hpSample = hpfFilters[ch][0].processSample(hpSample);
                    if (hpfCoeffs[1])
                        hpSample = hpfFilters[ch][1].processSample(hpSample);

                    const float hpFadeVal = hpfFade.getNextValue();
                    sample = dry + hpFadeVal * (hpSample - dry);
                }

                // 2. Process Low Pass Filter
                if (lpfActive)
                {
                    const float preLpSample = sample;
                    float lpSample = preLpSample;
                    lpSample = lpfFilters[ch][0].processSample(lpSample);
                    if (lpfCoeffs[1])
                        lpSample = lpfFilters[ch][1].processSample(lpSample);

                    const float lpFadeVal = lpfFade.getNextValue();
                    sample = preLpSample + lpFadeVal * (lpSample - preLpSample);
                }

                channelData[sampleIdx] = sample;
            }
        }
    }
}

float FilterSection::getMagnitudeForFrequency(float freqHz) const
{
    float totalMag = 1.0f;

    if (hpfEnabled && currentHpfCoeffFreq > 0.0f)
    {
        if (hpfCoeffs[0])
            totalMag *= static_cast<float>(hpfCoeffs[0]->getMagnitudeForFrequency(freqHz, currentSampleRate));
        if (hpfCoeffs[1])
            totalMag *= static_cast<float>(hpfCoeffs[1]->getMagnitudeForFrequency(freqHz, currentSampleRate));
    }

    if (lpfEnabled && currentLpfCoeffFreq > 0.0f)
    {
        if (lpfCoeffs[0])
            totalMag *= static_cast<float>(lpfCoeffs[0]->getMagnitudeForFrequency(freqHz, currentSampleRate));
        if (lpfCoeffs[1])
            totalMag *= static_cast<float>(lpfCoeffs[1]->getMagnitudeForFrequency(freqHz, currentSampleRate));
    }

    return totalMag;
}

float FilterSection::getHighPassMagnitudeForFrequency(float freqHz) const
{
    if (!hpfEnabled || currentHpfCoeffFreq <= 0.0f)
        return 1.0f;

    float mag = 1.0f;
    if (hpfCoeffs[0])
        mag *= static_cast<float>(hpfCoeffs[0]->getMagnitudeForFrequency(freqHz, currentSampleRate));
    if (hpfCoeffs[1])
        mag *= static_cast<float>(hpfCoeffs[1]->getMagnitudeForFrequency(freqHz, currentSampleRate));
    return mag;
}

float FilterSection::getLowPassMagnitudeForFrequency(float freqHz) const
{
    if (!lpfEnabled || currentLpfCoeffFreq <= 0.0f)
        return 1.0f;

    float mag = 1.0f;
    if (lpfCoeffs[0])
        mag *= static_cast<float>(lpfCoeffs[0]->getMagnitudeForFrequency(freqHz, currentSampleRate));
    if (lpfCoeffs[1])
        mag *= static_cast<float>(lpfCoeffs[1]->getMagnitudeForFrequency(freqHz, currentSampleRate));
    return mag;
}

} // namespace SJAudio
