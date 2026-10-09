#include "CompressorGRMeter.h"

namespace SJAudio
{

CompressorGRMeter::CompressorGRMeter(const Compressor& compRef)
    : comp(compRef)
{
}

void CompressorGRMeter::updateVisuals(float deltaTimeSeconds)
{
    const float rawDb = comp.getGainReductionDb(); // 0.0 down to -60.0 dB

    // Instant attack for gain reduction (more negative)
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

void CompressorGRMeter::paint(juce::Graphics& g)
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
    if (std::abs(currentVisualDb) < 0.2f)
    {
        g.setColour(juce::Colour(0xff28a745)); // Green "0.0"
        g.drawText("0.0", footerArea, juce::Justification::centred);
    }
    else
    {
        g.setColour(juce::Colour(0xffe67e22)); // Amber dB readout
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

    // Calibrated dB scale tick points: 0, -3, -6, -10, -20, -30
    struct ScalePoint { float db; const char* text; };
    const ScalePoint points[] = {
        {   0.0f,   "0" },
        {  -3.0f,  "-3" },
        {  -6.0f,  "-6" },
        { -10.0f, "-10" },
        { -20.0f, "-20" },
        { -30.0f, "-30" }
    };

    auto dbToY = [&](float db) -> float
    {
        float absDb = std::abs(juce::jlimit(-30.0f, 0.0f, db));
        float norm = 0.0f;
        if (absDb <= 3.0f)
            norm = (absDb / 3.0f) * 0.20f;
        else if (absDb <= 6.0f)
            norm = 0.20f + ((absDb - 3.0f) / 3.0f) * 0.20f;
        else if (absDb <= 10.0f)
            norm = 0.40f + ((absDb - 6.0f) / 4.0f) * 0.20f;
        else if (absDb <= 20.0f)
            norm = 0.60f + ((absDb - 10.0f) / 10.0f) * 0.25f;
        else
            norm = 0.85f + ((absDb - 20.0f) / 10.0f) * 0.15f;

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
            juce::Colour(0xffffa500), barArea.getX(), barArea.getY(),
            juce::Colour(0xffe74c3c), barArea.getX(), activeBar.getBottom(),
            false
        );
        g.setGradientFill(grGrad);
        g.fillRoundedRectangle(activeBar, 2.0f);

        // Tip line
        g.setColour(juce::Colour(0xffffdd55));
        g.drawHorizontalLine(static_cast<int>(activeBar.getBottom()), activeBar.getX(), activeBar.getRight());
    }
}

} // namespace SJAudio
