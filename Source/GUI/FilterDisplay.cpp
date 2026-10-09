#include "FilterDisplay.h"
#include <cmath>

namespace SJAudio
{

FilterDisplay::FilterDisplay(const FilterSection& filters)
    : filterSection(filters)
{
}

void FilterDisplay::resized()
{
}

float FilterDisplay::freqToNormalizedX(float freqHz, float width)
{
    // Log scale from 20 Hz to 20000 Hz
    constexpr float minLog = 1.30103f;  // log10(20)
    constexpr float maxLog = 4.30103f;  // log10(20000)

    const float clampedFreq = std::clamp(freqHz, 20.0f, 20000.0f);
    const float logFreq = std::log10(clampedFreq);
    const float norm = (logFreq - minLog) / (maxLog - minLog);

    return norm * width;
}

void FilterDisplay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Dark screen background
    g.setColour(juce::Colour(0xff121418));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(juce::Colour(0xff222630));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    auto displayArea = bounds.reduced(6.0f, 4.0f);
    const float w = displayArea.getWidth();
    const float h = displayArea.getHeight();

    // Grid lines for 100 Hz, 1 kHz, 10 kHz
    const float gridFreqs[] = { 100.0f, 1000.0f, 10000.0f };
    g.setColour(juce::Colour(0xff1e222b));

    for (float f : gridFreqs)
    {
        float x = displayArea.getX() + freqToNormalizedX(f, w);
        g.drawVerticalLine((int)x, displayArea.getY(), displayArea.getBottom());
    }

    // Horizontal 0 dB line (placed at 25% from top to show -24 dB to +6 dB range)
    const float zeroDbY = displayArea.getY() + h * 0.25f;
    g.setColour(juce::Colour(0xff262c38));
    g.drawHorizontalLine((int)zeroDbY, displayArea.getX(), displayArea.getRight());

    // Frequency labels
    g.setFont(juce::FontOptions(8.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xff4f586b));
    g.drawText("100Hz", (int)(displayArea.getX() + freqToNormalizedX(100.0f, w) - 15.0f), (int)(displayArea.getBottom() - 10.0f), 30, 10, juce::Justification::centred);
    g.drawText("1kHz", (int)(displayArea.getX() + freqToNormalizedX(1000.0f, w) - 15.0f), (int)(displayArea.getBottom() - 10.0f), 30, 10, juce::Justification::centred);
    g.drawText("10kHz", (int)(displayArea.getX() + freqToNormalizedX(10000.0f, w) - 15.0f), (int)(displayArea.getBottom() - 10.0f), 30, 10, juce::Justification::centred);

    // Draw combined Filter Magnitude curve
    juce::Path curvePath;
    constexpr int numPoints = 80;
    bool pathStarted = false;

    // Range: +6 dB (top) to -30 dB (bottom) -> span 36 dB
    constexpr float topDb = 6.0f;
    constexpr float dbSpan = 36.0f;

    for (int i = 0; i < numPoints; ++i)
    {
        float normX = (float)i / (float)(numPoints - 1);
        // Exponential frequency distribution across 20 Hz -> 20000 Hz
        float freq = 20.0f * std::pow(1000.0f, normX);

        float mag = filterSection.getMagnitudeForFrequency(freq);
        float magDb = juce::Decibels::gainToDecibels(mag, -100.0f);
        magDb = std::clamp(magDb, -30.0f, 6.0f);

        float px = displayArea.getX() + normX * w;
        float py = displayArea.getY() + ((topDb - magDb) / dbSpan) * h;

        if (!pathStarted)
        {
            curvePath.startNewSubPath(px, py);
            pathStarted = true;
        }
        else
        {
            curvePath.lineTo(px, py);
        }
    }

    // Glowing cyan response curve
    g.setColour(juce::Colour(0xff2ac3a2));
    g.strokePath(curvePath, juce::PathStrokeType(1.8f));

    // Title overlay
    g.setFont(juce::FontOptions(8.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff758199));
    g.drawText("FILTER CURVE", (int)displayArea.getX() + 4, (int)displayArea.getY() + 2, 80, 10, juce::Justification::left);
}

} // namespace SJAudio
