#include "EQDisplay.h"
#include "../Parameters.h"
#include <cmath>
#include <algorithm>

namespace SJAudio
{

const EQDisplay::BandInfo EQDisplay::bands[4] = {
    { "LOW",   ParameterIDs::lowFreq,     ParameterIDs::lowGain,     ParameterIDs::lowQ,     ParameterIDs::lowEnabled,     juce::Colour(0xffe74c3c) },
    { "L-MID", ParameterIDs::lowMidFreq,  ParameterIDs::lowMidGain,  ParameterIDs::lowMidQ,  ParameterIDs::lowMidEnabled,  juce::Colour(0xfff39c12) },
    { "H-MID", ParameterIDs::highMidFreq, ParameterIDs::highMidGain, ParameterIDs::highMidQ, ParameterIDs::highMidEnabled, juce::Colour(0xff2ecc71) },
    { "HIGH",  ParameterIDs::highFreq,    ParameterIDs::highGain,    ParameterIDs::highQ,    ParameterIDs::highEnabled,    juce::Colour(0xff9b59b6) }
};

EQDisplay::EQDisplay(juce::AudioProcessorValueTreeState& apvts,
                     const EQSection& eq,
                     const FilterSection& filters,
                     SpectrumFIFO& fifo,
                     const DynamicEQ* dynEq)
    : apvtsRef(apvts),
      eqSection(eq),
      filterSection(filters),
      fifoRef(fifo),
      dynamicEQRef(dynEq),
      forwardFFT(fftOrder),
      window(fftSize, juce::dsp::WindowingFunction<float>::hann)
{
    fftBuffer.fill(0.0f);
    timeDomainBuffer.fill(0.0f);
    spectrumLevels.resize(numSpectrumBins, -40.0f);

    // Precompute lookup tables for GUI & Analyzer optimization (Phase 13)
    precomputeBinRanges();
    precomputeCurvePoints();

    // Setup RTA On/Off toggle switch
    analyzerToggleButton.setButtonText("RTA");
    analyzerToggleButton.setLedColor(juce::Colour(0xff2ac3a2));
    analyzerToggleButton.setTooltip("Enable / Disable Real-Time FFT Spectrum Analyzer");
    addAndMakeVisible(analyzerToggleButton);

    analyzerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvtsRef, ParameterIDs::analyzerEnabled, analyzerToggleButton);

    // Start 30 Hz GUI refresh timer for smooth 30 fps spectrum animation and decay
    startTimerHz(30);
}

void EQDisplay::precomputeBinRanges()
{
    constexpr float refSampleRate = 44100.0f;
    constexpr float binWidth = refSampleRate / static_cast<float>(fftSize); // ~21.5 Hz

    for (int i = 0; i < numSpectrumBins; ++i)
    {
        const float norm = static_cast<float>(i) / static_cast<float>(numSpectrumBins - 1);
        const float freq = 20.0f * std::pow(1000.0f, norm);

        const int centerBin = std::clamp(static_cast<int>(std::round(freq / binWidth)), 1, (fftSize / 2) - 1);
        const int halfWindow = std::clamp(static_cast<int>(std::round(freq * 0.035f / binWidth)), 1, 14);

        precomputedBinRanges[i].startBin = std::max(1, centerBin - halfWindow);
        precomputedBinRanges[i].endBin   = std::min((fftSize / 2) - 1, centerBin + halfWindow);
    }
}

void EQDisplay::precomputeCurvePoints()
{
    for (int i = 0; i < numCurvePoints; ++i)
    {
        const float normX = static_cast<float>(i) / static_cast<float>(numCurvePoints - 1);
        precomputedCurveNormX[i] = normX;
        precomputedCurveFrequencies[i] = 20.0f * std::pow(1000.0f, normX);
    }
}

EQDisplay::~EQDisplay()
{
    stopTimer();
}

void EQDisplay::resized()
{
    // Position RTA toggle button in top right of EQ display
    auto bounds = getLocalBounds();
    analyzerToggleButton.setBounds(bounds.getRight() - 68, 6, 56, 18);
}

void EQDisplay::timerCallback()
{
    bool isRtaActive = true;
    if (auto* param = apvtsRef.getRawParameterValue(ParameterIDs::analyzerEnabled))
        isRtaActive = (param->load() > 0.5f);

    if (isRtaActive)
    {
        bool hadNewData = false;
        // Drain available FIFO frames to analyze the most recent audio block
        while (fifoRef.getNumReady() >= fftSize)
        {
            if (fifoRef.pull(timeDomainBuffer.data(), fftSize))
                hadNewData = true;
        }

        if (hadNewData)
        {
            std::copy(timeDomainBuffer.begin(), timeDomainBuffer.end(), fftBuffer.begin());
            std::fill(fftBuffer.begin() + fftSize, fftBuffer.end(), 0.0f);

            window.multiplyWithWindowingTable(fftBuffer.data(), fftSize);
            forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer.data());

            for (int i = 0; i < numSpectrumBins; ++i)
            {
                const auto& range = precomputedBinRanges[i];
                float maxMag = 0.0f;
                for (int b = range.startBin; b <= range.endBin; ++b)
                    maxMag = std::max(maxMag, fftBuffer[b]);

                // Normalize magnitude and convert to dBFS
                const float magNorm = maxMag / static_cast<float>(fftSize);
                const float db = juce::Decibels::gainToDecibels(magNorm, -100.0f);

                // Calibrated display offset (+18 dB) so musical mix levels align with the dB grid
                const float targetDb = std::clamp(db + 18.0f, -40.0f, 18.0f);

                // Attack and decay ballistics: fast attack, smooth analog falloff
                if (targetDb > spectrumLevels[i])
                    spectrumLevels[i] += 0.65f * (targetDb - spectrumLevels[i]);
                else
                    spectrumLevels[i] = std::max(-40.0f, spectrumLevels[i] - 1.25f);
            }
        }
        else
        {
            // Smooth decay during pauses
            for (auto& lvl : spectrumLevels)
                lvl = std::max(-40.0f, lvl - 1.25f);
        }
    }
    else
    {
        // Smoothly fade out spectrum when RTA is disabled
        for (auto& lvl : spectrumLevels)
            lvl = std::max(-40.0f, lvl - 2.5f);
    }

    repaint();
}

float EQDisplay::freqToX(float freqHz, float width)
{
    constexpr float minLog = 1.30103f; // log10(20)
    constexpr float maxLog = 4.30103f; // log10(20000)

    const float clamped = std::clamp(freqHz, 20.0f, 20000.0f);
    const float norm = (std::log10(clamped) - minLog) / (maxLog - minLog);
    return norm * width;
}

float EQDisplay::xToFreq(float x, float width)
{
    const float norm = std::clamp(x / width, 0.0f, 1.0f);
    return 20.0f * std::pow(1000.0f, norm);
}

float EQDisplay::gainToY(float gainDb, float height)
{
    // Calibrated range: +18 dB (top, y=0) to -24 dB (bottom, y=height), 42 dB span
    constexpr float maxDb = 18.0f;
    constexpr float spanDb = 42.0f;

    const float norm = (maxDb - gainDb) / spanDb;
    return norm * height;
}

float EQDisplay::yToGain(float y, float height)
{
    const float norm = std::clamp(y / height, 0.0f, 1.0f);
    return 18.0f - norm * 42.0f;
}

juce::Point<float> EQDisplay::getHandlePosition(int bandIdx, float width, float height) const
{
    if (bandIdx < 0 || bandIdx >= 4)
        return {};

    float freq = 1000.0f;
    float gain = 0.0f;

    if (auto* fParam = apvtsRef.getRawParameterValue(bands[bandIdx].freqParamId))
        freq = fParam->load();
    if (auto* gParam = apvtsRef.getRawParameterValue(bands[bandIdx].gainParamId))
        gain = gParam->load();

    return { freqToX(freq, width), gainToY(gain, height) };
}

void EQDisplay::mouseDown(const juce::MouseEvent& e)
{
    auto bounds = getLocalBounds().toFloat().reduced(8.0f, 6.0f);
    draggedBandIndex = -1;

    for (int i = 0; i < 4; ++i)
    {
        auto pt = getHandlePosition(i, bounds.getWidth(), bounds.getHeight()) + bounds.getTopLeft();
        if (e.position.getDistanceFrom(pt) <= 15.0f)
        {
            draggedBandIndex = i;
            repaint();
            return;
        }
    }
}

void EQDisplay::mouseDrag(const juce::MouseEvent& e)
{
    if (draggedBandIndex < 0 || draggedBandIndex >= 4)
        return;

    auto bounds = getLocalBounds().toFloat().reduced(8.0f, 6.0f);

    float relX = e.position.x - bounds.getX();
    float relY = e.position.y - bounds.getY();

    float targetFreq = xToFreq(relX, bounds.getWidth());
    float targetGain = yToGain(relY, bounds.getHeight());

    if (auto* fParam = apvtsRef.getParameter(bands[draggedBandIndex].freqParamId))
        fParam->setValueNotifyingHost(fParam->convertTo0to1(targetFreq));

    if (auto* gParam = apvtsRef.getParameter(bands[draggedBandIndex].gainParamId))
        gParam->setValueNotifyingHost(gParam->convertTo0to1(targetGain));

    repaint();
}

void EQDisplay::mouseUp(const juce::MouseEvent&)
{
    draggedBandIndex = -1;
    repaint();
}

void EQDisplay::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    auto bounds = getLocalBounds().toFloat().reduced(8.0f, 6.0f);
    int targetIdx = -1;

    for (int i = 0; i < 4; ++i)
    {
        auto pt = getHandlePosition(i, bounds.getWidth(), bounds.getHeight()) + bounds.getTopLeft();
        if (e.position.getDistanceFrom(pt) <= 18.0f)
        {
            targetIdx = i;
            break;
        }
    }

    if (targetIdx >= 0)
    {
        if (auto* qParam = apvtsRef.getParameter(bands[targetIdx].qParamId))
        {
            float currentQ = 1.0f;
            if (auto* rawQ = apvtsRef.getRawParameterValue(bands[targetIdx].qParamId))
                currentQ = rawQ->load();

            float newQ = std::clamp(currentQ + wheel.deltaY * 0.35f, 0.1f, 10.0f);
            qParam->setValueNotifyingHost(qParam->convertTo0to1(newQ));
            repaint();
        }
    }
}

void EQDisplay::mouseDoubleClick(const juce::MouseEvent& e)
{
    auto bounds = getLocalBounds().toFloat().reduced(8.0f, 6.0f);

    for (int i = 0; i < 4; ++i)
    {
        auto pt = getHandlePosition(i, bounds.getWidth(), bounds.getHeight()) + bounds.getTopLeft();
        if (e.position.getDistanceFrom(pt) <= 15.0f)
        {
            // Reset band gain to 0 dB
            if (auto* gParam = apvtsRef.getParameter(bands[i].gainParamId))
            {
                gParam->setValueNotifyingHost(gParam->convertTo0to1(0.0f));
                repaint();
            }
            return;
        }
    }
}

void EQDisplay::paint(juce::Graphics& g)
{
    auto totalBounds = getLocalBounds().toFloat();

    // 1. Dark chassis console backdrop
    g.setColour(juce::Colour(0xff121418));
    g.fillRoundedRectangle(totalBounds, 4.0f);
    g.setColour(juce::Colour(0xff222630));
    g.drawRoundedRectangle(totalBounds, 4.0f, 1.0f);

    auto graphArea = totalBounds.reduced(8.0f, 6.0f);
    const float w = graphArea.getWidth();
    const float h = graphArea.getHeight();

    if (w <= 10.0f || h <= 10.0f)
        return;

    // 2. Frequency Grid Lines & Labels: 20, 50, 100, 200, 500, 1k, 2k, 5k, 10k, 20k
    const struct { float f; const char* txt; } freqTicks[] = {
        { 20.0f, "20" }, { 50.0f, "50" }, { 100.0f, "100" }, { 200.0f, "200" },
        { 500.0f, "500" }, { 1000.0f, "1k" }, { 2000.0f, "2k" }, { 5000.0f, "5k" },
        { 10000.0f, "10k" }, { 20000.0f, "20k" }
    };

    g.setFont(juce::FontOptions(8.5f, juce::Font::plain));

    for (const auto& tick : freqTicks)
    {
        float x = graphArea.getX() + freqToX(tick.f, w);
        g.setColour(juce::Colour(0xff1c2028));
        g.drawVerticalLine((int)x, graphArea.getY(), graphArea.getBottom());

        g.setColour(juce::Colour(0xff4a5364));
        g.drawText(tick.txt, (int)x - 14, (int)graphArea.getBottom() - 10, 28, 10, juce::Justification::centred);
    }

    // 3. Gain Grid Lines & Labels: +18 dB, +12 dB, +6 dB, 0 dB, -6 dB, -12 dB, -18 dB, -24 dB
    const float gainTicks[] = { 18.0f, 12.0f, 6.0f, 0.0f, -6.0f, -12.0f, -18.0f, -24.0f };

    for (float db : gainTicks)
    {
        float y = graphArea.getY() + gainToY(db, h);

        if (db == 0.0f)
        {
            g.setColour(juce::Colour(0xff3b4556)); // Highlight 0 dB reference line
            g.drawHorizontalLine((int)y, graphArea.getX(), graphArea.getRight());
        }
        else
        {
            g.setColour(juce::Colour(0xff181b22));
            g.drawHorizontalLine((int)y, graphArea.getX(), graphArea.getRight());
        }

        auto txt = (db > 0.0f ? "+" : "") + juce::String((int)db) + "dB";
        g.setColour(db == 0.0f ? juce::Colour(0xff7a879e) : juce::Colour(0xff3f4757));
        g.drawText(txt, (int)graphArea.getX() + 4, (int)y - 5, 36, 10, juce::Justification::left);
    }

    // 4. RTA Real-Time FFT Spectrum Analyzer Rendering
    bool isRtaActive = true;
    if (auto* param = apvtsRef.getRawParameterValue(ParameterIDs::analyzerEnabled))
        isRtaActive = (param->load() > 0.5f);

    if (isRtaActive)
    {
        juce::Path rtaPath;
        bool rtaStarted = false;

        for (int i = 0; i < numSpectrumBins; ++i)
        {
            const float normX = static_cast<float>(i) / static_cast<float>(numSpectrumBins - 1);
            const float px = graphArea.getX() + normX * w;
            const float py = graphArea.getY() + gainToY(spectrumLevels[i], h);

            if (!rtaStarted)
            {
                rtaPath.startNewSubPath(px, py);
                rtaStarted = true;
            }
            else
            {
                rtaPath.lineTo(px, py);
            }
        }

        if (rtaStarted)
        {
            // Fill area under RTA spectrum towards bottom of graph
            juce::Path rtaFill = rtaPath;
            rtaFill.lineTo(graphArea.getRight(), graphArea.getBottom());
            rtaFill.lineTo(graphArea.getX(), graphArea.getBottom());
            rtaFill.closeSubPath();

            juce::ColourGradient rtaGrad(
                juce::Colour(0x242ac3a2), 0.0f, graphArea.getY() + h * 0.3f,
                juce::Colour(0x020e141a), 0.0f, graphArea.getBottom(),
                false
            );
            g.setGradientFill(rtaGrad);
            g.fillPath(rtaFill);

            // Glowing cyan/teal RTA contour trace
            g.setColour(juce::Colour(0x754ea8de));
            g.strokePath(rtaPath, juce::PathStrokeType(1.4f));
        }
    }

    // 5. Individual EQ Curves: HPF, LPF, Low, Low Mid, High Mid, High
    auto drawIndividualCurve = [&](const std::function<float(float)>& magFunc, juce::Colour col, float strokeW)
    {
        juce::Path path;
        bool started = false;

        for (int i = 0; i < numCurvePoints; ++i)
        {
            const float normX = precomputedCurveNormX[i];
            const float freq  = precomputedCurveFrequencies[i];

            const float mag = magFunc(freq);
            const float db = std::clamp(juce::Decibels::gainToDecibels(mag, -100.0f), -24.0f, 18.0f);

            const float px = graphArea.getX() + normX * w;
            const float py = graphArea.getY() + gainToY(db, h);

            if (!started)
            {
                path.startNewSubPath(px, py);
                started = true;
            }
            else
            {
                path.lineTo(px, py);
            }
        }

        g.setColour(col);
        g.strokePath(path, juce::PathStrokeType(strokeW));
    };

    // (a) HPF Curve
    if (filterSection.isHighPassActive())
    {
        drawIndividualCurve([this](float f) {
            return filterSection.getHighPassMagnitudeForFrequency(f);
        }, juce::Colour(0x804ecdc4), 1.2f);
    }

    // (b) LPF Curve
    if (filterSection.isLowPassActive())
    {
        drawIndividualCurve([this](float f) {
            return filterSection.getLowPassMagnitudeForFrequency(f);
        }, juce::Colour(0x804ecdc4), 1.2f);
    }

    // (c) Low Band Curve
    if (eqSection.getLowBand().isEnabled())
    {
        drawIndividualCurve([this](float f) {
            return eqSection.getLowBand().getMagnitudeForFrequency(f);
        }, bands[0].colour.withAlpha(0.6f), 1.2f);
    }

    // (d) Low Mid Band Curve
    if (eqSection.getLowMidBand().isEnabled())
    {
        drawIndividualCurve([this](float f) {
            return eqSection.getLowMidBand().getMagnitudeForFrequency(f);
        }, bands[1].colour.withAlpha(0.6f), 1.2f);
    }

    // (e) High Mid Band Curve
    if (eqSection.getHighMidBand().isEnabled())
    {
        drawIndividualCurve([this](float f) {
            return eqSection.getHighMidBand().getMagnitudeForFrequency(f);
        }, bands[2].colour.withAlpha(0.6f), 1.2f);
    }

    // (f) High Band Curve
    if (eqSection.getHighBand().isEnabled())
    {
        drawIndividualCurve([this](float f) {
            return eqSection.getHighBand().getMagnitudeForFrequency(f);
        }, bands[3].colour.withAlpha(0.6f), 1.2f);
    }

    // (g) Dynamic EQ Band Curve (Priority 1)
    if (dynamicEQRef != nullptr && dynamicEQRef->isEnabled())
    {
        drawIndividualCurve([this](float f) {
            return dynamicEQRef->getMagnitudeForFrequency(f);
        }, juce::Colour(0xff00d2ff).withAlpha(0.75f), 1.4f);
    }

    // 6. Combined Master EQ + Filters Curve
    juce::Path combinedPath;
    bool pathStarted = false;

    for (int i = 0; i < numCurvePoints; ++i)
    {
        const float normX = precomputedCurveNormX[i];
        const float freq  = precomputedCurveFrequencies[i];

        float mag = eqSection.getCombinedMagnitudeForFrequency(freq) *
                    filterSection.getMagnitudeForFrequency(freq);

        if (dynamicEQRef != nullptr && dynamicEQRef->isEnabled())
        {
            mag *= dynamicEQRef->getMagnitudeForFrequency(freq);
        }

        const float magDb = std::clamp(juce::Decibels::gainToDecibels(mag, -100.0f), -24.0f, 18.0f);

        const float px = graphArea.getX() + normX * w;
        const float py = graphArea.getY() + gainToY(magDb, h);

        if (!pathStarted)
        {
            combinedPath.startNewSubPath(px, py);
            pathStarted = true;
        }
        else
        {
            combinedPath.lineTo(px, py);
        }
    }

    // Area fill under combined curve toward 0 dB line
    const float zeroY = graphArea.getY() + gainToY(0.0f, h);
    juce::Path fillPath = combinedPath;
    fillPath.lineTo(graphArea.getRight(), zeroY);
    fillPath.lineTo(graphArea.getX(), zeroY);
    fillPath.closeSubPath();

    juce::ColourGradient fillGrad(
        juce::Colour(0x2cf3ba4f), 0.0f, graphArea.getY(),
        juce::Colour(0x04f3ba4f), 0.0f, graphArea.getBottom(),
        false
    );
    g.setGradientFill(fillGrad);
    g.fillPath(fillPath);

    // Thick luminous golden master response curve
    g.setColour(juce::Colour(0xfff3ba4f));
    g.strokePath(combinedPath, juce::PathStrokeType(2.2f));

    // 7. Interactive Band Handles (Low, Low-Mid, High-Mid, High)
    for (int i = 0; i < 4; ++i)
    {
        auto pt = getHandlePosition(i, w, h) + graphArea.getTopLeft();

        bool enabled = true;
        if (auto* enParam = apvtsRef.getRawParameterValue(bands[i].enableParamId))
            enabled = (enParam->load() > 0.5f);

        auto bandCol = enabled ? bands[i].colour : bands[i].colour.withAlpha(0.35f);

        // Outer glow on hover or drag
        if (draggedBandIndex == i)
        {
            g.setColour(bandCol.withAlpha(0.35f));
            g.fillEllipse(pt.x - 10.0f, pt.y - 10.0f, 20.0f, 20.0f);
        }

        // Handle body
        g.setColour(juce::Colour(0xff121418));
        g.fillEllipse(pt.x - 7.0f, pt.y - 7.0f, 14.0f, 14.0f);
        g.setColour(bandCol);
        g.drawEllipse(pt.x - 7.0f, pt.y - 7.0f, 14.0f, 14.0f, 1.8f);

        // Center dot
        g.fillEllipse(pt.x - 2.5f, pt.y - 2.5f, 5.0f, 5.0f);

        // Label above handle
        g.setFont(juce::FontOptions(8.0f, juce::Font::bold));
        g.setColour(bandCol);
        g.drawText(bands[i].name, (int)pt.x - 16, (int)pt.y - 18, 32, 10, juce::Justification::centred);
    }

    // 8. Title badge in top right (offset to leave room for RTA button)
    g.setFont(juce::FontOptions(9.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff6e7a91));
    g.drawText("4-BAND EQ + RTA SPECTRUM", (int)graphArea.getRight() - 250, (int)graphArea.getY() + 7, 175, 12, juce::Justification::right);
}

} // namespace SJAudio
