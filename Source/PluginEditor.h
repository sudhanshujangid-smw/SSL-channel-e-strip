#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "GUI/SJLookAndFeel.h"
#include "GUI/SJComponents.h"
#include "GUI/Meter.h"
#include "GUI/FilterDisplay.h"
#include "GUI/EQDisplay.h"
#include "GUI/GateGRMeter.h"
#include "GUI/CompressorGRMeter.h"
#include "GUI/SaturationMeter.h"
#include "GUI/DynamicEQMeter.h"

class SJChannelStripAudioProcessorEditor : public juce::AudioProcessorEditor,
                                           private juce::Timer
{
public:
    explicit SJChannelStripAudioProcessorEditor(SJChannelStripAudioProcessor&);
    ~SJChannelStripAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    SJChannelStripAudioProcessor& processorRef;

    // Custom SJ Audio LookAndFeel
    SJAudio::SJLookAndFeel lnf;

    // Built-in Tooltip Window for accessibility and user guidance
    juce::TooltipWindow tooltipWindow { this, 650 };

    // Top Header with Branding and Preset Browser
    SJAudio::SJHeader header;

    // 1. Input Section Controls
    SJAudio::SJKnob         inputGainSlider;
    SJAudio::SJLabel        inputGainLabel;
    SJAudio::SJToggleButton polarityButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> polarityAttachment;

    // 2. Filter Section Controls
    SJAudio::SJToggleButton hpOnButton;
    SJAudio::SJKnob         hpFreqSlider;
    SJAudio::SJLabel        hpFreqLabel;
    juce::ComboBox          hpSlopeBox;
    SJAudio::SJLabel        hpSlopeLabel;

    SJAudio::SJToggleButton lpOnButton;
    SJAudio::SJKnob         lpFreqSlider;
    SJAudio::SJLabel        lpFreqLabel;
    juce::ComboBox          lpSlopeBox;
    SJAudio::SJLabel        lpSlopeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>   hpOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   hpFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> hpSlopeAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>   lpOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   lpFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lpSlopeAttachment;

    SJAudio::FilterDisplay filterDisplay;

    // 3. 4-Band Parametric EQ Controls
    SJAudio::SJToggleButton eqMasterButton;

    // Low Band
    SJAudio::SJToggleButton lowOnButton;
    SJAudio::SJKnob         lowFreqSlider, lowGainSlider, lowQSlider;
    SJAudio::SJLabel        lowFreqLabel, lowGainLabel, lowQLabel;
    SJAudio::SJToggleButton lowShelfButton;

    // Low-Mid Band
    SJAudio::SJToggleButton lowMidOnButton;
    SJAudio::SJKnob         lowMidFreqSlider, lowMidGainSlider, lowMidQSlider;
    SJAudio::SJLabel        lowMidFreqLabel, lowMidGainLabel, lowMidQLabel;

    // High-Mid Band
    SJAudio::SJToggleButton highMidOnButton;
    SJAudio::SJKnob         highMidFreqSlider, highMidGainSlider, highMidQSlider;
    SJAudio::SJLabel        highMidFreqLabel, highMidGainLabel, highMidQLabel;

    // High Band
    SJAudio::SJToggleButton highOnButton;
    SJAudio::SJKnob         highFreqSlider, highGainSlider, highQSlider;
    SJAudio::SJLabel        highFreqLabel, highGainLabel, highQLabel;
    SJAudio::SJToggleButton highShelfButton;

    // EQ Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> eqMasterAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lowOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowQAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lowShelfAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lowMidOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowMidFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowMidGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowMidQAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> highMidOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highMidFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highMidGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highMidQAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> highOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highQAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> highShelfAttachment;

    SJAudio::EQDisplay eqDisplay;

    // EQ Module Tab Navigation (4-Band vs Dynamic EQ)
    int currentEqTab = 0; // 0 = 4-Band EQ, 1 = Dynamic EQ
    juce::TextButton tab4BandBtn;
    juce::TextButton tabDynEqBtn;
    void updateEqTabVisibility();

    // 3b. Dynamic EQ Controls (Priority 1)
    SJAudio::SJToggleButton dynEqOnButton;
    SJAudio::SJToggleButton dynEqListenButton;
    juce::ComboBox          dynEqModeBox;
    SJAudio::SJLabel        dynEqModeLabel;

    SJAudio::SJKnob         dynEqFreqSlider, dynEqGainSlider, dynEqQSlider;
    SJAudio::SJLabel        dynEqFreqLabel, dynEqGainLabel, dynEqQLabel;

    SJAudio::SJKnob         dynEqThreshSlider, dynEqRangeSlider, dynEqAttackSlider, dynEqReleaseSlider;
    SJAudio::SJLabel        dynEqThreshLabel, dynEqRangeLabel, dynEqAttackLabel, dynEqReleaseLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>   dynEqOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>   dynEqListenAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> dynEqModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   dynEqFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   dynEqGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   dynEqQAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   dynEqThreshAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   dynEqRangeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   dynEqAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   dynEqReleaseAttachment;

    SJAudio::DynamicEQMeter dynEqMeter;

    // 4. Gate / Expander Controls
    SJAudio::SJToggleButton gateOnButton;
    juce::ComboBox          gateModeBox;
    SJAudio::SJLabel        gateModeLabel;
    SJAudio::SJKnob         gateThresholdSlider, gateRangeSlider, gateAttackSlider, gateHoldSlider, gateReleaseSlider;
    SJAudio::SJLabel        gateThresholdLabel, gateRangeLabel, gateAttackLabel, gateHoldLabel, gateReleaseLabel;

    SJAudio::SJToggleButton gateScOnButton;
    SJAudio::SJKnob         gateScHpfSlider, gateScLpfSlider;
    SJAudio::SJLabel        gateScHpfLabel, gateScLpfLabel;
    juce::ComboBox          gateScSourceBox;
    SJAudio::SJLabel        gateScSourceLabel;
    SJAudio::SJToggleButton gateScListenButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>   gateOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> gateModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   gateThresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   gateRangeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   gateAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   gateHoldAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   gateReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>   gateScOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   gateScHpfAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   gateScLpfAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> gateScSourceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>   gateScListenAttachment;

    SJAudio::GateGRMeter gateMeter;

    // 5. Compressor Controls
    SJAudio::SJToggleButton compOnButton;
    SJAudio::SJToggleButton compAutoRelButton;
    SJAudio::SJKnob         compThresholdSlider, compRatioSlider, compAttackSlider, compReleaseSlider;
    SJAudio::SJLabel        compThresholdLabel, compRatioLabel, compAttackLabel, compReleaseLabel;
    SJAudio::SJKnob         compKneeSlider, compMakeupSlider, compMixSlider;
    SJAudio::SJLabel        compKneeLabel, compMakeupLabel, compMixLabel;

    SJAudio::SJToggleButton compScOnButton;
    juce::ComboBox          compScSourceBox;
    SJAudio::SJLabel        compScSourceLabel;
    SJAudio::SJToggleButton compScListenButton;
    SJAudio::SJKnob         compScHpfSlider, compScLpfSlider;
    SJAudio::SJLabel        compScHpfLabel, compScLpfLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> compOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> compAutoRelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compThresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compRatioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compKneeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compMakeupAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compMixAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>   compScOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> compScSourceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>   compScListenAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   compScHpfAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   compScLpfAttachment;

    SJAudio::CompressorGRMeter compMeter;

    // 6. Analog Color (Saturation) Controls
    SJAudio::SJToggleButton satOnButton;
    SJAudio::SJKnob         satDriveSlider, satCharacterSlider, satMixSlider, satOutputSlider;
    SJAudio::SJLabel        satDriveLabel, satCharacterLabel, satMixLabel, satOutputLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> satOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> satDriveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> satCharacterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> satMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> satOutputAttachment;

    SJAudio::SaturationMeter satMeter;

    // 7. Output Section Controls
    SJAudio::SJKnob         outputGainSlider, globalMixSlider;
    SJAudio::SJLabel        outputGainLabel, globalMixLabel;
    SJAudio::SJToggleButton bypassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> globalMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    // 8. Metering Engines
    SJAudio::StereoBarMeter   inputMeter;
    SJAudio::StereoBarMeter   outputMeter;
    SJAudio::VUMeterComponent vuMeter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SJChannelStripAudioProcessorEditor)
};
