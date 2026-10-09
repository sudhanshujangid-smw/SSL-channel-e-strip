#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../DSP/FilterSection.h"

namespace SJAudio
{

class FilterDisplay : public juce::Component
{
public:
    explicit FilterDisplay(const FilterSection& filters);
    ~FilterDisplay() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    const FilterSection& filterSection;

    static float freqToNormalizedX(float freqHz, float width);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterDisplay)
};

} // namespace SJAudio
