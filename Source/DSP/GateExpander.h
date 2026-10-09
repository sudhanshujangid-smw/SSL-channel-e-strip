#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <atomic>
#include <cmath>

namespace SJAudio
{

/**
 * @brief Professional, original Gate / Downward Expander DSP module.
 *
 * Features:
 * - Dual Mode: Hard/Soft Gate or Musical Downward Expander.
 * - Stereo-linked peak envelope detector.
 * - Attack (0.1 - 100 ms), Hold (0 - 500 ms), Release (5 - 2000 ms) ballistics.
 * - Variable attenuation Range (0 - 80 dB) and Threshold (-80 - 0 dB).
 * - Optional dedicated Sidechain High-Pass & Low-Pass filters (does not alter main audio).
 * - 15 ms clickless master bypass crossfade.
 * - Thread-safe atomic gain-reduction metering.
 * - Real-time safe: Zero memory allocation in audio processing loop.
 */
class GateExpander
{
public:
    enum class Mode
    {
        Gate = 0,
        Expander = 1
    };

    GateExpander();
    ~GateExpander() = default;

    void prepare(double sampleRate, int maxBlockSize);
    void reset();

    void setEnabled(bool enabled) noexcept;
    void setMode(Mode newMode) noexcept;
    void setThreshold(float thresholdDb) noexcept;
    void setRange(float rangeDb) noexcept;
    void setAttack(float attackMs) noexcept;
    void setHold(float holdMs) noexcept;
    void setRelease(float releaseMs) noexcept;

    void setSidechainEnabled(bool enabled) noexcept;
    void setSidechainHPF(float freqHz) noexcept;
    void setSidechainLPF(float freqHz) noexcept;

    enum class SidechainSource
    {
        Internal = 0,
        External = 1
    };

    void setSidechainSource(SidechainSource source) noexcept;
    void setSidechainListen(bool listen) noexcept;

    void process(juce::AudioBuffer<float>& buffer) noexcept;
    void process(juce::AudioBuffer<float>& buffer, const juce::AudioBuffer<float>* externalSidechain) noexcept;

    /**
     * @brief Returns current gain reduction in dB (e.g. 0.0 dB = unity / no reduction,
     * -18.0 dB = 18 dB attenuation).
     */
    float getGainReductionDb() const noexcept
    {
        return gainReductionDb.load(std::memory_order_relaxed);
    }

    bool isEnabled() const noexcept { return enabled; }

private:
    void updateBallisticCoefficients() noexcept;
    void updateSidechainFilters() noexcept;

    // Filter biquad state for sidechain
    struct Biquad
    {
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;
        float z1_L = 0.0f, z2_L = 0.0f;
        float z1_R = 0.0f, z2_R = 0.0f;

        void reset() noexcept
        {
            z1_L = z2_L = z1_R = z2_R = 0.0f;
        }

        inline float processSample(int channel, float in) noexcept
        {
            float& z1 = (channel == 0) ? z1_L : z1_R;
            float& z2 = (channel == 0) ? z2_L : z2_R;
            float out = b0 * in + z1;
            z1 = b1 * in - a1 * out + z2;
            z2 = b2 * in - a2 * out;
            return out;
        }
    };

    double currentSampleRate = 44100.0;
    int maxBlockSizeSamples = 512;

    bool enabled = false;
    Mode currentMode = Mode::Gate;

    float thresholdDb = -40.0f;
    float rangeDb = 20.0f;
    float cachedGainFloorLinear = 0.1f;
    float attackMs = 5.0f;
    float holdMs = 50.0f;
    float releaseMs = 100.0f;

    bool sidechainEnabled = false;
    float sidechainHpfFreq = 20.0f;
    float sidechainLpfFreq = 20000.0f;
    SidechainSource sidechainSource = SidechainSource::Internal;
    bool sidechainListen = false;

    // Ballistics
    float alphaAttack = 0.01f;
    float alphaRelease = 0.001f;
    int holdSamples = 0;
    int holdCounter = 0;
    float currentGain = 1.0f; // Linear envelope gain [10^(-range/20), 1.0]

    // Bypass crossfade (15 ms)
    juce::SmoothedValue<float> bypassFade;

    // Sidechain Biquads
    Biquad scHpf;
    Biquad scLpf;

    // Sidechain working buffer (preallocated in prepare)
    juce::AudioBuffer<float> sidechainBuffer;

    // Metering
    std::atomic<float> gainReductionDb { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GateExpander)
};

} // namespace SJAudio
