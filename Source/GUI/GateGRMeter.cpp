#include "GateGRMeter.h"

namespace SJAudio
{

GateGRMeter::GateGRMeter(const GateExpander& gateRef)
    : gate(gateRef)
{
}

void GateGRMeter::updateVisuals(float deltaTimeSeconds)
{
    const float rawDb = gate.getGainReductionDb(); // 0.0 down to -80.0 dB

    // Instant attack for reduction (more negative)
    if (rawDb < currentVisualDb)
    {
        currentVisualDb = rawDb;
    }
    else
    {
        // Smooth release upwards towards 0 dB (~250 ms)
        const float releaseRate = 120.0f; // dB per second
        currentVisualDb = std::min(0.0f, currentVisualDb + releaseRate * deltaTimeSeconds);
    }

    repaint();
}

void GateGRMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Dark chassis meter frame
    g.setColour(juce::Colour(0xff121417));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(juce::Colour(0xff22252e));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    auto contentArea = bounds.reduced(4.0f, 4.0f);

    // Header label
    auto headerArea = contentArea.removeFromTop(16.0f);
    g.setFont(juce::FontOptions(9.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff8b949e));
    g.drawText("GR (dB)", headerArea, juce::Justification::centred);

    // Value display at bottom
    auto footerArea = contentArea.removeFromBottom(16.0f);
    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    if (std::abs(currentVisualDb) < 0.3f)
    {
        g.setColour(juce::Colour(0xff28a745)); // Green "OPEN"
        g.drawText("OPEN", footerArea, juce::Justification::centred);
    }
    else
    {
        g.setColour(juce::Colour(0xfff39c12)); // Amber dB
        g.drawText(juce::String(currentVisualDb, 1), footerArea, juce::Justification::centred);
    }

    // Meter bar + scale area
    auto meterArea = contentArea.reduced(2.0f, 2.0f);
    float labelWidth = 24.0f;
    auto barArea = meterArea.removeFromLeft(meterArea.getWidth() - labelWidth).reduced(2.0f, 0.0f);
    auto scaleArea = meterArea;

    // Background track for meter bar
    g.setColour(juce::Colour(0xff0d0e11));
    g.fillRoundedRectangle(barArea, 2.0f);

    // Calibrated dB scale tick points: 0, -5, -10, -20, -40, -60
    struct ScalePoint { float db; const char* text; };
    const ScalePoint points[] = {
        {   0.0f,   "0" },
        {  -5.0f,  "-5" },
        { -10.0f, "-10" },
        { -20.0f, "-20" },
        { -40.0f, "-40" },
        { -60.0f, "-60" }
    };

    auto dbToY = [&](float db) -> float
    {
        // Piecewise mapping for high resolution in 0 to -20 dB range
        float absDb = std::abs(juce::jlimit(-60.0f, 0.0f, db));
        float norm = 0.0f;
        if (absDb <= 5.0f)
            norm = (absDb / 5.0f) * 0.20f;
        else if (absDb <= 10.0f)
            norm = 0.20f + ((absDb - 5.0f) / 5.0f) * 0.20f;
        else if (absDb <= 20.0f)
            norm = 0.40f + ((absDb - 10.0f) / 10.0f) * 0.25f;
        else if (absDb <= 40.0f)
            norm = 0.65f + ((absDb - 20.0f) / 20.0f) * 0.20f;
        else
            norm = 0.85f + ((absDb - 40.0f) / 20.0f) * 0.15f;

        return barArea.getY() + norm * barArea.getHeight();
    };

    // Draw scale markings
    g.setFont(juce::FontOptions(8.5f, juce::Font::plain));
    g.setColour(juce::Colour(0xff6e7681));
    for (const auto& pt : points)
    {
        float y = dbToY(pt.db);
        g.drawHorizontalLine(static_cast<int>(y), barArea.getRight() + 1.0f, barArea.getRight() + 4.0f);
        g.drawText(pt.text, juce::Rectangle<float>(scaleArea.getX() + 4.0f, y - 6.0f, labelWidth - 4.0f, 12.0f), juce::Justification::centredLeft);
    }

    // Draw active Gain Reduction bar downward from top (y = 0 dB)
    float activeY = dbToY(currentVisualDb);
    float barHeight = activeY - barArea.getY();

    if (barHeight > 1.0f)
    {
        auto activeBar = barArea.withHeight(barHeight);

        juce::ColourGradient grGrad(
            juce::Colour(0xffff9900), barArea.getX(), barArea.getY(),
            juce::Colour(0xffe74c3c), barArea.getX(), activeBar.getBottom(),
            false
        );
        g.setGradientFill(grGrad);
        g.fillRoundedRectangle(activeBar, 2.0f);

        // Illuminating tip line
        g.setColour(juce::Colour(0xffffdd55));
        g.drawHorizontalLine(static_cast<int>(activeBar.getBottom()), activeBar.getX(), activeBar.getRight());
    }
}

} // namespace SJAudio
