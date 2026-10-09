#include "GateExpander.h"

namespace SJAudio
{

GateExpander::GateExpander()
{
    updateBallisticCoefficients();
    updateSidechainFilters();
}

void GateExpander::prepare(double sampleRate, int maxBlockSize)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 44100.0;
    maxBlockSizeSamples = std::max(64, std::max(maxBlockSize, 4096));

    sidechainBuffer.setSize(2, maxBlockSizeSamples, false, false, true);

    bypassFade.reset(currentSampleRate, 0.015); // 15 ms clickless crossfade
    bypassFade.setCurrentAndTargetValue(enabled ? 1.0f : 0.0f);

    scHpf.reset();
    scLpf.reset();

    currentGain = 1.0f;
    holdCounter = 0;
    gainReductionDb.store(0.0f, std::memory_order_relaxed);

    updateBallisticCoefficients();
    updateSidechainFilters();
}

void GateExpander::reset()
{
    scHpf.reset();
    scLpf.reset();
    currentGain = 1.0f;
    holdCounter = 0;
    bypassFade.setCurrentAndTargetValue(enabled ? 1.0f : 0.0f);
    gainReductionDb.store(0.0f, std::memory_order_relaxed);
}

void GateExpander::setEnabled(bool newEnabled) noexcept
{
    if (enabled != newEnabled)
    {
        enabled = newEnabled;
        bypassFade.setTargetValue(enabled ? 1.0f : 0.0f);
    }
}

void GateExpander::setMode(Mode newMode) noexcept
{
    currentMode = newMode;
}

void GateExpander::setThreshold(float thDb) noexcept
{
    thresholdDb = juce::jlimit(-80.0f, 0.0f, thDb);
}

void GateExpander::setRange(float rDb) noexcept
{
    rangeDb = juce::jlimit(0.0f, 80.0f, rDb);
    cachedGainFloorLinear = std::pow(10.0f, -rangeDb / 20.0f);
}

void GateExpander::setAttack(float attMs) noexcept
{
    if (std::abs(attackMs - attMs) > 0.01f)
    {
        attackMs = juce::jlimit(0.1f, 100.0f, attMs);
        updateBallisticCoefficients();
    }
}

void GateExpander::setHold(float hMs) noexcept
{
    if (std::abs(holdMs - hMs) > 0.1f)
    {
        holdMs = juce::jlimit(0.0f, 500.0f, hMs);
        updateBallisticCoefficients();
    }
}

void GateExpander::setRelease(float relMs) noexcept
{
    if (std::abs(releaseMs - relMs) > 0.1f)
    {
        releaseMs = juce::jlimit(5.0f, 2000.0f, relMs);
        updateBallisticCoefficients();
    }
}

void GateExpander::setSidechainEnabled(bool scEnabled) noexcept
{
    sidechainEnabled = scEnabled;
}

void GateExpander::setSidechainHPF(float freqHz) noexcept
{
    if (std::abs(sidechainHpfFreq - freqHz) > 0.5f)
    {
        sidechainHpfFreq = juce::jlimit(20.0f, 2000.0f, freqHz);
        updateSidechainFilters();
    }
}

void GateExpander::setSidechainLPF(float freqHz) noexcept
{
    if (std::abs(sidechainLpfFreq - freqHz) > 1.0f)
    {
        sidechainLpfFreq = juce::jlimit(1000.0f, 20000.0f, freqHz);
        updateSidechainFilters();
    }
}

void GateExpander::setSidechainSource(SidechainSource source) noexcept
{
    sidechainSource = source;
}

void GateExpander::setSidechainListen(bool listen) noexcept
{
    sidechainListen = listen;
}

void GateExpander::updateBallisticCoefficients() noexcept
{
    const double fs = currentSampleRate > 0.0 ? currentSampleRate : 44100.0;

    // Standard 10% - 90% rise time factor (-ln(0.1) ~ 2.2)
    const float attSec = std::max(0.0001f, attackMs * 0.001f);
    alphaAttack = 1.0f - std::exp(-2.2f / static_cast<float>(attSec * fs));
    alphaAttack = juce::jlimit(0.0001f, 1.0f, alphaAttack);

    const float relSec = std::max(0.005f, releaseMs * 0.001f);
    alphaRelease = 1.0f - std::exp(-2.2f / static_cast<float>(relSec * fs));
    alphaRelease = juce::jlimit(0.00001f, 1.0f, alphaRelease);

    holdSamples = static_cast<int>(holdMs * 0.001f * fs);
}

void GateExpander::updateSidechainFilters() noexcept
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

void GateExpander::process(juce::AudioBuffer<float>& buffer) noexcept
{
    process(buffer, nullptr);
}

void GateExpander::process(juce::AudioBuffer<float>& buffer, const juce::AudioBuffer<float>* externalSidechain) noexcept
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numSamples == 0 || numChannels == 0)
        return;

    // Fast-path bypass check: if disabled and crossfade is fully 0.0
    if (!enabled && bypassFade.getCurrentValue() == 0.0f && bypassFade.getTargetValue() == 0.0f)
    {
        currentGain = 1.0f;
        gainReductionDb.store(0.0f, std::memory_order_relaxed);
        return;
    }

    bypassFade.setTargetValue(enabled ? 1.0f : 0.0f);

    // 1. Prepare detector signal in sidechainBuffer
    if (sidechainBuffer.getNumSamples() < numSamples)
        sidechainBuffer.setSize(2, numSamples, false, false, true);

    const bool useExternal = (sidechainSource == SidechainSource::External)
                          && (externalSidechain != nullptr)
                          && (externalSidechain->getNumChannels() > 0);

    if (useExternal)
    {
        const int extChannels = externalSidechain->getNumChannels();
        sidechainBuffer.copyFrom(0, 0, *externalSidechain, 0, 0, numSamples);
        if (extChannels > 1)
            sidechainBuffer.copyFrom(1, 0, *externalSidechain, 1, 0, numSamples);
        else
            sidechainBuffer.copyFrom(1, 0, *externalSidechain, 0, 0, numSamples);
    }
    else
    {
        // Graceful fallback to internal audio
        sidechainBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
        if (numChannels > 1)
            sidechainBuffer.copyFrom(1, 0, buffer, 1, 0, numSamples);
        else
            sidechainBuffer.copyFrom(1, 0, buffer, 0, 0, numSamples);
    }

    // 2. Apply optional sidechain filtering (HPF & LPF) to detector signal only
    if (sidechainEnabled)
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            float* dst = sidechainBuffer.getWritePointer(ch);
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
    if (sidechainListen && enabled)
    {
        buffer.copyFrom(0, 0, sidechainBuffer, 0, 0, numSamples);
        if (numChannels > 1)
            buffer.copyFrom(1, 0, sidechainBuffer, 1, 0, numSamples);
        gainReductionDb.store(0.0f, std::memory_order_relaxed);
        return;
    }

    const float gainFloor = cachedGainFloorLinear;
    float minGainInBlock = 1.0f;

    float* ch0 = buffer.getWritePointer(0);
    float* ch1 = (numChannels > 1) ? buffer.getWritePointer(1) : nullptr;
    const float* sc0 = sidechainBuffer.getReadPointer(0);
    const float* sc1 = sidechainBuffer.getReadPointer(1);

    for (int i = 0; i < numSamples; ++i)
    {
        // Stereo-linked peak detection with direct pointers
        const float sL = sc0[i];
        const float sR = (sc1 != nullptr) ? sc1[i] : sL;

        const float detPeak = std::max(std::abs(sL), std::abs(sR));
        const float detFloor = std::max(detPeak, 1.0e-5f); // -100 dB minimum floor
        const float detDb = 20.0f * std::log10(detFloor);

        // Compute static target gain in dB
        float targetDb = 0.0f;

        if (currentMode == Mode::Gate)
        {
            if (detDb >= thresholdDb)
            {
                targetDb = 0.0f;
            }
            else if (detDb > thresholdDb - 2.0f)
            {
                // 2.0 dB soft-knee transition
                const float t = (thresholdDb - detDb) / 2.0f;
                targetDb = -rangeDb * t;
            }
            else
            {
                targetDb = -rangeDb;
            }
        }
        else // Expander mode (Musical Downward Expander)
        {
            if (detDb >= thresholdDb)
            {
                targetDb = 0.0f;
            }
            else
            {
                // 1:2.5 downward expansion slope (1.5 dB attenuation per 1 dB below threshold)
                const float delta = thresholdDb - detDb;
                const float atten = std::min(rangeDb, 1.5f * delta);
                targetDb = -atten;
            }
        }

        // Convert target gain to linear with fast-path bounds
        float gTarget;
        if (targetDb == 0.0f)
            gTarget = 1.0f;
        else if (targetDb <= -rangeDb)
            gTarget = gainFloor;
        else
            gTarget = juce::jlimit(gainFloor, 1.0f, std::pow(10.0f, targetDb / 20.0f));

        // Ballistics state machine: Attack, Hold, Release
        if (detDb >= thresholdDb)
        {
            // Signal above threshold: reset hold timer and open gate
            holdCounter = holdSamples;
            currentGain += alphaAttack * (gTarget - currentGain);
        }
        else
        {
            if (holdCounter > 0)
            {
                --holdCounter;
                // While holding, keep gate fully open or finish opening if currently attacking
                if (currentGain < 1.0f && gTarget >= currentGain)
                    currentGain += alphaAttack * (1.0f - currentGain);
            }
            else
            {
                // Hold timer expired: release gate towards closed state
                if (gTarget < currentGain)
                    currentGain += alphaRelease * (gTarget - currentGain);
                else
                    currentGain += alphaAttack * (gTarget - currentGain);
            }
        }

        currentGain = juce::jlimit(gainFloor, 1.0f, currentGain);
        minGainInBlock = std::min(minGainInBlock, currentGain);

        // Smooth 15 ms bypass crossfade
        const float fade = bypassFade.getNextValue();

        const float in0 = ch0[i];
        ch0[i] = in0 * (1.0f - fade) + in0 * currentGain * fade;

        if (ch1 != nullptr)
        {
            const float in1 = ch1[i];
            ch1[i] = in1 * (1.0f - fade) + in1 * currentGain * fade;
        }
    }

    // Update gain-reduction meter value
    const float blockGrDb = 20.0f * std::log10(std::max(minGainInBlock, 1.0e-4f)) * bypassFade.getCurrentValue();
    gainReductionDb.store(blockGrDb, std::memory_order_relaxed);
}

} // namespace SJAudio
