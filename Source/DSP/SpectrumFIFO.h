#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <algorithm>

namespace SJAudio
{

/**
 * Thread-safe, lock-free, zero-allocation FIFO buffer for streaming
 * audio blocks from the realtime audio thread (processBlock) to the
 * GUI thread for FFT spectrum analysis.
 */
class SpectrumFIFO
{
public:
    SpectrumFIFO()
        : abstractFifo(capacity)
    {
        fifoBuffer.resize(capacity, 0.0f);
    }

    ~SpectrumFIFO() = default;

    /**
     * Push incoming audio samples into the FIFO (called from the audio thread).
     * Automatically downmixes stereo/multichannel to mono.
     * Performs zero memory allocations and uses lock-free atomics.
     */
    void push(const juce::AudioBuffer<float>& buffer)
    {
        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();

        if (numSamples <= 0 || numChannels <= 0)
            return;

        // If incoming block exceeds capacity, only take the most recent samples
        int samplesToPush = std::min(numSamples, capacity);
        int startSample = numSamples - samplesToPush;

        int start1, size1, start2, size2;
        abstractFifo.prepareToWrite(samplesToPush, start1, size1, start2, size2);

        if (size1 > 0)
            copyChannels(buffer, startSample, start1, size1);

        if (size2 > 0)
            copyChannels(buffer, startSample + size1, start2, size2);

        abstractFifo.finishedWrite(size1 + size2);
    }

    /**
     * Pull a block of contiguous samples for FFT analysis (called from GUI/worker thread).
     * Returns true if numSamples was successfully copied, false if insufficient samples available.
     */
    bool pull(float* dest, int numSamples)
    {
        if (dest == nullptr || numSamples <= 0)
            return false;

        if (abstractFifo.getNumReady() < numSamples)
            return false;

        int start1, size1, start2, size2;
        abstractFifo.prepareToRead(numSamples, start1, size1, start2, size2);

        if (size1 > 0)
            std::copy_n(fifoBuffer.data() + start1, size1, dest);

        if (size2 > 0)
            std::copy_n(fifoBuffer.data() + start2, size2, dest + size1);

        abstractFifo.finishedRead(size1 + size2);
        return true;
    }

    /** Reset the FIFO pointers and clear internal buffer */
    void reset()
    {
        abstractFifo.reset();
        std::fill(fifoBuffer.begin(), fifoBuffer.end(), 0.0f);
    }

    int getNumReady() const noexcept
    {
        return abstractFifo.getNumReady();
    }

private:
    static constexpr int capacity = 8192;
    juce::AbstractFifo abstractFifo;
    std::vector<float> fifoBuffer;

    void copyChannels(const juce::AudioBuffer<float>& buffer, int bufferStart, int fifoStart, int num)
    {
        const int numChannels = buffer.getNumChannels();
        float* dst = fifoBuffer.data() + fifoStart;

        if (numChannels == 1)
        {
            const float* src = buffer.getReadPointer(0, bufferStart);
            std::copy_n(src, num, dst);
        }
        else
        {
            const float* srcL = buffer.getReadPointer(0, bufferStart);
            const float* srcR = buffer.getReadPointer(1, bufferStart);

            for (int i = 0; i < num; ++i)
            {
                dst[i] = 0.5f * (srcL[i] + srcR[i]);
            }
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumFIFO)
};

} // namespace SJAudio
