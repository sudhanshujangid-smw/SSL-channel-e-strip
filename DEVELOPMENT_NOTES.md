# SJ Channel Strip — Development Notes & Technical Architecture

**SJ Channel Strip v1.0.0**  
**Manufacturer:** SJ Audio  
**Framework:** JUCE 9.0.3 / C++20  
**Target Formats:** VST3, Standalone (x64)

---

## 1. Codebase Architecture & File Structure

The project follows a strictly modular separation between DSP processing routines and GUI presentation components.

```
Source/
├── Parameters.h             # All 73 APVTS parameter IDs, ranges, default values, and layout
├── PluginProcessor.h/cpp    # AudioProcessor lifecycle, APVTS, bus layouts, DSP processing loop
├── PluginEditor.h/cpp       # Main AudioProcessorEditor, layout grid, timer callback, module cards
├── PresetManager.h/cpp      # Preset serialization, XML parsing, factory preset definitions
│
├── DSP/
│   ├── InputStage.h/cpp     # Pre-gain trim, polarity inversion, smoothed crossfades
│   ├── FilterSection.h/cpp  # Butterworth HPF/LPF (12/18/24 dB/oct), frequency responses
│   ├── GateExpander.h/cpp   # Dynamics gate/expander, detector biquads, sidechain routing
│   ├── EQBand.h/cpp         # Transposed Direct-Form II parametric biquad band
│   ├── EQSection.h/cpp      # 4-band equalizer cascade, bypass logic, curve evaluation
│   ├── DynamicEQ.h/cpp      # Real-time level-dependent dynamic biquad, 16-sample smoothing
│   ├── Compressor.h/cpp     # VCA soft-knee compressor, auto-release, sidechain, parallel mix
│   ├── Saturation.h/cpp     # Algebraic sigmoid waveshaper, asymmetric harmonics, DC blocker
│   └── SpectrumFIFO.h       # Lock-free single-producer single-consumer ring buffer for FFT
│
└── GUI/
    ├── SJLookAndFeel.h/cpp  # Custom hardware console look-and-feel (knobs, sliders, toggles)
    ├── SJComponents.h/cpp   # Header console bar, preset dropdown, toggle buttons, rotary knobs
    ├── EQCurveComponent.h/cpp # Multi-band EQ graph, draggable nodes, FFT spectrum analyzer
    ├── FilterCurveComponent.h/cpp # High-pass and low-pass filter response curve display
    ├── Meter.h/cpp          # Peak/RMS meter bars and dual calibrated analog VU meter
    ├── GateGRMeter.h/cpp    # Real-time downward LED gain reduction meter for gate
    ├── CompressorGRMeter.h/cpp # Calibrated LED ladder gain reduction meter for compressor
    ├── DynamicEQMeter.h/cpp # Real-time detector level and dynamic activity meter
    └── SaturationMeter.h/cpp # Waveshaper curve display and 4-tier saturation activity ladder
```

---

## 2. Threading Model & Real-Time Safety

### Zero-Allocation Guarantee
In modern real-time audio systems, calling `malloc`, `free`, `new`, or allocating resizing containers (`std::vector::resize`, `juce::AudioBuffer::setSize`) on the audio rendering thread can trigger priority inversion and audio dropouts (clicks/glitches).

In `SJ Channel Strip`:
1. All working audio buffers (`dryCopyBuffer`, `detectorBuffer`, `sidechainBuffer`) are sized to a minimum of 4096 samples in `prepareToPlay()`.
2. The JUCE Oversampling engines (2x and 4x stages) are fully initialized and pre-allocated during `prepareToPlay()`.
3. In `processBlock()`, buffers are accessed strictly via `dryCopyBuffer.copyFrom(...)` using the host's provided `numSamples` with zero dynamic allocation.

### Lock-Free Thread Communication
1. **Spectrum Analysis:**
   - The audio thread writes incoming time-domain samples into a lock-free single-producer single-consumer circular buffer (`SpectrumFIFO`).
   - The GUI thread (running at 60 Hz via `PluginEditor::timerCallback`) pulls blocks from `SpectrumFIFO` and executes the 2048-point Fast Fourier Transform (JUCE FFT with Hann windowing) asynchronously.
   - The audio thread is never blocked by GUI FFT computation.
2. **Meter Telemetry:**
   - Meter reduction values, detector amplitudes, and RMS levels are stored in relaxed atomic variables (`std::atomic<float>`).
   - The GUI reads these values without locks or mutexes.

### Denormal Number Prevention
- Audio processing blocks are guarded with `juce::ScopedNoDenormals noDenormals;`.
- Floating-point calculations inside IIR filters and envelope detectors incorporate anti-denormal offsets (e.g., $1.0\times 10^{-15}$) to prevent CPU denormal stalls during periods of silence.

---

## 3. Mathematical DSP Implementations

### A. Butterworth Filter Biquad Design
High-Pass and Low-Pass filters utilize cascaded 2nd-order Direct-Form II transposed biquad sections:
- **12 dB/oct:** 1 biquad section ($Q = 0.7071$).
- **18 dB/oct:** 1 biquad section ($Q = 1.0$) + 1 single-pole RC section.
- **24 dB/oct:** 2 cascaded biquad sections ($Q_1 = 0.5412$, $Q_2 = 1.3065$).

### B. Dynamic EQ Architecture
The dynamic equalizer computes gain modifications dynamically on a per-sample detector:
1. **Detector Bandpass Filter:**
   A narrow 2nd-order bandpass filter isolates the target frequency band:
   $$H_{\text{bp}}(s) = \frac{\frac{\omega_0}{Q} s}{s^2 + \frac{\omega_0}{Q} s + \omega_0^2}$$
2. **Level Detection & Ballistics:**
   The filtered detector signal is converted to instantaneous decibels:
   $$L_{\text{det}} = 20 \log_{10} (|x_{\text{bp}}| + 10^{-5})$$
   Envelope follower smoothing:
   $$L_{\text{env}}[n] = \alpha \cdot L_{\text{env}}[n-1] + (1 - \alpha) \cdot L_{\text{det}}[n]$$
3. **Gain Modification:**
   If $L_{\text{env}} > \text{Threshold}$:
   $$\Delta G_{\text{dB}} = \text{clamp}\left( (L_{\text{env}} - \text{Threshold}) \cdot \text{Slope}, 0.0, \text{Range} \right)$$
   - In **Compress Mode**: $G_{\text{target}} = G_{\text{static}} - \Delta G_{\text{dB}}$.
   - In **Expand Mode**: $G_{\text{target}} = G_{\text{static}} + \Delta G_{\text{dB}}$.
4. **16-Sample Block Smoothing:**
   To eliminate high CPU consumption from recomputing biquad trigonometric coefficients on every single sample, filter coefficients are updated every 16 samples using linear parameter interpolation across the sub-block.

### C. VCA Compressor Ballistics
1. **Continuous Quadratic Soft Knee:**
   For threshold $T$, ratio $R$, knee width $W$, and detector level $x_G$ in dB:
   $$y_G = \begin{cases}
   x_G & \text{if } x_G < T - W/2 \\
   x_G + \frac{(1/R - 1)(x_G - T + W/2)^2}{2W} & \text{if } |x_G - T| \le W/2 \\
   T + \frac{x_G - T}{R} & \text{if } x_G > T + W/2
   \end{cases}$$
2. **Log-Domain Ballistics Smoother:**
   Smoothing is performed in decibels:
   $$\text{GR}[n] = \alpha \cdot \text{GR}[n-1] + (1 - \alpha) \cdot (y_G - x_G)$$
   Where $\alpha$ switches between $\alpha_{\text{att}} = 1 - e^{-2.2 / (\tau_{\text{att}} \cdot f_s)}$ and $\alpha_{\text{rel}} = 1 - e^{-2.2 / (\tau_{\text{rel}} \cdot f_s)}$.
3. **Dual-Time-Constant Auto-Release:**
   A short-term envelope (50 ms) tracks transient attacks, while a leaky background envelope (400 ms) tracks continuous program density. The release time dynamically interpolates between 50 ms and 750 ms based on program density.

### D. Nonlinear Saturation & Harmonic Shaping
1. **Harmonic Asymmetry:**
   To produce 2nd-order even harmonics (warm tube character), a quadratic expansion is blended based on character $c \in [0, 1]$:
   $$A = (1 - c) \cdot 0.32$$
   $$x_a = x + A \frac{x^2}{1 + |x|}$$
2. **Morphing Algebraic Sigmoid:**
   Curvature exponent $p = 1.25 + 1.75 \cdot c$:
   $$y = \frac{x_a}{(1 + |x_a|^p)^{1/p}}$$
3. **Perceptual Loudness Normalization:**
   $$G_{\text{comp}} = \frac{1}{\sqrt{1 + 0.35 \cdot (G_{\text{drive}} - 1.0)}}$$
4. **Sample-Exact DC Blocker:**
   Removes DC offset introduced by asymmetric waveshaping:
   $$y_{\text{dc}}[n] = x[n] - x[n-1] + R \cdot y_{\text{dc}}[n-1]$$
   Where $R = 1.0 - (2\pi \cdot 10.0 / f_s)$.

---

## 4. Parameter Architecture & APVTS

The plugin manages 73 distinct parameters via JUCE's `AudioProcessorValueTreeState`:
- **Parameter Groups:**
  - `Input`: Gain, Polarity
  - `Filters`: HPF On/Freq/Slope, LPF On/Freq/Slope
  - `Gate`: On, Mode, Thresh, Range, Attack, Hold, Release, SC On/HPF/LPF/Src/Listen
  - `EQ`: Master On, Low On/Freq/Gain/Q/Shelf, Low-Mid On/Freq/Gain/Q, High-Mid On/Freq/Gain/Q, High On/Freq/Gain/Q/Shelf
  - `Dynamic EQ`: On, Mode, Freq, Static Gain, Q, Thresh, Range, Attack, Release, Listen
  - `Compressor`: On, Auto-Rel, Thresh, Ratio, Attack, Release, Knee, Makeup, Mix, SC On/HPF/LPF/Src/Listen
  - `Saturation`: On, Drive, Character, Mix, Output
  - `Master`: Output Gain, Global Mix, Bypass, Oversampling

### Cached Atomic Pointers
In `PluginProcessor.cpp`, parameter pointers (`std::atomic<float>*`) are queried during construction and cached in a lightweight struct `paramPointers` to avoid hash-table lookups inside `processBlock()`.

---

## 5. Build System & Compilation

### Environment Prerequisites
- Windows 10 / 11 (x64)
- Visual Studio 2026 Build Tools (MSVC v143 toolset)
- CMake 3.22+ and Ninja build system
- JUCE 9.0.3

### Build Commands

#### Release Build (VST3 + Standalone)
```cmd
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
cmake -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --config Release
```

#### Automated Installation & Deployment
```powershell
powershell -ExecutionPolicy Bypass -File .\install_vst3.ps1 -Config Release -DeployStandalone
```

### Dependency Verification
Release binaries must be confirmed to have zero debug runtime dependencies:
```cmd
dumpbin /dependents "%LOCALAPPDATA%\Programs\Common\VST3\SJ Channel Strip.vst3\Contents\x86_64-win\SJ Channel Strip.vst3"
```
*Expected dependencies:* `KERNEL32.dll`, `USER32.dll`, `GDI32.dll`, `SHELL32.dll`, `ole32.dll`, `OLEAUT32.dll`, `MSVCP140.dll`, `VCRUNTIME140.dll`, `VCRUNTIME140_1.dll`, and standard UCRT DLLs.

---

## 6. Testing & Quality Assurance Protocol

Prior to tagging any release, perform the complete Phase 15 & Phase 17 checklist:
1. **Automation Test:** Sweep all 73 parameters rapidly in FL Studio automation clips; verify clickless operation.
2. **Buffer Stress:** Run playback under 32 samples (low latency) and 4096 samples (high latency).
3. **Denormal Stress:** Feed pure silence ($-\infty$ dBFS) for 60 seconds; check CPU meter stays steady at $< 0.5\%$.
4. **Oversampling Switching:** Toggle 1x, 2x, and 4x modes during active audio playback; verify host latency compensation updates smoothly without clicks.
5. **XML Preset Stress:** Save, reload, and verify parameter tree equivalence across restarts.

© 2026 SJ Audio. All rights reserved.
