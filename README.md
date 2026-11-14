# SlabMaster

A VST3 mastering chain voiced for modern metal: thick low end, tight palm-mutes, forward mids, polished top.

## Signal chain

1. **Input** trim, low cut, high cut
2. **Tone EQ**: low shelf, low-mid bell (mud cut), high-mid bell (presence), high shelf (air)
3. **3-band compressor** (Linkwitz-Riley 4th-order crossovers, phase-aligned), per-band threshold / ratio / gain, shared attack and release
4. **Bus compressor**: stereo-linked glue compressor with threshold, ratio, attack, release (or dual-stage auto release), makeup, sidechain high-pass (so kick and low end don't pump the mix) and a parallel mix knob. Soft knee, with its own gain-reduction meter.
5. **Stereo width** with **mono-below** frequency (keeps kick and bass mono)
6. **Saturation** (tanh, level-compensated) with its own wet mix
7. **Soft clipper** (2x oversampled) to shave transients before the limiter
8. **Limiter** with push (drive), release and ceiling, plus a hard safety clamp at the ceiling
9. **Dry/Wet** mix (latency-compensated)

The saturator and clipper run 2x oversampled, so the plugin reports a small fixed latency to your DAW.

## Presets

1. Heavy Polish (Architects)
2. Atmospheric Sheen (Make Them Suffer)
3. Gritty Aggression (Silent Planet)
4. Technical Clarity (Currents)
5. Punch & Thump (Braunstein)
6. Modern Weight (Cervini)

Every preset is a starting point. All 43 knobs stay editable, and double-clicking a knob resets it to default.

## Build on GitHub (no local setup)

1. Create a new GitHub repository and push this folder to it.
2. Open the **Actions** tab. The "Build SlabMaster VST3" workflow runs on every push (or press **Run workflow**).
3. When it finishes, open the run and download the artifact for your OS.

## Install

- **Windows:** copy `SlabMaster.vst3` to `C:\Program Files\Common Files\VST3\`
- **macOS:** copy to `~/Library/Audio/Plug-Ins/VST3/`, then run `xattr -cr ~/Library/Audio/Plug-Ins/VST3/SlabMaster.vst3` (the build is unsigned)
- **Linux:** copy to `~/.vst3/`

Rescan plugins in your DAW.

## Mastering tips

- Leave 0.5 dB or more of ceiling for streaming (default is -0.5 dB).
- Use **Dry/Wet** and the **Input** trim to level-match when A/B-ing.
- The limiter is a sample-peak limiter, not true-peak; keep the ceiling conservative.

## License note

JUCE is dual-licensed (AGPLv3 / commercial). Personal use is fine; if you distribute SlabMaster, you need to comply with the AGPL or hold a JUCE license.
