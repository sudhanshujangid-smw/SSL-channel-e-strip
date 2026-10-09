#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace SJAudio
{

class OutputStage
{
public:
    OutputStage();
    ~OutputStage() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    /** Set target output gain in decibels (-24 dB to +24 dB) */
    void setGainDb(float gainDb);

    /** Process audio buffer in-place */
    void process(juce::AudioBuffer<float>& buffer);

private:
    double currentSampleRate = 44100.0;
    juce::SmoothedValue<float> smoothedGain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutputStage)
};

} // namespace SJAudio
