#pragma once
#include <vector>

struct PresetValue
{
    const char* id;
    float value;
};

struct Preset
{
    const char* name;
    std::vector<PresetValue> values;
};

// Any parameter not listed in a preset falls back to its default.
inline const std::vector<Preset>& getPresets()
{
    static const std::vector<Preset> presets =
    {
        { "Heavy Polish (Architects)", {
            { "bcThr", -14.0f }, { "bcRatio", 2.0f }, { "bcAttack", 30.0f }, { "bcRelease", 200.0f },
            { "bcMakeup", 1.0f }, { "bcSC", 90.0f }, { "bcMix", 100.0f },
            { "inGain", 0.0f }, { "hpf", 28.0f }, { "lpf", 20000.0f },
            { "lsFreq", 90.0f },  { "lsGain", 1.0f },
            { "lmFreq", 280.0f }, { "lmGain", -1.5f }, { "lmQ", 1.2f },
            { "hmFreq", 3200.0f }, { "hmGain", 1.0f }, { "hmQ", 0.9f },
            { "hsFreq", 11000.0f }, { "hsGain", 1.5f },
            { "xo1", 140.0f }, { "xo2", 2600.0f },
            { "lowThr", -16.0f }, { "lowRatio", 2.5f }, { "lowGain", 0.3f },
            { "midThr", -15.0f }, { "midRatio", 2.0f }, { "midGain", 0.0f },
            { "highThr", -17.0f }, { "highRatio", 2.0f }, { "highGain", 0.2f },
            { "mbAttack", 25.0f }, { "mbRelease", 140.0f },
            { "satDrive", 22.0f }, { "satMix", 45.0f },
            { "clipDrive", 2.0f }, { "clipCeil", -2.5f },
            { "width", 115.0f }, { "monoBelow", 100.0f },
            { "limDrive", 5.0f }, { "limRelease", 70.0f }, { "ceiling", -0.5f } } },

        { "Atmospheric Sheen (Make Them Suffer)", {
            { "bcThr", -12.0f }, { "bcRatio", 1.5f }, { "bcAttack", 30.0f }, { "bcRelease", 300.0f },
            { "bcAuto", 1.0f }, { "bcMakeup", 0.5f }, { "bcSC", 100.0f }, { "bcMix", 100.0f },
            { "hpf", 25.0f },
            { "lsFreq", 70.0f },  { "lsGain", 1.5f },
            { "lmFreq", 350.0f }, { "lmGain", -1.0f }, { "lmQ", 0.9f },
            { "hmFreq", 2800.0f }, { "hmGain", 0.5f }, { "hmQ", 0.8f },
            { "hsFreq", 12000.0f }, { "hsGain", 2.5f },
            { "xo1", 120.0f }, { "xo2", 3000.0f },
            { "lowThr", -14.0f }, { "lowRatio", 2.0f }, { "lowGain", 0.5f },
            { "midThr", -13.0f }, { "midRatio", 1.8f }, { "midGain", 0.0f },
            { "highThr", -15.0f }, { "highRatio", 1.6f }, { "highGain", 0.5f },
            { "mbAttack", 30.0f }, { "mbRelease", 200.0f },
            { "satDrive", 18.0f }, { "satMix", 40.0f },
            { "clipDrive", 1.0f }, { "clipCeil", -2.0f },
            { "width", 135.0f }, { "monoBelow", 120.0f },
            { "limDrive", 4.0f }, { "limRelease", 100.0f }, { "ceiling", -0.5f } } },

        { "Gritty Aggression (Silent Planet)", {
            { "bcThr", -16.0f }, { "bcRatio", 4.0f }, { "bcAttack", 10.0f }, { "bcRelease", 120.0f },
            { "bcMakeup", 2.0f }, { "bcSC", 120.0f }, { "bcMix", 70.0f },
            { "hpf", 32.0f },
            { "lsFreq", 100.0f }, { "lsGain", 0.5f },
            { "lmFreq", 250.0f }, { "lmGain", -2.0f }, { "lmQ", 1.4f },
            { "hmFreq", 2500.0f }, { "hmGain", 1.8f }, { "hmQ", 1.0f },
            { "hsFreq", 9000.0f }, { "hsGain", 1.0f },
            { "xo1", 160.0f }, { "xo2", 2200.0f },
            { "lowThr", -18.0f }, { "lowRatio", 3.0f }, { "lowGain", 0.0f },
            { "midThr", -17.0f }, { "midRatio", 3.0f }, { "midGain", 0.5f },
            { "highThr", -18.0f }, { "highRatio", 2.5f }, { "highGain", 0.0f },
            { "mbAttack", 15.0f }, { "mbRelease", 110.0f },
            { "satDrive", 45.0f }, { "satMix", 60.0f },
            { "clipDrive", 3.5f }, { "clipCeil", -3.0f },
            { "width", 105.0f }, { "monoBelow", 130.0f },
            { "limDrive", 6.0f }, { "limRelease", 50.0f }, { "ceiling", -0.5f } } },

        { "Technical Clarity (Currents)", {
            { "bcThr", -13.0f }, { "bcRatio", 2.0f }, { "bcAttack", 10.0f }, { "bcRelease", 100.0f },
            { "bcMakeup", 0.8f }, { "bcSC", 110.0f }, { "bcMix", 80.0f },
            { "hpf", 30.0f },
            { "lsFreq", 110.0f }, { "lsGain", 0.5f },
            { "lmFreq", 320.0f }, { "lmGain", -1.8f }, { "lmQ", 1.5f },
            { "hmFreq", 3800.0f }, { "hmGain", 1.2f }, { "hmQ", 1.1f },
            { "hsFreq", 10000.0f }, { "hsGain", 1.0f },
            { "xo1", 130.0f }, { "xo2", 3200.0f },
            { "lowThr", -15.0f }, { "lowRatio", 2.0f }, { "lowGain", 0.0f },
            { "midThr", -14.0f }, { "midRatio", 1.8f }, { "midGain", 0.0f },
            { "highThr", -16.0f }, { "highRatio", 2.2f }, { "highGain", 0.3f },
            { "mbAttack", 10.0f }, { "mbRelease", 90.0f },
            { "satDrive", 10.0f }, { "satMix", 35.0f },
            { "clipDrive", 1.0f }, { "clipCeil", -1.5f },
            { "width", 110.0f }, { "monoBelow", 110.0f },
            { "limDrive", 4.0f }, { "limRelease", 60.0f }, { "ceiling", -0.5f } } },

        { "Punch & Thump (Braunstein)", {
            { "bcThr", -15.0f }, { "bcRatio", 4.0f }, { "bcAttack", 40.0f }, { "bcRelease", 150.0f },
            { "bcMakeup", 1.5f }, { "bcSC", 110.0f }, { "bcMix", 65.0f },
            { "hpf", 26.0f },
            { "lsFreq", 80.0f },  { "lsGain", 2.0f },
            { "lmFreq", 300.0f }, { "lmGain", -2.0f }, { "lmQ", 1.0f },
            { "hmFreq", 4200.0f }, { "hmGain", 1.5f }, { "hmQ", 1.2f },
            { "hsFreq", 12000.0f }, { "hsGain", 1.2f },
            { "xo1", 150.0f }, { "xo2", 2800.0f },
            { "lowThr", -13.0f }, { "lowRatio", 3.0f }, { "lowGain", 0.5f },
            { "midThr", -12.0f }, { "midRatio", 2.5f }, { "midGain", 0.0f },
            { "highThr", -14.0f }, { "highRatio", 1.8f }, { "highGain", 0.0f },
            { "mbAttack", 35.0f }, { "mbRelease", 160.0f },
            { "satDrive", 25.0f }, { "satMix", 50.0f },
            { "clipDrive", 2.5f }, { "clipCeil", -2.5f },
            { "width", 108.0f }, { "monoBelow", 100.0f },
            { "limDrive", 5.0f }, { "limRelease", 80.0f }, { "ceiling", -0.5f } } },

        { "Modern Weight (Cervini)", {
            { "bcThr", -17.0f }, { "bcRatio", 4.0f }, { "bcAttack", 20.0f }, { "bcRelease", 150.0f },
            { "bcAuto", 1.0f }, { "bcMakeup", 2.0f }, { "bcSC", 100.0f }, { "bcMix", 75.0f },
            { "hpf", 28.0f },
            { "lsFreq", 95.0f },  { "lsGain", 2.0f },
            { "lmFreq", 400.0f }, { "lmGain", -1.0f }, { "lmQ", 0.8f },
            { "hmFreq", 2000.0f }, { "hmGain", 1.5f }, { "hmQ", 0.8f },
            { "hsFreq", 8000.0f }, { "hsGain", 1.0f },
            { "xo1", 140.0f }, { "xo2", 2400.0f },
            { "lowThr", -17.0f }, { "lowRatio", 3.5f }, { "lowGain", 0.5f },
            { "midThr", -16.0f }, { "midRatio", 3.0f }, { "midGain", 0.5f },
            { "highThr", -17.0f }, { "highRatio", 2.5f }, { "highGain", 0.0f },
            { "mbAttack", 20.0f }, { "mbRelease", 120.0f },
            { "satDrive", 35.0f }, { "satMix", 55.0f },
            { "clipDrive", 3.0f }, { "clipCeil", -3.0f },
            { "width", 115.0f }, { "monoBelow", 110.0f },
            { "limDrive", 7.0f }, { "limRelease", 60.0f }, { "ceiling", -0.5f } } }
    };
    return presets;
}
