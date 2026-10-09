#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../DSP/Metering.h"

namespace SJAudio
{

/**
 * Professional Stereo Bar Meter with Peak, RMS, Peak Hold, and interactive Clip LED.
 */
class StereoBarMeter : public juce::Component
{
public:
    StereoBarMeter(const juce::String& title, MeterSource& source);
    ~StereoBarMeter() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;

    /** Update visual ballistics (called from GUI timer, e.g. 30-60 Hz) */
    void updateVisuals(float deltaSeconds);

private:
    juce::String meterTitle;
    MeterSource& meterSource;

    // Visual smoothing state
    float visualPeakL = -100.0f;
    float visualPeakR = -100.0f;
    float visualRmsL  = -100.0f;
    float visualRmsR  = -100.0f;

    // Peak hold
    float peakHoldL = -100.0f;
    float peakHoldR = -100.0f;
    float peakHoldTimerL = 0.0f;
    float peakHoldTimerR = 0.0f;

    // Clip indicator
    bool  visualClip = false;
    float clipTimer = 0.0f;

    juce::Rectangle<float> clipLedBounds;

    // Convert dBFS (-60 to 0) to normalized height (0.0 to 1.0)
    static float dbToNormalized(float db);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoBarMeter)
};

/**
 * Analog Style VU Meter with authentic ballistics and standard VU markings.
 */
class VUMeterComponent : public juce::Component
{
public:
    explicit VUMeterComponent(MeterSource& outputSource);
    ~VUMeterComponent() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    /** Update visual needle ballistics */
    void updateVisuals(float deltaSeconds);

private:
    MeterSource& meterSource;
    float visualVuValue = -20.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VUMeterComponent)
};

} // namespace SJAudio
