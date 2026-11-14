#include "PluginProcessor.h"
#include "PluginEditor.h"

using Coeffs = juce::dsp::IIR::Coefficients<float>;

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout SlabMasterAudioProcessor::createLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto addF = [&params] (const char* id, const char* name, float lo, float hi, float def,
                           float centre, const char* unit, int decimals)
    {
        juce::NormalisableRange<float> range (lo, hi);
        if (centre > 0.0f)
            range.setSkewForCentre (centre);

        const juce::String u (unit);
        auto attrs = juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction ([u, decimals] (float v, int)
                                          { return (juce::String (v, decimals) + " " + u).trim(); })
            .withValueFromStringFunction ([] (const juce::String& t) { return t.getFloatValue(); });

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { id, 1 }, name, range, def, attrs));
    };

    auto addB = [&params] (const char* id, const char* name, bool def)
    {
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            juce::ParameterID { id, 1 }, name, def));
    };

    addB ("bypass", "Bypass", false);

    // Input and filters
    addF ("inGain", "Input",    -12.0f, 12.0f, 0.0f, 0.0f, "dB", 1);
    addF ("hpf",    "Low Cut",   20.0f, 150.0f, 25.0f, 60.0f, "Hz", 0);
    addF ("lpf",    "High Cut", 8000.0f, 20000.0f, 20000.0f, 14000.0f, "Hz", 0);

    // Tone EQ
    addF ("lsFreq", "LS Freq", 40.0f, 300.0f, 100.0f, 100.0f, "Hz", 0);
    addF ("lsGain", "LS Gain", -6.0f, 6.0f, 0.0f, 0.0f, "dB", 1);
    addF ("lmFreq", "LM Freq", 100.0f, 800.0f, 300.0f, 300.0f, "Hz", 0);
    addF ("lmGain", "LM Gain", -6.0f, 6.0f, 0.0f, 0.0f, "dB", 1);
    addF ("lmQ",    "LM Q",    0.3f, 4.0f, 1.0f, 1.0f, "", 2);
    addF ("hmFreq", "HM Freq", 1000.0f, 8000.0f, 3000.0f, 3000.0f, "Hz", 0);
    addF ("hmGain", "HM Gain", -6.0f, 6.0f, 0.0f, 0.0f, "dB", 1);
    addF ("hmQ",    "HM Q",    0.3f, 4.0f, 1.0f, 1.0f, "", 2);
    addF ("hsFreq", "HS Freq", 4000.0f, 16000.0f, 10000.0f, 9000.0f, "Hz", 0);
    addF ("hsGain", "HS Gain", -6.0f, 6.0f, 0.0f, 0.0f, "dB", 1);

    // Multiband compressor
    addB ("mbOn", "Multiband On", true);
    addF ("xo1", "Xover 1", 60.0f, 400.0f, 150.0f, 150.0f, "Hz", 0);
    addF ("xo2", "Xover 2", 1000.0f, 6000.0f, 2500.0f, 2500.0f, "Hz", 0);
    addF ("lowThr",   "Low Thr",   -40.0f, 0.0f, -14.0f, 0.0f, "dB", 1);
    addF ("lowRatio", "Low Ratio", 1.0f, 10.0f, 2.0f, 0.0f, ":1", 1);
    addF ("lowGain",  "Low Gain",  -6.0f, 6.0f, 0.0f, 0.0f, "dB", 1);
    addF ("midThr",   "Mid Thr",   -40.0f, 0.0f, -14.0f, 0.0f, "dB", 1);
    addF ("midRatio", "Mid Ratio", 1.0f, 10.0f, 2.0f, 0.0f, ":1", 1);
    addF ("midGain",  "Mid Gain",  -6.0f, 6.0f, 0.0f, 0.0f, "dB", 1);
    addF ("highThr",   "High Thr",   -40.0f, 0.0f, -14.0f, 0.0f, "dB", 1);
    addF ("highRatio", "High Ratio", 1.0f, 10.0f, 2.0f, 0.0f, ":1", 1);
    addF ("highGain",  "High Gain",  -6.0f, 6.0f, 0.0f, 0.0f, "dB", 1);
    addF ("mbAttack",  "MB Attack",  1.0f, 100.0f, 20.0f, 20.0f, "ms", 1);
    addF ("mbRelease", "MB Release", 20.0f, 500.0f, 150.0f, 150.0f, "ms", 0);

    // Bus compressor
    addB ("bcOn", "Bus Comp On", true);
    addB ("bcAuto", "Bus Auto Release", false);
    addF ("bcThr",     "Bus Thr",     -30.0f, 0.0f, -12.0f, 0.0f, "dB", 1);
    addF ("bcRatio",   "Bus Ratio",   1.5f, 10.0f, 2.0f, 4.0f, ":1", 1);
    addF ("bcAttack",  "Bus Attack",  0.1f, 100.0f, 30.0f, 20.0f, "ms", 1);
    addF ("bcRelease", "Bus Release", 20.0f, 1200.0f, 200.0f, 200.0f, "ms", 0);
    addF ("bcMakeup",  "Bus Makeup",  0.0f, 12.0f, 0.0f, 0.0f, "dB", 1);
    addF ("bcSC",      "Bus SC HPF",  20.0f, 300.0f, 90.0f, 90.0f, "Hz", 0);
    addF ("bcMix",     "Bus Mix",     0.0f, 100.0f, 100.0f, 0.0f, "%", 0);

    // Saturation and clipper
    addF ("satDrive", "Sat Drive", 0.0f, 100.0f, 15.0f, 0.0f, "%", 0);
    addF ("satMix",   "Sat Mix",   0.0f, 100.0f, 50.0f, 0.0f, "%", 0);
    addB ("clipOn", "Clipper On", true);
    addF ("clipDrive", "Clip Drive", 0.0f, 12.0f, 1.5f, 0.0f, "dB", 1);
    addF ("clipCeil",  "Clip Ceil", -12.0f, 0.0f, -2.0f, 0.0f, "dB", 1);

    // Stereo
    addF ("width",     "Width", 0.0f, 200.0f, 100.0f, 0.0f, "%", 0);
    addF ("monoBelow", "Mono Below", 20.0f, 300.0f, 90.0f, 100.0f, "Hz", 0);

    // Limiter and output
    addF ("limDrive",   "Push",    0.0f, 18.0f, 3.0f, 0.0f, "dB", 1);
    addF ("limRelease", "Release", 10.0f, 500.0f, 80.0f, 100.0f, "ms", 0);
    addF ("ceiling",    "Ceiling", -3.0f, 0.0f, -0.5f, 0.0f, "dB", 1);
    addF ("mix",        "Dry/Wet", 0.0f, 100.0f, 100.0f, 0.0f, "%", 0);

    return { params.begin(), params.end() };
}

//==============================================================================
SlabMasterAudioProcessor::SlabMasterAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout())
{
    applyPreset (0);
}

bool SlabMasterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

//==============================================================================
int SlabMasterAudioProcessor::getNumPrograms() { return (int) getPresets().size(); }

const juce::String SlabMasterAudioProcessor::getProgramName (int index)
{
    const auto& p = getPresets();
    return (index >= 0 && index < (int) p.size()) ? juce::String (p[(size_t) index].name) : juce::String();
}

void SlabMasterAudioProcessor::applyPreset (int index)
{
    const auto& presets = getPresets();
    if (index < 0 || index >= (int) presets.size())
        return;

    // Reset everything (except bypass) to default, then apply the preset values on top.
    for (auto* p : getParameters())
        if (auto* rp = dynamic_cast<juce::RangedAudioParameter*> (p))
            if (rp->paramID != "bypass")
                rp->setValueNotifyingHost (rp->getDefaultValue());

    for (const auto& v : presets[(size_t) index].values)
        if (auto* rp = apvts.getParameter (v.id))
            rp->setValueNotifyingHost (rp->convertTo0to1 (v.value));

    currentPreset = index;
}

void SlabMasterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    apvts.state.setProperty ("preset", currentPreset, nullptr);
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void SlabMasterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
        {
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
            currentPreset = juce::jlimit (0, getNumPrograms() - 1,
                                          (int) apvts.state.getProperty ("preset", 0));
        }
}

//==============================================================================
void SlabMasterAudioProcessor::ensureCapacity (int n)
{
    if (n <= maxBlock)
        return;

    // Only reached if a host exceeds the block size it announced. Rare, so allocating is acceptable.
    maxBlock = n;
    dryBuf.setSize (2, n, false, false, true);
    for (auto& b : bandBuf)
        b.setSize (2, n, false, false, true);
    oversampling.initProcessing ((size_t) n);
}

void SlabMasterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    maxBlock = juce::jmax (1, samplesPerBlock);

    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) maxBlock, 2 };

    for (auto* f : { &hpf, &lpf, &lowShelf, &lowMid, &highMid, &highShelf })
    {
        f->prepare (spec);
        f->reset();
    }

    xo1.reset(); xo2.reset(); lowAllpass.reset();
    lastXo1 = lastXo2 = lastMono = -1.0f;

    for (auto& c : comp)
    {
        c.prepare (spec);
        c.reset();
    }

    dryBuf.setSize (2, maxBlock);
    for (auto& b : bandBuf)
        b.setSize (2, maxBlock);

    sideHP.reset();

    for (auto& f : bcSC)
        f.reset();
    bcEnv = bcEnvFast = bcEnvSlow = 0.0f;
    lastBcSC = -1.0f;

    oversampling.initProcessing ((size_t) maxBlock);
    oversampling.reset();
    const int latency = (int) std::round (oversampling.getLatencyInSamples());
    setLatencySamples (latency);

    limiter.prepare (spec);
    limiter.reset();

    dryDelay.prepare (spec);
    dryDelay.setDelay ((float) latency);

    lastInGain = juce::Decibels::decibelsToGain (apvts.getRawParameterValue ("inGain")->load());
    lastMix = 1.0f;
}

//==============================================================================
void SlabMasterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int n = buffer.getNumSamples();
    if (buffer.getNumChannels() < 2 || n == 0)
        return;

    ensureCapacity (n);

    auto get = [this] (const char* id) { return apvts.getRawParameterValue (id)->load(); };
    auto dbToGain = [] (float db) { return juce::Decibels::decibelsToGain (db); };
    const auto sr = currentSampleRate;

    const bool bypass = get ("bypass") > 0.5f;

    // ---- meters (input) and dry copy -----------------------------------------------------
    const float inPeak = juce::jmax (buffer.getMagnitude (0, 0, n), buffer.getMagnitude (1, 0, n));
    inMeter.store (juce::jmax (inMeter.load(), inPeak));

    for (int ch = 0; ch < 2; ++ch)
        dryBuf.copyFrom (ch, 0, buffer, ch, 0, n);

    // ---- input trim -----------------------------------------------------------------------
    const float inGain = dbToGain (get ("inGain"));
    buffer.applyGainRamp (0, n, lastInGain, inGain);
    lastInGain = inGain;

    // ---- filters + tone EQ ----------------------------------------------------------------
    {
        const float hpfF = get ("hpf"), lpfF = get ("lpf");

        *hpf.state       = *Coeffs::makeHighPass (sr, hpfF);
        *lpf.state       = *Coeffs::makeLowPass  (sr, juce::jmin (lpfF, (float) (sr * 0.45)));
        *lowShelf.state  = *Coeffs::makeLowShelf  (sr, get ("lsFreq"), 0.707f, dbToGain (get ("lsGain")));
        *lowMid.state    = *Coeffs::makePeakFilter (sr, get ("lmFreq"), get ("lmQ"), dbToGain (get ("lmGain")));
        *highMid.state   = *Coeffs::makePeakFilter (sr, get ("hmFreq"), get ("hmQ"), dbToGain (get ("hmGain")));
        *highShelf.state = *Coeffs::makeHighShelf (sr, juce::jmin (get ("hsFreq"), (float) (sr * 0.45)),
                                                   0.707f, dbToGain (get ("hsGain")));

        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> ctx (block);

        if (hpfF > 20.5f)       hpf.process (ctx);
        if (lpfF < 19900.0f)    lpf.process (ctx);
        lowShelf.process (ctx);
        lowMid.process (ctx);
        highMid.process (ctx);
        highShelf.process (ctx);
    }

    // ---- 3-band compression ---------------------------------------------------------------
    if (get ("mbOn") > 0.5f)
    {
        const float f1 = get ("xo1"), f2 = get ("xo2");
        if (f1 != lastXo1) { xo1.set (sr, f1); lastXo1 = f1; }
        if (f2 != lastXo2) { xo2.set (sr, f2); lowAllpass.set (sr, f2); lastXo2 = f2; }

        for (int ch = 0; ch < 2; ++ch)
        {
            const float* in = buffer.getReadPointer (ch);
            float* lo = bandBuf[0].getWritePointer (ch);
            float* mi = bandBuf[1].getWritePointer (ch);
            float* hi = bandBuf[2].getWritePointer (ch);

            for (int i = 0; i < n; ++i)
            {
                float low, rest, mid, high, apLow, apHigh;
                xo1.split (ch, in[i], low, rest);
                xo2.split (ch, rest, mid, high);
                // Allpass-match the low band to the second crossover's phase shift
                lowAllpass.split (ch, low, apLow, apHigh);

                lo[i] = apLow + apHigh;
                mi[i] = mid;
                hi[i] = high;
            }
        }

        const char* thr[3]   = { "lowThr", "midThr", "highThr" };
        const char* ratio[3] = { "lowRatio", "midRatio", "highRatio" };
        const char* gain[3]  = { "lowGain", "midGain", "highGain" };

        for (int b = 0; b < 3; ++b)
        {
            comp[b].setThreshold (get (thr[b]));
            comp[b].setRatio (get (ratio[b]));
            comp[b].setAttack (get ("mbAttack"));
            comp[b].setRelease (get ("mbRelease"));

            juce::dsp::AudioBlock<float> bb (bandBuf[b]);
            auto sub = bb.getSubBlock (0, (size_t) n);
            juce::dsp::ProcessContextReplacing<float> ctx (sub);
            comp[b].process (ctx);

            bandBuf[b].applyGain (0, n, dbToGain (get (gain[b])));
        }

        for (int ch = 0; ch < 2; ++ch)
        {
            buffer.copyFrom (ch, 0, bandBuf[0], ch, 0, n);
            buffer.addFrom  (ch, 0, bandBuf[1], ch, 0, n);
            buffer.addFrom  (ch, 0, bandBuf[2], ch, 0, n);
        }
    }

    // ---- stereo width with mono bass ------------------------------------------------------
    {
        const float width = get ("width") * 0.01f;
        const float monoF = get ("monoBelow");
        if (monoF != lastMono)
        {
            sideHP.coefficients = Coeffs::makeHighPass (sr, monoF);
            lastMono = monoF;
        }

        float* l = buffer.getWritePointer (0);
        float* r = buffer.getWritePointer (1);
        for (int i = 0; i < n; ++i)
        {
            const float m = 0.5f * (l[i] + r[i]);
            float s = 0.5f * (l[i] - r[i]);
            s = sideHP.processSample (s) * width;
            l[i] = m + s;
            r[i] = m - s;
        }
    }

    // ---- bus compressor (stereo-linked glue, sidechain HPF, parallel mix) -----------------
    if (get ("bcOn") > 0.5f)
    {
        const float scF = get ("bcSC");
        if (scF != lastBcSC)
        {
            for (auto& f : bcSC)
                f.coefficients = Coeffs::makeHighPass (sr, scF);
            lastBcSC = scF;
        }

        const float thr = get ("bcThr");
        const float slope = 1.0f - 1.0f / get ("bcRatio");
        const float makeup = dbToGain (get ("bcMakeup"));
        const float mix = get ("bcMix") * 0.01f;
        const bool autoRel = get ("bcAuto") > 0.5f;

        auto coef = [sr] (float ms) { return std::exp (-1.0f / (0.001f * ms * (float) sr)); };
        const float aAtt = coef (get ("bcAttack"));
        const float aRel = coef (get ("bcRelease"));
        const float aFast = coef (60.0f), aSlow = coef (450.0f);
        constexpr float knee = 6.0f;

        float* l = buffer.getWritePointer (0);
        float* r = buffer.getWritePointer (1);
        float peakGr = 0.0f;

        for (int i = 0; i < n; ++i)
        {
            const float dl = l[i], dr = r[i];
            const float level = juce::jmax (std::abs (bcSC[0].processSample (dl)),
                                            std::abs (bcSC[1].processSample (dr)));
            const float over = juce::Decibels::gainToDecibels (level, -100.0f) - thr;

            float gr = 0.0f;
            if (2.0f * over > knee)
                gr = slope * over;
            else if (2.0f * over > -knee)
            {
                const float t = over + knee * 0.5f;
                gr = slope * t * t / (2.0f * knee);
            }

            auto smooth = [gr] (float& env, float aA, float aR)
            {
                const float a = gr > env ? aA : aR;
                env = a * env + (1.0f - a) * gr;
            };

            if (autoRel)
            {
                // Dual-stage release: fast for transients, slow for sustained material
                smooth (bcEnvFast, aAtt, aFast);
                smooth (bcEnvSlow, aAtt, aSlow);
                bcEnv = juce::jmax (bcEnvFast, bcEnvSlow);
            }
            else
            {
                smooth (bcEnv, aAtt, aRel);
                bcEnvFast = bcEnvSlow = bcEnv;
            }

            peakGr = juce::jmax (peakGr, bcEnv);

            const float g = std::pow (10.0f, -bcEnv * 0.05f) * makeup;
            l[i] = dl + mix * (dl * g - dl);
            r[i] = dr + mix * (dr * g - dr);
        }

        bcMeter.store (juce::jmax (bcMeter.load(), peakGr));
    }
    else
    {
        bcEnv = bcEnvFast = bcEnvSlow = 0.0f;
    }

    // ---- saturation + soft clipper (2x oversampled) ---------------------------------------
    {
        const float satDrive = get ("satDrive");
        const float satMix = get ("satMix") * 0.01f;
        const bool satActive = satDrive > 0.5f && satMix > 0.0f;

        const float d = 1.0f + satDrive * 0.05f;            // 1 .. 6
        const float satNorm = 1.0f / (2.0f * std::tanh (0.5f * d));

        const bool clipOn = get ("clipOn") > 0.5f;
        const float ceilLin = dbToGain (get ("clipCeil"));
        const float clipIn = dbToGain (get ("clipDrive")) / ceilLin;
        constexpr float knee = 0.75f;

        if (satActive || clipOn)
        {
            juce::dsp::AudioBlock<float> blk (buffer);
            auto up = oversampling.processSamplesUp (blk);

            for (size_t ch = 0; ch < up.getNumChannels(); ++ch)
            {
                float* x = up.getChannelPointer (ch);
                for (size_t i = 0; i < up.getNumSamples(); ++i)
                {
                    float v = x[i];

                    if (satActive)
                    {
                        const float wet = std::tanh (d * v) * satNorm;
                        v += satMix * (wet - v);
                    }

                    if (clipOn)
                    {
                        float a = std::abs (v) * clipIn;
                        if (a > knee)
                            a = knee + (1.0f - knee) * std::tanh ((a - knee) / (1.0f - knee));
                        v = std::copysign (a, v) * ceilLin;
                    }

                    x[i] = v;
                }
            }

            oversampling.processSamplesDown (blk);
        }
        else
        {
            // Keep the latency constant when the stage is switched off
            // by running the signal through the oversampler untouched.
            juce::dsp::AudioBlock<float> blk (buffer);
            oversampling.processSamplesUp (blk);
            oversampling.processSamplesDown (blk);
        }
    }

    // ---- limiter --------------------------------------------------------------------------
    {
        const float ceilDb = get ("ceiling");
        const float ceilLin = dbToGain (ceilDb);

        buffer.applyGain (dbToGain (get ("limDrive")));
        const float prePeak = juce::jmax (buffer.getMagnitude (0, 0, n), buffer.getMagnitude (1, 0, n));

        limiter.setThreshold (ceilDb);
        limiter.setRelease (get ("limRelease"));

        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        limiter.process (ctx);

        // Safety clamp so nothing can exceed the ceiling
        for (int ch = 0; ch < 2; ++ch)
        {
            float* x = buffer.getWritePointer (ch);
            for (int i = 0; i < n; ++i)
                x[i] = juce::jlimit (-ceilLin, ceilLin, x[i]);
        }

        const float postPeak = juce::jmax (buffer.getMagnitude (0, 0, n), buffer.getMagnitude (1, 0, n));
        const float gr = (postPeak > 1.0e-6f && prePeak > postPeak)
                             ? juce::Decibels::gainToDecibels (prePeak / postPeak) : 0.0f;
        grMeter.store (juce::jmax (grMeter.load(), gr));
    }

    // ---- latency-compensated dry/wet (bypass = fully dry) ---------------------------------
    {
        const float mix = bypass ? 0.0f : get ("mix") * 0.01f;
        const float start = lastMix;
        lastMix = mix;

        for (int ch = 0; ch < 2; ++ch)
        {
            float* w = buffer.getWritePointer (ch);
            const float* dry = dryBuf.getReadPointer (ch);

            for (int i = 0; i < n; ++i)
            {
                dryDelay.pushSample (ch, dry[i]);
                const float dd = dryDelay.popSample (ch);
                const float m = start + (mix - start) * ((float) i / (float) n);
                w[i] = dd * (1.0f - m) + w[i] * m;
            }
        }
    }

    const float outPeak = juce::jmax (buffer.getMagnitude (0, 0, n), buffer.getMagnitude (1, 0, n));
    outMeter.store (juce::jmax (outMeter.load(), outPeak));
}

//==============================================================================
juce::AudioProcessorEditor* SlabMasterAudioProcessor::createEditor()
{
    return new SlabMasterAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SlabMasterAudioProcessor();
}
