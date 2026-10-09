#include "OutputStage.h"

namespace SJAudio
{

OutputStage::OutputStage()
{
    smoothedGain.setCurrentAndTargetValue(1.0f);
}

void OutputStage::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;
    // 20 ms linear smoothing for output gain changes
    smoothedGain.reset(currentSampleRate, 0.02);
}

void OutputStage::reset()
{
    smoothedGain.setCurrentAndTargetValue(smoothedGain.getTargetValue());
}

void OutputStage::setGainDb(float gainDb)
{
    const float linearGain = juce::Decibels::decibelsToGain(gainDb);
    smoothedGain.setTargetValue(linearGain);
}

void OutputStage::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    if (numChannels == 0 || numSamples == 0)
        return;

    // Fast SIMD path if static
    if (!smoothedGain.isSmoothing())
    {
        const float target = smoothedGain.getTargetValue();
        if (target != 1.0f)
        {
            buffer.applyGain(target);
        }
        return;
    }

    // Smoothed per-sample ramp
    if (numChannels == 1)
    {
        auto* chData = buffer.getWritePointer(0);
        for (int i = 0; i < numSamples; ++i)
        {
            chData[i] *= smoothedGain.getNextValue();
        }
    }
    else if (numChannels == 2)
    {
        auto* chL = buffer.getWritePointer(0);
        auto* chR = buffer.getWritePointer(1);

        for (int i = 0; i < numSamples; ++i)
        {
            const float g = smoothedGain.getNextValue();
            chL[i] *= g;
            chR[i] *= g;
        }
    }
    else
    {
        for (int i = 0; i < numSamples; ++i)
        {
            const float g = smoothedGain.getNextValue();
            for (int ch = 0; ch < numChannels; ++ch)
            {
                buffer.getWritePointer(ch)[i] *= g;
            }
        }
    }
}

} // namespace SJAudio
