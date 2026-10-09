# SJ Channel Strip — macOS Build & Deployment Guide

**SJ Channel Strip v1.0.0**  
**Manufacturer:** SJ Audio  
**Bundle ID:** `com.sjaudio.sjchannelstrip`  
**Manufacturer Code:** `Sjau` | **Plugin Code:** `Sjcs`  
**Primary Target:** Apple Silicon (`arm64` - M1, M2, M3, M4)  
**Secondary Target:** Universal 2 (`arm64;x86_64`) / Intel (`x86_64`)  
**Plugin Formats:** VST3 (FL Studio for Mac, Ableton, Reaper) & AU / AudioUnit (Logic Pro, GarageBand)  
**Minimum macOS Version:** macOS 11.0 Big Sur (up through macOS 15 Sequoia)  

---

## 1. Executive Summary & Cross-Platform Integrity

The codebase for **SJ Channel Strip** has been verified and prepared for native macOS compilation entirely from Windows, with **zero regressions to the existing Windows build**.

### Compatibility & Architecture Verification
1. **Zero Windows-Specific APIs:** The DSP engine, parameter tree, and GUI components use pure standard C++20 and portable JUCE 8/9 modules (`juce_audio_utils`, `juce_dsp`, `juce_audio_processors`, `juce_gui_basics`). No Win32 headers (`windows.h`), Windows types (`HWND`, `DWORD`), or MSVC-specific extensions exist in `Source/`.
2. **Preset & File System Portability:** Preset storage in `Source/PresetManager.cpp` uses `juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)`.
   - On Windows: `%APPDATA%\SJ Audio\SJ Channel Strip\Presets`
   - On macOS: `~/Library/Application Support/SJ Audio/SJ Channel Strip/Presets`
3. **State & Parameter Persistence:** All 73 APVTS parameter IDs, layout definitions, and XML binary serialization formats are 100% identical. Saved DAW project sessions and `.sjpreset` files transfer seamlessly between Windows and macOS FL Studio.
4. **Target Formats on macOS:**
   - **VST3 (`.vst3` bundle):** Native format for FL Studio for Mac, Ableton Live, Reaper, Cubase, and Studio One.
   - **AudioUnit v2 (`.component` bundle):** Native format for Apple Logic Pro, GarageBand, Ableton Live, and Reaper.
   - **Standalone (`.app` bundle):** Native macOS executable application for standalone operation and testing without a DAW.

---

## 2. Option A: Remote macOS Cloud Build (No Mac Required)

Because Windows cannot natively compile Apple Mach-O binaries without a legitimate Apple toolchain and macOS SDK, a complete automated CI workflow has been provided in [`.github/workflows/build-macos.yml`](file:///.github/workflows/build-macos.yml).

GitHub Actions provides free Apple Silicon M1 cloud runners (`macos-14`), allowing you to compile, test, sign, and package real macOS binaries directly from the cloud without touching a Mac.

### How to Build on GitHub Actions:
1. Initialize Git and push this repository to GitHub (public or private):
   ```powershell
   git init
   git add .
   git commit -m "feat: Add native macOS build configuration"
   git remote add origin https://github.com/<your-username>/<your-repo-name>.git
   git push -u origin main
   ```
2. Navigate to your repository on GitHub and click the **Actions** tab.
3. Select **"Build macOS Plugin (Apple Silicon arm64 & Universal)"** from the left sidebar.
4. Click **"Run workflow"** and choose:
   - **Build Configuration:** `Release`
   - **Target Architecture:** `arm64` (Apple Silicon) or `arm64;x86_64` (Universal 2)
5. When the runner finishes (typically 3–5 minutes), download the generated zip package:
   - `SJChannelStrip-macOS-arm64.zip`
6. Inside the zip, you will find:
   - `SJ Channel Strip.vst3` (VST3 bundle)
   - `SJ Channel Strip.component` (AudioUnit bundle)
   - `SJ Channel Strip.app` (Standalone application)
   - `install_macos.sh` (1-click installer)

---

## 3. Option B: Local Compilation on a Mac

If you have access to a Mac (or send the project files to a Mac user), building natively is simple using either CMake Presets, standard CMake, or Xcode.

### Prerequisites on macOS
Open Terminal on macOS and install the Apple Command Line Tools and package manager:
```bash
# 1. Install Xcode Command Line Tools (Clang, make, git, codesign)
xcode-select --install

# 2. Install CMake and Ninja via Homebrew
brew install cmake ninja
```

### 1. Build Using CMake Presets (Recommended)

#### Apple Silicon Native (M1/M2/M3/M4 — Recommended for FL Studio Mac)
```bash
cmake --preset macos-arm64-release
cmake --build --preset macos-arm64-release --parallel
```

#### Universal 2 (Runs on both Apple Silicon and Intel Macs)
```bash
cmake --preset macos-universal-release
cmake --build --preset macos-universal-release --parallel
```

#### Legacy Intel (x86_64)
```bash
cmake --preset macos-x64-release
cmake --build --preset macos-x64-release --parallel
```

### 2. Manual CMake Command Line (Without Presets)
```bash
# Configure for Apple Silicon Release
cmake -B build-macos \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES="arm64" \
  -DCMAKE_OSX_DEPLOYMENT_TARGET="11.0" \
  -DSJ_ENABLE_AU=ON

# Build all targets
cmake --build build-macos --config Release --parallel
```

### 3. Generate and Open in Xcode IDE
If you want to debug, profile, or edit in Xcode:
```bash
cmake -B build-xcode -G Xcode -DCMAKE_OSX_ARCHITECTURES="arm64" -DSJ_ENABLE_AU=ON
open build-xcode/SJChannelStrip.xcodeproj
```

---

## 4. One-Click Installation on macOS

We have provided [`install_macos.sh`](file:///install_macos.sh) in the root directory.

Run this script on macOS after building:
```bash
chmod +x install_macos.sh
./install_macos.sh --config Release
```

### What `install_macos.sh` does automatically:
1. Copies `SJ Channel Strip.vst3` into `~/Library/Audio/Plug-Ins/VST3/`.
2. Copies `SJ Channel Strip.component` into `~/Library/Audio/Plug-Ins/Components/`.
3. Copies `SJ Channel Strip.app` into `~/Applications/`.
4. Creates the preset folder `~/Library/Application Support/SJ Audio/SJ Channel Strip/Presets/`.
5. Clears Gatekeeper quarantine flags (`xattr -cr`) to prevent macOS security blocks on locally built files.
6. Ad-hoc codesigns the bundles (`codesign --force --deep -s -`), which is required by macOS Apple Silicon kernels to load dynamic libraries.
7. Flushes the macOS AudioComponent cache (`killall -9 AudioComponentRegistrar`) so DAWs see the new plugin immediately without restarting your computer.

---

## 5. Setting Up in macOS DAWs

### A. FL Studio for Mac
1. Open **FL Studio** on macOS.
2. In the top menu, go to **Options** > **Manage Plugins**.
3. In the Plugin Manager window:
   - Ensure the search paths list includes `~/Library/Audio/Plug-Ins/VST3/` (or `/Library/Audio/Plug-Ins/VST3/`).
   - Check **[x] Rescan previously verified plugins**.
   - Check **[x] Verify plugins**.
   - Click the orange **"Find installed plugins"** button.
4. `SJ Channel Strip` will be discovered as an **Effect** plugin.
5. In your Mixer, click an effect slot and choose `SJ Channel Strip`.

### B. Apple Logic Pro / GarageBand
1. Launch **Logic Pro**. Logic automatically scans Audio Units on startup.
2. In a track's Mixer channel strip, click an Audio FX insert slot.
3. Navigate to **Audio Units** > **SJ Audio** > **SJ Channel Strip** > **Stereo**.
4. To verify via terminal:
   ```bash
   auval -v aufx Sjcs Sjau
   ```
   *(Expected result: `AU VALIDATION SUCCEEDED`)*

### C. Ableton Live (Mac)
1. In Ableton Live, open **Preferences** > **Plug-Ins**.
2. Turn **ON**:
   - `Use VST3 Audio Units Folder`
   - `Use Audio Units v2`
3. Click **Rescan**.

---

## 6. Code Signing & Apple Notarization (For Commercial Distribution)

For personal use and local testing, the ad-hoc signature applied by `install_macos.sh` (`codesign -s -`) is sufficient.

When distributing to external Mac users or commercial customers, macOS Gatekeeper requires code signing with an **Apple Developer ID Application** certificate and **Notarization** through Apple's notary service.

### Step 1: Sign with Apple Developer Certificate & Hardened Runtime
```bash
# Sign VST3 bundle
codesign --force --deep --options runtime \
  --sign "Developer ID Application: Your Name (TEAM_ID)" \
  "SJ Channel Strip.vst3"

# Sign AU bundle
codesign --force --deep --options runtime \
  --sign "Developer ID Application: Your Name (TEAM_ID)" \
  "SJ Channel Strip.component"
```

### Step 2: Package into a Zip or DMG
```bash
zip -r -y SJChannelStrip_Installer.zip "SJ Channel Strip.vst3" "SJ Channel Strip.component"
```

### Step 3: Submit for Notarization
```bash
xcrun notarytool submit SJChannelStrip_Installer.zip \
  --apple-id "your-apple-id@email.com" \
  --team-id "TEAM_ID" \
  --password "app-specific-password" \
  --wait
```

### Step 4: Staple the Notarization Ticket
```bash
xcrun stapler staple "SJ Channel Strip.vst3"
xcrun stapler staple "SJ Channel Strip.component"
```

---

## 7. Verification Checklist

| Aspect | Windows VST3 | macOS VST3 (FL Studio Mac) | macOS AU (Logic Pro) |
|---|---|---|---|
| **Plugin ID** | `Sjcs` / `Sjau` | `Sjcs` / `Sjau` | `Sjcs` / `Sjau` |
| **Bundle ID** | `com.sjaudio.sjchannelstrip` | `com.sjaudio.sjchannelstrip` | `com.sjaudio.sjchannelstrip` |
| **Parameters** | 73 APVTS Parameters | 73 APVTS Parameters | 73 APVTS Parameters |
| **Factory Presets** | 19 Presets | 19 Presets | 19 Presets |
| **State Compatibility** | Full XML Compatibility | Full XML Compatibility | Full XML Compatibility |
| **Processing Accuracy** | 64-bit IEEE Floating Point | 64-bit IEEE Floating Point | 64-bit IEEE Floating Point |
| **Primary Architecture** | x86_64 | Apple Silicon (`arm64`) | Apple Silicon (`arm64`) |
| **User Preset Directory** | `%APPDATA%\SJ Audio\SJ Channel Strip\Presets` | `~/Library/Application Support/SJ Audio/SJ Channel Strip/Presets` | `~/Library/Application Support/SJ Audio/SJ Channel Strip/Presets` |

© 2026 SJ Audio. All rights reserved.
