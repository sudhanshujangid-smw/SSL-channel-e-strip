#include "Meter.h"
#include <cmath>

namespace SJAudio
{

//==============================================================================
// StereoBarMeter
//==============================================================================
StereoBarMeter::StereoBarMeter(const juce::String& title, MeterSource& source)
    : meterTitle(title), meterSource(source)
{
}

void StereoBarMeter::resized()
{
    // Clip indicator at top center
    clipLedBounds = juce::Rectangle<float>(
        (float)getWidth() * 0.5f - 14.0f,
        22.0f,
        28.0f,
        14.0f
    );
}

void StereoBarMeter::mouseDown(const juce::MouseEvent& e)
{
    if (clipLedBounds.contains(e.position))
    {
        meterSource.resetClip();
        visualClip = false;
        clipTimer = 0.0f;
        repaint();
    }
}

float StereoBarMeter::dbToNormalized(float db)
{
    if (db <= -60.0f) return 0.0f;
    if (db >= 0.0f)   return 1.0f;

    // Piecewise mapping providing enhanced resolution between -24 dB and 0 dB
    if (db < -36.0f)
        return juce::jmap(db, -60.0f, -36.0f, 0.0f, 0.18f);
    if (db < -24.0f)
        return juce::jmap(db, -36.0f, -24.0f, 0.18f, 0.38f);
    if (db < -12.0f)
        return juce::jmap(db, -24.0f, -12.0f, 0.38f, 0.64f);
    if (db < -6.0f)
        return juce::jmap(db, -12.0f, -6.0f, 0.64f, 0.80f);

    return juce::jmap(db, -6.0f, 0.0f, 0.80f, 1.0f);
}

void StereoBarMeter::updateVisuals(float dt)
{
    auto readout = meterSource.getReadout();

    // Fast attack (instant peak response), smooth release (~40 dB/sec decay)
    const float decayRate = 35.0f * dt;

    // Peak Left
    if (readout.peakDbL > visualPeakL)
        visualPeakL = readout.peakDbL;
    else
        visualPeakL = std::max(-100.0f, visualPeakL - decayRate);

    // Peak Right
    if (readout.peakDbR > visualPeakR)
        visualPeakR = readout.peakDbR;
    else
        visualPeakR = std::max(-100.0f, visualPeakR - decayRate);

    // RMS Left & Right with smooth ballistics
    visualRmsL = visualRmsL + (readout.rmsDbL - visualRmsL) * std::clamp(dt * 12.0f, 0.0f, 1.0f);
    visualRmsR = visualRmsR + (readout.rmsDbR - visualRmsR) * std::clamp(dt * 12.0f, 0.0f, 1.0f);

    // Peak Hold logic (1.5 seconds hold time)
    if (visualPeakL >= peakHoldL)
    {
        peakHoldL = visualPeakL;
        peakHoldTimerL = 1.5f;
    }
    else
    {
        peakHoldTimerL -= dt;
        if (peakHoldTimerL <= 0.0f)
            peakHoldL = std::max(-100.0f, peakHoldL - decayRate * 1.5f);
    }

    if (visualPeakR >= peakHoldR)
    {
        peakHoldR = visualPeakR;
        peakHoldTimerR = 1.5f;
    }
    else
    {
        peakHoldTimerR -= dt;
        if (peakHoldTimerR <= 0.0f)
            peakHoldR = std::max(-100.0f, peakHoldR - decayRate * 1.5f);
    }

    // Clip hold logic (hold for 2.0s)
    if (readout.isClipped)
    {
        visualClip = true;
        clipTimer = 2.0f;
    }
    else if (visualClip)
    {
        clipTimer -= dt;
        if (clipTimer <= 0.0f)
            visualClip = false;
    }

    repaint();
}

void StereoBarMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Container background
    g.setColour(juce::Colour(0xff15171b));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(juce::Colour(0xff252932));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    // Header Title
    g.setFont(juce::FontOptions(10.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff98a2b3));
    g.drawText(meterTitle, bounds.removeFromTop(20.0f), juce::Justification::centred);

    // Clip Indicator
    if (visualClip)
    {
        g.setColour(juce::Colour(0xffff2a2a));
        g.fillRoundedRectangle(clipLedBounds, 3.0f);
        g.setFont(juce::FontOptions(9.0f, juce::Font::bold));
        g.setColour(juce::Colours::white);
        g.drawText("CLIP", clipLedBounds, juce::Justification::centred);
    }
    else
    {
        g.setColour(juce::Colour(0xff2b1414));
        g.fillRoundedRectangle(clipLedBounds, 3.0f);
        g.setFont(juce::FontOptions(9.0f, juce::Font::plain));
        g.setColour(juce::Colour(0xff552828));
        g.drawText("CLIP", clipLedBounds, juce::Justification::centred);
    }

    bounds.removeFromTop(18.0f);
    auto meterArea = bounds.reduced(8.0f, 6.0f);

    // Scale column in middle, Left bar on left, Right bar on right
    float barWidth = 14.0f;
    float scaleWidth = meterArea.getWidth() - (barWidth * 2.0f);

    auto leftBarRect  = juce::Rectangle<float>(meterArea.getX(), meterArea.getY(), barWidth, meterArea.getHeight());
    auto scaleRect    = juce::Rectangle<float>(leftBarRect.getRight(), meterArea.getY(), scaleWidth, meterArea.getHeight());
    auto rightBarRect = juce::Rectangle<float>(scaleRect.getRight(), meterArea.getY(), barWidth, meterArea.getHeight());

    // Slot backgrounds
    g.setColour(juce::Colour(0xff0d0e11));
    g.fillRect(leftBarRect);
    g.fillRect(rightBarRect);

    // Scale markings
    const float dbSteps[] = { 0.0f, -3.0f, -6.0f, -12.0f, -18.0f, -24.0f, -36.0f, -48.0f, -60.0f };
    g.setFont(juce::FontOptions(8.5f, juce::Font::plain));

    for (float db : dbSteps)
    {
        float norm = dbToNormalized(db);
        float tickY = meterArea.getBottom() - norm * meterArea.getHeight();

        g.setColour(db >= -3.0f ? juce::Colour(0xff883333) : juce::Colour(0xff2d3340));
        g.drawHorizontalLine((int)tickY, scaleRect.getX() + 2.0f, scaleRect.getRight() - 2.0f);

        if (db == 0.0f || db == -6.0f || db == -12.0f || db == -18.0f || db == -24.0f || db == -36.0f)
        {
            g.setColour(db >= -3.0f ? juce::Colour(0xffd05555) : juce::Colour(0xff6e788c));
            auto labelText = (db == 0.0f) ? "0" : juce::String((int)db);
            g.drawText(labelText, (int)scaleRect.getX(), (int)(tickY - 6.0f), (int)scaleWidth, 12, juce::Justification::centred);
        }
    }

    // Helper lambda to draw vertical LED gradient bar
    auto drawBar = [&](juce::Rectangle<float> rect, float peakDb, float rmsDb, float holdDb)
    {
        float normPeak = dbToNormalized(peakDb);
        float normRms  = dbToNormalized(rmsDb);

        float peakHeight = normPeak * rect.getHeight();
        float rmsHeight  = normRms  * rect.getHeight();

        // Gradient for Peak Bar
        if (peakHeight > 0.0f)
        {
            auto peakBar = rect.withTrimmedTop(rect.getHeight() - peakHeight);
            juce::ColourGradient grad(
                juce::Colour(0xff2ac3a2), rect.getX(), rect.getBottom(),
                juce::Colour(0xffe5a93c), rect.getX(), rect.getY() + rect.getHeight() * 0.35f,
                false
            );
            grad.addColour(0.9f, juce::Colour(0xffe0533b));
            g.setGradientFill(grad);
            g.fillRect(peakBar);
        }

        // Overlay RMS bar in center with distinct semi-translucent highlight
        if (rmsHeight > 0.0f)
        {
            auto rmsBar = rect.withTrimmedTop(rect.getHeight() - rmsHeight).reduced(3.0f, 0.0f);
            g.setColour(juce::Colour(0xaa38ef7d));
            g.fillRect(rmsBar);
        }

        // Peak Hold line
        if (holdDb > -60.0f)
        {
            float normHold = dbToNormalized(holdDb);
            float holdY = rect.getBottom() - normHold * rect.getHeight();
            g.setColour(juce::Colour(0xfffff275)); // Golden bright peak tick
            g.drawHorizontalLine((int)holdY, rect.getX(), rect.getRight());
        }

        // Border around slot
        g.setColour(juce::Colour(0xff20242d));
        g.drawRect(rect, 1.0f);
    };

    drawBar(leftBarRect, visualPeakL, visualRmsL, peakHoldL);
    drawBar(rightBarRect, visualPeakR, visualRmsR, peakHoldR);

    // Channel labels at bottom
    g.setFont(juce::FontOptions(8.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff555f73));
    g.drawText("L", (int)leftBarRect.getX(), (int)(meterArea.getBottom() + 1.0f), (int)barWidth, 12, juce::Justification::centred);
    g.drawText("R", (int)rightBarRect.getX(), (int)(meterArea.getBottom() + 1.0f), (int)barWidth, 12, juce::Justification::centred);
}

//==============================================================================
// VUMeterComponent
//==============================================================================
VUMeterComponent::VUMeterComponent(MeterSource& outputSource)
    : meterSource(outputSource)
{
}

void VUMeterComponent::resized()
{
}

void VUMeterComponent::updateVisuals(float dt)
{
    auto readout = meterSource.getReadout();
    // Visual needle ballistic smoothing (spring-like response)
    visualVuValue += (readout.vuValue - visualVuValue) * std::clamp(dt * 10.0f, 0.0f, 1.0f);
    repaint();
}

void VUMeterComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Vintage console bezel with dark warm vignette
    juce::ColourGradient bgGrad(
        juce::Colour(0xff20232a), bounds.getCentreX(), bounds.getY(),
        juce::Colour(0xff141519), bounds.getCentreX(), bounds.getBottom(),
        false
    );
    g.setGradientFill(bgGrad);
    g.fillRoundedRectangle(bounds, 6.0f);

    g.setColour(juce::Colour(0xff303644));
    g.drawRoundedRectangle(bounds, 6.0f, 1.5f);

    // Header label
    g.setFont(juce::FontOptions(10.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xffd8dee9));
    g.drawText("ANALOG VU METER", bounds.removeFromTop(18.0f), juce::Justification::centred);

    // Calibration subtitle
    g.setFont(juce::FontOptions(8.5f, juce::Font::plain));
    g.setColour(juce::Colour(0xff6e788c));
    g.drawText("0 VU = -18 dBFS", bounds.removeFromTop(12.0f), juce::Justification::centred);

    auto meterFace = bounds.reduced(8.0f, 6.0f);

    // Meter face arc/scale background
    g.setColour(juce::Colour(0xff0d0f12));
    g.fillRoundedRectangle(meterFace, 4.0f);
    g.setColour(juce::Colour(0xff1d222b));
    g.drawRoundedRectangle(meterFace, 4.0f, 1.0f);

    // Needle pivot point at bottom center of meter face
    const float pivotX = meterFace.getCentreX();
    const float pivotY = meterFace.getBottom() + 15.0f;
    const float needleRadius = meterFace.getHeight() * 1.15f;

    // Angle mapping: -20 VU maps to -45 deg (-0.785 rad), +3 VU maps to +45 deg (+0.785 rad)
    // 0 VU is at approx +22.5 deg (+0.392 rad)
    auto vuToAngle = [](float vu) -> float
    {
        // Clamp between -20 and +5 VU
        float clamped = std::clamp(vu, -20.0f, 3.0f);
        // Map -20 to 3 VU to angles [-0.75 rad, +0.75 rad] (~ -43 deg to +43 deg)
        return juce::jmap(clamped, -20.0f, 3.0f, -0.75f, 0.75f);
    };

    // Draw VU scale markings: -20, -10, -7, -5, -3, -2, -1, 0, +1, +3
    const struct { float vu; const char* text; bool isRed; } vuTicks[] = {
        { -20.0f, "-20", false },
        { -10.0f, "-10", false },
        {  -7.0f, "-7",  false },
        {  -5.0f, "-5",  false },
        {  -3.0f, "-3",  false },
        {  -2.0f, "-2",  false },
        {  -1.0f, "-1",  false },
        {   0.0f, "0",   false },
        {   1.0f, "+1",  true  },
        {   3.0f, "+3",  true  }
    };

    g.setFont(juce::FontOptions(8.5f, juce::Font::bold));

    for (const auto& tick : vuTicks)
    {
        float angle = vuToAngle(tick.vu);
        float sinA = std::sin(angle);
        float cosA = -std::cos(angle);

        float rInner = needleRadius - 18.0f;
        float rOuter = needleRadius - 6.0f;
        float rText  = needleRadius - 26.0f;

        float x1 = pivotX + rInner * sinA;
        float y1 = pivotY + rInner * cosA;
        float x2 = pivotX + rOuter * sinA;
        float y2 = pivotY + rOuter * cosA;

        g.setColour(tick.isRed ? juce::Colour(0xffe04848) : juce::Colour(0xff9aa4b8));
        g.drawLine(x1, y1, x2, y2, (tick.vu == 0.0f) ? 2.0f : 1.0f);

        float xt = pivotX + rText * sinA;
        float yt = pivotY + rText * cosA;
        g.drawText(tick.text, (int)(xt - 12.0f), (int)(yt - 6.0f), 24, 12, juce::Justification::centred);
    }

    // Needle shadow
    float needleAngle = vuToAngle(visualVuValue);
    float nSin = std::sin(needleAngle);
    float nCos = -std::cos(needleAngle);

    float tipX = pivotX + (needleRadius - 4.0f) * nSin;
    float tipY = pivotY + (needleRadius - 4.0f) * nCos;

    g.setColour(juce::Colour(0x66000000));
    g.drawLine(pivotX + 2.0f, pivotY + 2.0f, tipX + 2.0f, tipY + 2.0f, 1.8f);

    // Glowing warm amber needle
    g.setColour(juce::Colour(0xfff5a623));
    g.drawLine(pivotX, pivotY, tipX, tipY, 1.8f);

    // Needle pivot cap
    g.setColour(juce::Colour(0xff2b2f3a));
    g.fillEllipse(pivotX - 10.0f, pivotY - 10.0f, 20.0f, 20.0f);
    g.setColour(juce::Colour(0xffe5a93c));
    g.drawEllipse(pivotX - 10.0f, pivotY - 10.0f, 20.0f, 20.0f, 1.5f);
}

} // namespace SJAudio
