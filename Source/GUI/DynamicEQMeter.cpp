#include "DynamicEQMeter.h"

namespace SJAudio
{

DynamicEQMeter::DynamicEQMeter(const DynamicEQ& dynEqRef)
    : dynEq(dynEqRef)
{
}

void DynamicEQMeter::updateVisuals(float deltaTimeSeconds)
{
    const float rawDeltaDb = dynEq.getDynamicGainReductionDb();
    const float rawDetDb = dynEq.getDetectorLevelDb();

    // Fast response into action, smooth decay back to 0 dB
    if (std::abs(rawDeltaDb) > std::abs(currentVisualDeltaDb))
    {
        currentVisualDeltaDb = rawDeltaDb;
    }
    else
    {
        const float decayRate = 60.0f; // dB per second
        if (currentVisualDeltaDb > 0.0f)
            currentVisualDeltaDb = std::max(0.0f, currentVisualDeltaDb - decayRate * deltaTimeSeconds);
        else
            currentVisualDeltaDb = std::min(0.0f, currentVisualDeltaDb + decayRate * deltaTimeSeconds);
    }

    // Detector level envelope (peak meter)
    if (rawDetDb > currentVisualDetDb)
    {
        currentVisualDetDb = rawDetDb;
    }
    else
    {
        const float detDecayRate = 80.0f; // dB per second
        currentVisualDetDb = std::max(-60.0f, currentVisualDetDb - detDecayRate * deltaTimeSeconds);
    }

    repaint();
}

void DynamicEQMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Chassis frame
    g.setColour(juce::Colour(0xff121417));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(juce::Colour(0xff22252e));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    auto contentArea = bounds.reduced(4.0f, 4.0f);

    // Header label
    auto headerArea = contentArea.removeFromTop(16.0f);
    g.setFont(juce::FontOptions(9.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff8b949e));
    g.drawText("DYN ACT", headerArea, juce::Justification::centred);

    // Footer digital value
    auto footerArea = contentArea.removeFromBottom(16.0f);
    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    if (std::abs(currentVisualDeltaDb) < 0.1f)
    {
        g.setColour(juce::Colour(0xff28a745));
        g.drawText("0.0 dB", footerArea, juce::Justification::centred);
    }
    else if (currentVisualDeltaDb < 0.0f)
    {
        g.setColour(juce::Colour(0xffe67e22)); // Amber / orange for compression
        g.drawText(juce::String(currentVisualDeltaDb, 1) + " dB", footerArea, juce::Justification::centred);
    }
    else
    {
        g.setColour(juce::Colour(0xff00d2ff)); // Cyan for boost / expansion
        g.drawText("+" + juce::String(currentVisualDeltaDb, 1) + " dB", footerArea, juce::Justification::centred);
    }

    // Two parallel vertical bars: Left = Dynamic GR/Boost, Right = Sidechain Detector level
    auto meterArea = contentArea.reduced(2.0f, 2.0f);
    const float gap = 4.0f;
    const float barWidth = (meterArea.getWidth() - gap) * 0.5f;

    auto grBarArea = juce::Rectangle<float>(meterArea.getX(), meterArea.getY(), barWidth, meterArea.getHeight());
    auto detBarArea = juce::Rectangle<float>(meterArea.getX() + barWidth + gap, meterArea.getY(), barWidth, meterArea.getHeight());

    // 1. Dynamic Action Bar background
    g.setColour(juce::Colour(0xff0d0e11));
    g.fillRoundedRectangle(grBarArea, 2.0f);

    // Dynamic Action scale: 0 dB at top (downward compression) or mid-point
    // For compress mode (typical): 0 dB at top, -18 dB at bottom
    // If delta is negative: top-down amber/orange fill
    // If delta is positive: bottom-up cyan fill
    const float maxActionDb = 18.0f;
    if (currentVisualDeltaDb < -0.05f)
    {
        // Downward bar from top
        float fraction = juce::jlimit(0.0f, 1.0f, std::abs(currentVisualDeltaDb) / maxActionDb);
        auto activeBar = grBarArea.removeFromTop(fraction * grBarArea.getHeight());

        juce::ColourGradient grad(
            juce::Colour(0xfff39c12), grBarArea.getX(), grBarArea.getY(),
            juce::Colour(0xffe74c3c), grBarArea.getX(), grBarArea.getBottom(),
            false
        );
        g.setGradientFill(grad);
        g.fillRoundedRectangle(activeBar, 1.5f);
    }
    else if (currentVisualDeltaDb > 0.05f)
    {
        // Upward bar from bottom
        float fraction = juce::jlimit(0.0f, 1.0f, currentVisualDeltaDb / maxActionDb);
        auto activeBar = grBarArea.removeFromBottom(fraction * grBarArea.getHeight());

        juce::ColourGradient grad(
            juce::Colour(0xff00b4d8), grBarArea.getX(), grBarArea.getBottom(),
            juce::Colour(0xff0077b6), grBarArea.getX(), grBarArea.getY(),
            false
        );
        g.setGradientFill(grad);
        g.fillRoundedRectangle(activeBar, 1.5f);
    }

    // 2. Detector Input Bar background
    g.setColour(juce::Colour(0xff0d0e11));
    g.fillRoundedRectangle(detBarArea, 2.0f);

    // Detector range: -60 dB (bottom) to 0 dB (top)
    float detNorm = juce::jlimit(0.0f, 1.0f, (currentVisualDetDb + 60.0f) / 60.0f);
    if (detNorm > 0.01f)
    {
        auto activeDet = detBarArea.removeFromBottom(detNorm * detBarArea.getHeight());
        juce::ColourGradient detGrad(
            juce::Colour(0xff2ecc71), detBarArea.getX(), detBarArea.getBottom(),
            juce::Colour(0xfff1c40f), detBarArea.getX(), detBarArea.getY(),
            false
        );
        g.setGradientFill(detGrad);
        g.fillRoundedRectangle(activeDet, 1.5f);
    }

    // Threshold indicator line on detector bar
    const float thDb = dynEq.getThresholdDb();
    float thNorm = juce::jlimit(0.0f, 1.0f, (thDb + 60.0f) / 60.0f);
    float thY = detBarArea.getBottom() - thNorm * detBarArea.getHeight();

    g.setColour(juce::Colour(0xffff4757)); // Bright red threshold needle
    g.drawLine(detBarArea.getX() - 1.0f, thY, detBarArea.getRight() + 1.0f, thY, 1.5f);
}

} // namespace SJAudio
