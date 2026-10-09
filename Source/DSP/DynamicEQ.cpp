#include "DynamicEQ.h"

namespace SJAudio
{

DynamicEQ::DynamicEQ()
{
    smoothedFreq.setCurrentAndTargetValue(targetFreq);
    smoothedStaticGain.setCurrentAndTargetValue(targetGainDb);
    smoothedQ.setCurrentAndTargetValue(targetQ);
    bypassFade.setCurrentAndTargetValue(0.0f);

    updateBallistics();
    updateDetectorCoeffs();
    updateFilterCoeffs(0.0f);
}

void DynamicEQ::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;
    maxBlockSizeSamples = std::max(64, static_cast<int>(spec.maximumBlockSize));
    currentNumChannels = std::min(static_cast<int>(spec.numChannels), maxChannels);

    // 25 ms smoothing for continuous slider automation
    smoothedFreq.reset(currentSampleRate, 0.025);
    smoothedFreq.setCurrentAndTargetValue(targetFreq);

    smoothedStaticGain.reset(currentSampleRate, 0.025);
    smoothedStaticGain.setCurrentAndTargetValue(targetGainDb);

    smoothedQ.reset(currentSampleRate, 0.025);
    smoothedQ.setCurrentAndTargetValue(targetQ);

    // 15 ms clickless bypass crossfade
    bypassFade.reset(currentSampleRate, 0.015);
    bypassFade.setCurrentAndTargetValue(enabled ? 1.0f : 0.0f);

    reset();
}

void DynamicEQ::reset()
{
    curDynOffsetDb = 0.0f;
    gainReductionDb.store(0.0f, std::memory_order_relaxed);
    detectorLevelDb.store(-100.0f, std::memory_order_relaxed);

    for (int ch = 0; ch < maxChannels; ++ch)
    {
        eqFilters[ch].reset();
        detectorFilters[ch].reset();
    }

    cachedFilterFreq = -1.0f;
    cachedFilterGain = -999.0f;
    cachedFilterQ = -1.0f;

    cachedDetectorFreq = -1.0f;
    cachedDetectorQ = -1.0f;

    updateBallistics();
    updateDetectorCoeffs();
    updateFilterCoeffs(targetGainDb);
}

void DynamicEQ::setEnabled(bool newEnabled) noexcept
{
    if (enabled != newEnabled)
    {
        enabled = newEnabled;
        bypassFade.setTargetValue(enabled ? 1.0f : 0.0f);
    }
}

void DynamicEQ::setFrequency(float freqHz) noexcept
{
    targetFreq = juce::jlimit(20.0f, 20000.0f, freqHz);
    smoothedFreq.setTargetValue(targetFreq);
}

void DynamicEQ::setStaticGain(float gainDb) noexcept
{
    targetGainDb = juce::jlimit(-18.0f, 18.0f, gainDb);
    smoothedStaticGain.setTargetValue(targetGainDb);
}

void DynamicEQ::setQ(float q) noexcept
{
    targetQ = juce::jlimit(0.1f, 10.0f, q);
    smoothedQ.setTargetValue(targetQ);
}

void DynamicEQ::setThreshold(float thDb) noexcept
{
    thresholdDb = juce::jlimit(-60.0f, 0.0f, thDb);
}

void DynamicEQ::setRange(float rDb) noexcept
{
    rangeDb = juce::jlimit(-24.0f, 24.0f, rDb);
}

void DynamicEQ::setAttack(float attMs) noexcept
{
    if (std::abs(attackMs - attMs) > 0.01f)
    {
        attackMs = juce::jlimit(0.1f, 100.0f, attMs);
        updateBallistics();
    }
}

void DynamicEQ::setRelease(float relMs) noexcept
{
    if (std::abs(releaseMs - relMs) > 0.1f)
    {
        releaseMs = juce::jlimit(5.0f, 2000.0f, relMs);
        updateBallistics();
    }
}

void DynamicEQ::setMode(DynamicMode mode) noexcept
{
    currentMode = mode;
}

void DynamicEQ::setListen(bool listen) noexcept
{
    listenEnabled = listen;
}

void DynamicEQ::updateBallistics() noexcept
{
    const double fs = currentSampleRate > 0.0 ? currentSampleRate : 44100.0;

    // Log-domain 10% - 90% rise time factor (-ln(0.1) ~ 2.2)
    const float attSec = std::max(0.0001f, attackMs * 0.001f);
    alphaAttack = 1.0f - std::exp(-2.2f / static_cast<float>(attSec * fs));
    alphaAttack = juce::jlimit(0.0001f, 1.0f, alphaAttack);

    const float relSec = std::max(0.005f, releaseMs * 0.001f);
    alphaRelease = 1.0f - std::exp(-2.2f / static_cast<float>(relSec * fs));
    alphaRelease = juce::jlimit(0.00001f, 1.0f, alphaRelease);
}

void DynamicEQ::updateDetectorCoeffs() noexcept
{
    const double fs = currentSampleRate > 0.0 ? currentSampleRate : 44100.0;
    const float nyquist = static_cast<float>(0.49 * fs);

    const float f = juce::jlimit(20.0f, nyquist, targetFreq);
    const float q = juce::jlimit(0.1f, 10.0f, targetQ);

    if (std::abs(f - cachedDetectorFreq) < 0.1f && std::abs(q - cachedDetectorQ) < 0.01f)
        return;

    // Audio EQ Cookbook 2nd-order Bandpass Filter (constant 0 dB peak gain)
    const float w0 = 2.0f * juce::MathConstants<float>::pi * f / static_cast<float>(fs);
    const float sinW = std::sin(w0);
    const float cosW = std::cos(w0);
    const float alpha = sinW / (2.0f * q);
    const float a0 = 1.0f + alpha;

    const float b0 = alpha / a0;
    const float b1 = 0.0f;
    const float b2 = -alpha / a0;
    const float a1 = (-2.0f * cosW) / a0;
    const float a2 = (1.0f - alpha) / a0;

    for (int ch = 0; ch < maxChannels; ++ch)
    {
        detectorFilters[ch].b0 = b0;
        detectorFilters[ch].b1 = b1;
        detectorFilters[ch].b2 = b2;
        detectorFilters[ch].a1 = a1;
        detectorFilters[ch].a2 = a2;
    }

    cachedDetectorFreq = f;
    cachedDetectorQ = q;
}

void DynamicEQ::updateFilterCoeffs(float effectiveGainDb) noexcept
{
    const double fs = currentSampleRate > 0.0 ? currentSampleRate : 44100.0;
    const float nyquist = static_cast<float>(0.49 * fs);

    const float f = juce::jlimit(20.0f, nyquist, targetFreq);
    const float q = juce::jlimit(0.1f, 10.0f, targetQ);

    if (std::abs(f - cachedFilterFreq) < 0.1f &&
        std::abs(q - cachedFilterQ) < 0.01f &&
        std::abs(effectiveGainDb - cachedFilterGain) < 0.02f)
    {
        return;
    }

    // Audio EQ Cookbook 2nd-order Peaking (Bell) Filter
    const float A = std::pow(10.0f, effectiveGainDb / 40.0f);
    const float w0 = 2.0f * juce::MathConstants<float>::pi * f / static_cast<float>(fs);
    const float sinW = std::sin(w0);
    const float cosW = std::cos(w0);
    const float alpha = sinW / (2.0f * q);
    const float a0 = 1.0f + alpha / A;

    const float b0 = (1.0f + alpha * A) / a0;
    const float b1 = (-2.0f * cosW) / a0;
    const float b2 = (1.0f - alpha * A) / a0;
    const float a1 = (-2.0f * cosW) / a0;
    const float a2 = (1.0f - alpha / A) / a0;

    for (int ch = 0; ch < maxChannels; ++ch)
    {
        eqFilters[ch].b0 = b0;
        eqFilters[ch].b1 = b1;
        eqFilters[ch].b2 = b2;
        eqFilters[ch].a1 = a1;
        eqFilters[ch].a2 = a2;
    }

    cachedFilterFreq = f;
    cachedFilterQ = q;
    cachedFilterGain = effectiveGainDb;
}

void DynamicEQ::process(juce::AudioBuffer<float>& buffer) noexcept
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = std::min(buffer.getNumChannels(), maxChannels);

    if (numSamples == 0 || numChannels == 0)
        return;

    // Fast-path bypass check: if disabled and crossfade is fully 0.0
    if (!enabled && bypassFade.getCurrentValue() == 0.0f && bypassFade.getTargetValue() == 0.0f)
    {
        curDynOffsetDb = 0.0f;
        gainReductionDb.store(0.0f, std::memory_order_relaxed);
        detectorLevelDb.store(-100.0f, std::memory_order_relaxed);
        return;
    }

    bypassFade.setTargetValue(enabled ? 1.0f : 0.0f);
    updateDetectorCoeffs();

    float* ch0 = buffer.getWritePointer(0);
    float* ch1 = (numChannels > 1) ? buffer.getWritePointer(1) : nullptr;

    float minDynOffsetInBlock = 0.0f;
    float maxDetDbInBlock = -100.0f;

    // Process in sub-blocks of 16 samples for smooth, zipper-free coefficient updates
    constexpr int subBlockSize = 16;

    for (int startIdx = 0; startIdx < numSamples; startIdx += subBlockSize)
    {
        const int samplesInSubBlock = std::min(subBlockSize, numSamples - startIdx);

        // 1. Detect signal level in target frequency band across sub-block
        float subBlockPeak = 0.0f;

        for (int i = 0; i < samplesInSubBlock; ++i)
        {
            const int idx = startIdx + i;
            const float inL = ch0[idx];
            const float inR = (ch1 != nullptr) ? ch1[idx] : inL;

            const float detL = detectorFilters[0].processSample(inL);
            const float detR = (ch1 != nullptr) ? detectorFilters[1].processSample(inR) : detL;

            const float peak = std::max(std::abs(detL), std::abs(detR));
            subBlockPeak = std::max(subBlockPeak, peak);

            // If Listen mode is active, output the bandpass detector signal
            if (listenEnabled && enabled)
            {
                ch0[idx] = detL;
                if (ch1 != nullptr) ch1[idx] = detR;
            }
        }

        // If Listen mode is soloed, skip dynamic EQ attenuation
        if (listenEnabled && enabled)
            continue;

        const float detFloor = std::max(subBlockPeak, 1.0e-5f);
        const float detDb = 20.0f * std::log10(detFloor);
        maxDetDbInBlock = std::max(maxDetDbInBlock, detDb);

        // 2. Dynamics logic: calculate target dynamic gain offset in dB
        float targetDynDb = 0.0f;

        if (detDb > thresholdDb)
        {
            const float overshootDb = detDb - thresholdDb;
            const float maxRange = std::abs(rangeDb);

            if (currentMode == DynamicMode::Compress)
            {
                // Downward dynamic compression (tames harshness / sibilance / resonances)
                targetDynDb = -std::min(maxRange, overshootDb * 1.0f);
            }
            else // Expand
            {
                // Upward dynamic expansion (enhances presence / transients)
                targetDynDb = +std::min(maxRange, overshootDb * 1.0f);
            }
        }

        // 3. Log-domain ballistics smoothing
        if ((currentMode == DynamicMode::Compress && targetDynDb < curDynOffsetDb) ||
            (currentMode == DynamicMode::Expand   && targetDynDb > curDynOffsetDb))
        {
            // Attack phase
            curDynOffsetDb += alphaAttack * (targetDynDb - curDynOffsetDb);
        }
        else
        {
            // Release phase (recovers toward 0 dB)
            curDynOffsetDb += alphaRelease * (targetDynDb - curDynOffsetDb);
        }

        curDynOffsetDb = juce::jlimit(-24.0f, 24.0f, curDynOffsetDb);
        minDynOffsetInBlock = curDynOffsetDb;

        // 4. Update peaking filter coefficients with total effective gain
        const float curStaticGain = smoothedStaticGain.getNextValue();
        const float effectiveGainDb = curStaticGain + curDynOffsetDb;
        updateFilterCoeffs(effectiveGainDb);

        // 5. Apply peaking filter to audio and blend with clickless bypass
        for (int i = 0; i < samplesInSubBlock; ++i)
        {
            const int idx = startIdx + i;
            const float inL = ch0[idx];
            const float filteredL = eqFilters[0].processSample(inL);
            const float fade = bypassFade.getNextValue();

            ch0[idx] = inL * (1.0f - fade) + filteredL * fade;

            if (ch1 != nullptr)
            {
                const float inR = ch1[idx];
                const float filteredR = eqFilters[1].processSample(inR);
                ch1[idx] = inR * (1.0f - fade) + filteredR * fade;
            }
        }
    }

    // Atomically report gain reduction and detector level for GUI meters
    gainReductionDb.store(minDynOffsetInBlock * bypassFade.getCurrentValue(), std::memory_order_relaxed);
    detectorLevelDb.store(maxDetDbInBlock, std::memory_order_relaxed);
}

float DynamicEQ::getMagnitudeForFrequency(float freqHz) const noexcept
{
    const double fs = currentSampleRate > 0.0 ? currentSampleRate : 44100.0;
    const float effectiveGainDb = targetGainDb + curDynOffsetDb;
    const float A = std::pow(10.0f, effectiveGainDb / 40.0f);
    const float w0 = 2.0f * juce::MathConstants<float>::pi * targetFreq / static_cast<float>(fs);
    const float sinW = std::sin(w0);
    const float cosW = std::cos(w0);
    const float alpha = sinW / (2.0f * targetQ);
    const float a0 = 1.0f + alpha / A;

    const float b0 = (1.0f + alpha * A) / a0;
    const float b1 = (-2.0f * cosW) / a0;
    const float b2 = (1.0f - alpha * A) / a0;
    const float a1 = (-2.0f * cosW) / a0;
    const float a2 = (1.0f - alpha / A) / a0;

    const float w = 2.0f * juce::MathConstants<float>::pi * freqHz / static_cast<float>(fs);
    const float cos1 = std::cos(w);
    const float cos2 = std::cos(2.0f * w);
    const float sin1 = std::sin(w);
    const float sin2 = std::sin(2.0f * w);

    const float numReal = b0 + b1 * cos1 + b2 * cos2;
    const float numImag = -(b1 * sin1 + b2 * sin2);
    const float denReal = 1.0f + a1 * cos1 + a2 * cos2;
    const float denImag = -(a1 * sin1 + a2 * sin2);

    const float numMagSq = numReal * numReal + numImag * numImag;
    const float denMagSq = denReal * denReal + denImag * denImag;

    if (denMagSq < 1e-12f) return 1.0f;
    return std::sqrt(numMagSq / denMagSq);
}

} // namespace SJAudio
