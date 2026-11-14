#include "PluginEditor.h"

//==============================================================================
SlabLookAndFeel::SlabLookAndFeel()
{
    setColour (juce::Slider::textBoxTextColourId, slab::text);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxHighlightColourId, slab::accent.withAlpha (0.4f));
    setColour (juce::Label::textColourId, slab::text);
    setColour (juce::ComboBox::backgroundColourId, slab::panelHi);
    setColour (juce::ComboBox::textColourId, slab::text);
    setColour (juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    setColour (juce::ComboBox::arrowColourId, slab::accent);
    setColour (juce::PopupMenu::backgroundColourId, slab::panel);
    setColour (juce::PopupMenu::textColourId, slab::text);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, slab::accent);
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour (juce::TextButton::buttonColourId, slab::panelHi);
    setColour (juce::TextButton::textColourOffId, slab::text);
}

juce::Font SlabLookAndFeel::getLabelFont (juce::Label& l)
{
    return l.getFont();
}

void SlabLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                                        float startAngle, float endAngle, juce::Slider& s)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (5.0f);
    const float r = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto c = bounds.getCentre();
    const float angle = startAngle + pos * (endAngle - startAngle);
    const bool bipolar = s.getMinimum() < 0.0 && s.getMaximum() > 0.0;

    // Track
    juce::Path track;
    track.addCentredArc (c.x, c.y, r, r, 0.0f, startAngle, endAngle, true);
    g.setColour (slab::panelHi);
    g.strokePath (track, juce::PathStrokeType (3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Value arc (from centre for bipolar controls, from the start otherwise)
    const float zeroAngle = bipolar
        ? startAngle + (float) s.valueToProportionOfLength (0.0) * (endAngle - startAngle)
        : startAngle;

    juce::Path val;
    val.addCentredArc (c.x, c.y, r, r, 0.0f, juce::jmin (zeroAngle, angle), juce::jmax (zeroAngle, angle), true);
    g.setColour (s.isEnabled() ? slab::accent : slab::dim);
    g.strokePath (val, juce::PathStrokeType (3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Knob body
    const float kr = r * 0.68f;
    g.setGradientFill (juce::ColourGradient (slab::panelHi.brighter (0.15f), c.x, c.y - kr,
                                             slab::panel.darker (0.2f), c.x, c.y + kr, false));
    g.fillEllipse (c.x - kr, c.y - kr, kr * 2.0f, kr * 2.0f);
    g.setColour (juce::Colours::black.withAlpha (0.5f));
    g.drawEllipse (c.x - kr, c.y - kr, kr * 2.0f, kr * 2.0f, 1.0f);

    // Pointer
    juce::Path p;
    p.addRoundedRectangle (-1.5f, -kr + 2.0f, 3.0f, kr * 0.5f, 1.5f);
    g.setColour (juce::Colours::white);
    g.fillPath (p, juce::AffineTransform::rotation (angle).translated (c.x, c.y));
}

void SlabLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool highlighted, bool)
{
    auto r = b.getLocalBounds().toFloat().reduced (1.0f);
    const bool on = b.getToggleState();

    g.setColour (on ? slab::accent : slab::panelHi);
    g.fillRoundedRectangle (r, r.getHeight() * 0.5f);
    if (highlighted)
    {
        g.setColour (juce::Colours::white.withAlpha (0.08f));
        g.fillRoundedRectangle (r, r.getHeight() * 0.5f);
    }

    g.setColour (on ? juce::Colours::white : slab::dim);
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    g.drawText (b.getButtonText(), r, juce::Justification::centred);
}

void SlabLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& b, const juce::Colour&,
                                            bool highlighted, bool down)
{
    auto r = b.getLocalBounds().toFloat().reduced (1.0f);
    g.setColour (down ? slab::accent : (highlighted ? slab::panelHi.brighter (0.1f) : slab::panelHi));
    g.fillRoundedRectangle (r, 6.0f);
}

//==============================================================================
Knob::Knob (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID, const juce::String& name)
    : attachment (apvts, paramID, slider)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 16);
    slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f,
                                juce::MathConstants<float>::pi * 2.75f, true);
    slider.setVelocityBasedMode (false);

    if (auto* p = apvts.getParameter (paramID))
        slider.setDoubleClickReturnValue (true, p->convertFrom0to1 (p->getDefaultValue()));

    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setFont (juce::FontOptions (12.0f));
    label.setColour (juce::Label::textColourId, slab::dim);

    addAndMakeVisible (slider);
    addAndMakeVisible (label);
}

void Knob::resized()
{
    auto r = getLocalBounds();
    label.setBounds (r.removeFromTop (16));
    slider.setBounds (r);
}

//==============================================================================
SlabMasterAudioProcessorEditor::SlabMasterAudioProcessorEditor (SlabMasterAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&laf);

    // Presets
    const auto& presets = getPresets();
    for (size_t i = 0; i < presets.size(); ++i)
        presetBox.addItem (presets[i].name, (int) i + 1);

    presetBox.setSelectedId (proc.getCurrentPresetIndex() + 1, juce::dontSendNotification);
    presetBox.onChange = [this] { proc.applyPreset (presetBox.getSelectedId() - 1); };
    prevBtn.onClick = [this] { stepPreset (-1); };
    nextBtn.onClick = [this] { stepPreset (+1); };

    addAndMakeVisible (presetBox);
    addAndMakeVisible (prevBtn);
    addAndMakeVisible (nextBtn);

    for (auto* b : { &bypassBtn, &mbBtn, &clipBtn, &bcBtn, &bcAutoBtn })
        addAndMakeVisible (*b);

    bypassAtt = std::make_unique<BtnAtt> (proc.apvts, "bypass", bypassBtn);
    mbAtt     = std::make_unique<BtnAtt> (proc.apvts, "mbOn", mbBtn);
    clipAtt   = std::make_unique<BtnAtt> (proc.apvts, "clipOn", clipBtn);
    bcAtt     = std::make_unique<BtnAtt> (proc.apvts, "bcOn", bcBtn);
    bcAutoAtt = std::make_unique<BtnAtt> (proc.apvts, "bcAuto", bcAutoBtn);

    // Sections
    secInput  = { "INPUT & FILTERS", { addKnob ("inGain", "Input"), addKnob ("hpf", "Low Cut"), addKnob ("lpf", "High Cut") } };

    secTone   = { "TONE EQ", {
        addKnob ("lsFreq", "LS Freq"), addKnob ("lsGain", "LS Gain"),
        addKnob ("lmFreq", "LM Freq"), addKnob ("lmGain", "LM Gain"), addKnob ("lmQ", "LM Q"),
        addKnob ("hmFreq", "HM Freq"), addKnob ("hmGain", "HM Gain"), addKnob ("hmQ", "HM Q"),
        addKnob ("hsFreq", "HS Freq"), addKnob ("hsGain", "HS Gain") } };

    secMulti  = { "3-BAND COMPRESSOR", {
        addKnob ("xo1", "Xover 1"), addKnob ("xo2", "Xover 2"),
        addKnob ("mbAttack", "Attack"), addKnob ("mbRelease", "Release"),
        addKnob ("lowThr", "Low Thr"), addKnob ("lowRatio", "Low Ratio"), addKnob ("lowGain", "Low Gain"),
        addKnob ("midThr", "Mid Thr"), addKnob ("midRatio", "Mid Ratio"), addKnob ("midGain", "Mid Gain"),
        addKnob ("highThr", "High Thr"), addKnob ("highRatio", "High Ratio"), addKnob ("highGain", "High Gain") },
        { &mbBtn } };

    secBus    = { "BUS COMPRESSOR", {
        addKnob ("bcThr", "Threshold"), addKnob ("bcRatio", "Ratio"),
        addKnob ("bcAttack", "Attack"), addKnob ("bcRelease", "Release"),
        addKnob ("bcMakeup", "Makeup"), addKnob ("bcSC", "SC HPF"), addKnob ("bcMix", "Mix") },
        { &bcBtn, &bcAutoBtn } };

    secSat    = { "SATURATION & CLIPPER", {
        addKnob ("satDrive", "Sat Drive"), addKnob ("satMix", "Sat Mix"),
        addKnob ("clipDrive", "Clip Drive"), addKnob ("clipCeil", "Clip Ceil") }, { &clipBtn } };

    secStereo = { "STEREO", { addKnob ("width", "Width"), addKnob ("monoBelow", "Mono Below") } };

    secLimit  = { "LIMITER & OUTPUT", {
        addKnob ("limDrive", "Push"), addKnob ("limRelease", "Release"),
        addKnob ("ceiling", "Ceiling"), addKnob ("mix", "Dry/Wet") } };

    setSize (1040, 790);
    startTimerHz (30);
}

SlabMasterAudioProcessorEditor::~SlabMasterAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

Knob* SlabMasterAudioProcessorEditor::addKnob (const juce::String& id, const juce::String& name)
{
    knobs.push_back (std::make_unique<Knob> (proc.apvts, id, name));
    addAndMakeVisible (*knobs.back());
    return knobs.back().get();
}

void SlabMasterAudioProcessorEditor::stepPreset (int delta)
{
    const int count = presetBox.getNumItems();
    if (count == 0)
        return;

    int idx = presetBox.getSelectedItemIndex() + delta;
    idx = (idx + count) % count;
    presetBox.setSelectedItemIndex (idx, juce::sendNotificationSync);
}

void SlabMasterAudioProcessorEditor::timerCallback()
{
    // Follow preset changes made by the host (program change)
    const int cur = proc.getCurrentPresetIndex() + 1;
    if (presetBox.getSelectedId() != cur)
        presetBox.setSelectedId (cur, juce::dontSendNotification);

    auto toDb = [] (float lin) { return lin > 1.0e-5f ? juce::Decibels::gainToDecibels (lin) : -100.0f; };

    auto follow = [] (float& shown, float target, float fall)
    {
        shown = target > shown ? target : juce::jmax (target, shown - fall);
    };

    follow (inLevel,  toDb (proc.inMeter.exchange (0.0f)), 1.5f);
    follow (outLevel, toDb (proc.outMeter.exchange (0.0f)), 1.5f);
    follow (grLevel,  proc.grMeter.exchange (0.0f), 0.8f);
    follow (bcLevel,  proc.bcMeter.exchange (0.0f), 0.5f);

    repaint (meterArea.getSmallestIntegerContainer());
}

//==============================================================================
void SlabMasterAudioProcessorEditor::drawMeter (juce::Graphics& g, juce::Rectangle<float> r,
                                                const juce::String& name, float fraction, juce::Colour c)
{
    auto label = r.removeFromLeft (28.0f);
    g.setColour (slab::dim);
    g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
    g.drawText (name, label, juce::Justification::centredLeft);

    r = r.withSizeKeepingCentre (r.getWidth(), 6.0f);
    g.setColour (slab::panelHi);
    g.fillRoundedRectangle (r, 4.0f);

    fraction = juce::jlimit (0.0f, 1.0f, fraction);
    g.setColour (c);
    g.fillRoundedRectangle (r.withWidth (juce::jmax (0.0f, r.getWidth() * fraction)), 4.0f);
}

void SlabMasterAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (slab::bg);

    // Header
    auto header = getLocalBounds().removeFromTop (64);
    g.setColour (slab::panel);
    g.fillRect (header);
    g.setColour (slab::accent);
    g.fillRect (header.removeFromBottom (2));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (26.0f, juce::Font::bold));
    g.drawText ("SLABMASTER", 20, 8, 260, 30, juce::Justification::centredLeft);
    g.setColour (slab::dim);
    g.setFont (juce::FontOptions (12.0f));
    g.drawText ("modern metal mastering chain", 21, 36, 260, 18, juce::Justification::centredLeft);

    // Sections
    for (auto* s : { &secInput, &secTone, &secMulti, &secBus, &secSat, &secStereo, &secLimit })
    {
        g.setColour (slab::panel);
        g.fillRoundedRectangle (s->area.toFloat(), 8.0f);
        g.setColour (slab::dim);
        g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
        g.drawText (s->title, s->area.getX() + 14, s->area.getY() + 6, 260, 20, juce::Justification::centredLeft);
    }

    // Meters
    auto m = meterArea;
    const float rowH = m.getHeight() / 4.0f;
    drawMeter (g, m.removeFromTop (rowH), "IN",  juce::jmap (inLevel,  -48.0f, 0.0f, 0.0f, 1.0f), slab::text.withAlpha (0.8f));
    drawMeter (g, m.removeFromTop (rowH), "OUT", juce::jmap (outLevel, -48.0f, 0.0f, 0.0f, 1.0f),
               outLevel > -0.5f ? slab::accent : slab::text.withAlpha (0.8f));
    drawMeter (g, m.removeFromTop (rowH), "BUS", juce::jmap (bcLevel, 0.0f, 12.0f, 0.0f, 1.0f), slab::accent);
    drawMeter (g, m.removeFromTop (rowH), "LIM", juce::jmap (grLevel, 0.0f, 12.0f, 0.0f, 1.0f), slab::accent);
}

//==============================================================================
void SlabMasterAudioProcessorEditor::layoutRow (juce::Rectangle<int> row, std::vector<Section*> sections)
{
    const int gap = 10;
    int totalKnobs = 0;
    for (auto* s : sections)
        totalKnobs += (int) s->knobs.size();

    const int usable = row.getWidth() - gap * ((int) sections.size() - 1);
    int x = row.getX();

    for (size_t i = 0; i < sections.size(); ++i)
    {
        auto* s = sections[i];
        const int w = (i + 1 == sections.size()) ? row.getRight() - x
                                                 : usable * (int) s->knobs.size() / totalKnobs;
        s->area = { x, row.getY(), w, row.getHeight() };
        x += w + gap;

        auto inner = s->area.reduced (8, 6);
        auto title = inner.removeFromTop (22);
        for (auto* t : s->toggles)
        {
            t->setBounds (title.removeFromRight (78).reduced (0, 1));
            title.removeFromRight (4);
        }

        inner.removeFromTop (2);
        const int n = (int) s->knobs.size();
        const int kw = inner.getWidth() / n;
        for (int k = 0; k < n; ++k)
            s->knobs[(size_t) k]->setBounds (inner.getX() + k * kw, inner.getY(), kw, inner.getHeight());
    }
}

void SlabMasterAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    auto header = area.removeFromTop (64);

    // Header controls
    prevBtn.setBounds (300, 18, 30, 28);
    presetBox.setBounds (336, 18, 250, 28);
    nextBtn.setBounds (592, 18, 30, 28);
    bypassBtn.setBounds (640, 20, 84, 24);
    meterArea = juce::Rectangle<float> (748.0f, 8.0f, (float) header.getWidth() - 748.0f - 20.0f, 48.0f);

    // Body: four rows
    auto body = area.reduced (12, 12);
    const int gap = 10;
    const int rowH = (body.getHeight() - 3 * gap) / 4;

    layoutRow (body.removeFromTop (rowH), { &secInput, &secTone });
    body.removeFromTop (gap);
    layoutRow (body.removeFromTop (rowH), { &secMulti });
    body.removeFromTop (gap);
    layoutRow (body.removeFromTop (rowH), { &secBus, &secStereo });
    body.removeFromTop (gap);
    layoutRow (body, { &secSat, &secLimit });
}
