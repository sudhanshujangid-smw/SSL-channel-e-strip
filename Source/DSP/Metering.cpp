#include "Metering.h"
#include <algorithm>

namespace SJAudio
{

MeterSource::MeterSource()
{
}

void MeterSource::prepare(double sampleRate)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
    reset();
}

void MeterSource::reset()
{
    currentVuLinear = 0.0f;
    atomicPeakL.store(0.0f, std::memory_order_relaxed);
    atomicPeakR.store(0.0f, std::memory_order_relaxed);
    atomicRmsL.store(0.0f, std::memory_order_relaxed);
    atomicRmsR.store(0.0f, std::memory_order_relaxed);
    atomicVu.store(-20.0f, std::memory_order_relaxed);
    clipFlag.store(false, std::memory_order_relaxed);
}

void MeterSource::process(const juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    if (numChannels == 0 || numSamples == 0)
        return;

    float peakL = 0.0f;
    float peakR = 0.0f;
    float sumSqL = 0.0f;
    float sumSqR = 0.0f;
    float sumAbs = 0.0f;

    const auto* dataL = buffer.getReadPointer(0);
    const auto* dataR = (numChannels > 1) ? buffer.getReadPointer(1) : dataL;

    for (int i = 0; i < numSamples; ++i)
    {
        const float sL = dataL[i];
        const float sR = dataR[i];

        const float absL = std::abs(sL);
        const float absR = std::abs(sR);

        if (absL > peakL) peakL = absL;
        if (absR > peakR) peakR = absR;

        sumSqL += sL * sL;
        sumSqR += sR * sR;

        // Combined absolute average for VU ballistic tracking
        sumAbs += (absL + absR) * 0.5f;
    }

    // Clip detection (>= 0 dBFS, 1.0 linear)
    if (peakL >= 1.0f || peakR >= 1.0f)
    {
        clipFlag.store(true, std::memory_order_relaxed);
    }

    const float rmsL = std::sqrt(sumSqL / static_cast<float>(numSamples));
    const float rmsR = std::sqrt(sumSqR / static_cast<float>(numSamples));

    // Analog VU Ballistics: 300 ms response integration filter
    // Tau = 0.300 seconds
    const float tau = 0.300f;
    const float blockDt = static_cast<float>(numSamples) / static_cast<float>(currentSampleRate);
    const float alpha = 1.0f - std::exp(-blockDt / tau);

    const float meanAbs = sumAbs / static_cast<float>(numSamples);
    currentVuLinear += alpha * (meanAbs - currentVuLinear);

    // Standard calibration: 0 VU = -18 dBFS (approx 0.1259 linear)
    constexpr float refLinear0VU = 0.12589254f;
    float vuDb = -20.0f;
    if (currentVuLinear > 1.0e-5f)
    {
        vuDb = 20.0f * std::log10(currentVuLinear / refLinear0VU);
    }
    vuDb = std::clamp(vuDb, -20.0f, 6.0f);

    // Atomic publishing
    atomicPeakL.store(peakL, std::memory_order_relaxed);
    atomicPeakR.store(peakR, std::memory_order_relaxed);
    atomicRmsL.store(rmsL, std::memory_order_relaxed);
    atomicRmsR.store(rmsR, std::memory_order_relaxed);
    atomicVu.store(vuDb, std::memory_order_relaxed);
}

MeterReadout MeterSource::getReadout() const noexcept
{
    MeterReadout readout;

    const float pL = atomicPeakL.load(std::memory_order_relaxed);
    const float pR = atomicPeakR.load(std::memory_order_relaxed);
    const float rL = atomicRmsL.load(std::memory_order_relaxed);
    const float rR = atomicRmsR.load(std::memory_order_relaxed);

    readout.peakDbL  = juce::Decibels::gainToDecibels(pL, -100.0f);
    readout.peakDbR  = juce::Decibels::gainToDecibels(pR, -100.0f);
    readout.rmsDbL   = juce::Decibels::gainToDecibels(rL, -100.0f);
    readout.rmsDbR   = juce::Decibels::gainToDecibels(rR, -100.0f);
    readout.vuValue  = atomicVu.load(std::memory_order_relaxed);
    readout.isClipped = clipFlag.load(std::memory_order_relaxed);

    return readout;
}

} // namespace SJAudio
