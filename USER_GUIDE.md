# SJ Channel Strip — User Guide

**SJ Channel Strip v1.0.0**  
**Manufacturer:** SJ Audio  
**Formats:** VST3 (64-bit) & Standalone  
**Platform:** Windows 10 / 11 (x64)

---

## Table of Contents
1. [Introduction](#1-introduction)
2. [Interface Overview & Navigation](#2-interface-overview--navigation)
3. [Module-by-Module Guide](#3-module-by-module-guide)
   - [Input Stage](#input-stage)
   - [Filters (HPF & LPF)](#filters-hpf--lpf)
   - [Gate / Expander](#gate--expander)
   - [4-Band Parametric Equalizer](#4-band-parametric-equalizer)
   - [Dynamic Equalizer](#dynamic-equalizer)
   - [VCA Compressor](#vca-compressor)
   - [Analog Color (Saturation)](#analog-color-saturation)
   - [Output Stage & Master Meters](#output-stage--master-meters)
4. [Routing & Sidechaining (FL Studio)](#4-routing--sidechaining-fl-studio)
5. [Preset Management](#5-preset-management)
6. [Best Practice Mixing Workflows](#6-best-practice-mixing-workflows)
7. [Specifications & Support](#7-specifications--support)

---

## 1. Introduction

The **SJ Channel Strip** is an original analog-console-inspired channel processing strip engineered from the ground up for modern digital audio workstations. Combining surgical parametric control, musical dynamic equalisation, analog-modeled VCA dynamics, and continuous-morphing nonlinear saturation, it delivers the warmth, punch, and authority of classic large-format analog consoles with zero digital compromises.

### Core Architecture Highlights
- **10-Stage Inline Processing:** Every essential mixing stage in a single, coherent signal flow.
- **Dynamic EQ (Priority 1):** Level-dependent dynamic equalization for effortless vocal de-essing and surgical resonance suppression.
- **Analog Soft-Knee Compression:** Featuring program-dependent Auto-Release ballistics and parallel mix capabilities.
- **Morphing Saturation:** Smooth algebraic sigmoid saturation that continuously blends from 2nd-order even tube warmth to odd-order console drive.
- **Polyphase Oversampling:** Selectable 1x, 2x, and 4x halfband oversampling for artifact-free nonlinear saturation.
- **Zero Real-Time Allocations:** Rock-solid stability with denormal protection across buffer sizes from 32 to 4096 samples.

---

## 2. Interface Overview & Navigation

The interface is structured into an ergonomic top-to-bottom layout inspired by modular analog console master sections:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ SJ HEADER: Preset Manager [Save / Load] | Oversampling [1x/2x/4x]           │
├───────┬────────┬──────────┬──────────────┬────────────┬──────────┬──────────┤
│ INPUT │ FILTER │ GATE/EXP │  EQUALIZER   │ COMPRESSOR │  COLOR   │  OUTPUT  │
│       │        │          │ [4-BAND/DYN] │            │  (SAT)   │ & METERS │
├───────┴────────┴──────────┴──────────────┴────────────┴──────────┴──────────┤
│ FOOTER: Signal Chain Flow Summary | Version & Manufacturer Stamp            │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Resizing the Window
- **Default Resolution:** 1280 x 720 px.
- **Minimum Resolution:** 900 x 550 px.
- **Maximum Resolution:** 1920 x 1080 px (Full HD).
- Click and drag the lower-right corner resize handle to adjust the plugin interface size dynamically to suit your screen or display scaling.

### Equalizer Tabs
The central Equalizer module features two interchangeable views:
- **4-BAND EQ:** Displays all 4 parametric frequency bands alongside the real-time spectrum analyzer and combined curve graph.
- **DYNAMIC EQ:** Opens the dedicated dynamic equalizer controls, detector mode selectors, listen audition switch, and dynamic activity meter.

---

## 3. Module-by-Module Guide

### Input Stage
The front end of the channel strip manages pre-processing level calibration and phase coherence:
- **GAIN (-24 dB to +24 dB):** Smooth logarithmic trim to align incoming audio to optimal gain staging (recommended: around -18 dBFS nominal RMS).
- **POLARITY (Ø):** Inverts phase by $180^\circ$. Essential for aligning multi-mic drum kits (snare top/bottom, inside/outside kick) or stereo microphone pairs.
- **INPUT METER:** Calibrated true-peak and RMS meter displaying input audio levels.

---

### Filters (HPF & LPF)
Clean up frequency extremes before dynamic or nonlinear processing:
- **HPF ON / LPF ON:** Independent module bypass toggles.
- **HP FREQ (20 Hz - 1000 Hz):** Butterworth high-pass cutoff to eliminate rumble, mic stand thumps, and low air-conditioner hum.
- **HP SLOPE (12, 18, 24 dB/oct):** Adjusts filter steepness. Use 12 dB/oct for transparent vocal roll-off and 24 dB/oct for sharp sub-bass containment.
- **LP FREQ (1000 Hz - 20000 Hz):** Butterworth low-pass cutoff to tame harsh ultrasonic frequencies or high-end fizz.
- **LP SLOPE (12, 18, 24 dB/oct):** Selectable roll-off rate.

---

### Gate / Expander
Cleans up background bleed, headphone spill, and room noise:
- **GATE ON:** Engages the dynamics gating engine.
- **MODE:**
  - *GATE:* Classic hard gate with a 2.0 dB soft knee to prevent boundary chatter.
  - *EXPANDER:* Smooth 2:1 downward expander for gentle, transparent decay on vocal tails, acoustic instruments, or tom rings.
- **THRESH (-60 dB to +10 dB):** Level below which gating or expansion begins.
- **RANGE (-80 dB to 0 dB):** Maximum depth of attenuation applied when closed.
- **ATTACK (0.01 ms - 50 ms):** Opening speed. Ultra-fast times catch sharp transients; slower settings preserve natural attack envelopes.
- **HOLD (0 ms - 500 ms):** Keeps the gate fully open after the signal drops below threshold, preventing premature gating between vocal syllables.
- **RELEASE (5 ms - 1000 ms):** Decay time for the gate to close.
- **SC FILTER (HPF / LPF):** High-Pass (20 Hz - 4 kHz) and Low-Pass (500 Hz - 20 kHz) filters applied exclusively to the detector path. Allows isolating specific frequency triggers (e.g., keying off a snare transient while ignoring kick bleed).
- **SC SRC (INT / EXT):** Switches detector between the internal track signal and the external sidechain bus.
- **LISTEN:** Routes the detector audio directly to the output to allow precise filtering adjustment.
- **GR METER:** Visualizes real-time gain attenuation in decibels.

---

### 4-Band Parametric Equalizer
Four fully overlapping parametric bands with interactive response graph:
- **EQ ON:** Master bypass for all 4 bands.
- **LOW BAND (20 Hz - 500 Hz, $\pm 15$ dB, Q 0.5 - 5.0):** Bass control. Toggle the **SHELF** switch to transition between a standard peaking bell and a low shelf curve.
- **LOW-MID BAND (200 Hz - 3000 Hz, $\pm 15$ dB, Q 0.5 - 5.0):** Lower midrange control for adding body or cleaning up boxy frequencies.
- **HIGH-MID BAND (600 Hz - 7000 Hz, $\pm 15$ dB, Q 0.5 - 5.0):** Upper midrange presence and vocal clarity sculpting.
- **HIGH BAND (1.5 kHz - 20 kHz, $\pm 15$ dB, Q 0.5 - 5.0):** Air and sheen control. Switch to **SHELF** for high-end shelf boosts.
- **INTERACTIVE GRAPH:** Click and drag any band node to modify frequency and gain simultaneously. Adjust the mouse wheel over a node to change Q bandwidth.

---

### Dynamic Equalizer
A powerful, surgical tool that applies boost or attenuation only when the signal crosses a specified threshold:
- **DYN EQ ON:** Engages the dynamic EQ stage.
- **MODE:**
  - *COMPRESS (CUT):* Attenuates frequencies when the detector exceeds threshold. Perfect for de-essing vocals (around 5 kHz - 8 kHz) or controlling harsh cymbal buildup.
  - *EXPAND (BOOST):* Boosts frequencies dynamically when active. Ideal for adding punch to kick drums or snare slap on strong hits.
- **FREQ (20 Hz - 20 kHz):** Center target frequency.
- **STATIC (-12 dB to +12 dB):** Base static EQ gain applied regardless of signal level.
- **Q (0.5 - 10.0):** Filter bandwidth sharpness. Use high Q (4.0 - 8.0) for surgical notch de-essing or resonance suppression; use moderate Q (1.0 - 2.5) for musical tonal sculpting.
- **THRESH (-60 dB to 0 dB):** Signal level at which dynamic modification begins.
- **RANGE (0 dB to 24 dB):** Maximum allowable dynamic cut or boost.
- **ATTACK (0.1 ms - 100 ms):** Reaction speed of dynamic gain adjustment.
- **RELEASE (10 ms - 1000 ms):** Recovery speed once signal falls back below threshold.
- **LISTEN:** Audition the internal detector bandpass filter to pinpoint exact problem resonances.
- **DYN ACT METER:** Real-time dual display tracking detector signal level and current dynamic gain reduction.

---

### VCA Compressor
An analog-inspired VCA compressor with smooth soft-knee curves:
- **COMP ON:** Toggles the compressor module.
- **THRESH (-40 dB to +10 dB):** Level where compression begins.
- **RATIO (1:1 to 20:1):** Compression ratio. Gentle 2:1 for mix bus glue; punchy 4:1 to 8:1 for drums and aggressive vocals.
- **ATTACK (0.1 ms - 50 ms):** Transient reaction time. Fast settings catch vocal peaks; slower settings let drum transients punch through before clamping down.
- **RELEASE (20 ms - 1200 ms):** Recovery speed.
- **AUTO REL:** Engages an intelligent, program-dependent dual-time-constant circuit. Fast transients recover quickly, while sustained bass notes release smoothly to avoid low-frequency distortion.
- **KNEE (0 dB to 18 dB):** Quadratic soft-knee width for smooth transition into compression.
- **MAKEUP (-12 dB to +24 dB):** Gain compensation applied post-compression.
- **MIX (0% to 100%):** Parallel compression wet/dry blend. Dial in heavy compression and blend back to 30-50% for maximum punch with full transient preservation.
- **SC FILTER (HPF 20 Hz - 2 kHz, LPF 500 Hz - 20 kHz):** Isolates the compressor detector. High-passing the detector at 80-120 Hz prevents heavy bass or kick drums from excessively pumping the mix.
- **SC SRC (INT / EXT):** Switches between internal audio and external sidechain bus (e.g., ducking bass to a kick drum).
- **LISTEN:** Auditions the filtered detector signal.
- **GR METER:** 10-segment LED ladder showing calibrated gain reduction from 0 to -30 dB.

---

### Analog Color (Saturation)
Adds analog harmonic richness, density, and saturation:
- **COLOR ON:** Activates the saturation processor.
- **DRIVE (0% to 100%):** Drives the internal nonlinear waveshaper from subtle console warmth up to +36 dB of rich overdrive.
- **CHARACTER (0% to 100%):** Morphs the harmonic spectrum:
  - *0% (Even Harmonics):* Asymmetrical 2nd-order tube/transformer warmth for full body and low-end thickness.
  - *100% (Odd Harmonics):* Symmetrical odd-order console saturation for forward midrange presence and edge.
- **MIX (0% to 100%):** Parallel saturation blend.
- **OUTPUT (-12 dB to +12 dB):** Post-saturation output level trim.
- **WAVESHAPER DISPLAY:** Real-time animated transfer curve illustrating current drive, character curvature, and saturation activity LEDs (`WARM`, `DRIVE`, `COLOR`, `CRUNCH`).

---

### Output Stage & Master Meters
- **OUTPUT GAIN (-24 dB to +24 dB):** Final calibrated channel strip trim.
- **GLOBAL MIX (0% to 100%):** Blends entire channel strip processing with the bit-accurate dry input signal.
- **MASTER BYPASS:** Seamless 15 ms clickless master bypass of the entire plugin.
- **DUAL ANALOG VU METERS:** Calibrated to -18 dBFS = 0 VU standard for analog console gain staging.
- **OUTPUT STEREO METER:** Displays full-scale Peak and RMS levels with peak-hold indicators and clip warning LEDs.

---

## 4. Routing & Sidechaining (FL Studio)

The SJ Channel Strip features a dedicated stereo sidechain auxiliary bus.

### Setting Up External Sidechain Ducking in FL Studio
1. Place **SJ Channel Strip** on your destination track (e.g., **Mixer Track 2 - Bass**).
2. Select your trigger track (e.g., **Mixer Track 1 - Kick**).
3. Right-click the send switch at the bottom of Track 2 and choose **"Sidechain to this track"**.
4. In the SJ Channel Strip plugin wrapper header, click the **Gear icon (Plugin Options)** -> **Processing** tab.
5. In the **Connections** list, ensure the sidechain input is mapped to Track 1 (Aux).
6. Inside SJ Channel Strip, locate the Compressor or Gate module:
   - Change **SC SRC** from `INT` to `EXT`.
   - Press **LISTEN** briefly to confirm the kick drum is driving the detector.
   - Adjust **THRESH** and **RATIO** to taste.

---

## 5. Preset Management

### Factory Presets
SJ Channel Strip includes 19 production-ready presets:
- **Vocals:** *Clean Vocal*, *Vocal Presence*, *Vocal Warmth*, *Rap Vocal*, *Aggressive Vocal*, *Smooth Vocal*
- **Bass:** *Bass Tight*, *Bass Warm*
- **Drums:** *Kick Punch*, *Snare Crack*, *Drum Bus*
- **Guitars & Keys:** *Acoustic Guitar*, *Electric Guitar*, *Synth*, *Keys*
- **Mastering & Bus:** *Mix Bus*, *Gentle Mix Bus*, *Punchy Mix*, *Clean Master*

### Managing Presets
- **Dropdown Menu:** Click to select any factory or user preset.
- **PREV / NEXT (< / >):** Instant stepping through presets during playback.
- **SAVE:** Overwrite changes to the active user preset (factory presets prompt Save As).
- **SAVE AS:** Name and save current settings as a new custom user preset.
- **DELETE:** Remove custom user presets (factory presets are permanently write-protected).

Presets are stored as standard XML files at:
`%APPDATA%\SJ Audio\SJ Channel Strip\Presets\`

---

## 6. Best Practice Mixing Workflows

### 1. Lead Vocal Polish & De-Essing
1. Set **Input Gain** so vocal peaks average around -18 to -14 dBFS on the Input Meter.
2. Engage **HPF** at 80 Hz with an 18 dB/oct slope to remove plosives.
3. Switch Equalizer to **DYNAMIC EQ**:
   - Set **FREQ** to 6.5 kHz, **Q** to 4.5, **MODE** to `COMPRESS`.
   - Lower **THRESH** until harsh "S" and "T" sibilants trigger 3 to 6 dB of dynamic reduction on the Dyn Act Meter.
4. In the **Compressor** section:
   - Set **Ratio** to 3:1, **Attack** to 10 ms, enable **AUTO REL**.
   - Adjust **Thresh** for 2 to 4 dB of gain reduction on loud phrases.
5. In **Analog Color**:
   - Set **Drive** to 15%, **Character** to 25% (Tube warmth), and **Mix** to 60%.

### 2. Snare Drum Crack & Body
1. Engage **Gate** in `EXPANDER` mode, **Range** to -24 dB, **Hold** to 40 ms, **Release** to 120 ms to cleanly reduce hi-hat spill.
2. In **4-Band EQ**:
   - Low-Mid: Boost 200 Hz by +3 dB (Q 1.5) for body.
   - High-Mid: Boost 4.5 kHz by +4 dB (Q 2.0) for crack.
3. In **Compressor**:
   - Set **Attack** to 25 ms (allows snare stick transient to pass) and **Release** to 80 ms.
   - Dial 6 to 8 dB of compression, then set **Mix** to 65% for parallel punch.
4. In **Analog Color**:
   - Set **Drive** to 30%, **Character** to 80% (Console crunch) for aggressive edge.

### 3. Mix Bus Glue
1. Enable **Compressor**:
   - Set **Ratio** to 2:1, **Attack** to 30 ms, **AUTO REL** active.
   - Set **Sidechain HPF** to 100 Hz to prevent deep sub-bass from triggering unwanted pumping.
   - Aim for a subtle 1 to 2 dB of gain reduction on chorus peaks.
2. Set **Oversampling** in the Header to `2x` or `4x`.
3. In **Analog Color**:
   - Set **Drive** to 10%, **Character** to 50%, **Mix** to 40% for subtle master console tape warmth.
4. Verify gain staging against the **Analog VU Meter** (hovering around 0 VU).

---

## 7. Specifications & Support

| Feature | Specification |
|---|---|
| **Plugin Formats** | VST3 (64-bit), Standalone Application |
| **Supported DAWs** | FL Studio 20/21/24, Ableton Live, Reaper, Cubase, Studio One, Bitwig |
| **Operating System** | Windows 10 / Windows 11 (64-bit) |
| **Sample Rates** | 44.1 kHz, 48 kHz, 88.2 kHz, 96 kHz, 176.4 kHz, 192 kHz |
| **Buffer Sizes** | 32 to 4096 samples |
| **Internal Processing** | 64-bit floating point math |
| **Latency** | 0 samples (1x mode); compensated in 2x and 4x oversampling modes |
| **Memory Footprint** | ~35 MB RAM |

© 2026 SJ Audio. All rights reserved.
