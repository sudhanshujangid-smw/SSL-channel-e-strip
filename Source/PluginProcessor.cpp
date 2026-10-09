#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SJChannelStripAudioProcessor::SJChannelStripAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)
                     .withInput("Sidechain", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "Parameters", createParameterLayout())
{
    cacheParameterPointers();
}

SJChannelStripAudioProcessor::~SJChannelStripAudioProcessor()
{
}

void SJChannelStripAudioProcessor::cacheParameterPointers()
{
    // Input
    paramPointers.inputGain             = parameters.getRawParameterValue(ParameterIDs::inputGain);
    paramPointers.polarity              = parameters.getRawParameterValue(ParameterIDs::polarity);

    // Filter
    paramPointers.highPassEnabled       = parameters.getRawParameterValue(ParameterIDs::highPassEnabled);
    paramPointers.highPassFrequency     = parameters.getRawParameterValue(ParameterIDs::highPassFrequency);
    paramPointers.highPassSlope         = parameters.getRawParameterValue(ParameterIDs::highPassSlope);
    paramPointers.lowPassEnabled        = parameters.getRawParameterValue(ParameterIDs::lowPassEnabled);
    paramPointers.lowPassFrequency      = parameters.getRawParameterValue(ParameterIDs::lowPassFrequency);
    paramPointers.lowPassSlope          = parameters.getRawParameterValue(ParameterIDs::lowPassSlope);

    // EQ
    paramPointers.eqEnabled             = parameters.getRawParameterValue(ParameterIDs::eqEnabled);
    paramPointers.lowEnabled            = parameters.getRawParameterValue(ParameterIDs::lowEnabled);
    paramPointers.lowFreq               = parameters.getRawParameterValue(ParameterIDs::lowFreq);
    paramPointers.lowGain               = parameters.getRawParameterValue(ParameterIDs::lowGain);
    paramPointers.lowQ                  = parameters.getRawParameterValue(ParameterIDs::lowQ);
    paramPointers.lowShelf              = parameters.getRawParameterValue(ParameterIDs::lowShelf);

    paramPointers.lowMidEnabled         = parameters.getRawParameterValue(ParameterIDs::lowMidEnabled);
    paramPointers.lowMidFreq            = parameters.getRawParameterValue(ParameterIDs::lowMidFreq);
    paramPointers.lowMidGain            = parameters.getRawParameterValue(ParameterIDs::lowMidGain);
    paramPointers.lowMidQ               = parameters.getRawParameterValue(ParameterIDs::lowMidQ);

    paramPointers.highMidEnabled        = parameters.getRawParameterValue(ParameterIDs::highMidEnabled);
    paramPointers.highMidFreq           = parameters.getRawParameterValue(ParameterIDs::highMidFreq);
    paramPointers.highMidGain           = parameters.getRawParameterValue(ParameterIDs::highMidGain);
    paramPointers.highMidQ              = parameters.getRawParameterValue(ParameterIDs::highMidQ);

    paramPointers.highEnabled           = parameters.getRawParameterValue(ParameterIDs::highEnabled);
    paramPointers.highFreq              = parameters.getRawParameterValue(ParameterIDs::highFreq);
    paramPointers.highGain              = parameters.getRawParameterValue(ParameterIDs::highGain);
    paramPointers.highQ                 = parameters.getRawParameterValue(ParameterIDs::highQ);
    paramPointers.highShelf             = parameters.getRawParameterValue(ParameterIDs::highShelf);

    // Gate
    paramPointers.gateEnabled           = parameters.getRawParameterValue(ParameterIDs::gateEnabled);
    paramPointers.gateThreshold         = parameters.getRawParameterValue(ParameterIDs::gateThreshold);
    paramPointers.gateRange             = parameters.getRawParameterValue(ParameterIDs::gateRange);
    paramPointers.gateAttack            = parameters.getRawParameterValue(ParameterIDs::gateAttack);
    paramPointers.gateHold              = parameters.getRawParameterValue(ParameterIDs::gateHold);
    paramPointers.gateRelease           = parameters.getRawParameterValue(ParameterIDs::gateRelease);
    paramPointers.gateMode              = parameters.getRawParameterValue(ParameterIDs::gateMode);
    paramPointers.gateSidechainEnabled  = parameters.getRawParameterValue(ParameterIDs::gateSidechainEnabled);
    paramPointers.gateSidechainHighPass = parameters.getRawParameterValue(ParameterIDs::gateSidechainHighPass);
    paramPointers.gateSidechainLowPass  = parameters.getRawParameterValue(ParameterIDs::gateSidechainLowPass);
    paramPointers.gateSidechainSource   = parameters.getRawParameterValue(ParameterIDs::gateSidechainSource);
    paramPointers.gateSidechainListen   = parameters.getRawParameterValue(ParameterIDs::gateSidechainListen);

    // Compressor
    paramPointers.compressorEnabled     = parameters.getRawParameterValue(ParameterIDs::compressorEnabled);
    paramPointers.compressorThreshold   = parameters.getRawParameterValue(ParameterIDs::compressorThreshold);
    paramPointers.compressorRatio       = parameters.getRawParameterValue(ParameterIDs::compressorRatio);
    paramPointers.compressorAttack      = parameters.getRawParameterValue(ParameterIDs::compressorAttack);
    paramPointers.compressorRelease     = parameters.getRawParameterValue(ParameterIDs::compressorRelease);
    paramPointers.compressorKnee        = parameters.getRawParameterValue(ParameterIDs::compressorKnee);
    paramPointers.compressorMakeup      = parameters.getRawParameterValue(ParameterIDs::compressorMakeup);
    paramPointers.compressorAutoRelease = parameters.getRawParameterValue(ParameterIDs::compressorAutoRelease);
    paramPointers.compressorMix         = parameters.getRawParameterValue(ParameterIDs::compressorMix);
    paramPointers.compressorSidechainEnabled  = parameters.getRawParameterValue(ParameterIDs::compressorSidechainEnabled);
    paramPointers.compressorSidechainHighPass = parameters.getRawParameterValue(ParameterIDs::compressorSidechainHighPass);
    paramPointers.compressorSidechainLowPass  = parameters.getRawParameterValue(ParameterIDs::compressorSidechainLowPass);
    paramPointers.compressorSidechainSource   = parameters.getRawParameterValue(ParameterIDs::compressorSidechainSource);
    paramPointers.compressorSidechainListen   = parameters.getRawParameterValue(ParameterIDs::compressorSidechainListen);

    // Saturation
    paramPointers.saturationEnabled     = parameters.getRawParameterValue(ParameterIDs::saturationEnabled);
    paramPointers.saturationDrive       = parameters.getRawParameterValue(ParameterIDs::saturationDrive);
    paramPointers.saturationCharacter   = parameters.getRawParameterValue(ParameterIDs::saturationCharacter);
    paramPointers.saturationMix         = parameters.getRawParameterValue(ParameterIDs::saturationMix);
    paramPointers.saturationOutput      = parameters.getRawParameterValue(ParameterIDs::saturationOutput);

    // Dynamic EQ (Priority 1)
    paramPointers.dynamicEqEnabled      = parameters.getRawParameterValue(ParameterIDs::dynamicEqEnabled);
    paramPointers.dynamicEqFrequency    = parameters.getRawParameterValue(ParameterIDs::dynamicEqFrequency);
    paramPointers.dynamicEqGain         = parameters.getRawParameterValue(ParameterIDs::dynamicEqGain);
    paramPointers.dynamicEqQ            = parameters.getRawParameterValue(ParameterIDs::dynamicEqQ);
    paramPointers.dynamicEqThreshold    = parameters.getRawParameterValue(ParameterIDs::dynamicEqThreshold);
    paramPointers.dynamicEqRange        = parameters.getRawParameterValue(ParameterIDs::dynamicEqRange);
    paramPointers.dynamicEqAttack       = parameters.getRawParameterValue(ParameterIDs::dynamicEqAttack);
    paramPointers.dynamicEqRelease      = parameters.getRawParameterValue(ParameterIDs::dynamicEqRelease);
    paramPointers.dynamicEqMode         = parameters.getRawParameterValue(ParameterIDs::dynamicEqMode);
    paramPointers.dynamicEqListen       = parameters.getRawParameterValue(ParameterIDs::dynamicEqListen);

    // Output
    paramPointers.outputGain            = parameters.getRawParameterValue(ParameterIDs::outputGain);
    paramPointers.globalMix             = parameters.getRawParameterValue(ParameterIDs::globalMix);
    paramPointers.globalBypass          = parameters.getRawParameterValue(ParameterIDs::globalBypass);

    // Oversampling
    paramPointers.oversampling          = parameters.getRawParameterValue(ParameterIDs::oversampling);

    // Analyzer (Phase 11)
    paramPointers.analyzerEnabled       = parameters.getRawParameterValue(ParameterIDs::analyzerEnabled);
}

//==============================================================================
const juce::String SJChannelStripAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SJChannelStripAudioProcessor::acceptsMidi() const
{
    return false;
}

bool SJChannelStripAudioProcessor::producesMidi() const
{
    return false;
}

bool SJChannelStripAudioProcessor::isMidiEffect() const
{
    return false;
}

double SJChannelStripAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SJChannelStripAudioProcessor::getNumPrograms()
{
    return presetManager.getNumPresets();
}

int SJChannelStripAudioProcessor::getCurrentProgram()
{
    return presetManager.getCurrentIndex();
}

void SJChannelStripAudioProcessor::setCurrentProgram(int index)
{
    presetManager.applyPreset(index, parameters);
}

const juce::String SJChannelStripAudioProcessor::getProgramName(int index)
{
    return presetManager.getPresetName(index);
}

void SJChannelStripAudioProcessor::changeProgramName(int /*index*/, const juce::String& /*newName*/)
{
}

//==============================================================================
void SJChannelStripAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(std::max(1, getTotalNumOutputChannels()));

    inputStage.prepare(spec);
    filterSection.prepare(spec);
    gateExpander.prepare(spec.sampleRate, samplesPerBlock);
    eqSection.prepare(spec);
    dynamicEQ.prepare(spec);
    compressor.prepare(spec.sampleRate, samplesPerBlock);
    saturation.prepare(spec.sampleRate, samplesPerBlock, static_cast<int>(spec.numChannels));
    outputStage.prepare(spec);

    inputMeterSource.prepare(spec.sampleRate);
    outputMeterSource.prepare(spec.sampleRate);

    // Pre-allocate dry buffer for clickless Global Mix & Master Bypass (Phase 9)
    const int maxChannels = std::max(1, getTotalNumOutputChannels());
    const int allocatedSamples = std::max(samplesPerBlock, 4096);
    dryCopyBuffer.setSize(maxChannels, allocatedSamples, false, false, true);

    // Pre-allocate dry delay ring buffer for phase-coherent parallel mix with oversampling (Phase 13)
    dryDelayRingBuffer.setSize(maxChannels, 256, false, false, true);
    dryDelayRingBuffer.clear();
    dryDelayWritePos = 0;

    // Apply initial oversampling mode and report latency to host
    if (paramPointers.oversampling != nullptr)
    {
        const int osChoice = static_cast<int>(paramPointers.oversampling->load(std::memory_order_relaxed));
        saturation.setOversampling(osChoice);
    }
    setLatencySamples(saturation.getOversamplingLatency());

    globalActiveFade.reset(spec.sampleRate, 0.015); // 15 ms clickless bypass fade
    globalActiveFade.setCurrentAndTargetValue(1.0f);

    smoothedGlobalMix.reset(spec.sampleRate, 0.020); // 20 ms linear dry/wet smoothing
    smoothedGlobalMix.setCurrentAndTargetValue(1.0f);

    spectrumFIFO.reset();
}

void SJChannelStripAudioProcessor::releaseResources()
{
    inputStage.reset();
    filterSection.reset();
    gateExpander.reset();
    eqSection.reset();
    dynamicEQ.reset();
    compressor.reset();
    saturation.reset();
    outputStage.reset();
    inputMeterSource.reset();
    outputMeterSource.reset();
    dryDelayRingBuffer.clear();
    dryDelayWritePos = 0;
    globalActiveFade.setCurrentAndTargetValue(1.0f);
    smoothedGlobalMix.setCurrentAndTargetValue(1.0f);
}

bool SJChannelStripAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainIn = layouts.getMainInputChannelSet();
    const auto& mainOut = layouts.getMainOutputChannelSet();

    // Main bus must be mono or stereo
    if (mainIn != juce::AudioChannelSet::mono() && mainIn != juce::AudioChannelSet::stereo())
        return false;

    // Main output must match main input, or be stereo from mono input
    if (mainOut != juce::AudioChannelSet::mono() && mainOut != juce::AudioChannelSet::stereo())
        return false;

    if (mainIn == juce::AudioChannelSet::stereo() && mainOut == juce::AudioChannelSet::mono())
        return false;

    // Sidechain input bus (bus 1) can be disabled, mono, or stereo
    if (layouts.inputBuses.size() > 1)
    {
        const auto& scIn = layouts.getChannelSet(true, 1);
        if (scIn != juce::AudioChannelSet::disabled() &&
            scIn != juce::AudioChannelSet::mono() &&
            scIn != juce::AudioChannelSet::stereo())
            return false;
    }

    return true;
}

void SJChannelStripAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    if (numSamples == 0)
        return;

    // Handle mono-to-stereo expansion if main input is mono and main output is stereo
    if (getChannelCountOfBus(true, 0) == 1 && getChannelCountOfBus(false, 0) > 1)
        buffer.copyFrom(1, 0, buffer, 0, 0, numSamples);

    // Update Oversampling parameter & latency reporting (Phase 13)
    if (paramPointers.oversampling != nullptr)
    {
        const int osChoice = static_cast<int>(paramPointers.oversampling->load(std::memory_order_relaxed));
        saturation.setOversampling(osChoice);
    }

    const int targetLatency = saturation.getOversamplingLatency();
    if (getLatencySamples() != targetLatency)
    {
        setLatencySamples(targetLatency);
    }

    // Extract optional external sidechain buffer safely (Phase 14)
    const juce::AudioBuffer<float>* sidechainBufferPtr = nullptr;
    juce::AudioBuffer<float> scBuffer;

    if (getBusCount(true) > 1)
    {
        auto* scBus = getBus(true, 1);
        if (scBus != nullptr && scBus->isEnabled() && scBus->getNumberOfChannels() > 0)
        {
            scBuffer = getBusBuffer(buffer, true, 1);
            if (scBuffer.getNumChannels() > 0 && scBuffer.getNumSamples() > 0)
            {
                sidechainBufferPtr = &scBuffer;
            }
        }
    }

    // Main buffer alias referencing output channels (0 and 1)
    const int mainChannels = std::min(2, getTotalNumOutputChannels());
    juce::AudioBuffer<float> mainBuffer(buffer.getArrayOfWritePointers(), mainChannels, numSamples);

    // Capture clean untouched dry input for Global Mix & Master Bypass
    // (Zero allocations: dryCopyBuffer was pre-allocated in prepareToPlay)
    const int maxChannelsToCopy = std::min(mainChannels, dryCopyBuffer.getNumChannels());
    const int samplesToCopy = std::min(numSamples, dryCopyBuffer.getNumSamples());
    for (int ch = 0; ch < maxChannelsToCopy; ++ch)
        dryCopyBuffer.copyFrom(ch, 0, mainBuffer, ch, 0, samplesToCopy);

    // Align dry copy buffer with oversampling latency for phase-coherent parallel mix
    if (targetLatency > 0 && dryDelayRingBuffer.getNumSamples() > 0)
    {
        const int ringSize = dryDelayRingBuffer.getNumSamples();
        for (int i = 0; i < numSamples; ++i)
        {
            for (int ch = 0; ch < maxChannelsToCopy; ++ch)
            {
                const float inSample = dryCopyBuffer.getSample(ch, i);
                dryDelayRingBuffer.setSample(ch, dryDelayWritePos, inSample);

                int readPos = dryDelayWritePos - targetLatency;
                if (readPos < 0) readPos += ringSize;

                dryCopyBuffer.setSample(ch, i, dryDelayRingBuffer.getSample(ch, readPos));
            }

            if (++dryDelayWritePos >= ringSize)
                dryDelayWritePos = 0;
        }
    }

    // Read Global Bypass & Global Mix parameters
    const bool isBypassed = (paramPointers.globalBypass != nullptr)
        && (paramPointers.globalBypass->load(std::memory_order_relaxed) > 0.5f);
    globalActiveFade.setTargetValue(isBypassed ? 0.0f : 1.0f);

    const float globalMixVal = (paramPointers.globalMix != nullptr)
        ? paramPointers.globalMix->load(std::memory_order_relaxed)
        : 100.0f;
    smoothedGlobalMix.setTargetValue(juce::jlimit(0.0f, 1.0f, globalMixVal * 0.01f));

    // Fast-path bypass check: if master bypass is fully engaged and crossfade has reached 0.0
    if (isBypassed && globalActiveFade.getCurrentValue() == 0.0f && globalActiveFade.getTargetValue() == 0.0f)
    {
        inputMeterSource.process(mainBuffer);
        outputMeterSource.process(mainBuffer);
        return;
    }

    // 1. INPUT STAGE: Polarity Invert + Input Gain
    if (paramPointers.inputGain != nullptr && paramPointers.polarity != nullptr)
    {
        inputStage.setGainDb(paramPointers.inputGain->load(std::memory_order_relaxed));
        inputStage.setPolarity(paramPointers.polarity->load(std::memory_order_relaxed) > 0.5f);
        inputStage.process(mainBuffer);
    }

    // 2. FILTER STAGE: HPF + LPF (Phase 4)
    if (paramPointers.highPassEnabled != nullptr && paramPointers.highPassFrequency != nullptr && paramPointers.highPassSlope != nullptr)
    {
        const bool hpOn = paramPointers.highPassEnabled->load(std::memory_order_relaxed) > 0.5f;
        const float hpFreq = paramPointers.highPassFrequency->load(std::memory_order_relaxed);
        const int hpSlope = static_cast<int>(paramPointers.highPassSlope->load(std::memory_order_relaxed));
        filterSection.setHighPass(hpOn, hpFreq, hpSlope);
    }

    if (paramPointers.lowPassEnabled != nullptr && paramPointers.lowPassFrequency != nullptr && paramPointers.lowPassSlope != nullptr)
    {
        const bool lpOn = paramPointers.lowPassEnabled->load(std::memory_order_relaxed) > 0.5f;
        const float lpFreq = paramPointers.lowPassFrequency->load(std::memory_order_relaxed);
        const int lpSlope = static_cast<int>(paramPointers.lowPassSlope->load(std::memory_order_relaxed));
        filterSection.setLowPass(lpOn, lpFreq, lpSlope);
    }

    filterSection.process(mainBuffer);

    // 3. GATE / EXPANDER STAGE (Phase 6 & Phase 14)
    if (paramPointers.gateEnabled != nullptr)
        gateExpander.setEnabled(paramPointers.gateEnabled->load(std::memory_order_relaxed) > 0.5f);

    if (paramPointers.gateMode != nullptr)
    {
        const int modeIdx = static_cast<int>(paramPointers.gateMode->load(std::memory_order_relaxed));
        gateExpander.setMode(modeIdx == 1 ? SJAudio::GateExpander::Mode::Expander : SJAudio::GateExpander::Mode::Gate);
    }

    if (paramPointers.gateThreshold != nullptr)
        gateExpander.setThreshold(paramPointers.gateThreshold->load(std::memory_order_relaxed));

    if (paramPointers.gateRange != nullptr)
        gateExpander.setRange(paramPointers.gateRange->load(std::memory_order_relaxed));

    if (paramPointers.gateAttack != nullptr)
        gateExpander.setAttack(paramPointers.gateAttack->load(std::memory_order_relaxed));

    if (paramPointers.gateHold != nullptr)
        gateExpander.setHold(paramPointers.gateHold->load(std::memory_order_relaxed));

    if (paramPointers.gateRelease != nullptr)
        gateExpander.setRelease(paramPointers.gateRelease->load(std::memory_order_relaxed));

    if (paramPointers.gateSidechainEnabled != nullptr)
        gateExpander.setSidechainEnabled(paramPointers.gateSidechainEnabled->load(std::memory_order_relaxed) > 0.5f);

    if (paramPointers.gateSidechainHighPass != nullptr)
        gateExpander.setSidechainHPF(paramPointers.gateSidechainHighPass->load(std::memory_order_relaxed));

    if (paramPointers.gateSidechainLowPass != nullptr)
        gateExpander.setSidechainLPF(paramPointers.gateSidechainLowPass->load(std::memory_order_relaxed));

    if (paramPointers.gateSidechainSource != nullptr)
    {
        const int srcIdx = static_cast<int>(paramPointers.gateSidechainSource->load(std::memory_order_relaxed));
        gateExpander.setSidechainSource(srcIdx == 1 ? SJAudio::GateExpander::SidechainSource::External
                                                    : SJAudio::GateExpander::SidechainSource::Internal);
    }

    if (paramPointers.gateSidechainListen != nullptr)
        gateExpander.setSidechainListen(paramPointers.gateSidechainListen->load(std::memory_order_relaxed) > 0.5f);

    gateExpander.process(mainBuffer, sidechainBufferPtr);

    // 4. 4-BAND PARAMETRIC EQ (Phase 5)
    if (paramPointers.eqEnabled != nullptr)
        eqSection.setMasterEnabled(paramPointers.eqEnabled->load(std::memory_order_relaxed) > 0.5f);

    if (paramPointers.lowFreq != nullptr && paramPointers.lowGain != nullptr &&
        paramPointers.lowQ != nullptr && paramPointers.lowShelf != nullptr && paramPointers.lowEnabled != nullptr)
    {
        eqSection.setLowBand(
            paramPointers.lowEnabled->load(std::memory_order_relaxed) > 0.5f,
            paramPointers.lowFreq->load(std::memory_order_relaxed),
            paramPointers.lowGain->load(std::memory_order_relaxed),
            paramPointers.lowQ->load(std::memory_order_relaxed),
            paramPointers.lowShelf->load(std::memory_order_relaxed) > 0.5f
        );
    }

    if (paramPointers.lowMidFreq != nullptr && paramPointers.lowMidGain != nullptr &&
        paramPointers.lowMidQ != nullptr && paramPointers.lowMidEnabled != nullptr)
    {
        eqSection.setLowMidBand(
            paramPointers.lowMidEnabled->load(std::memory_order_relaxed) > 0.5f,
            paramPointers.lowMidFreq->load(std::memory_order_relaxed),
            paramPointers.lowMidGain->load(std::memory_order_relaxed),
            paramPointers.lowMidQ->load(std::memory_order_relaxed)
        );
    }

    if (paramPointers.highMidFreq != nullptr && paramPointers.highMidGain != nullptr &&
        paramPointers.highMidQ != nullptr && paramPointers.highMidEnabled != nullptr)
    {
        eqSection.setHighMidBand(
            paramPointers.highMidEnabled->load(std::memory_order_relaxed) > 0.5f,
            paramPointers.highMidFreq->load(std::memory_order_relaxed),
            paramPointers.highMidGain->load(std::memory_order_relaxed),
            paramPointers.highMidQ->load(std::memory_order_relaxed)
        );
    }

    if (paramPointers.highFreq != nullptr && paramPointers.highGain != nullptr &&
        paramPointers.highQ != nullptr && paramPointers.highShelf != nullptr && paramPointers.highEnabled != nullptr)
    {
        eqSection.setHighBand(
            paramPointers.highEnabled->load(std::memory_order_relaxed) > 0.5f,
            paramPointers.highFreq->load(std::memory_order_relaxed),
            paramPointers.highGain->load(std::memory_order_relaxed),
            paramPointers.highQ->load(std::memory_order_relaxed),
            paramPointers.highShelf->load(std::memory_order_relaxed) > 0.5f
        );
    }

    eqSection.process(mainBuffer);

    // 4b. DYNAMIC EQ (Priority 1)
    if (paramPointers.dynamicEqEnabled != nullptr)
        dynamicEQ.setEnabled(paramPointers.dynamicEqEnabled->load(std::memory_order_relaxed) > 0.5f);

    if (paramPointers.dynamicEqFrequency != nullptr)
        dynamicEQ.setFrequency(paramPointers.dynamicEqFrequency->load(std::memory_order_relaxed));

    if (paramPointers.dynamicEqGain != nullptr)
        dynamicEQ.setStaticGain(paramPointers.dynamicEqGain->load(std::memory_order_relaxed));

    if (paramPointers.dynamicEqQ != nullptr)
        dynamicEQ.setQ(paramPointers.dynamicEqQ->load(std::memory_order_relaxed));

    if (paramPointers.dynamicEqThreshold != nullptr)
        dynamicEQ.setThreshold(paramPointers.dynamicEqThreshold->load(std::memory_order_relaxed));

    if (paramPointers.dynamicEqRange != nullptr)
        dynamicEQ.setRange(paramPointers.dynamicEqRange->load(std::memory_order_relaxed));

    if (paramPointers.dynamicEqAttack != nullptr)
        dynamicEQ.setAttack(paramPointers.dynamicEqAttack->load(std::memory_order_relaxed));

    if (paramPointers.dynamicEqRelease != nullptr)
        dynamicEQ.setRelease(paramPointers.dynamicEqRelease->load(std::memory_order_relaxed));

    if (paramPointers.dynamicEqMode != nullptr)
    {
        const int modeIdx = static_cast<int>(paramPointers.dynamicEqMode->load(std::memory_order_relaxed));
        dynamicEQ.setMode(modeIdx == 1 ? SJAudio::DynamicEQ::DynamicMode::Expand
                                       : SJAudio::DynamicEQ::DynamicMode::Compress);
    }

    if (paramPointers.dynamicEqListen != nullptr)
        dynamicEQ.setListen(paramPointers.dynamicEqListen->load(std::memory_order_relaxed) > 0.5f);

    dynamicEQ.process(mainBuffer);

    // 5. COMPRESSOR STAGE (Phase 7 & Phase 14)
    if (paramPointers.compressorEnabled != nullptr)
        compressor.setEnabled(paramPointers.compressorEnabled->load(std::memory_order_relaxed) > 0.5f);

    if (paramPointers.compressorThreshold != nullptr)
        compressor.setThreshold(paramPointers.compressorThreshold->load(std::memory_order_relaxed));

    if (paramPointers.compressorRatio != nullptr)
        compressor.setRatio(paramPointers.compressorRatio->load(std::memory_order_relaxed));

    if (paramPointers.compressorAttack != nullptr)
        compressor.setAttack(paramPointers.compressorAttack->load(std::memory_order_relaxed));

    if (paramPointers.compressorRelease != nullptr)
        compressor.setRelease(paramPointers.compressorRelease->load(std::memory_order_relaxed));

    if (paramPointers.compressorKnee != nullptr)
        compressor.setKnee(paramPointers.compressorKnee->load(std::memory_order_relaxed));

    if (paramPointers.compressorMakeup != nullptr)
        compressor.setMakeup(paramPointers.compressorMakeup->load(std::memory_order_relaxed));

    if (paramPointers.compressorAutoRelease != nullptr)
        compressor.setAutoRelease(paramPointers.compressorAutoRelease->load(std::memory_order_relaxed) > 0.5f);

    if (paramPointers.compressorMix != nullptr)
        compressor.setMix(paramPointers.compressorMix->load(std::memory_order_relaxed));

    if (paramPointers.compressorSidechainEnabled != nullptr)
        compressor.setSidechainEnabled(paramPointers.compressorSidechainEnabled->load(std::memory_order_relaxed) > 0.5f);

    if (paramPointers.compressorSidechainHighPass != nullptr)
        compressor.setSidechainHPF(paramPointers.compressorSidechainHighPass->load(std::memory_order_relaxed));

    if (paramPointers.compressorSidechainLowPass != nullptr)
        compressor.setSidechainLPF(paramPointers.compressorSidechainLowPass->load(std::memory_order_relaxed));

    if (paramPointers.compressorSidechainSource != nullptr)
    {
        const int srcIdx = static_cast<int>(paramPointers.compressorSidechainSource->load(std::memory_order_relaxed));
        compressor.setSidechainSource(srcIdx == 1 ? SJAudio::Compressor::SidechainSource::External
                                                  : SJAudio::Compressor::SidechainSource::Internal);
    }

    if (paramPointers.compressorSidechainListen != nullptr)
        compressor.setSidechainListen(paramPointers.compressorSidechainListen->load(std::memory_order_relaxed) > 0.5f);

    compressor.process(mainBuffer, sidechainBufferPtr);

    // 6. SATURATION STAGE (Phase 8)
    if (paramPointers.saturationEnabled != nullptr)
        saturation.setEnabled(paramPointers.saturationEnabled->load(std::memory_order_relaxed) > 0.5f);

    if (paramPointers.saturationDrive != nullptr)
        saturation.setDrive(paramPointers.saturationDrive->load(std::memory_order_relaxed));

    if (paramPointers.saturationCharacter != nullptr)
        saturation.setCharacter(paramPointers.saturationCharacter->load(std::memory_order_relaxed));

    if (paramPointers.saturationMix != nullptr)
        saturation.setMix(paramPointers.saturationMix->load(std::memory_order_relaxed));

    if (paramPointers.saturationOutput != nullptr)
        saturation.setOutput(paramPointers.saturationOutput->load(std::memory_order_relaxed));

    saturation.process(mainBuffer);

    // 7. INPUT / STAGE METER: Measures signal after Input, Filters, Gate, EQ, Compressor & Saturation
    inputMeterSource.process(mainBuffer);

    // 8. OUTPUT STAGE: Output Gain
    if (paramPointers.outputGain != nullptr)
    {
        outputStage.setGainDb(paramPointers.outputGain->load(std::memory_order_relaxed));
        outputStage.process(mainBuffer);
    }

    // 9. GLOBAL MIX & MASTER BYPASS BLEND (Phase 9)
    // Applies sample-accurate dry/wet blend and 15 ms clickless bypass crossfade
    const int blendChannels = std::min(mainChannels, dryCopyBuffer.getNumChannels());
    const int samplesToBlend = std::min(numSamples, dryCopyBuffer.getNumSamples());
    for (int i = 0; i < samplesToBlend; ++i)
    {
        const float curMix = smoothedGlobalMix.getNextValue();
        const float curActive = globalActiveFade.getNextValue();

        for (int ch = 0; ch < blendChannels; ++ch)
        {
            const float dry = dryCopyBuffer.getSample(ch, i);
            const float wet = mainBuffer.getSample(ch, i);

            // Phase-coherent Dry/Wet mix (0.0 = pure dry input, 1.0 = fully processed)
            const float mixed = dry * (1.0f - curMix) + wet * curMix;

            // Clickless Master Bypass crossfade: 1.0 = active plugin, 0.0 = clean original dry
            const float outSample = dry * (1.0f - curActive) + mixed * curActive;
            mainBuffer.setSample(ch, i, outSample);
        }
    }

    // 10. OUTPUT METER: Measures final output audio sent to DAW
    outputMeterSource.process(mainBuffer);

    // 11. SPECTRUM ANALYZER FIFO (Phase 11): Push processed output buffer if enabled
    if (paramPointers.analyzerEnabled != nullptr && paramPointers.analyzerEnabled->load(std::memory_order_relaxed) > 0.5f)
    {
        spectrumFIFO.push(mainBuffer);
    }
}

//==============================================================================
bool SJChannelStripAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SJChannelStripAudioProcessor::createEditor()
{
    return new SJChannelStripAudioProcessorEditor(*this);
}

//==============================================================================
void SJChannelStripAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    state.setProperty("currentPresetIndex", presetManager.getCurrentIndex(), nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SJChannelStripAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
    {
        auto vt = juce::ValueTree::fromXml(*xmlState);
        if (vt.hasProperty("currentPresetIndex"))
        {
            presetManager.setCurrentIndex(static_cast<int>(vt.getProperty("currentPresetIndex")));
        }
        parameters.replaceState(vt);
    }
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SJChannelStripAudioProcessor();
}
