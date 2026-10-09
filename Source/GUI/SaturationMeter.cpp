#include "SaturationMeter.h"

namespace SJAudio
{

SaturationMeter::SaturationMeter(const Saturation& satRef)
    : sat(satRef)
{
    setOpaque(false);
}

void SaturationMeter::updateVisuals(float deltaTimeSeconds)
{
    const float dt = std::max(0.001f, std::min(0.1f, deltaTimeSeconds));
    const float target = sat.isEnabled() ? sat.getSaturationAmount() : 0.0f;

    if (target > currentActivity)
        currentActivity += (target - currentActivity) * std::min(1.0f, dt * 25.0f);
    else
        currentActivity -= currentActivity * std::min(1.0f, dt * 6.0f);

    currentActivity = juce::jlimit(0.0f, 1.0f, currentActivity);
    repaint();
}

void SaturationMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background Card
    g.setColour(juce::Colour(0xff14171d));
    g.fillRoundedRectangle(bounds, 5.0f);
    g.setColour(juce::Colour(0xff252934));
    g.drawRoundedRectangle(bounds, 5.0f, 1.0f);

    const bool isActive = sat.isEnabled();
    const float charPct = sat.getCharacterPercent();
    const float drivePct = sat.getDrivePercent();

    // 1. Header Mode Text
    auto headerArea = bounds.removeFromTop(20.0f).reduced(4.0f, 2.0f);
    juce::String modeText;
    juce::Colour modeColour;

    if (!isActive)
    {
        modeText = "SAT OFF";
        modeColour = juce::Colour(0xff5c6370);
    }
    else if (charPct < 33.3f)
    {
        modeText = "WARM TUBE";
        modeColour = juce::Colour(0xffe5a93c);
    }
    else if (charPct < 66.6f)
    {
        modeText = "ANALOG TAPE";
        modeColour = juce::Colour(0xfff39c12);
    }
    else
    {
        modeText = "CONSOLE DRIVE";
        modeColour = juce::Colour(0xffff6b4a);
    }

    g.setFont(juce::FontOptions(9.5f, juce::Font::bold));
    g.setColour(modeColour);
    g.drawText(modeText, headerArea, juce::Justification::centred);

    // 2. Transfer Curve Scope
    auto scopeArea = bounds.removeFromTop(bounds.getHeight() - 22.0f).reduced(6.0f, 2.0f);
    g.setColour(juce::Colour(0xff0d0f14));
    g.fillRoundedRectangle(scopeArea, 3.0f);
    g.setColour(juce::Colour(0xff1e222d));
    g.drawRoundedRectangle(scopeArea, 3.0f, 1.0f);

    const float cx = scopeArea.getCentreX();
    const float cy = scopeArea.getCentreY();
    const float hw = (scopeArea.getWidth() - 8.0f) * 0.5f;
    const float hh = (scopeArea.getHeight() - 8.0f) * 0.5f;

    // Grid center axes
    g.setColour(juce::Colour(0xff1c212c));
    g.drawLine(cx, scopeArea.getY() + 3.0f, cx, scopeArea.getBottom() - 3.0f, 1.0f);
    g.drawLine(scopeArea.getX() + 3.0f, cy, scopeArea.getRight() - 3.0f, cy, 1.0f);

    // 45-degree linear reference dashed line
    g.setColour(juce::Colour(0xff2a3040));
    const float dashLengths[] = { 3.0f, 3.0f };
    g.drawDashedLine(juce::Line<float>(cx - hw, cy + hh, cx + hw, cy - hh), dashLengths, 2, 1.0f);

    // Active Waveshaper Transfer Curve
    juce::Path curvePath;
    const int numPoints = 50;
    const float normChar = juce::jlimit(0.0f, 1.0f, charPct * 0.01f);
    const float asym = (1.0f - normChar) * 0.32f;
    const float p = 1.25f + 1.75f * normChar;
    const float invP = 1.0f / p;
    const float displayGain = 1.0f + 2.5f * (drivePct * 0.01f);

    for (int i = 0; i <= numPoints; ++i)
    {
        const float xNorm = -1.5f + 3.0f * (static_cast<float>(i) / static_cast<float>(numPoints));
        const float driven = xNorm * displayGain;
        const float x_a = driven + asym * (driven * driven) / (1.0f + std::abs(driven));
        const float denom = std::pow(1.0f + std::pow(std::abs(x_a), p), invP);
        const float yNorm = x_a / denom;

        const float px = cx + (xNorm / 1.5f) * hw;
        const float py = cy - juce::jlimit(-1.15f, 1.15f, yNorm) * hh;

        if (i == 0)
            curvePath.startNewSubPath(px, py);
        else
            curvePath.lineTo(px, py);
    }

    // Glow effect when active
    if (isActive)
    {
        if (currentActivity > 0.05f)
        {
            g.setColour(modeColour.withAlpha(currentActivity * 0.45f));
            g.strokePath(curvePath, juce::PathStrokeType(4.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        g.setColour(modeColour);
        g.strokePath(curvePath, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    else
    {
        g.setColour(juce::Colour(0xff3f4553));
        g.strokePath(curvePath, juce::PathStrokeType(1.5f));
    }

    // Dynamic signal excursion dot on curve
    if (isActive && currentActivity > 0.02f)
    {
        const float excursion = currentActivity * 1.2f;
        const float drivenDot = excursion * displayGain;
        const float xaDot = drivenDot + asym * (drivenDot * drivenDot) / (1.0f + std::abs(drivenDot));
        const float denomDot = std::pow(1.0f + std::pow(std::abs(xaDot), p), invP);
        const float yDot = xaDot / denomDot;

        const float dotPx = cx + (excursion / 1.5f) * hw;
        const float dotPy = cy - juce::jlimit(-1.15f, 1.15f, yDot) * hh;

        g.setColour(juce::Colours::white);
        g.fillEllipse(dotPx - 2.5f, dotPy - 2.5f, 5.0f, 5.0f);
        g.setColour(modeColour.withAlpha(0.6f));
        g.drawEllipse(dotPx - 4.5f, dotPy - 4.5f, 9.0f, 9.0f, 1.2f);
    }

    // 3. Bottom 4-Segment LED Ladder: WARM -> DRIVE -> COLOR -> CRUNCH
    auto ledArea = bounds.reduced(6.0f, 3.0f);
    const float ledW = (ledArea.getWidth() - 9.0f) / 4.0f;
    const juce::Colour ledColors[4] = {
        juce::Colour(0xff28a745), // Green (Warm)
        juce::Colour(0xfff1c40f), // Yellow (Drive)
        juce::Colour(0xfff39c12), // Orange (Color)
        juce::Colour(0xffe74c3c)  // Red (Crunch)
    };
    const float thresholds[4] = { 0.05f, 0.25f, 0.55f, 0.85f };

    for (int i = 0; i < 4; ++i)
    {
        auto ledRect = juce::Rectangle<float>(ledArea.getX() + i * (ledW + 3.0f), ledArea.getY(), ledW, ledArea.getHeight());
        const bool isLit = isActive && (currentActivity >= thresholds[i]);

        if (isLit)
        {
            g.setColour(ledColors[i]);
            g.fillRoundedRectangle(ledRect, 2.0f);
        }
        else
        {
            g.setColour(juce::Colour(0xff1e222a));
            g.fillRoundedRectangle(ledRect, 2.0f);
            g.setColour(juce::Colour(0xff292e3a));
            g.drawRoundedRectangle(ledRect, 2.0f, 0.8f);
        }
    }
}

} // namespace SJAudio
