#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include "Presets.h"

class SlabMasterAudioProcessor : public juce::AudioProcessor
{
public:
    SlabMasterAudioProcessor();
    ~SlabMasterAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "SlabMaster"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override { return currentPreset; }
    void setCurrentProgram (int index) override { applyPreset (index); }
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void applyPreset (int index);
    int getCurrentPresetIndex() const { return currentPreset; }

    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    juce::AudioProcessorValueTreeState apvts;

    // Peak meters, written by the audio thread and read (and reset) by the editor.
    std::atomic<float> inMeter { 0.0f }, outMeter { 0.0f }, grMeter { 0.0f }, bcMeter { 0.0f };

private:
    using IIRDup = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                                  juce::dsp::IIR::Coefficients<float>>;

    // Stereo Linkwitz-Riley 4th-order crossover (two cascaded Butterworth biquads per side)
    struct LR4
    {
        juce::dsp::IIR::Filter<float> lp[2][2], hp[2][2];

        void set (double sr, float f)
        {
            auto l = juce::dsp::IIR::Coefficients<float>::makeLowPass (sr, f);
            auto h = juce::dsp::IIR::Coefficients<float>::makeHighPass (sr, f);
            for (int c = 0; c < 2; ++c)
                for (int s = 0; s < 2; ++s)
                {
                    lp[c][s].coefficients = l;
                    hp[c][s].coefficients = h;
                }
        }

        void reset()
        {
            for (int c = 0; c < 2; ++c)
                for (int s = 0; s < 2; ++s)
                {
                    lp[c][s].reset();
                    hp[c][s].reset();
                }
        }

        void split (int ch, float x, float& low, float& high)
        {
            low  = lp[ch][1].processSample (lp[ch][0].processSample (x));
            high = hp[ch][1].processSample (hp[ch][0].processSample (x));
        }
    };

    void ensureCapacity (int numSamples);

    double currentSampleRate = 44100.0;
    int maxBlock = 512;
    int currentPreset = 0;

    IIRDup hpf, lpf, lowShelf, lowMid, highMid, highShelf;
    LR4 xo1, xo2, lowAllpass;
    float lastXo1 = -1.0f, lastXo2 = -1.0f, lastMono = -1.0f;

    juce::dsp::Compressor<float> comp[3];
    juce::AudioBuffer<float> bandBuf[3];
    juce::AudioBuffer<float> dryBuf;

    juce::dsp::IIR::Filter<float> sideHP;

    // Bus compressor state
    juce::dsp::IIR::Filter<float> bcSC[2];
    float bcEnv = 0.0f, bcEnvFast = 0.0f, bcEnvSlow = 0.0f, lastBcSC = -1.0f;

    juce::dsp::Oversampling<float> oversampling { 2, 1,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true };

    juce::dsp::Limiter<float> limiter;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> dryDelay { 4096 };

    float lastInGain = 1.0f, lastMix = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SlabMasterAudioProcessor)
};
