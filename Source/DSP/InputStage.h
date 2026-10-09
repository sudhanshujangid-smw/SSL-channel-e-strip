#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace SJAudio
{

class InputStage
{
public:
    InputStage();
    ~InputStage() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    /** Set target input gain in decibels (-24 dB to +24 dB) */
    void setGainDb(float gainDb);

    /** Set polarity inversion (false = normal +1, true = inverted -1) */
    void setPolarity(bool invert);

    /** Process audio buffer in-place (mono or stereo) */
    void process(juce::AudioBuffer<float>& buffer);

private:
    double currentSampleRate = 44100.0;

    // Smoothed gain multiplier (linear scale)
    juce::SmoothedValue<float> smoothedGain;

    // Smoothed polarity multiplier (+1.0 to -1.0) for clickless phase inversion
    juce::SmoothedValue<float> smoothedPolarity;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InputStage)
};

} // namespace SJAudio
