#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../DSP/DynamicEQ.h"

namespace SJAudio
{

/**
 * @brief High-resolution Dynamic EQ Meter.
 *
 * Displays:
 * - Dynamic Action (Gain Reduction / Boost dB) with calibrated zero-center or downward/upward scale.
 * - Sidechain Detector input level (-60 dB to 0 dB) with threshold indicator.
 * - Numerical dB readout with color indication.
 */
class DynamicEQMeter : public juce::Component
{
public:
    explicit DynamicEQMeter(const DynamicEQ& dynEqRef);
    ~DynamicEQMeter() override = default;

    void updateVisuals(float deltaTimeSeconds);
    void paint(juce::Graphics& g) override;

private:
    const DynamicEQ& dynEq;
    float currentVisualDeltaDb = 0.0f; // Smoothed dynamic gain offset (dB)
    float currentVisualDetDb = -60.0f;  // Smoothed detector level (dB)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DynamicEQMeter)
};

} // namespace SJAudio
