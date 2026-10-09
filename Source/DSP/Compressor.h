#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <atomic>
#include <cmath>

namespace SJAudio
{

/**
 * @brief Professional VCA-style Compressor module.
 *
 * Features:
 * - Stereo-linked peak detection with -100 dB anti-denormal noise floor.
 * - Continuous quadratic soft knee (0 - 24 dB).
 * - Variable ratio (1:1 to 20:1) and threshold (-60 to 0 dB).
 * - Attack (0.1 - 100 ms) and Release (5 - 2000 ms) log-domain ballistics.
 * - Program-dependent adaptive Auto-Release mode.
 * - Phase-coherent Parallel Dry/Wet Mix (0% - 100%).
 * - Linear smoothed Makeup Gain (-12 to +24 dB).
 * - 15 ms clickless master bypass crossfade.
 * - Thread-safe atomic gain-reduction reporting for GUI metering.
 * - Real-time safe: Zero allocations in processBlock().
 */
class Compressor
{
public:
    Compressor();
    ~Compressor() = default;

    void prepare(double sampleRate, int maxBlockSize);
    void reset();

    void setEnabled(bool enabled) noexcept;
    void setThreshold(float thresholdDb) noexcept;
    void setRatio(float ratio) noexcept;
    void setAttack(float attackMs) noexcept;
    void setRelease(float releaseMs) noexcept;
    void setKnee(float kneeDb) noexcept;
    void setMakeup(float makeupDb) noexcept;
    void setAutoRelease(bool autoRel) noexcept;
    void setMix(float mixPercent) noexcept;

    enum class SidechainSource
    {
        Internal = 0,
        External = 1
    };

    void setSidechainEnabled(bool scEnabled) noexcept;
    void setSidechainHPF(float freqHz) noexcept;
    void setSidechainLPF(float freqHz) noexcept;
    void setSidechainSource(SidechainSource source) noexcept;
    void setSidechainListen(bool listen) noexcept;

    void process(juce::AudioBuffer<float>& buffer) noexcept;
    void process(juce::AudioBuffer<float>& buffer, const juce::AudioBuffer<float>* externalSidechain) noexcept;

    /**
     * @brief Returns the maximum gain reduction occurring in the current block in dB
     * (e.g. 0.0 dB = unity, -12.0 dB = 12 dB reduction).
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
    float thresholdDb = -18.0f;
    float ratio = 4.0f;
    float attackMs = 10.0f;
    float releaseMs = 100.0f;
    float kneeDb = 6.0f;
    float makeupDb = 0.0f;
    bool autoRelease = false;
    float mixNormalized = 1.0f; // 0.0 to 1.0

    // Sidechain (Phase 14)
    bool sidechainEnabled = false;
    float sidechainHpfFreq = 80.0f;
    float sidechainLpfFreq = 20000.0f;
    SidechainSource sidechainSource = SidechainSource::Internal;
    bool sidechainListen = false;

    Biquad scHpf;
    Biquad scLpf;
    juce::AudioBuffer<float> detectorBuffer;

    // Ballistics coefficients & states
    float alphaAttack = 0.01f;
    float alphaRelease = 0.001f;
    float alphaSlow = 0.0001f;
    float envGainDb = 0.0f;       // Current gain reduction in dB (<= 0.0)
    float slowEnvelopeDb = 0.0f;  // Leaky background envelope for auto-release

    // Parameter smoothing for clickless automation
    juce::SmoothedValue<float> smoothedMakeupLinear;
    juce::SmoothedValue<float> smoothedMix;

    // 15 ms clickless bypass crossfade
    juce::SmoothedValue<float> bypassFade;

    // Thread-safe gain reduction meter
    std::atomic<float> gainReductionDb { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Compressor)
};

} // namespace SJAudio
