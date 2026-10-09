#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <atomic>
#include <cmath>

namespace SJAudio
{

/**
 * @brief Original Nonlinear Saturation / Analog Color module.
 *
 * Features:
 * - Original continuous-morphing waveshaping core:
 *     * Low Character (0%): Asymmetric soft-knee saturation rich in even harmonics (tube / transformer warmth).
 *     * Mid Character (50%): Symmetric hyperbolic saturation rich in odd harmonics (tape / console bus vibe).
 *     * High Character (100%): Firm, punchy console overdrive with sharp transient bite and harmonic presence.
 * - Smooth continuous Drive control (0% to 100%) with internal loudness compensation.
 * - Anti-harshness analog high-frequency conditioning to eliminate digital brittle fizziness.
 * - Sample-exact DC-blocking high-pass filter (eliminates DC offset introduced by asymmetric waveshaping).
 * - Phase-coherent Parallel Dry/Wet Mix (0% to 100%).
 * - Linear smoothed Output Trim (-12 dB to +12 dB).
 * - 15 ms clickless master bypass crossfade.
 * - Thread-safe atomic saturation activity reporting for GUI visualization.
 * - Decoupled block-processing architecture ready for future oversampling integration.
 * - Real-time safe: Zero memory allocation in audio processing loop.
 */
class Saturation
{
public:
    Saturation();
    ~Saturation() = default;

    void prepare(double sampleRate, int maxBlockSize, int numChannels = 2);
    void reset();

    void setEnabled(bool enabled) noexcept;
    void setDrive(float drivePercent) noexcept;
    void setCharacter(float charPercent) noexcept;
    void setMix(float mixPercent) noexcept;
    void setOutput(float outputDb) noexcept;

    void setOversampling(int factorIndex) noexcept;
    int getOversamplingLatency() const noexcept;
    int getOversamplingFactor() const noexcept;

    void process(juce::AudioBuffer<float>& buffer) noexcept;

    /**
     * @brief Internal core waveshaping block processing on an AudioBlock at effective sample rate.
     */
    void processWaveshapingBlock(juce::dsp::AudioBlock<float>& block, double effectiveFs) noexcept;
    void processWaveshapingBlock(juce::AudioBuffer<float>& buffer) noexcept;

    /**
     * @brief Returns current saturation activity (0.0 to 1.0) for GUI visualization.
     */
    float getSaturationAmount() const noexcept
    {
        return saturationAmount.load(std::memory_order_relaxed);
    }

    bool isEnabled() const noexcept { return enabled; }

    // Read access for UI transfer curve visualization
    float getDrivePercent() const noexcept { return driveParam; }
    float getCharacterPercent() const noexcept { return characterParam; }

private:
    void updateCoefficients() noexcept;

    double currentSampleRate = 44100.0;
    int maxBlockSizeSamples = 512;
    int currentNumChannels = 2;

    bool enabled = false;
    float driveParam = 20.0f;       // 0% to 100%
    float characterParam = 50.0f;   // 0% to 100%
    float mixParam = 100.0f;        // 0% to 100%
    float outputDbParam = 0.0f;     // -12 dB to +12 dB

    // Oversampling engines (Phase 13)
    int oversamplingFactorIndex = 0; // 0 = 1x, 1 = 2x, 2 = 4x
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler2x;
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler4x;
    int latency2x = 0;
    int latency4x = 0;

    // Parameter smoothing for clickless automation
    juce::SmoothedValue<float> smoothedDriveGain;
    juce::SmoothedValue<float> smoothedAutoComp;
    juce::SmoothedValue<float> smoothedCharacter;
    juce::SmoothedValue<float> smoothedMix;
    juce::SmoothedValue<float> smoothedOutputGain;

    // 15 ms clickless bypass crossfade
    juce::SmoothedValue<float> bypassFade;

    // High-frequency anti-harshness 1-pole filter state (L & R)
    float hfFilterState[2] = { 0.0f, 0.0f };
    float hfFilterCoeff = 0.85f;

    // DC blocker states (L & R): y[n] = x[n] - x[n-1] + R * y[n-1]
    float dcBlockerX1[2] = { 0.0f, 0.0f };
    float dcBlockerY1[2] = { 0.0f, 0.0f };
    float dcBlockerR = 0.9985f;

    // Cached character waveshaping parameters to avoid recalculating per sample
    float lastCharVal = -1.0f;
    float cachedAsym = 0.16f;
    float cachedP = 2.125f;
    float cachedInvP = 1.0f / 2.125f;

    // Thread-safe saturation activity indicator
    std::atomic<float> saturationAmount { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Saturation)
};

} // namespace SJAudio
