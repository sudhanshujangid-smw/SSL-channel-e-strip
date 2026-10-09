#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../DSP/Saturation.h"

namespace SJAudio
{

/**
 * @brief Dynamic Waveshaper & Analog Color Visualizer for the Saturation module.
 *
 * Displays:
 * 1. Real-time transfer curve dynamically morphing with Drive and Character.
 * 2. Harmonic coloration mode label ("WARM TUBE", "ANALOG TAPE", "CONSOLE OVERDRIVE").
 * 3. Dynamic harmonic activity glow responding to real-time audio saturation.
 * 4. Calibrated analog drive LED ladder.
 */
class SaturationMeter : public juce::Component
{
public:
    explicit SaturationMeter(const Saturation& satRef);
    ~SaturationMeter() override = default;

    void updateVisuals(float deltaTimeSeconds);
    void paint(juce::Graphics& g) override;

private:
    const Saturation& sat;
    float currentActivity = 0.0f; // Smoothed visual activity (0.0 to 1.0)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SaturationMeter)
};

} // namespace SJAudio
