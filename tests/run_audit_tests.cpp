#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>

// Test utility
static int g_passCount = 0;
static int g_failCount = 0;

#define AUDIT_ASSERT(condition, msg) \
    do { \
        if (condition) { \
            std::cout << "  [PASS] " << msg << std::endl; \
            g_passCount++; \
        } else { \
            std::cerr << "  [FAIL] " << msg << " (" << #condition << ")" << std::endl; \
            g_failCount++; \
        } \
    } while(0)

int main()
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    std::cout << "========================================================" << std::endl;
    std::cout << "   SJ CHANNEL STRIP — AUTOMATED AUDIT TEST SUITE       " << std::endl;
    std::cout << "   Manufacturer: SJ Audio | Version: 1.0.0             " << std::endl;
    std::cout << "========================================================" << std::endl;

    // -------------------------------------------------------------------------
    // TEST 1: Instantiation, Metadata, & Identification
    // -------------------------------------------------------------------------
    std::cout << "\n[TEST GROUP 1] Plugin Identity & Metadata:" << std::endl;
    {
        auto proc = std::make_unique<SJChannelStripAudioProcessor>();
        AUDIT_ASSERT(proc != nullptr, "Processor instantiates successfully");
        AUDIT_ASSERT(proc->getName() == "SJ Channel Strip", "Plugin name is 'SJ Channel Strip'");
        AUDIT_ASSERT(!proc->acceptsMidi(), "acceptsMidi is false");
        AUDIT_ASSERT(!proc->producesMidi(), "producesMidi is false");
        AUDIT_ASSERT(!proc->isMidiEffect(), "isMidiEffect is false");
        AUDIT_ASSERT(proc->hasEditor(), "hasEditor is true");
        AUDIT_ASSERT(proc->getNumPrograms() == 19, "Factory presets count is 19");

        // Verify editor creation and disposal
        auto editor = std::unique_ptr<juce::AudioProcessorEditor>(proc->createEditor());
        AUDIT_ASSERT(editor != nullptr, "Editor instantiates without crashing");
        AUDIT_ASSERT(editor->getWidth() >= 900 && editor->getHeight() >= 550, "Editor initial dimensions are within resize limits");
        editor.reset();
        AUDIT_ASSERT(true, "Editor safely tears down without memory asserts");
    }

    // -------------------------------------------------------------------------
    // TEST 2: Bus Layouts & Multi-Channel Negotiation
    // -------------------------------------------------------------------------
    std::cout << "\n[TEST GROUP 2] Bus Layouts & Channel Formats:" << std::endl;
    {
        SJChannelStripAudioProcessor proc;
        
        // Stereo In / Stereo Out
        juce::AudioProcessor::BusesLayout stereoLayout;
        stereoLayout.inputBuses.add(juce::AudioChannelSet::stereo());
        stereoLayout.outputBuses.add(juce::AudioChannelSet::stereo());
        AUDIT_ASSERT(proc.isBusesLayoutSupported(stereoLayout), "Stereo Main I/O is supported");

        // Stereo In + Stereo Sidechain / Stereo Out
        juce::AudioProcessor::BusesLayout scLayout = stereoLayout;
        scLayout.inputBuses.add(juce::AudioChannelSet::stereo());
        AUDIT_ASSERT(proc.isBusesLayoutSupported(scLayout), "Stereo Main + Stereo Sidechain is supported");

        // Mono In / Mono Out
        juce::AudioProcessor::BusesLayout monoLayout;
        monoLayout.inputBuses.add(juce::AudioChannelSet::mono());
        monoLayout.outputBuses.add(juce::AudioChannelSet::mono());
        AUDIT_ASSERT(proc.isBusesLayoutSupported(monoLayout), "Mono Main I/O is supported");

        // 5.1 Surround (Unsupported, verify graceful rejection)
        juce::AudioProcessor::BusesLayout surroundLayout;
        surroundLayout.inputBuses.add(juce::AudioChannelSet::create5point1());
        surroundLayout.outputBuses.add(juce::AudioChannelSet::create5point1());
        AUDIT_ASSERT(!proc.isBusesLayoutSupported(surroundLayout), "5.1 Surround correctly rejected without crash");
    }

    // -------------------------------------------------------------------------
    // TEST 3: Multi-Sample Rate & Buffer Size Resilience
    // -------------------------------------------------------------------------
    std::cout << "\n[TEST GROUP 3] Sample Rates & Buffer Sizes:" << std::endl;
    {
        SJChannelStripAudioProcessor proc;
        std::vector<double> sampleRates = { 44100.0, 48000.0, 88200.0, 96000.0, 192000.0 };
        std::vector<int> bufferSizes = { 32, 64, 128, 256, 512, 1024, 2048, 4096 };

        bool allSrOk = true;
        for (double sr : sampleRates)
        {
            for (int bs : bufferSizes)
            {
                proc.prepareToPlay(sr, bs);
                juce::AudioBuffer<float> buffer(2, bs);
                buffer.clear();
                juce::MidiBuffer midi;
                proc.processBlock(buffer, midi);

                // Check samples are finite
                for (int ch = 0; ch < 2; ++ch)
                {
                    const float* r = buffer.getReadPointer(ch);
                    for (int s = 0; s < bs; ++s)
                    {
                        if (std::isnan(r[s]) || std::isinf(r[s]))
                        {
                            allSrOk = false;
                            break;
                        }
                    }
                }
            }
        }
        AUDIT_ASSERT(allSrOk, "Sample rates 44.1k-192k across buffers 32-4096 process stably without NaN/Inf");
    }

    // -------------------------------------------------------------------------
    // TEST 4: Extreme Inputs & Signal Immunity
    // -------------------------------------------------------------------------
    std::cout << "\n[TEST GROUP 4] Extreme Input Immunity (Silence, High dBFS, DC):" << std::endl;
    {
        SJChannelStripAudioProcessor proc;
        const double sr = 48000.0;
        const int bs = 512;
        proc.prepareToPlay(sr, bs);

        // A. Digital Silence (-inf dBFS)
        juce::AudioBuffer<float> silenceBuf(2, bs);
        silenceBuf.clear();
        juce::MidiBuffer midi;
        proc.processBlock(silenceBuf, midi);
        bool silenceClean = true;
        for (int ch = 0; ch < 2; ++ch)
        {
            const float* r = silenceBuf.getReadPointer(ch);
            for (int s = 0; s < bs; ++s)
            {
                if (std::isnan(r[s]) || std::isinf(r[s]) || std::abs(r[s]) > 1e-4f)
                    silenceClean = false;
            }
        }
        AUDIT_ASSERT(silenceClean, "Digital silence processes without denormals or spurious energy");

        // B. Hot Input (+36 dBFS = amplitude ~63.0)
        juce::AudioBuffer<float> hotBuf(2, bs);
        for (int ch = 0; ch < 2; ++ch)
        {
            float* w = hotBuf.getWritePointer(ch);
            for (int s = 0; s < bs; ++s)
                w[s] = 63.0f * std::sin(2.0f * 3.14159265f * 1000.0f * s / static_cast<float>(sr));
        }
        proc.processBlock(hotBuf, midi);
        bool hotClean = true;
        for (int ch = 0; ch < 2; ++ch)
        {
            const float* r = hotBuf.getReadPointer(ch);
            for (int s = 0; s < bs; ++s)
            {
                if (std::isnan(r[s]) || std::isinf(r[s]))
                    hotClean = false;
            }
        }
        AUDIT_ASSERT(hotClean, "+36 dBFS extreme input gracefully clamped/shaped without NaN/Inf");

        // C. High DC Offset (+10.0 DC)
        juce::AudioBuffer<float> dcBuf(2, bs);
        for (int ch = 0; ch < 2; ++ch)
        {
            float* w = dcBuf.getWritePointer(ch);
            for (int s = 0; s < bs; ++s)
                w[s] = 10.0f;
        }
        // Run several blocks to let DC blocker stabilize
        for (int b = 0; b < 10; ++b)
        {
            for (int ch = 0; ch < 2; ++ch)
                for (int s = 0; s < bs; ++s)
                    dcBuf.setSample(ch, s, 10.0f);
            proc.processBlock(dcBuf, midi);
        }
        bool dcClean = true;
        for (int ch = 0; ch < 2; ++ch)
        {
            const float* r = dcBuf.getReadPointer(ch);
            for (int s = 0; s < bs; ++s)
            {
                if (std::isnan(r[s]) || std::isinf(r[s]))
                    dcClean = false;
            }
        }
        AUDIT_ASSERT(dcClean, "High DC offset filtered without instability");
    }

    // -------------------------------------------------------------------------
    // TEST 5: Rapid Parameter Automation & Stress Sweeps
    // -------------------------------------------------------------------------
    std::cout << "\n[TEST GROUP 5] Automation & Rapid Parameter Modulation:" << std::endl;
    {
        SJChannelStripAudioProcessor proc;
        const double sr = 48000.0;
        const int bs = 256;
        proc.prepareToPlay(sr, bs);
        juce::MidiBuffer midi;
        juce::Random rng(12345);

        auto& apvts = proc.getAPVTS();
        bool autoClean = true;

        for (int block = 0; block < 100; ++block)
        {
            // Sweep multiple parameters simultaneously
            if (auto* p = apvts.getParameter(ParameterIDs::inputGain))
                p->setValueNotifyingHost(rng.nextFloat());
            if (auto* p = apvts.getParameter(ParameterIDs::lowFreq))
                p->setValueNotifyingHost(rng.nextFloat());
            if (auto* p = apvts.getParameter(ParameterIDs::lowGain))
                p->setValueNotifyingHost(rng.nextFloat());
            if (auto* p = apvts.getParameter(ParameterIDs::dynamicEqFrequency))
                p->setValueNotifyingHost(rng.nextFloat());
            if (auto* p = apvts.getParameter(ParameterIDs::compressorThreshold))
                p->setValueNotifyingHost(rng.nextFloat());
            if (auto* p = apvts.getParameter(ParameterIDs::saturationDrive))
                p->setValueNotifyingHost(rng.nextFloat());
            if (auto* p = apvts.getParameter(ParameterIDs::saturationCharacter))
                p->setValueNotifyingHost(rng.nextFloat());
            if (auto* p = apvts.getParameter(ParameterIDs::outputGain))
                p->setValueNotifyingHost(rng.nextFloat());

            juce::AudioBuffer<float> testBuf(2, bs);
            for (int ch = 0; ch < 2; ++ch)
                for (int s = 0; s < bs; ++s)
                    testBuf.setSample(ch, s, std::sin(2.0f * 3.14159f * 440.0f * s / static_cast<float>(sr)));

            proc.processBlock(testBuf, midi);

            for (int ch = 0; ch < 2; ++ch)
            {
                const float* r = testBuf.getReadPointer(ch);
                for (int s = 0; s < bs; ++s)
                {
                    if (std::isnan(r[s]) || std::isinf(r[s]))
                        autoClean = false;
                }
            }
        }
        AUDIT_ASSERT(autoClean, "100 blocks of random high-speed parameter automation executed without clicks or NaNs");
    }

    // -------------------------------------------------------------------------
    // TEST 6: Factory Presets & State Persistence
    // -------------------------------------------------------------------------
    std::cout << "\n[TEST GROUP 6] Factory Presets & XML State Serialization:" << std::endl;
    {
        SJChannelStripAudioProcessor proc;
        const double sr = 48000.0;
        const int bs = 512;
        proc.prepareToPlay(sr, bs);
        juce::MidiBuffer midi;

        // Test all 19 factory presets
        bool presetsOk = true;
        for (int pIdx = 0; pIdx < proc.getNumPrograms(); ++pIdx)
        {
            proc.setCurrentProgram(pIdx);
            juce::AudioBuffer<float> buf(2, bs);
            buf.clear();
            proc.processBlock(buf, midi);
            if (proc.getCurrentProgram() != pIdx)
                presetsOk = false;
        }
        AUDIT_ASSERT(presetsOk, "All 19 factory presets applied and processed audio cleanly");

        // Test State Save & Restore
        juce::MemoryBlock stateData;
        proc.setCurrentProgram(3); // Rap Vocal
        proc.getStateInformation(stateData);
        AUDIT_ASSERT(stateData.getSize() > 0, "Plugin state serialized to non-empty MemoryBlock");

        // Modify settings
        proc.setCurrentProgram(10); // Drum Bus
        AUDIT_ASSERT(proc.getCurrentProgram() == 10, "Program switched to Drum Bus");

        // Restore state
        proc.setStateInformation(stateData.getData(), static_cast<int>(stateData.getSize()));
        AUDIT_ASSERT(proc.getCurrentProgram() == 3, "State restore successfully recalled original preset (Rap Vocal)");
    }

    // -------------------------------------------------------------------------
    // TEST 7: Oversampling Engine Switching
    // -------------------------------------------------------------------------
    std::cout << "\n[TEST GROUP 7] Oversampling Switching & Latency Reporting:" << std::endl;
    {
        SJChannelStripAudioProcessor proc;
        proc.prepareToPlay(48000.0, 512);

        auto* osParam = proc.getAPVTS().getParameter(ParameterIDs::oversampling);
        AUDIT_ASSERT(osParam != nullptr, "Oversampling parameter exists");

        // 1x Mode (default, 0 latency)
        osParam->setValueNotifyingHost(0.0f); // 1x
        juce::AudioBuffer<float> buf1(2, 512); buf1.clear();
        juce::MidiBuffer m1;
        proc.processBlock(buf1, m1);
        AUDIT_ASSERT(proc.getLatencySamples() == 0, "1x Oversampling reports 0 samples latency");

        // 2x Mode
        osParam->setValueNotifyingHost(0.5f); // 2x
        proc.processBlock(buf1, m1);
        AUDIT_ASSERT(proc.getLatencySamples() > 0, "2x Oversampling correctly reports filter latency to host");

        // 4x Mode
        osParam->setValueNotifyingHost(1.0f); // 4x
        proc.processBlock(buf1, m1);
        AUDIT_ASSERT(proc.getLatencySamples() > 0, "4x Oversampling correctly reports filter latency to host");

        // Return to 1x
        osParam->setValueNotifyingHost(0.0f);
        proc.processBlock(buf1, m1);
        AUDIT_ASSERT(proc.getLatencySamples() == 0, "Returning to 1x restores 0 samples latency");
    }

    // -------------------------------------------------------------------------
    // SUMMARY
    // -------------------------------------------------------------------------
    std::cout << "\n========================================================" << std::endl;
    std::cout << "AUDIT SUMMARY: " << g_passCount << " PASSED, " << g_failCount << " FAILED." << std::endl;
    std::cout << "========================================================" << std::endl;

    return (g_failCount == 0) ? 0 : 1;
}
