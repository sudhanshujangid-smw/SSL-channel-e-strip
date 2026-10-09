#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "../DSP/EQSection.h"
#include "../DSP/FilterSection.h"
#include "../DSP/DynamicEQ.h"
#include "../DSP/SpectrumFIFO.h"
#include "SJComponents.h"
#include <vector>
#include <array>

namespace SJAudio
{

/**
 * Professional Parametric EQ Curve Display with Real-Time FFT Spectrum Analyzer (RTA).
 * Features:
 * - 2048-point JUCE FFT with Hann windowing
 * - Logarithmic frequency scaling (20 Hz -> 20 kHz)
 * - Calibrated Gain axis (-24 dB to +18 dB)
 * - Individual display curves for HPF, LPF, Low, Low Mid, High Mid, High, Dynamic EQ, and Combined Master Curve
 * - 4 interactive draggable handles with mouse wheel Q control and double-click reset
 * - Smooth frequency bin grouping & temporal analog decay ballistics (~36 dB/s)
 * - Analyzer ON/OFF toggle connected to APVTS
 * - Zero audio-thread allocation & lock-free FIFO communication
 */
class EQDisplay : public juce::Component,
                  public juce::Timer
{
public:
    EQDisplay(juce::AudioProcessorValueTreeState& apvts,
              const EQSection& eq,
              const FilterSection& filters,
              SpectrumFIFO& fifo,
              const DynamicEQ* dynEq = nullptr);
    ~EQDisplay() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;
    void mouseDoubleClick(const juce::MouseEvent& e) override;

    // Coordinate conversions
    static float freqToX(float freqHz, float width);
    static float xToFreq(float x, float width);
    static float gainToY(float gainDb, float height);
    static float yToGain(float y, float height);

private:
    juce::AudioProcessorValueTreeState& apvtsRef;
    const EQSection& eqSection;
    const FilterSection& filterSection;
    SpectrumFIFO& fifoRef;
    const DynamicEQ* dynamicEQRef = nullptr;

    // --- FFT Analyzer Engine ---
    static constexpr int fftOrder = 11;
    static constexpr int fftSize = 1 << fftOrder; // 2048
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

    std::array<float, fftSize * 2> fftBuffer;
    std::array<float, fftSize> timeDomainBuffer;

    static constexpr int numSpectrumBins = 160;
    std::vector<float> spectrumLevels;

    // Precomputed optimization tables (Phase 13)
    struct BinRange
    {
        int startBin = 1;
        int endBin = 1;
    };
    std::array<BinRange, numSpectrumBins> precomputedBinRanges;
    void precomputeBinRanges();

    static constexpr int numCurvePoints = 128;
    std::array<float, numCurvePoints> precomputedCurveFrequencies;
    std::array<float, numCurvePoints> precomputedCurveNormX;
    void precomputeCurvePoints();

    // --- RTA Controls ---
    SJToggleButton analyzerToggleButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> analyzerAttachment;

    // --- Interaction State ---
    int draggedBandIndex = -1;
    int hoveredBandIndex = -1;

    struct BandInfo
    {
        const char* name;
        const char* freqParamId;
        const char* gainParamId;
        const char* qParamId;
        const char* enableParamId;
        juce::Colour colour;
    };

    static const BandInfo bands[4];

    juce::Point<float> getHandlePosition(int bandIdx, float width, float height) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQDisplay)
};

} // namespace SJAudio
