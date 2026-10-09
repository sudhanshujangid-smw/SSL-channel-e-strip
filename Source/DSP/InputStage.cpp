#include "InputStage.h"

namespace SJAudio
{

InputStage::InputStage()
{
    smoothedGain.setCurrentAndTargetValue(1.0f);
    smoothedPolarity.setCurrentAndTargetValue(1.0f);
}

void InputStage::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;

    // 20 ms linear smoothing for gain parameter changes
    smoothedGain.reset(currentSampleRate, 0.02);

    // 10 ms ramp for polarity inversion to prevent audible clicks/pops
    smoothedPolarity.reset(currentSampleRate, 0.01);
}

void InputStage::reset()
{
    smoothedGain.setCurrentAndTargetValue(smoothedGain.getTargetValue());
    smoothedPolarity.setCurrentAndTargetValue(smoothedPolarity.getTargetValue());
}

void InputStage::setGainDb(float gainDb)
{
    const float linearGain = juce::Decibels::decibelsToGain(gainDb);
    smoothedGain.setTargetValue(linearGain);
}

void InputStage::setPolarity(bool invert)
{
    smoothedPolarity.setTargetValue(invert ? -1.0f : 1.0f);
}

void InputStage::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    if (numChannels == 0 || numSamples == 0)
        return;

    // If both parameters are static, apply vector SIMD gain across channels
    if (!smoothedGain.isSmoothing() && !smoothedPolarity.isSmoothing())
    {
        const float staticMultiplier = smoothedGain.getTargetValue() * smoothedPolarity.getTargetValue();
        if (staticMultiplier != 1.0f)
        {
            buffer.applyGain(staticMultiplier);
        }
        return;
    }

    // Otherwise, calculate sample-accurate smoothed gain ramp
    if (numChannels == 1)
    {
        auto* chData = buffer.getWritePointer(0);
        for (int i = 0; i < numSamples; ++i)
        {
            const float mult = smoothedGain.getNextValue() * smoothedPolarity.getNextValue();
            chData[i] *= mult;
        }
    }
    else if (numChannels == 2)
    {
        auto* chL = buffer.getWritePointer(0);
        auto* chR = buffer.getWritePointer(1);

        for (int i = 0; i < numSamples; ++i)
        {
            const float mult = smoothedGain.getNextValue() * smoothedPolarity.getNextValue();
            chL[i] *= mult;
            chR[i] *= mult;
        }
    }
    else
    {
        // Support surround / multi-channel safely
        for (int i = 0; i < numSamples; ++i)
        {
            const float mult = smoothedGain.getNextValue() * smoothedPolarity.getNextValue();
            for (int ch = 0; ch < numChannels; ++ch)
            {
                buffer.getWritePointer(ch)[i] *= mult;
            }
        }
    }
}

} // namespace SJAudio
