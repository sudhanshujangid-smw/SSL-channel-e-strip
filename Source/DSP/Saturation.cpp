#include "Saturation.h"

namespace SJAudio
{

Saturation::Saturation()
{
    updateCoefficients();
}

void Saturation::prepare(double sampleRate, int maxBlockSize, int numChannels)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 44100.0;
    maxBlockSizeSamples = std::max(64, std::max(maxBlockSize, 4096));
    currentNumChannels = std::clamp(numChannels, 1, 2);

    // Prepare JUCE Polyphase IIR Oversampling engines (Phase 13)
    // 2x: factor = 1 (2^1 = 2x); 4x: factor = 2 (2^2 = 4x)
    // Polyphase IIR provides >90 dB stopband rejection with near-zero latency,
    // zero pre-ringing on transients, and minimum CPU overhead.
    const size_t chCount = static_cast<size_t>(currentNumChannels);

    oversampler2x = std::make_unique<juce::dsp::Oversampling<float>>(
        chCount, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true);
    oversampler2x->initProcessing(static_cast<size_t>(maxBlockSizeSamples));
    latency2x = static_cast<int>(std::round(oversampler2x->getLatencyInSamples()));

    oversampler4x = std::make_unique<juce::dsp::Oversampling<float>>(
        chCount, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true);
    oversampler4x->initProcessing(static_cast<size_t>(maxBlockSizeSamples));
    latency4x = static_cast<int>(std::round(oversampler4x->getLatencyInSamples()));

    bypassFade.reset(currentSampleRate, 0.015); // 15 ms clickless crossfade
    bypassFade.setCurrentAndTargetValue(enabled ? 1.0f : 0.0f);

    smoothedDriveGain.reset(currentSampleRate, 0.025); // 25 ms smooth drive
    smoothedAutoComp.reset(currentSampleRate, 0.025);
    smoothedCharacter.reset(currentSampleRate, 0.025);
    smoothedMix.reset(currentSampleRate, 0.025);
    smoothedOutputGain.reset(currentSampleRate, 0.025);

    // Initial drive calculation
    const float driveDb = (driveParam / 100.0f) * 36.0f;
    const float driveLinear = std::pow(10.0f, driveDb / 20.0f);
    const float comp = 1.0f / std::sqrt(1.0f + 0.35f * (driveLinear - 1.0f));

    smoothedDriveGain.setCurrentAndTargetValue(driveLinear);
    smoothedAutoComp.setCurrentAndTargetValue(comp);
    smoothedCharacter.setCurrentAndTargetValue(characterParam * 0.01f);
    smoothedMix.setCurrentAndTargetValue(mixParam * 0.01f);
    smoothedOutputGain.setCurrentAndTargetValue(std::pow(10.0f, outputDbParam / 20.0f));

    lastCharVal = -1.0f;

    reset();
    updateCoefficients();
}

void Saturation::reset()
{
    if (oversampler2x != nullptr)
        oversampler2x->reset();

    if (oversampler4x != nullptr)
        oversampler4x->reset();

    bypassFade.setCurrentAndTargetValue(enabled ? 1.0f : 0.0f);

    hfFilterState[0] = 0.0f;
    hfFilterState[1] = 0.0f;

    dcBlockerX1[0] = 0.0f;
    dcBlockerX1[1] = 0.0f;
    dcBlockerY1[0] = 0.0f;
    dcBlockerY1[1] = 0.0f;

    saturationAmount.store(0.0f, std::memory_order_relaxed);
}

void Saturation::setEnabled(bool newEnabled) noexcept
{
    if (enabled != newEnabled)
    {
        enabled = newEnabled;
        bypassFade.setTargetValue(enabled ? 1.0f : 0.0f);
    }
}

void Saturation::setDrive(float drivePercent) noexcept
{
    driveParam = juce::jlimit(0.0f, 100.0f, drivePercent);

    // Map 0% - 100% to 0 dB - +36 dB input drive
    const float driveDb = (driveParam / 100.0f) * 36.0f;
    const float driveLinear = std::pow(10.0f, driveDb / 20.0f);
    smoothedDriveGain.setTargetValue(driveLinear);

    // Perceptual loudness compensation so extreme drive reveals harmonic saturation
    // rather than blasting volume
    const float comp = 1.0f / std::sqrt(1.0f + 0.35f * (driveLinear - 1.0f));
    smoothedAutoComp.setTargetValue(comp);
}

void Saturation::setCharacter(float charPercent) noexcept
{
    characterParam = juce::jlimit(0.0f, 100.0f, charPercent);
    smoothedCharacter.setTargetValue(characterParam * 0.01f);
}

void Saturation::setMix(float mixPercent) noexcept
{
    mixParam = juce::jlimit(0.0f, 100.0f, mixPercent);
    smoothedMix.setTargetValue(mixParam * 0.01f);
}

void Saturation::setOutput(float outputDb) noexcept
{
    outputDbParam = juce::jlimit(-12.0f, 12.0f, outputDb);
    smoothedOutputGain.setTargetValue(std::pow(10.0f, outputDbParam / 20.0f));
}

void Saturation::setOversampling(int factorIndex) noexcept
{
    oversamplingFactorIndex = juce::jlimit(0, 2, factorIndex);
}

int Saturation::getOversamplingLatency() const noexcept
{
    if (oversamplingFactorIndex == 1)
        return latency2x;
    if (oversamplingFactorIndex == 2)
        return latency4x;
    return 0;
}

int Saturation::getOversamplingFactor() const noexcept
{
    if (oversamplingFactorIndex == 1)
        return 2;
    if (oversamplingFactorIndex == 2)
        return 4;
    return 1;
}

void Saturation::updateCoefficients() noexcept
{
    const double fs = currentSampleRate > 0.0 ? currentSampleRate : 44100.0;

    // 1-pole high-frequency anti-harshness filter at 19 kHz
    const float hfCutoff = 19000.0f;
    hfFilterCoeff = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * hfCutoff / static_cast<float>(fs));
    hfFilterCoeff = juce::jlimit(0.02f, 1.0f, hfFilterCoeff);

    // 1-pole DC blocking high-pass filter at 10 Hz: y[n] = x[n] - x[n-1] + R * y[n-1]
    const float dcCutoff = 10.0f;
    dcBlockerR = 1.0f - (2.0f * juce::MathConstants<float>::pi * dcCutoff / static_cast<float>(fs));
    dcBlockerR = juce::jlimit(0.990f, 0.99999f, dcBlockerR);
}

void Saturation::process(juce::AudioBuffer<float>& buffer) noexcept
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numSamples == 0 || numChannels == 0)
        return;

    // Fast-path bypass check: if disabled and crossfade is fully 0.0
    if (!enabled && bypassFade.getCurrentValue() == 0.0f && bypassFade.getTargetValue() == 0.0f)
    {
        // When oversampling is active, pass through oversampling filters to maintain
        // constant integer latency without triggering DAW PDC renegotiation glitches.
        if (oversamplingFactorIndex == 1 && oversampler2x != nullptr)
        {
            juce::dsp::AudioBlock<float> inBlock(buffer);
            auto osBlock = oversampler2x->processSamplesUp(inBlock);
            oversampler2x->processSamplesDown(inBlock);
        }
        else if (oversamplingFactorIndex == 2 && oversampler4x != nullptr)
        {
            juce::dsp::AudioBlock<float> inBlock(buffer);
            auto osBlock = oversampler4x->processSamplesUp(inBlock);
            oversampler4x->processSamplesDown(inBlock);
        }

        hfFilterState[0] = 0.0f;
        hfFilterState[1] = 0.0f;
        dcBlockerX1[0] = 0.0f;
        dcBlockerX1[1] = 0.0f;
        dcBlockerY1[0] = 0.0f;
        dcBlockerY1[1] = 0.0f;
        saturationAmount.store(0.0f, std::memory_order_relaxed);
        return;
    }

    bypassFade.setTargetValue(enabled ? 1.0f : 0.0f);

    juce::dsp::AudioBlock<float> inBlock(buffer);

    // Multirate execution: 1x (native), 2x, or 4x oversampling
    if (oversamplingFactorIndex == 1 && oversampler2x != nullptr)
    {
        auto osBlock = oversampler2x->processSamplesUp(inBlock);
        processWaveshapingBlock(osBlock, currentSampleRate * 2.0);
        oversampler2x->processSamplesDown(inBlock);
    }
    else if (oversamplingFactorIndex == 2 && oversampler4x != nullptr)
    {
        auto osBlock = oversampler4x->processSamplesUp(inBlock);
        processWaveshapingBlock(osBlock, currentSampleRate * 4.0);
        oversampler4x->processSamplesDown(inBlock);
    }
    else
    {
        processWaveshapingBlock(inBlock, currentSampleRate);
    }
}

void Saturation::processWaveshapingBlock(juce::AudioBuffer<float>& buffer) noexcept
{
    juce::dsp::AudioBlock<float> block(buffer);
    processWaveshapingBlock(block, currentSampleRate);
}

void Saturation::processWaveshapingBlock(juce::dsp::AudioBlock<float>& block, double effectiveFs) noexcept
{
    const int numSamples = static_cast<int>(block.getNumSamples());
    const int numChannels = static_cast<int>(block.getNumChannels());

    if (numSamples == 0 || numChannels == 0)
        return;

    // Adapt anti-harshness filter and DC blocker to effective oversampled sample rate
    const float fs = static_cast<float>(effectiveFs > 0.0 ? effectiveFs : currentSampleRate);
    const float dynamicHfCoeff = juce::jlimit(0.02f, 1.0f,
        1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * 19000.0f / fs));
    const float dynamicDcR = juce::jlimit(0.990f, 0.99999f,
        1.0f - (2.0f * juce::MathConstants<float>::pi * 10.0f / fs));

    float* ch0 = block.getChannelPointer(0);
    float* ch1 = (numChannels > 1) ? block.getChannelPointer(1) : nullptr;

    float maxDistortion = 0.0f;

    for (int i = 0; i < numSamples; ++i)
    {
        const float curDriveGain = smoothedDriveGain.getNextValue();
        const float curAutoComp  = smoothedAutoComp.getNextValue();
        const float curChar      = smoothedCharacter.getNextValue();
        const float curMix       = smoothedMix.getNextValue();
        const float curOutGain   = smoothedOutputGain.getNextValue();
        const float curFade      = bypassFade.getNextValue();

        // Optimized character caching: only recompute exponents when character changes
        if (std::abs(curChar - lastCharVal) > 1.0e-5f)
        {
            lastCharVal = curChar;
            const float c = juce::jlimit(0.0f, 1.0f, curChar);
            cachedAsym = (1.0f - c) * 0.32f;
            cachedP = 1.25f + 1.75f * c;
            cachedInvP = 1.0f / cachedP;
        }

        const float asym = cachedAsym;
        const float p    = cachedP;
        const float invP = cachedInvP;

        // Process Left Channel (ch0)
        {
            const float dry = ch0[i];
            float driven = juce::jlimit(-50.0f, 50.0f, dry * curDriveGain);

            hfFilterState[0] += dynamicHfCoeff * (driven - hfFilterState[0]);
            const float conditioned = hfFilterState[0];

            float shaped;
            const float absCond = std::abs(conditioned);
            if (absCond < 1.0e-6f)
            {
                shaped = conditioned;
            }
            else
            {
                const float x_a = conditioned + asym * (conditioned * conditioned) / (1.0f + absCond);
                const float abs_xa = std::abs(x_a);
                const float denom = std::pow(1.0f + std::pow(abs_xa, p), invP);
                shaped = x_a / denom;
            }

            const float dcOut = shaped - dcBlockerX1[0] + dynamicDcR * dcBlockerY1[0];
            dcBlockerX1[0] = shaped;
            dcBlockerY1[0] = dcOut;

            const float wet = dcOut * curAutoComp;
            const float blended = dry * (1.0f - curMix) + wet * curMix;
            const float trimmed = blended * curOutGain;
            ch0[i] = dry * (1.0f - curFade) + trimmed * curFade;

            maxDistortion = std::max(maxDistortion, std::abs(wet - dry));
        }

        // Process Right Channel (ch1) if stereo
        if (ch1 != nullptr)
        {
            const float dry = ch1[i];
            float driven = juce::jlimit(-50.0f, 50.0f, dry * curDriveGain);

            hfFilterState[1] += dynamicHfCoeff * (driven - hfFilterState[1]);
            const float conditioned = hfFilterState[1];

            float shaped;
            const float absCond = std::abs(conditioned);
            if (absCond < 1.0e-6f)
            {
                shaped = conditioned;
            }
            else
            {
                const float x_a = conditioned + asym * (conditioned * conditioned) / (1.0f + absCond);
                const float abs_xa = std::abs(x_a);
                const float denom = std::pow(1.0f + std::pow(abs_xa, p), invP);
                shaped = x_a / denom;
            }

            const float dcOut = shaped - dcBlockerX1[1] + dynamicDcR * dcBlockerY1[1];
            dcBlockerX1[1] = shaped;
            dcBlockerY1[1] = dcOut;

            const float wet = dcOut * curAutoComp;
            const float blended = dry * (1.0f - curMix) + wet * curMix;
            const float trimmed = blended * curOutGain;
            ch1[i] = dry * (1.0f - curFade) + trimmed * curFade;

            maxDistortion = std::max(maxDistortion, std::abs(wet - dry));
        }
    }

    // Report scaled saturation activity to GUI
    saturationAmount.store(juce::jlimit(0.0f, 1.0f, maxDistortion * bypassFade.getCurrentValue()), std::memory_order_relaxed);
}

} // namespace SJAudio
