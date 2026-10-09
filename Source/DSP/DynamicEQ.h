#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <atomic>
#include <cmath>
#include <algorithm>

namespace SJAudio
{

/**
 * @brief Professional, real-time-safe Dynamic Equalizer module.
 *
 * Designed specifically for precision tone shaping, vocal de-essing,
 * harshness suppression, and dynamic resonance control.
 *
 * Features:
 * - Parametric Bell / Peak filter with dynamically modulated gain.
 * - Dedicated 2nd-order bandpass detector filter tracking target frequency and Q.
 * - Configurable Threshold (-60 to 0 dB) and Dynamic Range (-24 to +24 dB).
 * - Log-domain Attack (0.1 to 100 ms) and Release (5 to 2000 ms) ballistics.
 * - Dual Dynamic Mode:
 *     * Compress (Downward): Attenuates target band when signal exceeds threshold.
 *     * Expand (Upward): Boosts target band when signal exceeds threshold.
 * - Audition Mode ("Listen"): Solos the sidechain bandpass detector for fast acoustic tuning.
 * - 15 ms clickless bypass crossfading.
 * - Sub-block (16 samples) coefficient smoothing eliminating all zipper noise.
 * - Thread-safe atomic reporting of dynamic gain reduction and detector level for metering.
 * - Real-time safe: Zero heap memory allocations in audio thread.
 */
class DynamicEQ
{
public:
    enum class DynamicMode
    {
        Compress = 0,
        Expand   = 1
    };

    DynamicEQ();
    ~DynamicEQ() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void setEnabled(bool enabled) noexcept;
    void setFrequency(float freqHz) noexcept;
    void setStaticGain(float gainDb) noexcept;
    void setQ(float q) noexcept;
    void setThreshold(float thDb) noexcept;
    void setRange(float rangeDb) noexcept;
    void setAttack(float attMs) noexcept;
    void setRelease(float relMs) noexcept;
    void setMode(DynamicMode mode) noexcept;
    void setListen(bool listen) noexcept;

    void process(juce::AudioBuffer<float>& buffer) noexcept;

    /** Exact mathematical linear magnitude response at frequency */
    float getMagnitudeForFrequency(float freqHz) const noexcept;

    // Real-time thread-safe metering readouts
    float getDynamicGainReductionDb() const noexcept
    {
        return gainReductionDb.load(std::memory_order_relaxed);
    }

    float getDetectorLevelDb() const noexcept
    {
        return detectorLevelDb.load(std::memory_order_relaxed);
    }

    bool isEnabled() const noexcept        { return enabled; }
    float getTargetFreq() const noexcept    { return targetFreq; }
    float getTargetGainDb() const noexcept  { return targetGainDb; }
    float getTargetQ() const noexcept       { return targetQ; }
    float getThresholdDb() const noexcept   { return thresholdDb; }
    float getRangeDb() const noexcept       { return rangeDb; }
    float getAttackMs() const noexcept      { return attackMs; }
    float getReleaseMs() const noexcept     { return releaseMs; }
    DynamicMode getMode() const noexcept    { return currentMode; }
    bool isListening() const noexcept       { return listenEnabled; }

private:
    void updateBallistics() noexcept;
    void updateDetectorCoeffs() noexcept;
    void updateFilterCoeffs(float effectiveGainDb) noexcept;

    // Transposed Direct Form II Biquad filter
    struct Biquad
    {
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;
        float z1 = 0.0f, z2 = 0.0f;

        void reset() noexcept
        {
            z1 = z2 = 0.0f;
        }

        inline float processSample(float in) noexcept
        {
            const float out = b0 * in + z1;
            z1 = b1 * in - a1 * out + z2;
            z2 = b2 * in - a2 * out;
            return out;
        }
    };

    double currentSampleRate = 44100.0;
    int maxBlockSizeSamples = 512;
    int currentNumChannels = 2;

    bool enabled = false;
    float targetFreq = 3500.0f;
    float targetGainDb = 0.0f;
    float targetQ = 1.5f;
    float thresholdDb = -18.0f;
    float rangeDb = -6.0f;
    float attackMs = 10.0f;
    float releaseMs = 100.0f;
    DynamicMode currentMode = DynamicMode::Compress;
    bool listenEnabled = false;

    // Ballistics
    float alphaAttack = 0.01f;
    float alphaRelease = 0.001f;
    float curDynOffsetDb = 0.0f;

    // Audio path peaking filters (per channel)
    static constexpr int maxChannels = 2;
    Biquad eqFilters[maxChannels];

    // Sidechain bandpass detector filters (per channel)
    Biquad detectorFilters[maxChannels];

    // Parameter smoothing for clickless static controls
    juce::SmoothedValue<float> smoothedFreq;
    juce::SmoothedValue<float> smoothedStaticGain;
    juce::SmoothedValue<float> smoothedQ;
    juce::SmoothedValue<float> bypassFade;

    // Cached coefficient state to skip redundant calculations
    float cachedFilterFreq = -1.0f;
    float cachedFilterGain = -999.0f;
    float cachedFilterQ = -1.0f;

    float cachedDetectorFreq = -1.0f;
    float cachedDetectorQ = -1.0f;

    // Thread-safe meters
    std::atomic<float> gainReductionDb { 0.0f };
    std::atomic<float> detectorLevelDb { -100.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DynamicEQ)
};

} // namespace SJAudio
