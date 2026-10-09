#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <atomic>
#include <cmath>

namespace SJAudio
{

struct MeterReadout
{
    float peakDbL  = -100.0f;
    float peakDbR  = -100.0f;
    float rmsDbL   = -100.0f;
    float rmsDbR   = -100.0f;
    float vuValue  = -20.0f;  // In VU scale units (-20 to +3+)
    bool  isClipped = false;
};

/**
 * High-performance, lock-free, zero-allocation metering source.
 * Audio thread measures peak, RMS, and VU ballistics with SIMD-compatible accumulators.
 * GUI thread safely reads atomic snapshots for glitch-free visualization.
 */
class MeterSource
{
public:
    MeterSource();
    ~MeterSource() = default;

    void prepare(double sampleRate);
    void reset();

    /**
     * Process audio buffer block in the real-time audio thread.
     * Computes true peak, RMS, and 300 ms VU ballistics.
     */
    void process(const juce::AudioBuffer<float>& buffer);

    /**
     * Read instantaneous meter levels for GUI display (thread-safe, lock-free).
     */
    MeterReadout getReadout() const noexcept;

    /**
     * Check if a digital clip (>= 0 dBFS) occurred.
     */
    bool hasClipped() const noexcept { return clipFlag.load(std::memory_order_relaxed); }

    /**
     * Reset the clip latch (e.g. upon user click).
     */
    void resetClip() noexcept { clipFlag.store(false, std::memory_order_relaxed); }

private:
    double currentSampleRate = 44100.0;
    float vuCoeff = 0.05f; // Exponential smoothing coefficient for 300 ms integration
    float currentVuLinear = 0.0f;

    // Atomic values updated at the end of each processBlock
    std::atomic<float> atomicPeakL { 0.0f };
    std::atomic<float> atomicPeakR { 0.0f };
    std::atomic<float> atomicRmsL  { 0.0f };
    std::atomic<float> atomicRmsR  { 0.0f };
    std::atomic<float> atomicVu    { -20.0f };
    std::atomic<bool>  clipFlag    { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeterSource)
};

} // namespace SJAudio
