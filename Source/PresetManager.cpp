#include "PresetManager.h"
#include "Parameters.h"
#include <algorithm>

namespace SJAudio
{

PresetManager::PresetManager()
{
    initFactoryPresets();
    scanUserPresets();
}

void PresetManager::initFactoryPresets()
{
    presets.clear();

    auto addFactory = [this](const juce::String& name, std::initializer_list<std::pair<juce::String, float>> values)
    {
        Preset p;
        p.name = name;
        p.isFactory = true;
        p.file = juce::File();
        p.paramValues.assign(values.begin(), values.end());
        presets.push_back(std::move(p));
    };

    // 1. Clean Vocal
    addFactory("Clean Vocal", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 80.0f },
        { ParameterIDs::lowPassEnabled, 0.0f },
        { ParameterIDs::gateEnabled, 0.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowEnabled, 1.0f },
        { ParameterIDs::lowFreq, 100.0f },
        { ParameterIDs::lowGain, -2.0f },
        { ParameterIDs::lowShelf, 1.0f },
        { ParameterIDs::lowMidEnabled, 1.0f },
        { ParameterIDs::lowMidFreq, 350.0f },
        { ParameterIDs::lowMidGain, -2.0f },
        { ParameterIDs::lowMidQ, 1.4f },
        { ParameterIDs::highMidEnabled, 1.0f },
        { ParameterIDs::highMidFreq, 3500.0f },
        { ParameterIDs::highMidGain, 2.0f },
        { ParameterIDs::highMidQ, 1.2f },
        { ParameterIDs::highEnabled, 1.0f },
        { ParameterIDs::highFreq, 12000.0f },
        { ParameterIDs::highGain, 3.5f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -18.0f },
        { ParameterIDs::compressorRatio, 3.0f },
        { ParameterIDs::compressorAttack, 15.0f },
        { ParameterIDs::compressorRelease, 120.0f },
        { ParameterIDs::compressorKnee, 6.0f },
        { ParameterIDs::compressorMakeup, 2.5f },
        { ParameterIDs::compressorAutoRelease, 1.0f },
        { ParameterIDs::compressorMix, 100.0f },
        { ParameterIDs::dynamicEqEnabled, 1.0f },
        { ParameterIDs::dynamicEqFrequency, 6500.0f },
        { ParameterIDs::dynamicEqGain, 0.0f },
        { ParameterIDs::dynamicEqQ, 2.5f },
        { ParameterIDs::dynamicEqThreshold, -20.0f },
        { ParameterIDs::dynamicEqRange, -5.0f },
        { ParameterIDs::dynamicEqAttack, 4.0f },
        { ParameterIDs::dynamicEqRelease, 60.0f },
        { ParameterIDs::dynamicEqMode, 0.0f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 15.0f },
        { ParameterIDs::saturationCharacter, 40.0f },
        { ParameterIDs::saturationMix, 100.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 2. Vocal Presence
    addFactory("Vocal Presence", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 100.0f },
        { ParameterIDs::gateEnabled, 0.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 120.0f },
        { ParameterIDs::lowGain, -1.5f },
        { ParameterIDs::lowShelf, 1.0f },
        { ParameterIDs::lowMidFreq, 600.0f },
        { ParameterIDs::lowMidGain, -2.5f },
        { ParameterIDs::lowMidQ, 1.5f },
        { ParameterIDs::highMidFreq, 4500.0f },
        { ParameterIDs::highMidGain, 3.5f },
        { ParameterIDs::highMidQ, 1.0f },
        { ParameterIDs::highFreq, 14000.0f },
        { ParameterIDs::highGain, 4.0f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -20.0f },
        { ParameterIDs::compressorRatio, 4.0f },
        { ParameterIDs::compressorAttack, 10.0f },
        { ParameterIDs::compressorRelease, 80.0f },
        { ParameterIDs::compressorKnee, 4.0f },
        { ParameterIDs::compressorMakeup, 3.5f },
        { ParameterIDs::dynamicEqEnabled, 1.0f },
        { ParameterIDs::dynamicEqFrequency, 3800.0f },
        { ParameterIDs::dynamicEqGain, 0.0f },
        { ParameterIDs::dynamicEqQ, 2.0f },
        { ParameterIDs::dynamicEqThreshold, -20.0f },
        { ParameterIDs::dynamicEqRange, -4.0f },
        { ParameterIDs::dynamicEqAttack, 6.0f },
        { ParameterIDs::dynamicEqRelease, 80.0f },
        { ParameterIDs::dynamicEqMode, 0.0f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 25.0f },
        { ParameterIDs::saturationCharacter, 60.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 3. Vocal Warmth
    addFactory("Vocal Warmth", {
        { ParameterIDs::inputGain, 0.5f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 60.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 200.0f },
        { ParameterIDs::lowGain, 2.0f },
        { ParameterIDs::lowShelf, 1.0f },
        { ParameterIDs::lowMidFreq, 800.0f },
        { ParameterIDs::lowMidGain, 1.0f },
        { ParameterIDs::lowMidQ, 0.8f },
        { ParameterIDs::highMidFreq, 3000.0f },
        { ParameterIDs::highMidGain, -1.0f },
        { ParameterIDs::highMidQ, 1.0f },
        { ParameterIDs::highFreq, 10000.0f },
        { ParameterIDs::highGain, 1.0f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -16.0f },
        { ParameterIDs::compressorRatio, 2.5f },
        { ParameterIDs::compressorAttack, 25.0f },
        { ParameterIDs::compressorRelease, 200.0f },
        { ParameterIDs::compressorKnee, 12.0f },
        { ParameterIDs::compressorMakeup, 2.0f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 35.0f },
        { ParameterIDs::saturationCharacter, 25.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 4. Rap Vocal
    addFactory("Rap Vocal", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 110.0f },
        { ParameterIDs::gateEnabled, 1.0f },
        { ParameterIDs::gateThreshold, -42.0f },
        { ParameterIDs::gateRange, 24.0f },
        { ParameterIDs::gateAttack, 1.0f },
        { ParameterIDs::gateHold, 40.0f },
        { ParameterIDs::gateRelease, 80.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 150.0f },
        { ParameterIDs::lowGain, -3.0f },
        { ParameterIDs::lowShelf, 1.0f },
        { ParameterIDs::lowMidFreq, 400.0f },
        { ParameterIDs::lowMidGain, -3.0f },
        { ParameterIDs::lowMidQ, 2.0f },
        { ParameterIDs::highMidFreq, 3000.0f },
        { ParameterIDs::highMidGain, 4.0f },
        { ParameterIDs::highMidQ, 1.5f },
        { ParameterIDs::highFreq, 10000.0f },
        { ParameterIDs::highGain, 3.0f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -24.0f },
        { ParameterIDs::compressorRatio, 6.0f },
        { ParameterIDs::compressorAttack, 3.0f },
        { ParameterIDs::compressorRelease, 60.0f },
        { ParameterIDs::compressorKnee, 2.0f },
        { ParameterIDs::compressorMakeup, 5.0f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 30.0f },
        { ParameterIDs::saturationCharacter, 70.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 5. Aggressive Vocal
    addFactory("Aggressive Vocal", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 120.0f },
        { ParameterIDs::gateEnabled, 1.0f },
        { ParameterIDs::gateThreshold, -36.0f },
        { ParameterIDs::gateRange, 30.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 160.0f },
        { ParameterIDs::lowGain, -2.0f },
        { ParameterIDs::lowMidFreq, 1200.0f },
        { ParameterIDs::lowMidGain, 2.5f },
        { ParameterIDs::lowMidQ, 1.8f },
        { ParameterIDs::highMidFreq, 4000.0f },
        { ParameterIDs::highMidGain, 5.0f },
        { ParameterIDs::highMidQ, 1.2f },
        { ParameterIDs::highFreq, 8000.0f },
        { ParameterIDs::highGain, 4.0f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -26.0f },
        { ParameterIDs::compressorRatio, 8.0f },
        { ParameterIDs::compressorAttack, 1.0f },
        { ParameterIDs::compressorRelease, 50.0f },
        { ParameterIDs::compressorKnee, 0.0f },
        { ParameterIDs::compressorMakeup, 6.0f },
        { ParameterIDs::compressorMix, 90.0f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 55.0f },
        { ParameterIDs::saturationCharacter, 80.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 6. Smooth Vocal
    addFactory("Smooth Vocal", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 75.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 120.0f },
        { ParameterIDs::lowGain, 0.0f },
        { ParameterIDs::lowMidFreq, 500.0f },
        { ParameterIDs::lowMidGain, -1.5f },
        { ParameterIDs::lowMidQ, 1.0f },
        { ParameterIDs::highMidFreq, 2800.0f },
        { ParameterIDs::highMidGain, 1.5f },
        { ParameterIDs::highMidQ, 0.8f },
        { ParameterIDs::highFreq, 15000.0f },
        { ParameterIDs::highGain, 2.5f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -14.0f },
        { ParameterIDs::compressorRatio, 2.0f },
        { ParameterIDs::compressorAttack, 30.0f },
        { ParameterIDs::compressorRelease, 250.0f },
        { ParameterIDs::compressorKnee, 16.0f },
        { ParameterIDs::compressorAutoRelease, 1.0f },
        { ParameterIDs::compressorMakeup, 1.5f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 12.0f },
        { ParameterIDs::saturationCharacter, 35.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 7. Bass Tight
    addFactory("Bass Tight", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 35.0f },
        { ParameterIDs::lowPassEnabled, 1.0f },
        { ParameterIDs::lowPassFrequency, 8000.0f },
        { ParameterIDs::gateEnabled, 1.0f },
        { ParameterIDs::gateMode, 1.0f }, // Expander
        { ParameterIDs::gateThreshold, -45.0f },
        { ParameterIDs::gateRange, 15.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 70.0f },
        { ParameterIDs::lowGain, 3.0f },
        { ParameterIDs::lowShelf, 0.0f }, // Bell
        { ParameterIDs::lowQ, 1.6f },
        { ParameterIDs::lowMidFreq, 250.0f },
        { ParameterIDs::lowMidGain, -4.0f },
        { ParameterIDs::lowMidQ, 2.2f },
        { ParameterIDs::highMidFreq, 1500.0f },
        { ParameterIDs::highMidGain, 2.0f },
        { ParameterIDs::highMidQ, 1.5f },
        { ParameterIDs::highFreq, 5000.0f },
        { ParameterIDs::highGain, -3.0f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -20.0f },
        { ParameterIDs::compressorRatio, 5.0f },
        { ParameterIDs::compressorAttack, 8.0f },
        { ParameterIDs::compressorRelease, 90.0f },
        { ParameterIDs::compressorKnee, 4.0f },
        { ParameterIDs::compressorMakeup, 3.5f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 28.0f },
        { ParameterIDs::saturationCharacter, 45.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 8. Bass Warm
    addFactory("Bass Warm", {
        { ParameterIDs::inputGain, 1.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 30.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 90.0f },
        { ParameterIDs::lowGain, 3.5f },
        { ParameterIDs::lowShelf, 1.0f },
        { ParameterIDs::lowMidFreq, 400.0f },
        { ParameterIDs::lowMidGain, 1.0f },
        { ParameterIDs::lowMidQ, 0.9f },
        { ParameterIDs::highMidFreq, 2000.0f },
        { ParameterIDs::highMidGain, -2.0f },
        { ParameterIDs::highFreq, 7000.0f },
        { ParameterIDs::highGain, -4.0f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -16.0f },
        { ParameterIDs::compressorRatio, 3.0f },
        { ParameterIDs::compressorAttack, 20.0f },
        { ParameterIDs::compressorRelease, 180.0f },
        { ParameterIDs::compressorKnee, 10.0f },
        { ParameterIDs::compressorMakeup, 2.5f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 42.0f },
        { ParameterIDs::saturationCharacter, 20.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 9. Kick Punch
    addFactory("Kick Punch", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 30.0f },
        { ParameterIDs::gateEnabled, 1.0f },
        { ParameterIDs::gateThreshold, -32.0f },
        { ParameterIDs::gateRange, 40.0f },
        { ParameterIDs::gateAttack, 0.5f },
        { ParameterIDs::gateHold, 60.0f },
        { ParameterIDs::gateRelease, 120.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 55.0f },
        { ParameterIDs::lowGain, 4.5f },
        { ParameterIDs::lowShelf, 0.0f }, // Bell
        { ParameterIDs::lowQ, 1.8f },
        { ParameterIDs::lowMidFreq, 380.0f },
        { ParameterIDs::lowMidGain, -6.0f },
        { ParameterIDs::lowMidQ, 2.5f },
        { ParameterIDs::highMidFreq, 3500.0f },
        { ParameterIDs::highMidGain, 4.5f },
        { ParameterIDs::highMidQ, 1.5f },
        { ParameterIDs::highFreq, 10000.0f },
        { ParameterIDs::highGain, -3.0f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -22.0f },
        { ParameterIDs::compressorRatio, 6.0f },
        { ParameterIDs::compressorAttack, 15.0f },
        { ParameterIDs::compressorRelease, 80.0f },
        { ParameterIDs::compressorKnee, 3.0f },
        { ParameterIDs::compressorMakeup, 4.0f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 25.0f },
        { ParameterIDs::saturationCharacter, 55.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 10. Snare Crack
    addFactory("Snare Crack", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 90.0f },
        { ParameterIDs::gateEnabled, 1.0f },
        { ParameterIDs::gateThreshold, -30.0f },
        { ParameterIDs::gateRange, 35.0f },
        { ParameterIDs::gateAttack, 0.3f },
        { ParameterIDs::gateHold, 40.0f },
        { ParameterIDs::gateRelease, 150.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 180.0f },
        { ParameterIDs::lowGain, 3.0f },
        { ParameterIDs::lowShelf, 0.0f },
        { ParameterIDs::lowQ, 1.5f },
        { ParameterIDs::lowMidFreq, 700.0f },
        { ParameterIDs::lowMidGain, -2.0f },
        { ParameterIDs::lowMidQ, 1.8f },
        { ParameterIDs::highMidFreq, 4500.0f },
        { ParameterIDs::highMidGain, 4.0f },
        { ParameterIDs::highMidQ, 1.3f },
        { ParameterIDs::highFreq, 12000.0f },
        { ParameterIDs::highGain, 3.0f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -18.0f },
        { ParameterIDs::compressorRatio, 4.0f },
        { ParameterIDs::compressorAttack, 8.0f },
        { ParameterIDs::compressorRelease, 90.0f },
        { ParameterIDs::compressorKnee, 4.0f },
        { ParameterIDs::compressorMakeup, 3.0f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 35.0f },
        { ParameterIDs::saturationCharacter, 65.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 11. Drum Bus
    addFactory("Drum Bus", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 35.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 65.0f },
        { ParameterIDs::lowGain, 1.5f },
        { ParameterIDs::lowShelf, 1.0f },
        { ParameterIDs::lowMidFreq, 450.0f },
        { ParameterIDs::lowMidGain, -1.5f },
        { ParameterIDs::lowMidQ, 1.2f },
        { ParameterIDs::highMidFreq, 3000.0f },
        { ParameterIDs::highMidGain, 1.0f },
        { ParameterIDs::highMidQ, 0.8f },
        { ParameterIDs::highFreq, 10000.0f },
        { ParameterIDs::highGain, 2.0f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -14.0f },
        { ParameterIDs::compressorRatio, 2.5f },
        { ParameterIDs::compressorAttack, 30.0f },
        { ParameterIDs::compressorRelease, 100.0f },
        { ParameterIDs::compressorAutoRelease, 1.0f },
        { ParameterIDs::compressorKnee, 6.0f },
        { ParameterIDs::compressorMakeup, 2.0f },
        { ParameterIDs::compressorMix, 85.0f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 20.0f },
        { ParameterIDs::saturationCharacter, 50.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 12. Acoustic Guitar
    addFactory("Acoustic Guitar", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 95.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 180.0f },
        { ParameterIDs::lowGain, -2.0f },
        { ParameterIDs::lowShelf, 1.0f },
        { ParameterIDs::lowMidFreq, 400.0f },
        { ParameterIDs::lowMidGain, -2.0f },
        { ParameterIDs::lowMidQ, 1.5f },
        { ParameterIDs::highMidFreq, 3200.0f },
        { ParameterIDs::highMidGain, 2.0f },
        { ParameterIDs::highMidQ, 1.1f },
        { ParameterIDs::highFreq, 11000.0f },
        { ParameterIDs::highGain, 3.5f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -15.0f },
        { ParameterIDs::compressorRatio, 2.5f },
        { ParameterIDs::compressorAttack, 20.0f },
        { ParameterIDs::compressorRelease, 160.0f },
        { ParameterIDs::compressorKnee, 8.0f },
        { ParameterIDs::compressorMakeup, 1.8f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 10.0f },
        { ParameterIDs::saturationCharacter, 30.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 13. Electric Guitar
    addFactory("Electric Guitar", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 80.0f },
        { ParameterIDs::lowPassEnabled, 1.0f },
        { ParameterIDs::lowPassFrequency, 10000.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 120.0f },
        { ParameterIDs::lowGain, 1.0f },
        { ParameterIDs::lowMidFreq, 800.0f },
        { ParameterIDs::lowMidGain, 2.0f },
        { ParameterIDs::lowMidQ, 1.4f },
        { ParameterIDs::highMidFreq, 2800.0f },
        { ParameterIDs::highMidGain, 2.5f },
        { ParameterIDs::highMidQ, 1.2f },
        { ParameterIDs::highFreq, 6000.0f },
        { ParameterIDs::highGain, -1.5f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -18.0f },
        { ParameterIDs::compressorRatio, 3.5f },
        { ParameterIDs::compressorAttack, 10.0f },
        { ParameterIDs::compressorRelease, 120.0f },
        { ParameterIDs::compressorKnee, 4.0f },
        { ParameterIDs::compressorMakeup, 2.5f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 45.0f },
        { ParameterIDs::saturationCharacter, 75.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 14. Synth
    addFactory("Synth", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 40.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 90.0f },
        { ParameterIDs::lowGain, 1.0f },
        { ParameterIDs::lowMidFreq, 500.0f },
        { ParameterIDs::lowMidGain, -1.0f },
        { ParameterIDs::highMidFreq, 2200.0f },
        { ParameterIDs::highMidGain, 2.0f },
        { ParameterIDs::highMidQ, 1.0f },
        { ParameterIDs::highFreq, 9000.0f },
        { ParameterIDs::highGain, 3.0f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -16.0f },
        { ParameterIDs::compressorRatio, 3.0f },
        { ParameterIDs::compressorAttack, 12.0f },
        { ParameterIDs::compressorRelease, 100.0f },
        { ParameterIDs::compressorMakeup, 2.0f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 35.0f },
        { ParameterIDs::saturationCharacter, 60.0f },
        { ParameterIDs::saturationMix, 90.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 15. Keys
    addFactory("Keys", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 50.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 120.0f },
        { ParameterIDs::lowGain, 0.5f },
        { ParameterIDs::lowMidFreq, 350.0f },
        { ParameterIDs::lowMidGain, -1.5f },
        { ParameterIDs::lowMidQ, 1.2f },
        { ParameterIDs::highMidFreq, 3000.0f },
        { ParameterIDs::highMidGain, 1.5f },
        { ParameterIDs::highMidQ, 0.9f },
        { ParameterIDs::highFreq, 12000.0f },
        { ParameterIDs::highGain, 2.0f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -14.0f },
        { ParameterIDs::compressorRatio, 2.0f },
        { ParameterIDs::compressorAttack, 25.0f },
        { ParameterIDs::compressorRelease, 200.0f },
        { ParameterIDs::compressorKnee, 8.0f },
        { ParameterIDs::compressorMakeup, 1.5f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 15.0f },
        { ParameterIDs::saturationCharacter, 35.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 16. Mix Bus
    addFactory("Mix Bus", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 25.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 60.0f },
        { ParameterIDs::lowGain, 1.0f },
        { ParameterIDs::lowShelf, 1.0f },
        { ParameterIDs::lowMidFreq, 400.0f },
        { ParameterIDs::lowMidGain, -0.5f },
        { ParameterIDs::lowMidQ, 0.7f },
        { ParameterIDs::highMidFreq, 2500.0f },
        { ParameterIDs::highMidGain, 0.5f },
        { ParameterIDs::highMidQ, 0.7f },
        { ParameterIDs::highFreq, 12000.0f },
        { ParameterIDs::highGain, 1.2f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -10.0f },
        { ParameterIDs::compressorRatio, 1.5f },
        { ParameterIDs::compressorAttack, 30.0f },
        { ParameterIDs::compressorRelease, 100.0f },
        { ParameterIDs::compressorAutoRelease, 1.0f },
        { ParameterIDs::compressorKnee, 8.0f },
        { ParameterIDs::compressorMakeup, 1.2f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 12.0f },
        { ParameterIDs::saturationCharacter, 45.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 17. Gentle Mix Bus
    addFactory("Gentle Mix Bus", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 20.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 80.0f },
        { ParameterIDs::lowGain, 0.5f },
        { ParameterIDs::lowShelf, 1.0f },
        { ParameterIDs::highFreq, 14000.0f },
        { ParameterIDs::highGain, 0.8f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -8.0f },
        { ParameterIDs::compressorRatio, 1.3f },
        { ParameterIDs::compressorAttack, 40.0f },
        { ParameterIDs::compressorRelease, 150.0f },
        { ParameterIDs::compressorAutoRelease, 1.0f },
        { ParameterIDs::compressorKnee, 12.0f },
        { ParameterIDs::compressorMakeup, 0.8f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 8.0f },
        { ParameterIDs::saturationCharacter, 40.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 18. Punchy Mix
    addFactory("Punchy Mix", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 30.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 70.0f },
        { ParameterIDs::lowGain, 1.5f },
        { ParameterIDs::lowShelf, 1.0f },
        { ParameterIDs::lowMidFreq, 350.0f },
        { ParameterIDs::lowMidGain, -1.0f },
        { ParameterIDs::lowMidQ, 1.0f },
        { ParameterIDs::highMidFreq, 3500.0f },
        { ParameterIDs::highMidGain, 1.0f },
        { ParameterIDs::highMidQ, 0.8f },
        { ParameterIDs::highFreq, 10000.0f },
        { ParameterIDs::highGain, 1.8f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -12.0f },
        { ParameterIDs::compressorRatio, 2.0f },
        { ParameterIDs::compressorAttack, 25.0f },
        { ParameterIDs::compressorRelease, 80.0f },
        { ParameterIDs::compressorKnee, 4.0f },
        { ParameterIDs::compressorMakeup, 1.8f },
        { ParameterIDs::compressorMix, 90.0f },
        { ParameterIDs::saturationEnabled, 1.0f },
        { ParameterIDs::saturationDrive, 18.0f },
        { ParameterIDs::saturationCharacter, 55.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });

    // 19. Clean Master
    addFactory("Clean Master", {
        { ParameterIDs::inputGain, 0.0f },
        { ParameterIDs::highPassEnabled, 1.0f },
        { ParameterIDs::highPassFrequency, 22.0f },
        { ParameterIDs::eqEnabled, 1.0f },
        { ParameterIDs::lowFreq, 50.0f },
        { ParameterIDs::lowGain, 0.3f },
        { ParameterIDs::lowShelf, 1.0f },
        { ParameterIDs::highMidFreq, 3000.0f },
        { ParameterIDs::highMidGain, 0.4f },
        { ParameterIDs::highMidQ, 0.6f },
        { ParameterIDs::highFreq, 15000.0f },
        { ParameterIDs::highGain, 0.8f },
        { ParameterIDs::highShelf, 1.0f },
        { ParameterIDs::compressorEnabled, 1.0f },
        { ParameterIDs::compressorThreshold, -6.0f },
        { ParameterIDs::compressorRatio, 1.2f },
        { ParameterIDs::compressorAttack, 50.0f },
        { ParameterIDs::compressorRelease, 200.0f },
        { ParameterIDs::compressorAutoRelease, 1.0f },
        { ParameterIDs::compressorKnee, 14.0f },
        { ParameterIDs::compressorMakeup, 0.5f },
        { ParameterIDs::saturationEnabled, 0.0f },
        { ParameterIDs::outputGain, 0.0f },
        { ParameterIDs::globalMix, 100.0f }
    });
}

juce::File PresetManager::getUserPresetDirectory() const
{
    auto dir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("SJ Audio")
        .getChildFile("SJ Channel Strip")
        .getChildFile("Presets");

    if (!dir.exists())
        dir.createDirectory();

    return dir;
}

void PresetManager::scanUserPresets()
{
    // Retain only factory presets (first 19 items)
    if (presets.size() > 19)
        presets.erase(presets.begin() + 19, presets.end());

    auto dir = getUserPresetDirectory();
    if (dir.isDirectory())
    {
        auto userFiles = dir.findChildFiles(juce::File::findFiles, false, "*.sjpreset");
        for (const auto& file : userFiles)
        {
            Preset p;
            if (parsePresetFile(file, p))
            {
                presets.push_back(std::move(p));
            }
        }
    }
}

juce::String PresetManager::getPresetName(int index) const
{
    if (index >= 0 && index < static_cast<int>(presets.size()))
        return presets[index].name;

    return {};
}

bool PresetManager::isCurrentPresetFactory() const
{
    if (currentIndex >= 0 && currentIndex < static_cast<int>(presets.size()))
        return presets[currentIndex].isFactory;

    return true;
}

void PresetManager::setCurrentIndex(int index) noexcept
{
    if (!presets.empty())
        currentIndex = juce::jlimit(0, static_cast<int>(presets.size()) - 1, index);
}

bool PresetManager::applyPreset(int index, juce::AudioProcessorValueTreeState& apvts)
{
    if (index < 0 || index >= static_cast<int>(presets.size()))
        return false;

    currentIndex = index;
    const auto& p = presets[index];

    for (const auto& [paramId, val] : p.paramValues)
    {
        if (auto* param = apvts.getParameter(paramId))
        {
            param->setValueNotifyingHost(param->convertTo0to1(val));
        }
    }

    return true;
}

bool PresetManager::applyNextPreset(juce::AudioProcessorValueTreeState& apvts)
{
    if (presets.empty())
        return false;

    int nextIdx = (currentIndex + 1) % static_cast<int>(presets.size());
    return applyPreset(nextIdx, apvts);
}

bool PresetManager::applyPreviousPreset(juce::AudioProcessorValueTreeState& apvts)
{
    if (presets.empty())
        return false;

    int prevIdx = (currentIndex - 1 + static_cast<int>(presets.size())) % static_cast<int>(presets.size());
    return applyPreset(prevIdx, apvts);
}

bool PresetManager::saveCurrentPreset(juce::AudioProcessorValueTreeState& apvts)
{
    if (currentIndex < 0 || currentIndex >= static_cast<int>(presets.size()))
        return false;

    auto& p = presets[currentIndex];
    // Safety: Protect factory presets from ever being overwritten
    if (p.isFactory || !p.file.existsAsFile())
        return false;

    return savePresetToFile(p.name, p.file, apvts);
}

bool PresetManager::savePresetAs(const juce::String& presetName, juce::AudioProcessorValueTreeState& apvts)
{
    auto trimmed = presetName.trim();
    if (trimmed.isEmpty())
        return false;

    auto sanitized = juce::File::createLegalFileName(trimmed);
    auto dir = getUserPresetDirectory();
    auto targetFile = dir.getChildFile(sanitized + ".sjpreset");

    if (savePresetToFile(sanitized, targetFile, apvts))
    {
        scanUserPresets();

        // Switch to the newly saved preset
        for (int i = 0; i < static_cast<int>(presets.size()); ++i)
        {
            if (presets[i].file == targetFile)
            {
                currentIndex = i;
                break;
            }
        }
        return true;
    }

    return false;
}

bool PresetManager::deleteCurrentPreset(juce::AudioProcessorValueTreeState& apvts)
{
    if (currentIndex < 0 || currentIndex >= static_cast<int>(presets.size()))
        return false;

    auto& p = presets[currentIndex];
    // Safety: Factory presets cannot be deleted
    if (p.isFactory)
        return false;

    if (p.file.existsAsFile())
        p.file.deleteFile();

    scanUserPresets();
    currentIndex = std::min(currentIndex, static_cast<int>(presets.size()) - 1);

    if (currentIndex >= 0)
        applyPreset(currentIndex, apvts);

    return true;
}

bool PresetManager::parsePresetFile(const juce::File& file, Preset& outPreset)
{
    if (!file.existsAsFile())
        return false;

    auto xml = juce::parseXML(file);
    if (xml == nullptr || !xml->hasTagName("SJEChannelStripPreset"))
        return false;

    outPreset.name = xml->getStringAttribute("name", file.getFileNameWithoutExtension());
    outPreset.isFactory = false;
    outPreset.file = file;
    outPreset.paramValues.clear();

    if (auto* paramsXml = xml->getChildByName("Parameters"))
    {
        for (auto* pElem : paramsXml->getChildIterator())
        {
            if (pElem->hasTagName("PARAM"))
            {
                auto id = pElem->getStringAttribute("id");
                float val = static_cast<float>(pElem->getDoubleAttribute("value", 0.0));
                if (id.isNotEmpty())
                    outPreset.paramValues.emplace_back(id, val);
            }
        }
    }

    return true;
}

bool PresetManager::savePresetToFile(const juce::String& name, const juce::File& file, juce::AudioProcessorValueTreeState& apvts)
{
    auto root = std::make_unique<juce::XmlElement>("SJEChannelStripPreset");
    root->setAttribute("version", "1.0");
    root->setAttribute("manufacturer", "SJ Audio");
    root->setAttribute("name", name);

    auto* paramsXml = root->createNewChildElement("Parameters");

    // Enumerate all active parameters from APVTS state
    auto state = apvts.copyState();
    for (int i = 0; i < state.getNumProperties(); ++i)
    {
        auto propName = state.getPropertyName(i).toString();
        auto val = state.getProperty(propName);
        auto* elem = paramsXml->createNewChildElement("PARAM");
        elem->setAttribute("id", propName);
        elem->setAttribute("value", static_cast<double>(val));
    }

    // Also query APVTS directly for raw parameter values to ensure 100% parameter coverage
    for (auto* param : apvts.processor.getParameters())
    {
        if (auto* withID = dynamic_cast<juce::AudioProcessorParameterWithID*>(param))
        {
            auto id = withID->paramID;
            if (auto* raw = apvts.getRawParameterValue(id))
            {
                bool found = false;
                for (auto* p : paramsXml->getChildIterator())
                {
                    if (p->getStringAttribute("id") == id)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    auto* elem = paramsXml->createNewChildElement("PARAM");
                    elem->setAttribute("id", id);
                    elem->setAttribute("value", static_cast<double>(raw->load()));
                }
            }
        }
    }

    return root->writeTo(file, juce::XmlElement::TextFormat());
}

} // namespace SJAudio
