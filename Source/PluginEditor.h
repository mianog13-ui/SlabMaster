#pragma once
#include "PluginProcessor.h"

namespace slab
{
    const juce::Colour bg      { 0xff121418 };
    const juce::Colour panel   { 0xff1b1e24 };
    const juce::Colour panelHi { 0xff262a32 };
    const juce::Colour accent  { 0xffff5a36 };
    const juce::Colour text    { 0xffd5d9e0 };
    const juce::Colour dim     { 0xff7d8490 };
}

//==============================================================================
class SlabLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SlabLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos,
                           float startAngle, float endAngle, juce::Slider&) override;
    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool highlighted, bool down) override;
    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool highlighted, bool down) override;
    juce::Font getLabelFont (juce::Label&) override;
};

//==============================================================================
class Knob : public juce::Component
{
public:
    Knob (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID, const juce::String& name);
    void resized() override;

private:
    juce::Slider slider;
    juce::Label label;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
};

//==============================================================================
class SlabMasterAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       private juce::Timer
{
public:
    explicit SlabMasterAudioProcessorEditor (SlabMasterAudioProcessor&);
    ~SlabMasterAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Section
    {
        juce::String title;
        std::vector<Knob*> knobs;
        std::vector<juce::ToggleButton*> toggles;   // laid out right to left in the title bar
        juce::Rectangle<int> area;
    };

    Knob* addKnob (const juce::String& id, const juce::String& name);
    void layoutRow (juce::Rectangle<int> row, std::vector<Section*> sections);
    void drawMeter (juce::Graphics&, juce::Rectangle<float> r, const juce::String& name,
                    float fraction, juce::Colour c);
    void timerCallback() override;
    void stepPreset (int delta);

    SlabMasterAudioProcessor& proc;
    SlabLookAndFeel laf;

    juce::ComboBox presetBox;
    juce::TextButton prevBtn { "<" }, nextBtn { ">" };
    juce::ToggleButton bypassBtn { "BYPASS" }, mbBtn { "ON" }, clipBtn { "CLIP ON" },
                       bcBtn { "ON" }, bcAutoBtn { "AUTO REL" };

    using BtnAtt = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<BtnAtt> bypassAtt, mbAtt, clipAtt, bcAtt, bcAutoAtt;

    std::vector<std::unique_ptr<Knob>> knobs;
    Section secInput, secTone, secMulti, secBus, secSat, secStereo, secLimit;

    float inLevel = 0.0f, outLevel = 0.0f, grLevel = 0.0f, bcLevel = 0.0f;   // dB
    juce::Rectangle<float> meterArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SlabMasterAudioProcessorEditor)
};
