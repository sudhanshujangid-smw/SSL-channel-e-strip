#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Parameters.h"
#include "DSP/InputStage.h"
#include "DSP/FilterSection.h"
#include "DSP/GateExpander.h"
#include "DSP/EQSection.h"
#include "DSP/Compressor.h"
#include "DSP/DynamicEQ.h"
#include "DSP/Saturation.h"
#include "DSP/OutputStage.h"
#include "DSP/Metering.h"
#include "DSP/SpectrumFIFO.h"
#include "PresetManager.h"

class SJChannelStripAudioProcessor : public juce::AudioProcessor
{
public:
    SJChannelStripAudioProcessor();
    ~SJChannelStripAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState& getAPVTS() noexcept { return parameters; }

    SJAudio::FilterSection& getFilterSection() noexcept             { return filterSection; }
    const SJAudio::FilterSection& getFilterSection() const noexcept { return filterSection; }

    SJAudio::GateExpander& getGateExpander() noexcept               { return gateExpander; }
    const SJAudio::GateExpander& getGateExpander() const noexcept   { return gateExpander; }

    SJAudio::EQSection& getEQSection() noexcept                     { return eqSection; }
    const SJAudio::EQSection& getEQSection() const noexcept         { return eqSection; }

    SJAudio::DynamicEQ& getDynamicEQ() noexcept                     { return dynamicEQ; }
    const SJAudio::DynamicEQ& getDynamicEQ() const noexcept         { return dynamicEQ; }

    SJAudio::Compressor& getCompressor() noexcept                   { return compressor; }
    const SJAudio::Compressor& getCompressor() const noexcept       { return compressor; }

    SJAudio::Saturation& getSaturation() noexcept                   { return saturation; }
    const SJAudio::Saturation& getSaturation() const noexcept       { return saturation; }

    SJAudio::MeterSource& getInputMeter() noexcept  { return inputMeterSource; }
    SJAudio::MeterSource& getOutputMeter() noexcept { return outputMeterSource; }

    SJAudio::PresetManager& getPresetManager() noexcept { return presetManager; }
    const SJAudio::PresetManager& getPresetManager() const noexcept { return presetManager; }

    SJAudio::SpectrumFIFO& getSpectrumFIFO() noexcept { return spectrumFIFO; }

    // Cached raw parameter pointers for realtime-safe audio thread access
    struct ParameterPointers
    {
        // Input
        std::atomic<float>* inputGain             = nullptr;
        std::atomic<float>* polarity              = nullptr;

        // Filter
        std::atomic<float>* highPassEnabled       = nullptr;
        std::atomic<float>* highPassFrequency     = nullptr;
        std::atomic<float>* highPassSlope         = nullptr;
        std::atomic<float>* lowPassEnabled        = nullptr;
        std::atomic<float>* lowPassFrequency      = nullptr;
        std::atomic<float>* lowPassSlope          = nullptr;

        // EQ
        std::atomic<float>* eqEnabled             = nullptr;
        std::atomic<float>* lowEnabled            = nullptr;
        std::atomic<float>* lowFreq               = nullptr;
        std::atomic<float>* lowGain               = nullptr;
        std::atomic<float>* lowQ                  = nullptr;
        std::atomic<float>* lowShelf              = nullptr;

        std::atomic<float>* lowMidEnabled         = nullptr;
        std::atomic<float>* lowMidFreq            = nullptr;
        std::atomic<float>* lowMidGain            = nullptr;
        std::atomic<float>* lowMidQ               = nullptr;

        std::atomic<float>* highMidEnabled        = nullptr;
        std::atomic<float>* highMidFreq           = nullptr;
        std::atomic<float>* highMidGain           = nullptr;
        std::atomic<float>* highMidQ              = nullptr;

        std::atomic<float>* highEnabled           = nullptr;
        std::atomic<float>* highFreq              = nullptr;
        std::atomic<float>* highGain              = nullptr;
        std::atomic<float>* highQ                 = nullptr;
        std::atomic<float>* highShelf             = nullptr;

        // Gate
        std::atomic<float>* gateEnabled           = nullptr;
        std::atomic<float>* gateThreshold         = nullptr;
        std::atomic<float>* gateRange             = nullptr;
        std::atomic<float>* gateAttack            = nullptr;
        std::atomic<float>* gateHold              = nullptr;
        std::atomic<float>* gateRelease           = nullptr;
        std::atomic<float>* gateMode              = nullptr;
        std::atomic<float>* gateSidechainEnabled  = nullptr;
        std::atomic<float>* gateSidechainHighPass = nullptr;
        std::atomic<float>* gateSidechainLowPass  = nullptr;
        std::atomic<float>* gateSidechainSource   = nullptr;
        std::atomic<float>* gateSidechainListen   = nullptr;

        // Compressor
        std::atomic<float>* compressorEnabled     = nullptr;
        std::atomic<float>* compressorThreshold   = nullptr;
        std::atomic<float>* compressorRatio       = nullptr;
        std::atomic<float>* compressorAttack      = nullptr;
        std::atomic<float>* compressorRelease     = nullptr;
        std::atomic<float>* compressorKnee        = nullptr;
        std::atomic<float>* compressorMakeup      = nullptr;
        std::atomic<float>* compressorAutoRelease = nullptr;
        std::atomic<float>* compressorMix         = nullptr;
        std::atomic<float>* compressorSidechainEnabled  = nullptr;
        std::atomic<float>* compressorSidechainHighPass = nullptr;
        std::atomic<float>* compressorSidechainLowPass  = nullptr;
        std::atomic<float>* compressorSidechainSource   = nullptr;
        std::atomic<float>* compressorSidechainListen   = nullptr;

        // Saturation
        std::atomic<float>* saturationEnabled     = nullptr;
        std::atomic<float>* saturationDrive       = nullptr;
        std::atomic<float>* saturationCharacter   = nullptr;
        std::atomic<float>* saturationMix         = nullptr;
        std::atomic<float>* saturationOutput      = nullptr;

        // Dynamic EQ (Priority 1)
        std::atomic<float>* dynamicEqEnabled      = nullptr;
        std::atomic<float>* dynamicEqFrequency    = nullptr;
        std::atomic<float>* dynamicEqGain         = nullptr;
        std::atomic<float>* dynamicEqQ            = nullptr;
        std::atomic<float>* dynamicEqThreshold    = nullptr;
        std::atomic<float>* dynamicEqRange        = nullptr;
        std::atomic<float>* dynamicEqAttack       = nullptr;
        std::atomic<float>* dynamicEqRelease      = nullptr;
        std::atomic<float>* dynamicEqMode         = nullptr;
        std::atomic<float>* dynamicEqListen       = nullptr;

        // Output
        std::atomic<float>* outputGain            = nullptr;
        std::atomic<float>* globalMix             = nullptr;
        std::atomic<float>* globalBypass          = nullptr;

        // Oversampling
        std::atomic<float>* oversampling          = nullptr;

        // Analyzer
        std::atomic<float>* analyzerEnabled       = nullptr;
    };

    const ParameterPointers& getParameterPointers() const noexcept { return paramPointers; }

private:
    juce::AudioProcessorValueTreeState parameters;
    ParameterPointers paramPointers;

    // DSP Stages (Phase 8 & Priority 1)
    SJAudio::InputStage    inputStage;
    SJAudio::FilterSection filterSection;
    SJAudio::GateExpander  gateExpander;
    SJAudio::EQSection     eqSection;
    SJAudio::DynamicEQ     dynamicEQ;
    SJAudio::Compressor    compressor;
    SJAudio::Saturation    saturation;
    SJAudio::OutputStage   outputStage;

    // Metering Engines
    SJAudio::MeterSource inputMeterSource;
    SJAudio::MeterSource outputMeterSource;

    // Global processing controls (Phase 9 & Phase 13)
    juce::AudioBuffer<float> dryCopyBuffer;
    juce::AudioBuffer<float> dryDelayRingBuffer;
    int dryDelayWritePos = 0;
    juce::SmoothedValue<float> globalActiveFade;
    juce::SmoothedValue<float> smoothedGlobalMix;

    // Preset Manager (Phase 10)
    SJAudio::PresetManager presetManager;

    // Spectrum FIFO (Phase 11)
    SJAudio::SpectrumFIFO spectrumFIFO;

    void cacheParameterPointers();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SJChannelStripAudioProcessor)
};
