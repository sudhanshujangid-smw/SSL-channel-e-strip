#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace ParameterIDs
{
    // --- INPUT ---
    inline constexpr auto inputGain             = "inputGain";
    inline constexpr auto polarity              = "polarity";

    // --- FILTER ---
    inline constexpr auto highPassEnabled       = "highPassEnabled";
    inline constexpr auto highPassFrequency     = "highPassFrequency";
    inline constexpr auto highPassSlope         = "highPassSlope";
    inline constexpr auto lowPassEnabled        = "lowPassEnabled";
    inline constexpr auto lowPassFrequency      = "lowPassFrequency";
    inline constexpr auto lowPassSlope          = "lowPassSlope";

    // --- 4-BAND EQ ---
    inline constexpr auto eqEnabled             = "eqEnabled";
    inline constexpr auto lowEnabled            = "lowEnabled";
    inline constexpr auto lowFreq               = "lowFreq";
    inline constexpr auto lowGain               = "lowGain";
    inline constexpr auto lowQ                  = "lowQ";
    inline constexpr auto lowShelf              = "lowShelf";

    inline constexpr auto lowMidEnabled         = "lowMidEnabled";
    inline constexpr auto lowMidFreq            = "lowMidFreq";
    inline constexpr auto lowMidGain            = "lowMidGain";
    inline constexpr auto lowMidQ               = "lowMidQ";

    inline constexpr auto highMidEnabled        = "highMidEnabled";
    inline constexpr auto highMidFreq           = "highMidFreq";
    inline constexpr auto highMidGain           = "highMidGain";
    inline constexpr auto highMidQ              = "highMidQ";

    inline constexpr auto highEnabled           = "highEnabled";
    inline constexpr auto highFreq              = "highFreq";
    inline constexpr auto highGain              = "highGain";
    inline constexpr auto highQ                 = "highQ";
    inline constexpr auto highShelf             = "highShelf";

    // --- DYNAMIC EQ (PRIORITY 1) ---
    inline constexpr auto dynamicEqEnabled     = "dynamicEqEnabled";
    inline constexpr auto dynamicEqFrequency   = "dynamicEqFrequency";
    inline constexpr auto dynamicEqGain        = "dynamicEqGain";
    inline constexpr auto dynamicEqQ           = "dynamicEqQ";
    inline constexpr auto dynamicEqThreshold   = "dynamicEqThreshold";
    inline constexpr auto dynamicEqRange       = "dynamicEqRange";
    inline constexpr auto dynamicEqAttack      = "dynamicEqAttack";
    inline constexpr auto dynamicEqRelease     = "dynamicEqRelease";
    inline constexpr auto dynamicEqMode        = "dynamicEqMode";
    inline constexpr auto dynamicEqListen      = "dynamicEqListen";

    // --- GATE / EXPANDER ---
    inline constexpr auto gateEnabled           = "gateEnabled";
    inline constexpr auto gateThreshold         = "gateThreshold";
    inline constexpr auto gateRange             = "gateRange";
    inline constexpr auto gateAttack            = "gateAttack";
    inline constexpr auto gateHold              = "gateHold";
    inline constexpr auto gateRelease           = "gateRelease";
    inline constexpr auto gateMode              = "gateMode";
    inline constexpr auto gateSidechainEnabled  = "gateSidechainEnabled";
    inline constexpr auto gateSidechainHighPass = "gateSidechainHighPass";
    inline constexpr auto gateSidechainLowPass  = "gateSidechainLowPass";
    inline constexpr auto gateSidechainSource   = "gateSidechainSource";
    inline constexpr auto gateSidechainListen   = "gateSidechainListen";

    // --- COMPRESSOR ---
    inline constexpr auto compressorEnabled     = "compressorEnabled";
    inline constexpr auto compressorThreshold   = "compressorThreshold";
    inline constexpr auto compressorRatio       = "compressorRatio";
    inline constexpr auto compressorAttack      = "compressorAttack";
    inline constexpr auto compressorRelease     = "compressorRelease";
    inline constexpr auto compressorKnee        = "compressorKnee";
    inline constexpr auto compressorMakeup      = "compressorMakeup";
    inline constexpr auto compressorAutoRelease = "compressorAutoRelease";
    inline constexpr auto compressorMix         = "compressorMix";
    inline constexpr auto compressorSidechainEnabled  = "compressorSidechainEnabled";
    inline constexpr auto compressorSidechainHighPass = "compressorSidechainHighPass";
    inline constexpr auto compressorSidechainLowPass  = "compressorSidechainLowPass";
    inline constexpr auto compressorSidechainSource   = "compressorSidechainSource";
    inline constexpr auto compressorSidechainListen   = "compressorSidechainListen";

    // --- SATURATION ---
    inline constexpr auto saturationEnabled     = "saturationEnabled";
    inline constexpr auto saturationDrive       = "saturationDrive";
    inline constexpr auto saturationCharacter   = "saturationCharacter";
    inline constexpr auto saturationMix         = "saturationMix";
    inline constexpr auto saturationOutput      = "saturationOutput";

    // --- OUTPUT ---
    inline constexpr auto outputGain            = "outputGain";
    inline constexpr auto globalMix             = "globalMix";
    inline constexpr auto globalBypass          = "globalBypass";

    // --- OVERSAMPLING ---
    inline constexpr auto oversampling          = "oversampling";

    // --- SPECTRUM ANALYZER (PHASE 11) ---
    inline constexpr auto analyzerEnabled       = "analyzerEnabled";
}

namespace ParameterNames
{
    // --- INPUT ---
    inline constexpr auto inputGain             = "Input Gain";
    inline constexpr auto polarity              = "Phase / Polarity Invert";

    // --- FILTER ---
    inline constexpr auto highPassEnabled       = "High Pass Filter Enable";
    inline constexpr auto highPassFrequency     = "High Pass Frequency";
    inline constexpr auto highPassSlope         = "High Pass Slope";
    inline constexpr auto lowPassEnabled        = "Low Pass Filter Enable";
    inline constexpr auto lowPassFrequency      = "Low Pass Frequency";
    inline constexpr auto lowPassSlope          = "Low Pass Slope";

    // --- 4-BAND EQ ---
    inline constexpr auto eqEnabled             = "EQ Master Enable";
    inline constexpr auto lowEnabled            = "Low Band Enable";
    inline constexpr auto lowFreq               = "Low Band Frequency";
    inline constexpr auto lowGain               = "Low Band Gain";
    inline constexpr auto lowQ                  = "Low Band Q";
    inline constexpr auto lowShelf              = "Low Band Shelf Mode";

    inline constexpr auto lowMidEnabled         = "Low-Mid Band Enable";
    inline constexpr auto lowMidFreq            = "Low-Mid Frequency";
    inline constexpr auto lowMidGain            = "Low-Mid Gain";
    inline constexpr auto lowMidQ               = "Low-Mid Q";

    inline constexpr auto highMidEnabled        = "High-Mid Band Enable";
    inline constexpr auto highMidFreq           = "High-Mid Frequency";
    inline constexpr auto highMidGain           = "High-Mid Gain";
    inline constexpr auto highMidQ              = "High-Mid Q";

    inline constexpr auto highEnabled           = "High Band Enable";
    inline constexpr auto highFreq              = "High Band Frequency";
    inline constexpr auto highGain              = "High Band Gain";
    inline constexpr auto highQ                 = "High Band Q";
    inline constexpr auto highShelf             = "High Band Shelf Mode";

    // --- DYNAMIC EQ (PRIORITY 1) ---
    inline constexpr auto dynamicEqEnabled     = "Dynamic EQ Master Enable";
    inline constexpr auto dynamicEqFrequency   = "Dynamic EQ Frequency";
    inline constexpr auto dynamicEqGain        = "Dynamic EQ Baseline Gain";
    inline constexpr auto dynamicEqQ           = "Dynamic EQ Q / Bandwidth";
    inline constexpr auto dynamicEqThreshold   = "Dynamic EQ Threshold";
    inline constexpr auto dynamicEqRange       = "Dynamic EQ Dynamic Range";
    inline constexpr auto dynamicEqAttack      = "Dynamic EQ Attack";
    inline constexpr auto dynamicEqRelease     = "Dynamic EQ Release";
    inline constexpr auto dynamicEqMode        = "Dynamic EQ Dynamic Mode";
    inline constexpr auto dynamicEqListen      = "Dynamic EQ Detector Listen";

    // --- GATE / EXPANDER ---
    inline constexpr auto gateEnabled           = "Gate Enable";
    inline constexpr auto gateThreshold         = "Gate Threshold";
    inline constexpr auto gateRange             = "Gate Range";
    inline constexpr auto gateAttack            = "Gate Attack";
    inline constexpr auto gateHold              = "Gate Hold";
    inline constexpr auto gateRelease           = "Gate Release";
    inline constexpr auto gateMode              = "Gate Mode";
    inline constexpr auto gateSidechainEnabled  = "Gate Sidechain Filter Enable";
    inline constexpr auto gateSidechainHighPass = "Gate Sidechain High Pass";
    inline constexpr auto gateSidechainLowPass  = "Gate Sidechain Low Pass";
    inline constexpr auto gateSidechainSource   = "Gate Sidechain Source";
    inline constexpr auto gateSidechainListen   = "Gate Sidechain Listen";

    // --- COMPRESSOR ---
    inline constexpr auto compressorEnabled     = "Compressor Enable";
    inline constexpr auto compressorThreshold   = "Compressor Threshold";
    inline constexpr auto compressorRatio       = "Compressor Ratio";
    inline constexpr auto compressorAttack      = "Compressor Attack";
    inline constexpr auto compressorRelease     = "Compressor Release";
    inline constexpr auto compressorKnee        = "Compressor Knee";
    inline constexpr auto compressorMakeup      = "Compressor Makeup Gain";
    inline constexpr auto compressorAutoRelease = "Compressor Auto-Release";
    inline constexpr auto compressorMix         = "Compressor Mix";
    inline constexpr auto compressorSidechainEnabled  = "Compressor Sidechain Filter Enable";
    inline constexpr auto compressorSidechainHighPass = "Compressor Sidechain High Pass";
    inline constexpr auto compressorSidechainLowPass  = "Compressor Sidechain Low Pass";
    inline constexpr auto compressorSidechainSource   = "Compressor Sidechain Source";
    inline constexpr auto compressorSidechainListen   = "Compressor Sidechain Listen";

    // --- SATURATION ---
    inline constexpr auto saturationEnabled     = "Saturation Enable";
    inline constexpr auto saturationDrive       = "Saturation Drive";
    inline constexpr auto saturationCharacter   = "Saturation Character";
    inline constexpr auto saturationMix         = "Saturation Mix";
    inline constexpr auto saturationOutput      = "Saturation Output Level";

    // --- OUTPUT ---
    inline constexpr auto outputGain            = "Output Gain";
    inline constexpr auto globalMix             = "Global Dry/Wet Mix";
    inline constexpr auto globalBypass          = "Master Bypass";

    // --- OVERSAMPLING ---
    inline constexpr auto oversampling          = "Oversampling";

    // --- SPECTRUM ANALYZER (PHASE 11) ---
    inline constexpr auto analyzerEnabled       = "Spectrum Analyzer Enable";
}

//==============================================================================
// Parameter helper functions & layout builder
//==============================================================================
inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Helper lambdas for value-to-text formatting
    auto formatDecibels = [](float val, int) { return juce::String(val, 1) + " dB"; };
    auto formatHertz = [](float val, int)
    {
        if (val >= 1000.0f)
            return juce::String(val / 1000.0f, 2) + " kHz";
        return juce::String((int)val) + " Hz";
    };
    auto formatMs = [](float val, int)
    {
        if (val < 1.0f)
            return juce::String(val, 2) + " ms";
        if (val < 10.0f)
            return juce::String(val, 1) + " ms";
        return juce::String((int)val) + " ms";
    };
    auto formatPercent = [](float val, int) { return juce::String(val, 1) + " %"; };
    auto formatRatio = [](float val, int) { return juce::String(val, 1) + ":1"; };
    auto formatQ = [](float val, int) { return juce::String(val, 2); };

    // -------------------------------------------------------------------------
    // 1. INPUT SECTION
    // -------------------------------------------------------------------------
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::inputGain, 1),
        ParameterNames::inputGain,
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::polarity, 1),
        ParameterNames::polarity,
        false
    ));

    // -------------------------------------------------------------------------
    // 2. FILTER SECTION
    // -------------------------------------------------------------------------
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::highPassEnabled, 1),
        ParameterNames::highPassEnabled,
        false
    ));

    // High Pass Freq: 20 Hz to 1000 Hz, logarithmic skew (midpoint ~140 Hz)
    auto hpfRange = juce::NormalisableRange<float>(20.0f, 1000.0f, 0.1f);
    hpfRange.setSkewForCentre(140.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::highPassFrequency, 1),
        ParameterNames::highPassFrequency,
        hpfRange,
        20.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatHertz)
    ));

    juce::StringArray slopeOptions = { "12 dB/oct", "18 dB/oct", "24 dB/oct" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParameterIDs::highPassSlope, 1),
        ParameterNames::highPassSlope,
        slopeOptions,
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::lowPassEnabled, 1),
        ParameterNames::lowPassEnabled,
        false
    ));

    // Low Pass Freq: 1000 Hz to 20000 Hz, logarithmic skew (midpoint ~4500 Hz)
    auto lpfRange = juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f);
    lpfRange.setSkewForCentre(4500.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::lowPassFrequency, 1),
        ParameterNames::lowPassFrequency,
        lpfRange,
        20000.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatHertz)
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParameterIDs::lowPassSlope, 1),
        ParameterNames::lowPassSlope,
        slopeOptions,
        0
    ));

    // -------------------------------------------------------------------------
    // 3. 4-BAND EQ SECTION
    // -------------------------------------------------------------------------
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::eqEnabled, 1),
        ParameterNames::eqEnabled,
        true
    ));

    // Low Band: 20 to 500 Hz (default 100 Hz), Gain -18 to +18 dB, Q 0.1 to 10 (def 0.707), Shelf mode (def true)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::lowEnabled, 1),
        ParameterNames::lowEnabled,
        true
    ));

    auto lowFreqRange = juce::NormalisableRange<float>(20.0f, 500.0f, 0.5f);
    lowFreqRange.setSkewForCentre(100.0f);
    auto qRange = juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f);
    qRange.setSkewForCentre(1.0f);

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::lowFreq, 1),
        ParameterNames::lowFreq,
        lowFreqRange,
        100.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatHertz)
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::lowGain, 1),
        ParameterNames::lowGain,
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::lowQ, 1),
        ParameterNames::lowQ,
        qRange,
        0.707f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatQ)
    ));
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::lowShelf, 1),
        ParameterNames::lowShelf,
        true
    ));

    // Low-Mid Band: 100 to 2000 Hz (default 400 Hz), Gain -18 to +18 dB, Q 0.1 to 10 (def 1.0)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::lowMidEnabled, 1),
        ParameterNames::lowMidEnabled,
        true
    ));

    auto lowMidFreqRange = juce::NormalisableRange<float>(100.0f, 2000.0f, 1.0f);
    lowMidFreqRange.setSkewForCentre(400.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::lowMidFreq, 1),
        ParameterNames::lowMidFreq,
        lowMidFreqRange,
        400.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatHertz)
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::lowMidGain, 1),
        ParameterNames::lowMidGain,
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::lowMidQ, 1),
        ParameterNames::lowMidQ,
        qRange,
        1.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatQ)
    ));

    // High-Mid Band: 500 to 8000 Hz (default 2500 Hz), Gain -18 to +18 dB, Q 0.1 to 10 (def 1.0)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::highMidEnabled, 1),
        ParameterNames::highMidEnabled,
        true
    ));

    auto highMidFreqRange = juce::NormalisableRange<float>(500.0f, 8000.0f, 1.0f);
    highMidFreqRange.setSkewForCentre(2000.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::highMidFreq, 1),
        ParameterNames::highMidFreq,
        highMidFreqRange,
        2500.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatHertz)
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::highMidGain, 1),
        ParameterNames::highMidGain,
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::highMidQ, 1),
        ParameterNames::highMidQ,
        qRange,
        1.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatQ)
    ));

    // High Band: 2000 to 20000 Hz (default 8000 Hz), Gain -18 to +18 dB, Q 0.1 to 10 (def 0.707), Shelf mode (def true)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::highEnabled, 1),
        ParameterNames::highEnabled,
        true
    ));

    auto highFreqRange = juce::NormalisableRange<float>(2000.0f, 20000.0f, 1.0f);
    highFreqRange.setSkewForCentre(6500.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::highFreq, 1),
        ParameterNames::highFreq,
        highFreqRange,
        8000.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatHertz)
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::highGain, 1),
        ParameterNames::highGain,
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::highQ, 1),
        ParameterNames::highQ,
        qRange,
        0.707f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatQ)
    ));
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::highShelf, 1),
        ParameterNames::highShelf,
        true
    ));

    // -------------------------------------------------------------------------
    // 3.1 DYNAMIC EQ SECTION (PRIORITY 1)
    // -------------------------------------------------------------------------
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::dynamicEqEnabled, 1),
        ParameterNames::dynamicEqEnabled,
        false
    ));

    auto dynFreqRange = juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f);
    dynFreqRange.setSkewForCentre(3000.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::dynamicEqFrequency, 1),
        ParameterNames::dynamicEqFrequency,
        dynFreqRange,
        3500.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatHertz)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::dynamicEqGain, 1),
        ParameterNames::dynamicEqGain,
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));

    auto dynQRange = juce::NormalisableRange<float>(0.1f, 10.0f, 0.05f);
    dynQRange.setSkewForCentre(1.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::dynamicEqQ, 1),
        ParameterNames::dynamicEqQ,
        dynQRange,
        1.5f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatQ)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::dynamicEqThreshold, 1),
        ParameterNames::dynamicEqThreshold,
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        -18.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::dynamicEqRange, 1),
        ParameterNames::dynamicEqRange,
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        -6.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));

    auto dynAttackRange = juce::NormalisableRange<float>(0.1f, 100.0f, 0.05f);
    dynAttackRange.setSkewForCentre(10.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::dynamicEqAttack, 1),
        ParameterNames::dynamicEqAttack,
        dynAttackRange,
        10.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatMs)
    ));

    auto dynReleaseRange = juce::NormalisableRange<float>(5.0f, 2000.0f, 0.5f);
    dynReleaseRange.setSkewForCentre(100.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::dynamicEqRelease, 1),
        ParameterNames::dynamicEqRelease,
        dynReleaseRange,
        100.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatMs)
    ));

    juce::StringArray dynEqModes = { "Compress (Cut)", "Expand (Boost)" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParameterIDs::dynamicEqMode, 1),
        ParameterNames::dynamicEqMode,
        dynEqModes,
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::dynamicEqListen, 1),
        ParameterNames::dynamicEqListen,
        false
    ));

    // -------------------------------------------------------------------------
    // 4. GATE / EXPANDER SECTION
    // -------------------------------------------------------------------------
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::gateEnabled, 1),
        ParameterNames::gateEnabled,
        false
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::gateThreshold, 1),
        ParameterNames::gateThreshold,
        juce::NormalisableRange<float>(-80.0f, 0.0f, 0.1f),
        -40.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::gateRange, 1),
        ParameterNames::gateRange,
        juce::NormalisableRange<float>(0.0f, 80.0f, 0.1f),
        20.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));

    auto gateAttackRange = juce::NormalisableRange<float>(0.1f, 100.0f, 0.05f);
    gateAttackRange.setSkewForCentre(5.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::gateAttack, 1),
        ParameterNames::gateAttack,
        gateAttackRange,
        5.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatMs)
    ));

    auto gateHoldRange = juce::NormalisableRange<float>(0.0f, 500.0f, 0.5f);
    gateHoldRange.setSkewForCentre(50.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::gateHold, 1),
        ParameterNames::gateHold,
        gateHoldRange,
        50.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatMs)
    ));

    auto gateReleaseRange = juce::NormalisableRange<float>(5.0f, 2000.0f, 0.5f);
    gateReleaseRange.setSkewForCentre(100.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::gateRelease, 1),
        ParameterNames::gateRelease,
        gateReleaseRange,
        100.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatMs)
    ));

    juce::StringArray gateModes = { "Gate", "Expander" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParameterIDs::gateMode, 1),
        ParameterNames::gateMode,
        gateModes,
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::gateSidechainEnabled, 1),
        ParameterNames::gateSidechainEnabled,
        false
    ));

    auto scHpfRange = juce::NormalisableRange<float>(20.0f, 2000.0f, 1.0f);
    scHpfRange.setSkewForCentre(200.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::gateSidechainHighPass, 1),
        ParameterNames::gateSidechainHighPass,
        scHpfRange,
        20.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatHertz)
    ));

    auto scLpfRange = juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f);
    scLpfRange.setSkewForCentre(5000.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::gateSidechainLowPass, 1),
        ParameterNames::gateSidechainLowPass,
        scLpfRange,
        20000.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatHertz)
    ));

    juce::StringArray scSources = { "Internal", "External" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParameterIDs::gateSidechainSource, 1),
        ParameterNames::gateSidechainSource,
        scSources,
        0
    ));
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::gateSidechainListen, 1),
        ParameterNames::gateSidechainListen,
        false
    ));

    // -------------------------------------------------------------------------
    // 5. COMPRESSOR SECTION
    // -------------------------------------------------------------------------
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::compressorEnabled, 1),
        ParameterNames::compressorEnabled,
        false
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::compressorThreshold, 1),
        ParameterNames::compressorThreshold,
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        -18.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));

    auto ratioRange = juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f);
    ratioRange.setSkewForCentre(4.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::compressorRatio, 1),
        ParameterNames::compressorRatio,
        ratioRange,
        4.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatRatio)
    ));

    auto compAttackRange = juce::NormalisableRange<float>(0.1f, 100.0f, 0.05f);
    compAttackRange.setSkewForCentre(10.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::compressorAttack, 1),
        ParameterNames::compressorAttack,
        compAttackRange,
        10.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatMs)
    ));

    auto compReleaseRange = juce::NormalisableRange<float>(5.0f, 2000.0f, 0.5f);
    compReleaseRange.setSkewForCentre(100.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::compressorRelease, 1),
        ParameterNames::compressorRelease,
        compReleaseRange,
        100.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatMs)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::compressorKnee, 1),
        ParameterNames::compressorKnee,
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f),
        6.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::compressorMakeup, 1),
        ParameterNames::compressorMakeup,
        juce::NormalisableRange<float>(-12.0f, 24.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::compressorAutoRelease, 1),
        ParameterNames::compressorAutoRelease,
        false
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::compressorMix, 1),
        ParameterNames::compressorMix,
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatPercent)
    ));

    // Compressor Sidechain (Phase 14)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::compressorSidechainEnabled, 1),
        ParameterNames::compressorSidechainEnabled,
        false
    ));

    auto compScHpfRange = juce::NormalisableRange<float>(20.0f, 2000.0f, 1.0f);
    compScHpfRange.setSkewForCentre(150.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::compressorSidechainHighPass, 1),
        ParameterNames::compressorSidechainHighPass,
        compScHpfRange,
        80.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatHertz)
    ));

    auto compScLpfRange = juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f);
    compScLpfRange.setSkewForCentre(5000.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::compressorSidechainLowPass, 1),
        ParameterNames::compressorSidechainLowPass,
        compScLpfRange,
        20000.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatHertz)
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParameterIDs::compressorSidechainSource, 1),
        ParameterNames::compressorSidechainSource,
        scSources,
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::compressorSidechainListen, 1),
        ParameterNames::compressorSidechainListen,
        false
    ));

    // -------------------------------------------------------------------------
    // 6. SATURATION SECTION
    // -------------------------------------------------------------------------
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::saturationEnabled, 1),
        ParameterNames::saturationEnabled,
        false
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::saturationDrive, 1),
        ParameterNames::saturationDrive,
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        20.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatPercent)
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::saturationCharacter, 1),
        ParameterNames::saturationCharacter,
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatPercent)
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::saturationMix, 1),
        ParameterNames::saturationMix,
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatPercent)
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::saturationOutput, 1),
        ParameterNames::saturationOutput,
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));

    // -------------------------------------------------------------------------
    // 7. OUTPUT SECTION
    // -------------------------------------------------------------------------
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::outputGain, 1),
        ParameterNames::outputGain,
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatDecibels)
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParameterIDs::globalMix, 1),
        ParameterNames::globalMix,
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(formatPercent)
    ));
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::globalBypass, 1),
        ParameterNames::globalBypass,
        false
    ));

    // -------------------------------------------------------------------------
    // 8. OVERSAMPLING
    // -------------------------------------------------------------------------
    juce::StringArray osChoices = { "1x", "2x", "4x" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParameterIDs::oversampling, 1),
        ParameterNames::oversampling,
        osChoices,
        0
    ));

    // -------------------------------------------------------------------------
    // 9. SPECTRUM ANALYZER (PHASE 11)
    // -------------------------------------------------------------------------
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParameterIDs::analyzerEnabled, 1),
        ParameterNames::analyzerEnabled,
        true
    ));

    return layout;
}
