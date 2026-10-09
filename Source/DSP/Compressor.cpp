#include "Compressor.h"

namespace SJAudio
{

Compressor::Compressor()
{
    updateBallisticCoefficients();
    updateSidechainFilters();
}

void Compressor::prepare(double sampleRate, int maxBlockSize)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 44100.0;
    maxBlockSizeSamples = std::max(64, std::max(maxBlockSize, 4096));

    detectorBuffer.setSize(2, maxBlockSizeSamples, false, false, true);
    detectorBuffer.clear();

    scHpf.reset();
    scLpf.reset();

    bypassFade.reset(currentSampleRate, 0.015); // 15 ms clickless crossfade
    bypassFade.setCurrentAndTargetValue(enabled ? 1.0f : 0.0f);

    smoothedMakeupLinear.reset(currentSampleRate, 0.025); // 25 ms smooth makeup gain
    smoothedMakeupLinear.setCurrentAndTargetValue(std::pow(10.0f, makeupDb / 20.0f));

    smoothedMix.reset(currentSampleRate, 0.025); // 25 ms smooth parallel blend
    smoothedMix.setCurrentAndTargetValue(mixNormalized);

    envGainDb = 0.0f;
    slowEnvelopeDb = 0.0f;
    gainReductionDb.store(0.0f, std::memory_order_relaxed);

    updateBallisticCoefficients();
    updateSidechainFilters();
}

void Compressor::reset()
{
    scHpf.reset();
    scLpf.reset();
    detectorBuffer.clear();
    envGainDb = 0.0f;
    slowEnvelopeDb = 0.0f;
    bypassFade.setCurrentAndTargetValue(enabled ? 1.0f : 0.0f);
    smoothedMakeupLinear.setCurrentAndTargetValue(std::pow(10.0f, makeupDb / 20.0f));
    smoothedMix.setCurrentAndTargetValue(mixNormalized);
    gainReductionDb.store(0.0f, std::memory_order_relaxed);
}

void Compressor::setEnabled(bool newEnabled) noexcept
{
    if (enabled != newEnabled)
    {
        enabled = newEnabled;
        bypassFade.setTargetValue(enabled ? 1.0f : 0.0f);
    }
}

void Compressor::setThreshold(float thDb) noexcept
{
    thresholdDb = juce::jlimit(-60.0f, 0.0f, thDb);
}

void Compressor::setRatio(float newRatio) noexcept
{
    ratio = juce::jlimit(1.0f, 20.0f, newRatio);
}

void Compressor::setAttack(float attMs) noexcept
{
    if (std::abs(attackMs - attMs) > 0.01f)
    {
        attackMs = juce::jlimit(0.1f, 100.0f, attMs);
        updateBallisticCoefficients();
    }
}

void Compressor::setRelease(float relMs) noexcept
{
    if (std::abs(releaseMs - relMs) > 0.1f)
    {
        releaseMs = juce::jlimit(5.0f, 2000.0f, relMs);
        updateBallisticCoefficients();
    }
}

void Compressor::setKnee(float knee) noexcept
{
    kneeDb = juce::jlimit(0.0f, 24.0f, knee);
}

void Compressor::setMakeup(float mkDb) noexcept
{
    makeupDb = juce::jlimit(-12.0f, 24.0f, mkDb);
    smoothedMakeupLinear.setTargetValue(std::pow(10.0f, makeupDb / 20.0f));
}

void Compressor::setAutoRelease(bool autoRel) noexcept
{
    autoRelease = autoRel;
}

void Compressor::setMix(float mixPercent) noexcept
{
    mixNormalized = juce::jlimit(0.0f, 1.0f, mixPercent * 0.01f);
    smoothedMix.setTargetValue(mixNormalized);
}

void Compressor::setSidechainEnabled(bool scEnabled) noexcept
{
    sidechainEnabled = scEnabled;
}

void Compressor::setSidechainHPF(float freqHz) noexcept
{
    if (std::abs(sidechainHpfFreq - freqHz) > 0.5f)
    {
        sidechainHpfFreq = juce::jlimit(20.0f, 2000.0f, freqHz);
        updateSidechainFilters();
    }
}

void Compressor::setSidechainLPF(float freqHz) noexcept
{
    if (std::abs(sidechainLpfFreq - freqHz) > 1.0f)
    {
        sidechainLpfFreq = juce::jlimit(1000.0f, 20000.0f, freqHz);
        updateSidechainFilters();
    }
}

void Compressor::setSidechainSource(SidechainSource source) noexcept
{
    sidechainSource = source;
}

void Compressor::setSidechainListen(bool listen) noexcept
{
    sidechainListen = listen;
}

void Compressor::updateBallisticCoefficients() noexcept
{
    const double fs = currentSampleRate > 0.0 ? currentSampleRate : 44100.0;

    // Standard log-domain 10% - 90% rise time factor (-ln(0.1) ~ 2.2)
    const float attSec = std::max(0.0001f, attackMs * 0.001f);
    alphaAttack = 1.0f - std::exp(-2.2f / static_cast<float>(attSec * fs));
    alphaAttack = juce::jlimit(0.0001f, 1.0f, alphaAttack);

    const float relSec = std::max(0.005f, releaseMs * 0.001f);
    alphaRelease = 1.0f - std::exp(-2.2f / static_cast<float>(relSec * fs));
    alphaRelease = juce::jlimit(0.00001f, 1.0f, alphaRelease);

    // Precompute 400 ms slow detector coefficient (Phase 13 optimization)
    alphaSlow = 1.0f - std::exp(-2.2f / static_cast<float>(0.40 * fs));
}

void Compressor::updateSidechainFilters() noexcept
{
    const double fs = currentSampleRate > 0.0 ? currentSampleRate : 44100.0;
    const float nyquist = static_cast<float>(0.49 * fs);

    // 2nd-order Butterworth HPF
    {
        const float f = juce::jlimit(20.0f, nyquist, sidechainHpfFreq);
        const float w0 = 2.0f * juce::MathConstants<float>::pi * f / static_cast<float>(fs);
        const float cosW = std::cos(w0);
        const float sinW = std::sin(w0);
        const float alpha = sinW / (2.0f * 0.70710678f);
        const float a0 = 1.0f + alpha;

        scHpf.b0 = ((1.0f + cosW) * 0.5f) / a0;
        scHpf.b1 = (-(1.0f + cosW)) / a0;
        scHpf.b2 = ((1.0f + cosW) * 0.5f) / a0;
        scHpf.a1 = (-2.0f * cosW) / a0;
        scHpf.a2 = (1.0f - alpha) / a0;
    }

    // 2nd-order Butterworth LPF
    {
        const float f = juce::jlimit(1000.0f, nyquist, sidechainLpfFreq);
        const float w0 = 2.0f * juce::MathConstants<float>::pi * f / static_cast<float>(fs);
        const float cosW = std::cos(w0);
        const float sinW = std::sin(w0);
        const float alpha = sinW / (2.0f * 0.70710678f);
        const float a0 = 1.0f + alpha;

        scLpf.b0 = ((1.0f - cosW) * 0.5f) / a0;
        scLpf.b1 = (1.0f - cosW) / a0;
        scLpf.b2 = ((1.0f - cosW) * 0.5f) / a0;
        scLpf.a1 = (-2.0f * cosW) / a0;
        scLpf.a2 = (1.0f - alpha) / a0;
    }
}

void Compressor::process(juce::AudioBuffer<float>& buffer) noexcept
{
    process(buffer, nullptr);
}

void Compressor::process(juce::AudioBuffer<float>& buffer, const juce::AudioBuffer<float>* externalSidechain) noexcept
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numSamples == 0 || numChannels == 0)
        return;

    // Fast-path bypass check: if disabled and crossfade is fully 0.0
    if (!enabled && bypassFade.getCurrentValue() == 0.0f && bypassFade.getTargetValue() == 0.0f)
    {
        envGainDb = 0.0f;
        gainReductionDb.store(0.0f, std::memory_order_relaxed);
        return;
    }

    bypassFade.setTargetValue(enabled ? 1.0f : 0.0f);

    // 1. Prepare detector signal in detectorBuffer
    if (detectorBuffer.getNumSamples() < numSamples)
        detectorBuffer.setSize(2, numSamples, false, false, true);

    const bool useExternal = (sidechainSource == SidechainSource::External)
                          && (externalSidechain != nullptr)
                          && (externalSidechain->getNumChannels() > 0);

    if (useExternal)
    {
        const int extChannels = externalSidechain->getNumChannels();
        detectorBuffer.copyFrom(0, 0, *externalSidechain, 0, 0, numSamples);
        if (extChannels > 1)
            detectorBuffer.copyFrom(1, 0, *externalSidechain, 1, 0, numSamples);
        else
            detectorBuffer.copyFrom(1, 0, *externalSidechain, 0, 0, numSamples);
    }
    else
    {
        // Graceful fallback to internal audio
        detectorBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
        if (numChannels > 1)
            detectorBuffer.copyFrom(1, 0, buffer, 1, 0, numSamples);
        else
            detectorBuffer.copyFrom(1, 0, buffer, 0, 0, numSamples);
    }

    // 2. Apply optional internal/external sidechain filtering (HPF & LPF)
    // Filters affect detectorBuffer ONLY; main audio is NOT modified.
    if (sidechainEnabled)
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            float* dst = detectorBuffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i)
            {
                float s = dst[i];
                s = scHpf.processSample(ch, s);
                s = scLpf.processSample(ch, s);
                dst[i] = s;
            }
        }
    }

    // 3. Sidechain Listen / Audition Mode
    // Route filtered detector audio to main output so mixer can calibrate HPF/LPF cutoffs
    if (sidechainListen && enabled)
    {
        buffer.copyFrom(0, 0, detectorBuffer, 0, 0, numSamples);
        if (numChannels > 1)
            buffer.copyFrom(1, 0, detectorBuffer, 1, 0, numSamples);
        gainReductionDb.store(0.0f, std::memory_order_relaxed);
        return;
    }

    const float slope = 1.0f - (1.0f / std::max(1.0f, ratio));
    const float halfKnee = kneeDb * 0.5f;
    const float lowerKneeDb = thresholdDb - halfKnee;
    const float linearKneeLower = std::pow(10.0f, lowerKneeDb / 20.0f);

    float* ch0 = buffer.getWritePointer(0);
    float* ch1 = (numChannels > 1) ? buffer.getWritePointer(1) : nullptr;

    const float* det0 = detectorBuffer.getReadPointer(0);
    const float* det1 = detectorBuffer.getReadPointer(1);

    float minGainInBlock = 0.0f;

    for (int i = 0; i < numSamples; ++i)
    {
        // 4. Stereo-linked peak detection on DETECTOR signal
        const float sL_det = det0[i];
        const float sR_det = det1[i];
        const float detPeak = std::max(std::abs(sL_det), std::abs(sR_det));

        // 5. Fast-path below-threshold check to skip std::log10 on non-compressing samples
        float targetReductionDb = 0.0f;

        if (detPeak > linearKneeLower)
        {
            const float detFloor = std::max(detPeak, 1.0e-5f); // -100 dB floor
            const float detDb = 20.0f * std::log10(detFloor);

            if (kneeDb > 0.05f)
            {
                if (detDb >= thresholdDb + halfKnee)
                {
                    targetReductionDb = -slope * (detDb - thresholdDb);
                }
                else
                {
                    const float delta = detDb - thresholdDb + halfKnee;
                    targetReductionDb = -slope * (delta * delta) / (2.0f * kneeDb);
                }
            }
            else
            {
                targetReductionDb = -slope * (detDb - thresholdDb);
            }
        }

        // 6. Ballistics state machine: Attack / Release / Adaptive Auto-Release
        if (targetReductionDb < envGainDb)
        {
            // Attack phase: compression increases
            envGainDb += alphaAttack * (targetReductionDb - envGainDb);
        }
        else
        {
            // Release phase: compression recovers
            float curAlphaRelease = alphaRelease;

            if (autoRelease)
            {
                slowEnvelopeDb += alphaSlow * (envGainDb - slowEnvelopeDb);
                const float crest = std::abs(envGainDb - slowEnvelopeDb);

                const float autoRelMs = juce::jmap(juce::jlimit(0.0f, 15.0f, crest),
                                                   15.0f, 0.0f,
                                                   50.0f, 750.0f);

                const double fs = currentSampleRate > 0.0 ? currentSampleRate : 44100.0;
                curAlphaRelease = 1.0f - std::exp(-2.2f / static_cast<float>(autoRelMs * 0.001f * fs));
                curAlphaRelease = juce::jlimit(0.00001f, 1.0f, curAlphaRelease);
            }

            envGainDb += curAlphaRelease * (targetReductionDb - envGainDb);
        }

        envGainDb = juce::jlimit(-80.0f, 0.0f, envGainDb);
        minGainInBlock = std::min(minGainInBlock, envGainDb);

        // 7. Linear Gain application on MAIN pristine audio
        const float compLinear = (envGainDb >= -0.005f) ? 1.0f : std::pow(10.0f, envGainDb / 20.0f);
        const float makeupLinear = smoothedMakeupLinear.getNextValue();
        const float wetMix = smoothedMix.getNextValue();
        const float dryMix = 1.0f - wetMix;
        const float fade = bypassFade.getNextValue();

        const float sL = ch0[i];
        const float wetL = sL * compLinear * makeupLinear;
        const float blendedL = sL * dryMix + wetL * wetMix;
        ch0[i] = sL * (1.0f - fade) + blendedL * fade;

        if (ch1 != nullptr)
        {
            const float sR = ch1[i];
            const float wetR = sR * compLinear * makeupLinear;
            const float blendedR = sR * dryMix + wetR * wetMix;
            ch1[i] = sR * (1.0f - fade) + blendedR * fade;
        }
    }

    // Report gain reduction (scaled by bypass fade)
    gainReductionDb.store(minGainInBlock * bypassFade.getCurrentValue(), std::memory_order_relaxed);
}

} // namespace SJAudio
