#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SJChannelStripAudioProcessorEditor::SJChannelStripAudioProcessorEditor(SJChannelStripAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      header(p.getAPVTS(), p.getPresetManager()),
      filterDisplay(p.getFilterSection()),
      eqDisplay(p.getAPVTS(), p.getEQSection(), p.getFilterSection(), p.getSpectrumFIFO(), &p.getDynamicEQ()),
      dynEqMeter(p.getDynamicEQ()),
      gateMeter(p.getGateExpander()),
      compMeter(p.getCompressor()),
      satMeter(p.getSaturation()),
      inputMeter("INPUT", p.getInputMeter()),
      outputMeter("OUTPUT", p.getOutputMeter()),
      vuMeter(p.getOutputMeter())
{
    setLookAndFeel(&lnf);

    setSize(1280, 720);
    setResizable(true, true);
    setResizeLimits(900, 550, 1920, 1080);

    // 0. Top Header
    addAndMakeVisible(header);

    // -------------------------------------------------------------------------
    // 1. INPUT SECTION
    // -------------------------------------------------------------------------
    inputGainSlider.setAccentColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentInput));
    inputGainSlider.setTextValueSuffix(" dB");
    inputGainSlider.setTooltip("Input Trim Gain (-24 to +24 dB): Pre-gain stage for optimal signal level");
    addAndMakeVisible(inputGainSlider);

    inputGainLabel.setText("GAIN", juce::dontSendNotification);
    inputGainLabel.setTooltip("Input Trim Gain (-24 to +24 dB)");
    addAndMakeVisible(inputGainLabel);

    polarityButton.setButtonText("POLARITY (Ø)");
    polarityButton.setLedColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentInput));
    polarityButton.setTooltip("Polarity Invert (Ø): 180° phase inversion for multi-mic alignment");
    addAndMakeVisible(polarityButton);

    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getAPVTS(), ParameterIDs::inputGain, inputGainSlider);
    polarityAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.getAPVTS(), ParameterIDs::polarity, polarityButton);

    addAndMakeVisible(inputMeter);

    // -------------------------------------------------------------------------
    // 2. FILTER SECTION
    // -------------------------------------------------------------------------
    addAndMakeVisible(filterDisplay);

    hpOnButton.setButtonText("HPF ON");
    hpOnButton.setLedColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentFilter));
    hpOnButton.setTooltip("High-Pass Filter Enable: Engages Butterworth low-cut filter");
    addAndMakeVisible(hpOnButton);

    hpFreqSlider.setAccentColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentFilter));
    hpFreqSlider.setTooltip("HPF Frequency (20 Hz - 1000 Hz): Removes unwanted low-end rumble");
    addAndMakeVisible(hpFreqSlider);

    hpFreqLabel.setText("HP FREQ", juce::dontSendNotification);
    addAndMakeVisible(hpFreqLabel);

    hpSlopeLabel.setText("SLOPE", juce::dontSendNotification);
    addAndMakeVisible(hpSlopeLabel);

    hpSlopeBox.addItem("12 dB/oct", 1);
    hpSlopeBox.addItem("18 dB/oct", 2);
    hpSlopeBox.addItem("24 dB/oct", 3);
    hpSlopeBox.setTooltip("HPF Slope: 12, 18, or 24 dB/octave Butterworth roll-off");
    addAndMakeVisible(hpSlopeBox);

    lpOnButton.setButtonText("LPF ON");
    lpOnButton.setLedColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentFilter));
    lpOnButton.setTooltip("Low-Pass Filter Enable: Engages Butterworth high-cut filter");
    addAndMakeVisible(lpOnButton);

    lpFreqSlider.setAccentColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentFilter));
    lpFreqSlider.setTooltip("LPF Frequency (1 kHz - 20 kHz): Tames excessive high frequencies");
    addAndMakeVisible(lpFreqSlider);

    lpFreqLabel.setText("LP FREQ", juce::dontSendNotification);
    addAndMakeVisible(lpFreqLabel);

    lpSlopeLabel.setText("SLOPE", juce::dontSendNotification);
    addAndMakeVisible(lpSlopeLabel);

    lpSlopeBox.addItem("12 dB/oct", 1);
    lpSlopeBox.addItem("18 dB/oct", 2);
    lpSlopeBox.addItem("24 dB/oct", 3);
    lpSlopeBox.setTooltip("LPF Slope: 12, 18, or 24 dB/octave Butterworth roll-off");
    addAndMakeVisible(lpSlopeBox);

    hpOnAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::highPassEnabled, hpOnButton);
    hpFreqAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::highPassFrequency, hpFreqSlider);
    hpSlopeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processorRef.getAPVTS(), ParameterIDs::highPassSlope, hpSlopeBox);

    lpOnAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::lowPassEnabled, lpOnButton);
    lpFreqAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::lowPassFrequency, lpFreqSlider);
    lpSlopeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processorRef.getAPVTS(), ParameterIDs::lowPassSlope, lpSlopeBox);

    // -------------------------------------------------------------------------
    // 3. GATE / EXPANDER SECTION
    // -------------------------------------------------------------------------
    gateOnButton.setButtonText("GATE ON");
    gateOnButton.setLedColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentGate));
    gateOnButton.setTooltip("Gate/Expander Enable: Toggles noise gating and downward expansion");
    addAndMakeVisible(gateOnButton);

    gateModeBox.addItem("GATE", 1);
    gateModeBox.addItem("EXPANDER", 2);
    gateModeBox.setTooltip("Dynamics Mode: Switch between hard Gate and smooth 2:1 Expander");
    addAndMakeVisible(gateModeBox);

    auto setupGateKnob = [this](juce::Slider& sld, juce::Label& label, const juce::String& name, const juce::String& suffix, const juce::String& tip = {})
    {
        sld.setTextValueSuffix(suffix);
        sld.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(SJAudio::SJLookAndFeel::colAccentGate));
        if (tip.isNotEmpty())
        {
            sld.setTooltip(tip);
            label.setTooltip(tip);
        }
        addAndMakeVisible(sld);

        label.setText(name, juce::dontSendNotification);
        addAndMakeVisible(label);
    };

    setupGateKnob(gateThresholdSlider, gateThresholdLabel, "THRESH",  " dB", "Gate Threshold (-60 to +10 dB): Signal level below which gate closes");
    setupGateKnob(gateRangeSlider,     gateRangeLabel,     "RANGE",   " dB", "Gate Range (-80 to 0 dB): Maximum attenuation applied when closed");
    setupGateKnob(gateAttackSlider,    gateAttackLabel,    "ATTACK",  " ms", "Gate Attack Time (0.01 to 50 ms): Opening speed when signal exceeds threshold");
    setupGateKnob(gateHoldSlider,      gateHoldLabel,      "HOLD",    " ms", "Gate Hold Time (0 to 500 ms): Duration gate remains open after signal falls below threshold");
    setupGateKnob(gateReleaseSlider,   gateReleaseLabel,   "RELEASE", " ms", "Gate Release Time (5 to 1000 ms): Rate at which gate closes after hold time");

    gateScOnButton.setButtonText("SC FILTER");
    gateScOnButton.setLedColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentGate));
    gateScOnButton.setTooltip("Gate Sidechain Filter Enable: Dedicated detector bandpass filtering");
    addAndMakeVisible(gateScOnButton);

    setupGateKnob(gateScHpfSlider, gateScHpfLabel, "SC HPF", " Hz", "Gate Sidechain HPF (20 Hz - 4 kHz): Eliminates low-end bleed from detector");
    setupGateKnob(gateScLpfSlider, gateScLpfLabel, "SC LPF", " Hz", "Gate Sidechain LPF (500 Hz - 20 kHz): Eliminates high-frequency spill from detector");

    gateScSourceBox.addItem("INT", 1);
    gateScSourceBox.addItem("EXT", 2);
    gateScSourceBox.setTooltip("Gate Detector Source: Internal audio or External sidechain bus");
    addAndMakeVisible(gateScSourceBox);

    gateScSourceLabel.setText("SC SRC", juce::dontSendNotification);
    addAndMakeVisible(gateScSourceLabel);

    gateScListenButton.setButtonText("LISTEN");
    gateScListenButton.setLedColor(juce::Colour(0xfff39c12));
    gateScListenButton.setTooltip("Gate Sidechain Audition: Listen directly to filtered detector audio");
    addAndMakeVisible(gateScListenButton);

    addAndMakeVisible(gateMeter);

    gateOnAttachment        = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::gateEnabled, gateOnButton);
    gateModeAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processorRef.getAPVTS(), ParameterIDs::gateMode, gateModeBox);
    gateThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::gateThreshold, gateThresholdSlider);
    gateRangeAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::gateRange, gateRangeSlider);
    gateAttackAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::gateAttack, gateAttackSlider);
    gateHoldAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::gateHold, gateHoldSlider);
    gateReleaseAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::gateRelease, gateReleaseSlider);

    gateScOnAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::gateSidechainEnabled, gateScOnButton);
    gateScHpfAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::gateSidechainHighPass, gateScHpfSlider);
    gateScLpfAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::gateSidechainLowPass, gateScLpfSlider);
    gateScSourceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processorRef.getAPVTS(), ParameterIDs::gateSidechainSource, gateScSourceBox);
    gateScListenAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::gateSidechainListen, gateScListenButton);

    // -------------------------------------------------------------------------
    // 4. 4-BAND PARAMETRIC EQ SECTION
    // -------------------------------------------------------------------------
    addAndMakeVisible(eqDisplay);

    eqMasterButton.setButtonText("EQ ON");
    eqMasterButton.setLedColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentEQ));
    eqMasterButton.setTooltip("Equalizer Master Bypass: Toggles all 4 parametric EQ bands");
    addAndMakeVisible(eqMasterButton);

    auto setupBand = [this](SJAudio::SJToggleButton& onBtn, juce::Colour col,
                            juce::Slider& fKnob, juce::Label& fLbl,
                            juce::Slider& gKnob, juce::Label& gLbl,
                            juce::Slider& qKnob, juce::Label& qLbl,
                            SJAudio::SJToggleButton* shelfBtn, const juce::String& bName,
                            const juce::String& freqTip, const juce::String& gainTip, const juce::String& qTip)
    {
        onBtn.setButtonText(bName + " ON");
        onBtn.setLedColor(col);
        onBtn.setTooltip(bName + " Band Enable: Toggle " + bName + " frequency band");
        addAndMakeVisible(onBtn);

        fKnob.setColour(juce::Slider::rotarySliderFillColourId, col);
        fKnob.setTooltip(freqTip);
        fLbl.setTooltip(freqTip);
        addAndMakeVisible(fKnob);
        fLbl.setText("FREQ", juce::dontSendNotification);
        addAndMakeVisible(fLbl);

        gKnob.setColour(juce::Slider::rotarySliderFillColourId, col);
        gKnob.setTextValueSuffix(" dB");
        gKnob.setTooltip(gainTip);
        gLbl.setTooltip(gainTip);
        addAndMakeVisible(gKnob);
        gLbl.setText("GAIN", juce::dontSendNotification);
        addAndMakeVisible(gLbl);

        qKnob.setColour(juce::Slider::rotarySliderFillColourId, col);
        qKnob.setTooltip(qTip);
        qLbl.setTooltip(qTip);
        addAndMakeVisible(qKnob);
        qLbl.setText("Q", juce::dontSendNotification);
        addAndMakeVisible(qLbl);

        if (shelfBtn != nullptr)
        {
            shelfBtn->setButtonText("SHELF");
            shelfBtn->setLedColor(col);
            shelfBtn->setTooltip(bName + " Band Shelf Mode: Toggle between Bell peak and Shelf filter");
            addAndMakeVisible(*shelfBtn);
        }
    };

    setupBand(lowOnButton, juce::Colour(0xffe74c3c), lowFreqSlider, lowFreqLabel, lowGainSlider, lowGainLabel, lowQSlider, lowQLabel, &lowShelfButton, "LOW",
              "Low Frequency (20 Hz - 500 Hz): Center or corner frequency",
              "Low Gain (-15 to +15 dB): Boost or cut low frequencies",
              "Low Q (0.5 to 5.0): Bandwidth curve sharpness (bell mode)");
    setupBand(lowMidOnButton, juce::Colour(0xfff39c12), lowMidFreqSlider, lowMidFreqLabel, lowMidGainSlider, lowMidGainLabel, lowMidQSlider, lowMidQLabel, nullptr, "L-MID",
              "Low-Mid Frequency (200 Hz - 3000 Hz): Center frequency for body and warmth",
              "Low-Mid Gain (-15 to +15 dB): Boost or cut lower midrange",
              "Low-Mid Q (0.5 to 5.0): Bandwidth sharpness for tonal shaping");
    setupBand(highMidOnButton, juce::Colour(0xff2ecc71), highMidFreqSlider, highMidFreqLabel, highMidGainSlider, highMidGainLabel, highMidQSlider, highMidQLabel, nullptr, "H-MID",
              "High-Mid Frequency (600 Hz - 7000 Hz): Center frequency for vocal presence",
              "High-Mid Gain (-15 to +15 dB): Boost or cut upper midrange",
              "High-Mid Q (0.5 to 5.0): Bandwidth sharpness for vocal resonance control");
    setupBand(highOnButton, juce::Colour(0xff9b59b6), highFreqSlider, highFreqLabel, highGainSlider, highGainLabel, highQSlider, highQLabel, &highShelfButton, "HIGH",
              "High Frequency (1.5 kHz - 20 kHz): Center or corner frequency for air and brightness",
              "High Gain (-15 to +15 dB): Boost or cut high frequencies",
              "High Q (0.5 to 5.0): Bandwidth curve sharpness (bell mode)");

    eqMasterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::eqEnabled, eqMasterButton);

    lowOnAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::lowEnabled, lowOnButton);
    lowFreqAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::lowFreq, lowFreqSlider);
    lowGainAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::lowGain, lowGainSlider);
    lowQAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::lowQ, lowQSlider);
    lowShelfAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::lowShelf, lowShelfButton);

    lowMidOnAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::lowMidEnabled, lowMidOnButton);
    lowMidFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::lowMidFreq, lowMidFreqSlider);
    lowMidGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::lowMidGain, lowMidGainSlider);
    lowMidQAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::lowMidQ, lowMidQSlider);

    highMidOnAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::highMidEnabled, highMidOnButton);
    highMidFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::highMidFreq, highMidFreqSlider);
    highMidGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::highMidGain, highMidGainSlider);
    highMidQAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::highMidQ, highMidQSlider);

    highOnAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::highEnabled, highOnButton);
    highFreqAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::highFreq, highFreqSlider);
    highGainAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::highGain, highGainSlider);
    highQAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::highQ, highQSlider);
    highShelfAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::highShelf, highShelfButton);

    // -------------------------------------------------------------------------
    // 4b. EQUALIZER TAB BUTTONS & DYNAMIC EQ SECTION (Priority 1)
    // -------------------------------------------------------------------------
    tab4BandBtn.setButtonText("4-BAND EQ");
    tab4BandBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff22252e));
    tab4BandBtn.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff3a4150));
    tab4BandBtn.setColour(juce::TextButton::textColourOnId, juce::Colour(0xfff3ba4f));
    tab4BandBtn.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff8b949e));
    tab4BandBtn.setTooltip("Switch to 4-Band Parametric EQ controls");
    tab4BandBtn.setClickingTogglesState(false);
    tab4BandBtn.onClick = [this]() {
        currentEqTab = 0;
        updateEqTabVisibility();
    };
    addAndMakeVisible(tab4BandBtn);

    tabDynEqBtn.setButtonText("DYNAMIC EQ");
    tabDynEqBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff22252e));
    tabDynEqBtn.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff3a4150));
    tabDynEqBtn.setColour(juce::TextButton::textColourOnId, juce::Colour(0xff00d2ff));
    tabDynEqBtn.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff8b949e));
    tabDynEqBtn.setTooltip("Switch to Dynamic EQ controls with real-time detection & dynamics");
    tabDynEqBtn.setClickingTogglesState(false);
    tabDynEqBtn.onClick = [this]() {
        currentEqTab = 1;
        updateEqTabVisibility();
    };
    addAndMakeVisible(tabDynEqBtn);

    dynEqOnButton.setButtonText("DYN EQ ON");
    dynEqOnButton.setLedColor(juce::Colour(0xff00d2ff));
    dynEqOnButton.setTooltip("Dynamic EQ Enable: Engages level-dependent frequency equalization");
    addChildComponent(dynEqOnButton);

    dynEqListenButton.setButtonText("LISTEN");
    dynEqListenButton.setLedColor(juce::Colour(0xfff39c12));
    dynEqListenButton.setTooltip("Dynamic EQ Audition: Listen directly to bandpass detector signal");
    addChildComponent(dynEqListenButton);

    dynEqModeBox.addItem("COMPRESS (CUT)", 1);
    dynEqModeBox.addItem("EXPAND (BOOST)", 2);
    dynEqModeBox.setTooltip("Dynamic EQ Mode: Compress (cuts resonant peaks) or Expand (boosts frequencies)");
    addChildComponent(dynEqModeBox);

    dynEqModeLabel.setText("MODE", juce::dontSendNotification);
    addChildComponent(dynEqModeLabel);

    auto setupDynKnob = [this](juce::Slider& sld, juce::Label& label, const juce::String& name, const juce::String& suffix, juce::Colour col, const juce::String& tip = {})
    {
        sld.setTextValueSuffix(suffix);
        sld.setColour(juce::Slider::rotarySliderFillColourId, col);
        if (tip.isNotEmpty())
        {
            sld.setTooltip(tip);
            label.setTooltip(tip);
        }
        addChildComponent(sld);

        label.setText(name, juce::dontSendNotification);
        addChildComponent(label);
    };

    const juce::Colour colDynCyan(0xff00d2ff);
    const juce::Colour colDynAmber(0xfff39c12);

    setupDynKnob(dynEqFreqSlider,    dynEqFreqLabel,    "FREQ",      " Hz", colDynCyan,  "Dynamic EQ Frequency (20 Hz - 20 kHz): Target resonance or de-essing frequency");
    setupDynKnob(dynEqGainSlider,    dynEqGainLabel,    "STATIC",    " dB", colDynCyan,  "Dynamic EQ Static Gain (-12 to +12 dB): Base static boost or cut before dynamic action");
    setupDynKnob(dynEqQSlider,       dynEqQLabel,       "Q",         "",    colDynCyan,  "Dynamic EQ Q (0.5 to 10.0): Filter selectivity; higher Q targets narrow harsh resonances");
    setupDynKnob(dynEqThreshSlider,  dynEqThreshLabel,  "THRESH",    " dB", colDynAmber, "Dynamic EQ Threshold (-60 to 0 dB): Signal level where dynamic action begins");
    setupDynKnob(dynEqRangeSlider,   dynEqRangeLabel,   "RANGE",     " dB", colDynAmber, "Dynamic EQ Range (0 to 24 dB): Maximum allowable dynamic cut or boost applied");
    setupDynKnob(dynEqAttackSlider,  dynEqAttackLabel,  "ATTACK",    " ms", colDynAmber, "Dynamic EQ Attack Time (0.1 to 100 ms): Speed of dynamic attenuation onset");
    setupDynKnob(dynEqReleaseSlider, dynEqReleaseLabel, "RELEASE",   " ms", colDynAmber, "Dynamic EQ Release Time (10 to 1000 ms): Recovery speed after signal drops below threshold");

    addChildComponent(dynEqMeter);

    dynEqOnAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::dynamicEqEnabled, dynEqOnButton);
    dynEqListenAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::dynamicEqListen, dynEqListenButton);
    dynEqModeAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processorRef.getAPVTS(), ParameterIDs::dynamicEqMode, dynEqModeBox);
    dynEqFreqAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::dynamicEqFrequency, dynEqFreqSlider);
    dynEqGainAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::dynamicEqGain, dynEqGainSlider);
    dynEqQAttachment       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::dynamicEqQ, dynEqQSlider);
    dynEqThreshAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::dynamicEqThreshold, dynEqThreshSlider);
    dynEqRangeAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::dynamicEqRange, dynEqRangeSlider);
    dynEqAttackAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::dynamicEqAttack, dynEqAttackSlider);
    dynEqReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::dynamicEqRelease, dynEqReleaseSlider);

    // -------------------------------------------------------------------------
    // 5. COMPRESSOR SECTION
    // -------------------------------------------------------------------------
    auto setupCompKnob = [this](juce::Slider& sld, juce::Label& label, const juce::String& name, const juce::String& suffix, const juce::String& tip = {})
    {
        sld.setTextValueSuffix(suffix);
        sld.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(SJAudio::SJLookAndFeel::colAccentComp));
        if (tip.isNotEmpty())
        {
            sld.setTooltip(tip);
            label.setTooltip(tip);
        }
        addAndMakeVisible(sld);

        label.setText(name, juce::dontSendNotification);
        addAndMakeVisible(label);
    };

    compOnButton.setButtonText("COMP ON");
    compOnButton.setLedColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentComp));
    compOnButton.setTooltip("VCA Compressor Enable: Toggles analog-modeled soft-knee dynamic compression");
    addAndMakeVisible(compOnButton);

    compAutoRelButton.setButtonText("AUTO REL");
    compAutoRelButton.setLedColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentColor));
    compAutoRelButton.setTooltip("Auto-Release Mode: Program-dependent dual-timeconstant release for transparent smoothing");
    addAndMakeVisible(compAutoRelButton);

    setupCompKnob(compThresholdSlider, compThresholdLabel, "THRESH",  " dB", "Compressor Threshold (-40 to +10 dB): Signal level where compression begins");
    setupCompKnob(compRatioSlider,     compRatioLabel,     "RATIO",   ":1",  "Compressor Ratio (1:1 to 20:1): Compression slope steepness");
    setupCompKnob(compAttackSlider,    compAttackLabel,    "ATTACK",  " ms", "Compressor Attack Time (0.1 to 50 ms): Reaction speed to signal transients");
    setupCompKnob(compReleaseSlider,   compReleaseLabel,   "RELEASE", " ms", "Compressor Release Time (20 to 1200 ms): Recovery time (overridden in Auto-Rel)");
    setupCompKnob(compKneeSlider,      compKneeLabel,      "KNEE",    " dB", "Compressor Knee Width (0 to 18 dB): Smooth transition between linear and compressed gain");
    setupCompKnob(compMakeupSlider,    compMakeupLabel,    "MAKEUP",  " dB", "Compressor Makeup Gain (-12 to +24 dB): Compensate for dynamic gain reduction");
    setupCompKnob(compMixSlider,       compMixLabel,       "MIX",     " %",  "Compressor Parallel Mix (0 - 100%): Blend dry unprocessed audio with compressed audio");

    compScOnButton.setButtonText("SC FILTER");
    compScOnButton.setLedColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentComp));
    compScOnButton.setTooltip("Compressor Sidechain Filter Enable: Dedicated HPF/LPF detector filter");
    addAndMakeVisible(compScOnButton);

    setupCompKnob(compScHpfSlider, compScHpfLabel, "SC HPF", " Hz", "Compressor Sidechain HPF (20 Hz - 2 kHz): Prevents low bass pumping from triggering compression");
    setupCompKnob(compScLpfSlider, compScLpfLabel, "SC LPF", " Hz", "Compressor Sidechain LPF (500 Hz - 20 kHz): Ignores high-frequency harshness in detector");

    compScSourceBox.addItem("INT", 1);
    compScSourceBox.addItem("EXT", 2);
    compScSourceBox.setTooltip("Compressor Detector Source: Internal audio or External sidechain bus (e.g. kick ducking)");
    addAndMakeVisible(compScSourceBox);

    compScSourceLabel.setText("SC SRC", juce::dontSendNotification);
    addAndMakeVisible(compScSourceLabel);

    compScListenButton.setButtonText("LISTEN");
    compScListenButton.setLedColor(juce::Colour(0xfff39c12));
    compScListenButton.setTooltip("Compressor Sidechain Audition: Listen directly to filtered detector audio");
    addAndMakeVisible(compScListenButton);

    addAndMakeVisible(compMeter);

    compOnAttachment        = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::compressorEnabled, compOnButton);
    compAutoRelAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::compressorAutoRelease, compAutoRelButton);
    compThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::compressorThreshold, compThresholdSlider);
    compRatioAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::compressorRatio, compRatioSlider);
    compAttackAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::compressorAttack, compAttackSlider);
    compReleaseAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::compressorRelease, compReleaseSlider);
    compKneeAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::compressorKnee, compKneeSlider);
    compMakeupAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::compressorMakeup, compMakeupSlider);
    compMixAttachment       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::compressorMix, compMixSlider);

    compScOnAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::compressorSidechainEnabled, compScOnButton);
    compScHpfAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::compressorSidechainHighPass, compScHpfSlider);
    compScLpfAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::compressorSidechainLowPass, compScLpfSlider);
    compScSourceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processorRef.getAPVTS(), ParameterIDs::compressorSidechainSource, compScSourceBox);
    compScListenAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::compressorSidechainListen, compScListenButton);

    // -------------------------------------------------------------------------
    // 6. ANALOG COLOR (SATURATION) SECTION
    // -------------------------------------------------------------------------
    auto setupSatKnob = [this](juce::Slider& sld, juce::Label& label, const juce::String& name, const juce::String& suffix, const juce::String& tip = {})
    {
        sld.setTextValueSuffix(suffix);
        sld.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(SJAudio::SJLookAndFeel::colAccentColor));
        if (tip.isNotEmpty())
        {
            sld.setTooltip(tip);
            label.setTooltip(tip);
        }
        addAndMakeVisible(sld);

        label.setText(name, juce::dontSendNotification);
        addAndMakeVisible(label);
    };

    satOnButton.setButtonText("COLOR ON");
    satOnButton.setLedColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentColor));
    satOnButton.setTooltip("Analog Color Enable: Activates console tape/transformer nonlinear harmonic saturation");
    addAndMakeVisible(satOnButton);

    setupSatKnob(satDriveSlider,     satDriveLabel,     "DRIVE",     " %",  "Saturation Drive (0 - 100%): Controls harmonic saturation and analog warmth depth");
    setupSatKnob(satCharacterSlider, satCharacterLabel, "CHARACTER", " %",  "Harmonic Character (0 - 100%): Blends even tube warmth (0%) and odd console punch (100%)");
    setupSatKnob(satMixSlider,       satMixLabel,       "MIX",       " %",  "Saturation Parallel Mix (0 - 100%): Dry/wet blend of saturated harmonics");
    setupSatKnob(satOutputSlider,    satOutputLabel,    "OUTPUT",    " dB", "Saturation Output Trim (-12 to +12 dB): Post-saturation level adjustment");

    addAndMakeVisible(satMeter);

    satOnAttachment        = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ParameterIDs::saturationEnabled, satOnButton);
    satDriveAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::saturationDrive, satDriveSlider);
    satCharacterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::saturationCharacter, satCharacterSlider);
    satMixAttachment       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::saturationMix, satMixSlider);
    satOutputAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ParameterIDs::saturationOutput, satOutputSlider);

    // -------------------------------------------------------------------------
    // 7. OUTPUT SECTION
    // -------------------------------------------------------------------------
    outputGainSlider.setAccentColor(juce::Colour(0xffeceff4));
    outputGainSlider.setTextValueSuffix(" dB");
    outputGainSlider.setTooltip("Master Output Trim (-24 to +24 dB): Final level adjustment before host output");
    addAndMakeVisible(outputGainSlider);

    outputGainLabel.setText("OUTPUT GAIN", juce::dontSendNotification);
    outputGainLabel.setTooltip("Master Output Trim (-24 to +24 dB)");
    addAndMakeVisible(outputGainLabel);

    globalMixSlider.setAccentColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentInput));
    globalMixSlider.setTextValueSuffix(" %");
    globalMixSlider.setTooltip("Global Dry/Wet Mix (0 - 100%): Parallel blend of entire channel strip processing");
    addAndMakeVisible(globalMixSlider);

    globalMixLabel.setText("GLOBAL MIX", juce::dontSendNotification);
    globalMixLabel.setTooltip("Global Dry/Wet Mix (0 - 100%)");
    addAndMakeVisible(globalMixLabel);

    bypassButton.setButtonText("MASTER BYPASS");
    bypassButton.setLedColor(juce::Colour(SJAudio::SJLookAndFeel::colAccentOutput));
    bypassButton.setTooltip("Master Global Bypass: Bit-transparent master bypass of all processing modules");
    addAndMakeVisible(bypassButton);

    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getAPVTS(), ParameterIDs::outputGain, outputGainSlider);
    globalMixAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.getAPVTS(), ParameterIDs::globalMix, globalMixSlider);
    bypassAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.getAPVTS(), ParameterIDs::globalBypass, bypassButton);

    addAndMakeVisible(vuMeter);
    addAndMakeVisible(outputMeter);

    updateEqTabVisibility();
    startTimerHz(60);
}

SJChannelStripAudioProcessorEditor::~SJChannelStripAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void SJChannelStripAudioProcessorEditor::updateEqTabVisibility()
{
    const bool is4Band = (currentEqTab == 0);

    // 4-Band controls
    eqMasterButton.setVisible(is4Band);
    lowOnButton.setVisible(is4Band);
    lowFreqSlider.setVisible(is4Band); lowFreqLabel.setVisible(is4Band);
    lowGainSlider.setVisible(is4Band); lowGainLabel.setVisible(is4Band);
    lowQSlider.setVisible(is4Band);    lowQLabel.setVisible(is4Band);
    lowShelfButton.setVisible(is4Band);

    lowMidOnButton.setVisible(is4Band);
    lowMidFreqSlider.setVisible(is4Band); lowMidFreqLabel.setVisible(is4Band);
    lowMidGainSlider.setVisible(is4Band); lowMidGainLabel.setVisible(is4Band);
    lowMidQSlider.setVisible(is4Band);    lowMidQLabel.setVisible(is4Band);

    highMidOnButton.setVisible(is4Band);
    highMidFreqSlider.setVisible(is4Band); highMidFreqLabel.setVisible(is4Band);
    highMidGainSlider.setVisible(is4Band); highMidGainLabel.setVisible(is4Band);
    highMidQSlider.setVisible(is4Band);    highMidQLabel.setVisible(is4Band);

    highOnButton.setVisible(is4Band);
    highFreqSlider.setVisible(is4Band); highFreqLabel.setVisible(is4Band);
    highGainSlider.setVisible(is4Band); highGainLabel.setVisible(is4Band);
    highQSlider.setVisible(is4Band);    highQLabel.setVisible(is4Band);
    highShelfButton.setVisible(is4Band);

    // Dynamic EQ controls
    const bool isDyn = (currentEqTab == 1);
    dynEqOnButton.setVisible(isDyn);
    dynEqListenButton.setVisible(isDyn);
    dynEqModeBox.setVisible(isDyn);
    dynEqModeLabel.setVisible(isDyn);
    dynEqFreqSlider.setVisible(isDyn);    dynEqFreqLabel.setVisible(isDyn);
    dynEqGainSlider.setVisible(isDyn);    dynEqGainLabel.setVisible(isDyn);
    dynEqQSlider.setVisible(isDyn);       dynEqQLabel.setVisible(isDyn);
    dynEqThreshSlider.setVisible(isDyn);  dynEqThreshLabel.setVisible(isDyn);
    dynEqRangeSlider.setVisible(isDyn);   dynEqRangeLabel.setVisible(isDyn);
    dynEqAttackSlider.setVisible(isDyn);  dynEqAttackLabel.setVisible(isDyn);
    dynEqReleaseSlider.setVisible(isDyn); dynEqReleaseLabel.setVisible(isDyn);
    dynEqMeter.setVisible(isDyn);

    tab4BandBtn.setToggleState(is4Band, juce::dontSendNotification);
    tabDynEqBtn.setToggleState(isDyn, juce::dontSendNotification);

    resized();
}

void SJChannelStripAudioProcessorEditor::timerCallback()
{
    constexpr float dt = 1.0f / 60.0f;
    inputMeter.updateVisuals(dt);
    outputMeter.updateVisuals(dt);
    vuMeter.updateVisuals(dt);
    gateMeter.updateVisuals(dt);
    compMeter.updateVisuals(dt);
    satMeter.updateVisuals(dt);
    dynEqMeter.updateVisuals(dt);
    filterDisplay.repaint();
    eqDisplay.repaint();
}

//==============================================================================
void SJChannelStripAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Dark graphite chassis background
    juce::ColourGradient bgGradient(
        juce::Colour(SJAudio::SJLookAndFeel::colPanelBase), 0.0f, 0.0f,
        juce::Colour(0xff101115), 0.0f, bounds.getBottom(),
        false
    );
    g.setGradientFill(bgGradient);
    g.fillAll();

    // Outer border
    g.setColour(juce::Colour(SJAudio::SJLookAndFeel::colBorder));
    g.drawRect(bounds, 1.5f);

    // Leave room for Header (56px) and Footer (28px)
    bounds.removeFromTop(56.0f);
    auto footerArea = bounds.removeFromBottom(28.0f);

    // Footer bar
    g.setColour(juce::Colour(0xff121318));
    g.fillRect(footerArea);
    g.setColour(juce::Colour(0xff222733));
    g.drawHorizontalLine(static_cast<int>(footerArea.getY()), 0.0f, bounds.getRight());

    g.setFont(juce::FontOptions(10.0f, juce::Font::plain));
    g.setColour(juce::Colour(SJAudio::SJLookAndFeel::colTextMuted));
    g.drawText("SIGNAL CHAIN: INPUT > POLARITY > HPF > LPF > GATE/EXP > 4-BAND EQ > DYNAMIC EQ > COMPRESSOR > ANALOG COLOR > OUTPUT GAIN > MASTER METERS",
               footerArea.reduced(16.0f, 0.0f), juce::Justification::centredLeft);
    g.drawText("SJ Channel Strip v1.0.0 | SJ Audio",
               footerArea.reduced(16.0f, 0.0f), juce::Justification::centredRight);

    // 7 Module Cards (arranged across 8 column slots, EQ spans 2)
    auto centerArea = bounds.reduced(8.0f, 6.0f);
    const float colWidth = centerArea.getWidth() / 8.0f;

    auto drawModuleCard = [&g, &centerArea, colWidth](float colStart, float colSpan,
                                                      const juce::String& title,
                                                      juce::Colour accentCol)
    {
        auto cardRect = juce::Rectangle<float>(centerArea.getX() + colStart * colWidth, centerArea.getY(),
                                               colSpan * colWidth, centerArea.getHeight()).reduced(3.5f, 2.0f);

        // Hardware plate body
        g.setColour(juce::Colour(SJAudio::SJLookAndFeel::colPanelCard));
        g.fillRoundedRectangle(cardRect, 5.0f);
        g.setColour(juce::Colour(SJAudio::SJLookAndFeel::colBorder));
        g.drawRoundedRectangle(cardRect, 5.0f, 1.0f);

        // Module header bar
        auto cardHeader = cardRect.removeFromTop(38.0f);
        juce::ColourGradient headerGrad(
            juce::Colour(SJAudio::SJLookAndFeel::colPanelCardHeader), cardHeader.getX(), cardHeader.getY(),
            juce::Colour(0xff1a1d25), cardHeader.getX(), cardHeader.getBottom(),
            false
        );
        g.setGradientFill(headerGrad);
        g.fillRoundedRectangle(cardHeader.reduced(0.5f), 4.5f);

        // Accent indicator bar
        g.setColour(accentCol);
        g.fillRoundedRectangle(cardHeader.getX() + 8.0f, cardHeader.getCentreY() - 7.0f, 3.0f, 14.0f, 1.5f);

        // Title
        g.setFont(juce::FontOptions(11.5f, juce::Font::bold));
        g.setColour(juce::Colour(SJAudio::SJLookAndFeel::colTextPrimary));
        g.drawText(title, cardHeader.withTrimmedLeft(18.0f).withTrimmedRight(8.0f), juce::Justification::centredLeft);
    };

    drawModuleCard(0.0f, 1.0f, "INPUT",        juce::Colour(SJAudio::SJLookAndFeel::colAccentInput));
    drawModuleCard(1.0f, 1.0f, "FILTER",       juce::Colour(SJAudio::SJLookAndFeel::colAccentFilter));
    drawModuleCard(2.0f, 1.0f, "GATE / EXP",   juce::Colour(SJAudio::SJLookAndFeel::colAccentGate));
    drawModuleCard(3.0f, 2.0f, "EQUALIZER",    juce::Colour(SJAudio::SJLookAndFeel::colAccentEQ));
    drawModuleCard(5.0f, 1.0f, "COMPRESSOR",   juce::Colour(SJAudio::SJLookAndFeel::colAccentComp));
    drawModuleCard(6.0f, 1.0f, "ANALOG COLOR", juce::Colour(SJAudio::SJLookAndFeel::colAccentColor));
    drawModuleCard(7.0f, 1.0f, "OUTPUT",       juce::Colour(SJAudio::SJLookAndFeel::colAccentOutput));
}

void SJChannelStripAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // 0. Header (Height 56px)
    header.setBounds(bounds.removeFromTop(56));

    // Footer (Height 28px)
    bounds.removeFromBottom(28);

    auto centerArea = bounds.reduced(8, 6);
    const float colWidth = static_cast<float>(centerArea.getWidth()) / 8.0f;

    // Helper lambda for column bounds
    auto getColBounds = [&centerArea, colWidth](float colStart, float colSpan)
    {
        auto r = juce::Rectangle<int>(
            static_cast<int>(centerArea.getX() + colStart * colWidth),
            centerArea.getY(),
            static_cast<int>(colSpan * colWidth),
            centerArea.getHeight()
        ).reduced(4, 2);
        r.removeFromTop(38); // Header space
        r.removeFromTop(6);  // Padding
        return r;
    };

    // -------------------------------------------------------------------------
    // Col 0: INPUT SECTION
    // -------------------------------------------------------------------------
    {
        auto r = getColBounds(0.0f, 1.0f);
        polarityButton.setBounds(r.removeFromTop(24).reduced(6, 0));
        r.removeFromTop(6);

        inputGainLabel.setBounds(r.removeFromTop(14));
        inputGainSlider.setBounds(r.removeFromTop(74).reduced(6, 0));
        r.removeFromTop(10);

        inputMeter.setBounds(r.reduced(10, 4));
    }

    // -------------------------------------------------------------------------
    // Col 1: FILTER SECTION
    // -------------------------------------------------------------------------
    {
        auto r = getColBounds(1.0f, 1.0f);
        filterDisplay.setBounds(r.removeFromTop(90).reduced(4, 2));
        r.removeFromTop(8);

        hpOnButton.setBounds(r.removeFromTop(22).reduced(6, 0));
        hpFreqLabel.setBounds(r.removeFromTop(13));
        hpFreqSlider.setBounds(r.removeFromTop(72).reduced(6, 0));
        hpSlopeBox.setBounds(r.removeFromTop(22).reduced(8, 0));
        r.removeFromTop(10);

        lpOnButton.setBounds(r.removeFromTop(22).reduced(6, 0));
        lpFreqLabel.setBounds(r.removeFromTop(13));
        lpFreqSlider.setBounds(r.removeFromTop(72).reduced(6, 0));
        lpSlopeBox.setBounds(r.removeFromTop(22).reduced(8, 0));
    }

    // -------------------------------------------------------------------------
    // Col 2: GATE / EXPANDER SECTION
    // -------------------------------------------------------------------------
    {
        auto r = getColBounds(2.0f, 1.0f);

        // Split horizontally: Right area for Gain Reduction Meter
        const int meterW = 30;
        auto meterArea = r.removeFromRight(meterW);
        gateMeter.setBounds(meterArea.reduced(1, 2));

        auto ctrlArea = r.reduced(2, 0);
        gateOnButton.setBounds(ctrlArea.removeFromTop(22).reduced(2, 0));
        ctrlArea.removeFromTop(2);

        gateModeBox.setBounds(ctrlArea.removeFromTop(22).reduced(2, 0));
        ctrlArea.removeFromTop(4);

        const float subW = static_cast<float>(ctrlArea.getWidth()) * 0.5f;

        auto layoutKnobPair = [&ctrlArea, subW](juce::Label& l1, juce::Slider& s1, juce::Label& l2, juce::Slider& s2)
        {
            auto row = ctrlArea.removeFromTop(52);
            auto leftBox = row.removeFromLeft(static_cast<int>(subW)).reduced(1, 0);
            auto rightBox = row.reduced(1, 0);

            l1.setBounds(leftBox.removeFromTop(12));
            s1.setBounds(leftBox.reduced(1, 0));

            l2.setBounds(rightBox.removeFromTop(12));
            s2.setBounds(rightBox.reduced(1, 0));
        };

        // Row 1: Threshold & Range
        layoutKnobPair(gateThresholdLabel, gateThresholdSlider, gateRangeLabel, gateRangeSlider);
        // Row 2: Attack & Hold
        layoutKnobPair(gateAttackLabel, gateAttackSlider, gateHoldLabel, gateHoldSlider);

        // Row 3: Release & Sidechain Enable
        {
            auto row = ctrlArea.removeFromTop(52);
            auto leftBox = row.removeFromLeft(static_cast<int>(subW)).reduced(1, 0);
            auto rightBox = row.reduced(1, 0);

            gateReleaseLabel.setBounds(leftBox.removeFromTop(12));
            gateReleaseSlider.setBounds(leftBox.reduced(1, 0));

            rightBox.removeFromTop(14);
            gateScOnButton.setBounds(rightBox.removeFromTop(22).reduced(1, 0));
        }

        // Row 4: Sidechain HPF & LPF
        layoutKnobPair(gateScHpfLabel, gateScHpfSlider, gateScLpfLabel, gateScLpfSlider);

        // Row 5: Sidechain Source & Listen
        {
            auto row = ctrlArea.removeFromTop(44);
            auto leftBox = row.removeFromLeft(static_cast<int>(subW)).reduced(1, 0);
            auto rightBox = row.reduced(1, 0);

            gateScSourceLabel.setBounds(leftBox.removeFromTop(12));
            gateScSourceBox.setBounds(leftBox.removeFromTop(22).reduced(1, 0));

            rightBox.removeFromTop(12);
            gateScListenButton.setBounds(rightBox.removeFromTop(22).reduced(1, 0));
        }
    }

    // -------------------------------------------------------------------------
    // Col 3 & 4: EQUALIZER SECTION (4-Band EQ & Dynamic EQ Tabs)
    // -------------------------------------------------------------------------
    {
        auto r = getColBounds(3.0f, 2.0f);

        // Top: Interactive EQ Display with spectrum analyzer & dynamic curve
        eqDisplay.setBounds(r.removeFromTop(135).reduced(4, 2));
        r.removeFromTop(4);

        // Tab Switcher Row: [ 4-BAND EQ ]  [ DYNAMIC EQ ]
        auto tabRow = r.removeFromTop(24).reduced(20, 0);
        const int tabW = (tabRow.getWidth() - 8) / 2;
        tab4BandBtn.setBounds(tabRow.removeFromLeft(tabW));
        tabRow.removeFromLeft(8);
        tabDynEqBtn.setBounds(tabRow);
        r.removeFromTop(6);

        if (currentEqTab == 0)
        {
            // 4-BAND EQ VIEW
            eqMasterButton.setBounds(r.removeFromTop(24).reduced(40, 0));
            r.removeFromTop(6);

            const float bandColW = static_cast<float>(r.getWidth()) / 4.0f;

            auto layoutBand = [this, &r, bandColW](int idx, juce::ToggleButton& onBtn,
                                                   juce::Slider& fKnob, juce::Label& fLbl,
                                                   juce::Slider& gKnob, juce::Label& gLbl,
                                                   juce::Slider& qKnob, juce::Label& qLbl,
                                                   juce::ToggleButton* shelfBtn)
            {
                auto bRect = juce::Rectangle<int>(
                    static_cast<int>(r.getX() + static_cast<float>(idx) * bandColW),
                    r.getY(),
                    static_cast<int>(bandColW),
                    r.getHeight()
                ).reduced(2, 2);

                onBtn.setBounds(bRect.removeFromTop(22).reduced(2, 0));

                fLbl.setBounds(bRect.removeFromTop(13));
                fKnob.setBounds(bRect.removeFromTop(74).reduced(3, 0));

                gLbl.setBounds(bRect.removeFromTop(13));
                gKnob.setBounds(bRect.removeFromTop(74).reduced(3, 0));

                qLbl.setBounds(bRect.removeFromTop(13));
                qKnob.setBounds(bRect.removeFromTop(74).reduced(3, 0));

                if (shelfBtn != nullptr)
                {
                    shelfBtn->setBounds(bRect.removeFromTop(22).reduced(2, 0));
                }
            };

            layoutBand(0, lowOnButton, lowFreqSlider, lowFreqLabel, lowGainSlider, lowGainLabel, lowQSlider, lowQLabel, &lowShelfButton);
            layoutBand(1, lowMidOnButton, lowMidFreqSlider, lowMidFreqLabel, lowMidGainSlider, lowMidGainLabel, lowMidQSlider, lowMidQLabel, nullptr);
            layoutBand(2, highMidOnButton, highMidFreqSlider, highMidFreqLabel, highMidGainSlider, highMidGainLabel, highMidQSlider, highMidQLabel, nullptr);
            layoutBand(3, highOnButton, highFreqSlider, highFreqLabel, highGainSlider, highGainLabel, highQSlider, highQLabel, &highShelfButton);
        }
        else
        {
            // DYNAMIC EQ VIEW
            // Split right column for dedicated Dynamic Action & Detector Level meter
            const int meterW = 52;
            auto meterArea = r.removeFromRight(meterW);
            dynEqMeter.setBounds(meterArea.reduced(2, 4));

            auto ctrlArea = r.reduced(4, 0);

            // Row 1: Enable, Listen, Mode combo
            auto topCtrlRow = ctrlArea.removeFromTop(24);
            dynEqOnButton.setBounds(topCtrlRow.removeFromLeft(100).reduced(2, 0));
            dynEqListenButton.setBounds(topCtrlRow.removeFromLeft(75).reduced(2, 0));
            topCtrlRow.removeFromLeft(6);
            dynEqModeLabel.setBounds(topCtrlRow.removeFromLeft(40));
            dynEqModeBox.setBounds(topCtrlRow.reduced(2, 0));
            ctrlArea.removeFromTop(8);

            // Row 2: Filter Tone Controls (FREQ, STATIC GAIN, Q)
            auto rowA = ctrlArea.removeFromTop(110);
            const float knobColW3 = static_cast<float>(rowA.getWidth()) / 3.0f;
            auto layoutKnobColA = [&rowA, knobColW3](int idx, juce::Label& lbl, juce::Slider& sld)
            {
                auto col = juce::Rectangle<int>(
                    static_cast<int>(rowA.getX() + static_cast<float>(idx) * knobColW3),
                    rowA.getY(),
                    static_cast<int>(knobColW3),
                    rowA.getHeight()
                ).reduced(3, 0);
                lbl.setBounds(col.removeFromTop(14));
                sld.setBounds(col.removeFromTop(74).reduced(2, 0));
            };
            layoutKnobColA(0, dynEqFreqLabel, dynEqFreqSlider);
            layoutKnobColA(1, dynEqGainLabel, dynEqGainSlider);
            layoutKnobColA(2, dynEqQLabel, dynEqQSlider);

            ctrlArea.removeFromTop(8);

            // Row 3: Dynamics Ballistics Controls (THRESH, RANGE, ATTACK, RELEASE)
            auto rowB = ctrlArea.removeFromTop(110);
            const float knobColW4 = static_cast<float>(rowB.getWidth()) / 4.0f;
            auto layoutKnobColB = [&rowB, knobColW4](int idx, juce::Label& lbl, juce::Slider& sld)
            {
                auto col = juce::Rectangle<int>(
                    static_cast<int>(rowB.getX() + static_cast<float>(idx) * knobColW4),
                    rowB.getY(),
                    static_cast<int>(knobColW4),
                    rowB.getHeight()
                ).reduced(2, 0);
                lbl.setBounds(col.removeFromTop(14));
                sld.setBounds(col.removeFromTop(74).reduced(2, 0));
            };
            layoutKnobColB(0, dynEqThreshLabel, dynEqThreshSlider);
            layoutKnobColB(1, dynEqRangeLabel, dynEqRangeSlider);
            layoutKnobColB(2, dynEqAttackLabel, dynEqAttackSlider);
            layoutKnobColB(3, dynEqReleaseLabel, dynEqReleaseSlider);
        }
    }

    // -------------------------------------------------------------------------
    // Col 5: COMPRESSOR SECTION
    // -------------------------------------------------------------------------
    {
        auto r = getColBounds(5.0f, 1.0f);

        const int meterW = 30;
        auto meterArea = r.removeFromRight(meterW);
        compMeter.setBounds(meterArea.reduced(1, 2));

        auto ctrlArea = r.reduced(2, 0);
        compOnButton.setBounds(ctrlArea.removeFromTop(22).reduced(2, 0));
        ctrlArea.removeFromTop(2);

        compAutoRelButton.setBounds(ctrlArea.removeFromTop(22).reduced(2, 0));
        ctrlArea.removeFromTop(4);

        const float subW = static_cast<float>(ctrlArea.getWidth()) * 0.5f;

        auto layoutKnobPair = [&ctrlArea, subW](juce::Label& l1, juce::Slider& s1, juce::Label& l2, juce::Slider& s2)
        {
            auto row = ctrlArea.removeFromTop(52);
            auto leftBox = row.removeFromLeft(static_cast<int>(subW)).reduced(1, 0);
            auto rightBox = row.reduced(1, 0);

            l1.setBounds(leftBox.removeFromTop(12));
            s1.setBounds(leftBox.reduced(1, 0));

            l2.setBounds(rightBox.removeFromTop(12));
            s2.setBounds(rightBox.reduced(1, 0));
        };

        // Row 1: Thresh & Ratio
        layoutKnobPair(compThresholdLabel, compThresholdSlider, compRatioLabel, compRatioSlider);
        // Row 2: Attack & Release
        layoutKnobPair(compAttackLabel, compAttackSlider, compReleaseLabel, compReleaseSlider);
        // Row 3: Knee & Makeup
        layoutKnobPair(compKneeLabel, compKneeSlider, compMakeupLabel, compMakeupSlider);

        // Row 4: Mix & Sidechain Filter Enable
        {
            auto row = ctrlArea.removeFromTop(52);
            auto leftBox = row.removeFromLeft(static_cast<int>(subW)).reduced(1, 0);
            auto rightBox = row.reduced(1, 0);

            compMixLabel.setBounds(leftBox.removeFromTop(12));
            compMixSlider.setBounds(leftBox.reduced(1, 0));

            rightBox.removeFromTop(14);
            compScOnButton.setBounds(rightBox.removeFromTop(22).reduced(1, 0));
        }

        // Row 5: Sidechain HPF & LPF
        layoutKnobPair(compScHpfLabel, compScHpfSlider, compScLpfLabel, compScLpfSlider);

        // Row 6: Sidechain Source & Listen
        {
            auto row = ctrlArea.removeFromTop(44);
            auto leftBox = row.removeFromLeft(static_cast<int>(subW)).reduced(1, 0);
            auto rightBox = row.reduced(1, 0);

            compScSourceLabel.setBounds(leftBox.removeFromTop(12));
            compScSourceBox.setBounds(leftBox.removeFromTop(22).reduced(1, 0));

            rightBox.removeFromTop(12);
            compScListenButton.setBounds(rightBox.removeFromTop(22).reduced(1, 0));
        }
    }

    // -------------------------------------------------------------------------
    // Col 6: ANALOG COLOR (SATURATION) SECTION
    // -------------------------------------------------------------------------
    {
        auto r = getColBounds(6.0f, 1.0f);

        satOnButton.setBounds(r.removeFromTop(24).reduced(6, 0));
        r.removeFromTop(4);

        satMeter.setBounds(r.removeFromTop(115).reduced(4, 2));
        r.removeFromTop(8);

        auto layoutSatKnob = [&r](juce::Label& lbl, juce::Slider& sld, int knobH = 68)
        {
            lbl.setBounds(r.removeFromTop(13));
            sld.setBounds(r.removeFromTop(knobH).reduced(4, 0));
            r.removeFromTop(4);
        };

        layoutSatKnob(satDriveLabel,     satDriveSlider,     68);
        layoutSatKnob(satCharacterLabel, satCharacterSlider, 68);
        layoutSatKnob(satMixLabel,       satMixSlider,       68);
        layoutSatKnob(satOutputLabel,    satOutputSlider,    68);
    }

    // -------------------------------------------------------------------------
    // Col 7: OUTPUT SECTION
    // -------------------------------------------------------------------------
    {
        auto r = getColBounds(7.0f, 1.0f);

        outputGainLabel.setBounds(r.removeFromTop(13));
        outputGainSlider.setBounds(r.removeFromTop(70).reduced(6, 0));
        r.removeFromTop(4);

        globalMixLabel.setBounds(r.removeFromTop(13));
        globalMixSlider.setBounds(r.removeFromTop(70).reduced(6, 0));
        r.removeFromTop(6);

        bypassButton.setBounds(r.removeFromTop(24).reduced(6, 0));
        r.removeFromTop(8);

        int remainingH = r.getHeight();
        int vuH = std::min(115, remainingH / 2);
        vuMeter.setBounds(r.removeFromTop(vuH).reduced(4, 2));
        r.removeFromTop(6);
        outputMeter.setBounds(r.reduced(10, 4));
    }
}
