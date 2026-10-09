#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../DSP/GateExpander.h"

namespace SJAudio
{

/**
 * @brief Professional Gain-Reduction Meter for Gate / Expander.
 *
 * Displays downward gain reduction on a calibrated scale:
 * 0 dB, -5 dB, -10 dB, -20 dB, -40 dB, -60 dB.
 * Uses smooth decay ballistics on GUI thread and reads atomic value from DSP.
 */
class GateGRMeter : public juce::Component
{
public:
    explicit GateGRMeter(const GateExpander& gateRef);
    ~GateGRMeter() override = default;

    void updateVisuals(float deltaTimeSeconds);
    void paint(juce::Graphics& g) override;

private:
    const GateExpander& gate;
    float currentVisualDb = 0.0f; // Smoothed visual value (negative dB, 0 to -60)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GateGRMeter)
};

} // namespace SJAudio
