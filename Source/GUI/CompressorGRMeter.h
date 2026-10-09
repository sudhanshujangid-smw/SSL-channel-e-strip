#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../DSP/Compressor.h"

namespace SJAudio
{

/**
 * @brief Professional Gain-Reduction Meter for Compressor.
 *
 * Displays downward gain reduction on a calibrated scale:
 * 0, -3, -6, -10, -20, -30 dB.
 * Uses smooth decay ballistics on GUI thread and reads atomic value from DSP.
 */
class CompressorGRMeter : public juce::Component
{
public:
    explicit CompressorGRMeter(const Compressor& compRef);
    ~CompressorGRMeter() override = default;

    void updateVisuals(float deltaTimeSeconds);
    void paint(juce::Graphics& g) override;

private:
    const Compressor& comp;
    float currentVisualDb = 0.0f; // Smoothed visual value (negative dB, 0 to -30)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorGRMeter)
};

} // namespace SJAudio
