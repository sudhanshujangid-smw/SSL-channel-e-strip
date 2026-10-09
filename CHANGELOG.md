# Changelog

All notable changes to the **SJ Channel Strip** plugin are documented in this file.
The project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.0.0] - 2026-10-08

### Initial Production Release

#### Added
- **Core Signal Chain Architecture:**
  - Complete 10-stage console processing pipeline: Input Trim & Polarity -> HPF/LPF -> Gate/Expander -> 4-Band Parametric EQ -> Dynamic EQ -> VCA Compressor -> Nonlinear Saturation -> Output Trim -> Master Meters.
  - Bit-accurate dry-path copy for phase-coherent parallel processing and zero-click 15 ms crossfading.
  - Zero heap allocation in the audio processing thread (`processBlock`).

- **Input & Filter Modules:**
  - Input trim gain ranging from -24.0 dB to +24.0 dB with 25 ms exponential parameter smoothing.
  - Polarity inversion switch ($180^\circ$ phase flip) with clickless smoothed sign transition.
  - High-Pass Filter (20 Hz - 1000 Hz) and Low-Pass Filter (1000 Hz - 20000 Hz) with selectable 12, 18, and 24 dB/octave Butterworth slopes.
  - Dedicated interactive filter response curve preview.

- **Gate / Expander Module:**
  - Dual-mode dynamics processor: hard/soft noise gate and smooth 2:1 downward expander.
  - Comprehensive ballistics: Attack (0.01 - 50 ms), Hold (0 - 500 ms), Release (5 - 1000 ms), Threshold (-60 to +10 dB), and Range (-80 to 0 dB).
  - Stereo-linked peak detector with anti-denormal protection.
  - Internal detector filtering: High-Pass (20 Hz - 4 kHz) and Low-Pass (500 Hz - 20 kHz) 2nd-order biquads.
  - External sidechain input routing and one-click detector audition (Listen).
  - Calibrated real-time gain reduction LED meter (0 to -60 dB).

- **4-Band Parametric Equalizer:**
  - Four versatile frequency bands:
    - Low Band: 20 Hz - 500 Hz, $\pm 15$ dB, Q 0.5 - 5.0, Bell or Shelf switchable.
    - Low-Mid Band: 200 Hz - 3000 Hz, $\pm 15$ dB, Q 0.5 - 5.0, Peaking Bell.
    - High-Mid Band: 600 Hz - 7000 Hz, $\pm 15$ dB, Q 0.5 - 5.0, Peaking Bell.
    - High Band: 1.5 kHz - 20 kHz, $\pm 15$ dB, Q 0.5 - 5.0, Bell or Shelf switchable.
  - Interactive multi-band frequency response graph with draggable handles, mouse wheel Q adjustments, and curve overlays.

- **Dynamic Equalizer (Priority 1):**
  - Real-time level-dependent dynamic filter operating across the full audio spectrum (20 Hz - 20 kHz).
  - Dual dynamics modes: Compress (frequency-specific ducking/de-essing) and Expand (upward dynamic excitation).
  - Dedicated controls: Frequency, Static Gain ($\pm 12$ dB), Q (0.5 - 10.0), Threshold (-60 to 0 dB), Range (0 to 24 dB), Attack (0.1 - 100 ms), and Release (10 - 1000 ms).
  - Sidechain bandpass detector audition mode (Listen).
  - 16-sample block-smoothed biquad coefficient interpolation for ultra-low CPU overhead.
  - Dual-action visual meter displaying real-time detector level and dynamic gain adjustment.

- **VCA Compressor Module:**
  - Classic console-style VCA compression with continuous quadratic soft-knee (0 - 18 dB).
  - Threshold (-40 to +10 dB), Ratio (1:1 to 20:1), Attack (0.1 - 50 ms), and Release (20 - 1200 ms).
  - Program-dependent Auto-Release mode utilizing a dual-time-constant envelope detector.
  - Parallel compression wet/dry mix blend (0 - 100%) and smoothed makeup gain (-12 to +24 dB).
  - Dedicated sidechain filter section (HPF 20 Hz - 2 kHz, LPF 500 Hz - 20 kHz), external sidechain routing, and detector audition.
  - Calibrated 10-segment gain reduction LED ladder (0 to -30 dB).

- **Nonlinear Saturation & Analog Color:**
  - Original continuous-morphing algebraic sigmoid waveshaper ($y = x / (1 + |x|^p)^{1/p}$).
  - Character control blending between 2nd-order even tube warmth ($0\%$) and odd-order console punch ($100\%$).
  - Perceptual loudness compensation keeping perceived volume steady across drive extremes ($0 - 100\%$).
  - High-frequency analog anti-harshness conditioning filter at 19 kHz.
  - 10 Hz DC-blocking high-pass filter.
  - Dynamic transfer curve visualization and 4-tier saturation activity ladder.

- **Real-Time Spectrum Analyzer (RTA):**
  - 2048-point Fast Fourier Transform (JUCE FFT) with Hann windowing.
  - Lock-free, real-time-safe FIFO ring buffer isolating the audio thread from GUI calculations.
  - Logarithmic frequency axis (20 Hz - 20 kHz) and calibrated decibel scale (-24 to +18 dB).
  - Smooth exponential decay ballistics with instantaneous RTA toggle switch.

- **Multi-Rate Polyphase Oversampling:**
  - Selectable 1x (zero latency), 2x, and 4x oversampling modes powered by JUCE DSP polyphase halfband IIR filters.
  - Applied specifically to nonlinear saturation to eliminate inharmonic aliasing while preserving CPU efficiency.
  - Automatic dynamic latency reporting via `setLatencySamples()`.

- **Preset Management System:**
  - 19 meticulously tuned factory presets covering vocals, bass, drums, guitars, synths, mix bus, and mastering.
  - XML-based preset file format with version validation and error recovery.
  - Fast preset navigation: Previous/Next buttons, Save, Save As, and Delete.
  - User presets stored in `%APPDATA%\SJ Audio\SJ Channel Strip\Presets\`.

- **Metering & Monitoring:**
  - True Peak and RMS input and output level meters.
  - Calibrated analog-style dual VU meters (-18 dBFS reference).
  - Peak-hold indicators and clip warning LEDs.
  - Master output trim (-24 to +24 dB), global mix blend (0 - 100%), and master bypass switch.

- **User Interface & Experience:**
  - Custom dark graphite hardware console aesthetic via `SJLookAndFeel`.
  - Fully resizable window (900x550 minimum, 1920x1080 maximum, 1280x720 default).
  - Comprehensive interactive tooltips across all 73 parameters and controls.
  - Tabbed Equalizer layout cleanly switching between 4-Band Parametric and Dynamic EQ views.
  - 100% original design, typography, and artwork with zero proprietary or third-party assets.

#### Fixed & Optimized
- Hardened all audio buffers (`dryCopyBuffer`, `detectorBuffer`, `sidechainBuffer`) to pre-allocate memory in `prepareToPlay()`, guaranteeing zero runtime allocations.
- Fixed sample rate scaling on all filter and dynamics ballistics coefficients up to 192 kHz.
- Ensured denormal-free audio processing using `juce::ScopedNoDenormals`.
- Validated clean destruction and memory teardown across all APVTS attachments and timers.
- Verified absence of debug dependencies in Release binaries (`MSVCP140D.dll` / `VCRUNTIME140D.dll`).
