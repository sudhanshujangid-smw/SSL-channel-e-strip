# SJ Channel Strip — Installation & FL Studio Setup Guide

**Product:** SJ Channel Strip  
**Manufacturer:** SJ Audio  
**Format:** VST3 (64-bit) & Standalone App  
**Version:** 1.0.0  
**Host:** FL Studio (20 / 21 / 24+), Ableton Live, Reaper, Cubase, Studio One  
**Platform:** Windows 10 / 11 (64-bit)  

---

## 1. Quick One-Click Installation

To install the optimized Release VST3 build into your standard VST3 folder:

1. Open PowerShell in this project folder.
2. Run:
   ```powershell
   powershell -ExecutionPolicy Bypass -File .\install_vst3.ps1 -Config Release
   ```
3. The script will automatically copy `SJ Channel Strip.vst3` into:
   ```text
   %LOCALAPPDATA%\Programs\Common\VST3\SJ Channel Strip.vst3
   ```
   *(This is the official Steinberg VST3 user path scanned by FL Studio and all modern DAWs without needing administrator elevation).*

---

## 2. Manual Installation (Alternative)

If you prefer to copy the plugin manually:

1. Locate the built VST3 bundle:
   ```text
   build-release\SJChannelStrip_artefacts\Release\VST3\SJ Channel Strip.vst3
   ```
2. Copy the entire folder `SJ Channel Strip.vst3` into one of the following VST3 locations:
   - **User Location (Recommended):**
     ```text
     C:\Users\<YourUsername>\AppData\Local\Programs\Common\VST3\
     ```
   - **System Location (Requires Administrator):**
     ```text
     C:\Program Files\Common Files\VST3\
     ```

---

## 3. How to Scan and Add SJ Channel Strip in FL Studio

1. **Launch FL Studio**.
2. Go to the top menu and select **Options** > **Manage Plugins** (or click the plugin browser icon and select *Installed* > *Find plugins*).
3. The **FL Studio Plugin Manager** window will open.
4. Verify that the **Plugin search paths** list on the left includes:
   - `C:\Program Files\Common Files\VST3\`
   - `C:\Users\<YourUsername>\AppData\Local\Programs\Common\VST3\`
5. Check the following scan options:
   - **[x] Rescan previously verified plugins** (recommended if updating an existing version).
   - **[x] Verify plugins** (ensures FL Studio identifies it properly as an Effect).
6. Click the orange **"Find installed plugins"** button in the top-left corner.
7. Once the scan completes, search for **"SJ Channel Strip"** in the plugin list.
8. Click the checkmark / star next to **SJ Channel Strip** to favorite it in your Mixer Effect list.

---

## 4. How to Use on a Mixer Track

1. Press **`F9`** in FL Studio to open the Mixer.
2. Select any mixer track (e.g., *Insert 1*).
3. In the FX slots on the right, click an empty slot (Slot 1) and choose **SJ Channel Strip** under *SJ Audio* (or *Filter / EQ / Dynamics*).
4. The analog console interface will appear immediately.

---

## 5. External Sidechain Routing in FL Studio

To use the external sidechain feature with the Compressor or Gate (e.g. ducking a bassline to a kick drum):

1. In the FL Studio Mixer, select the **Trigger Track** (e.g. *Track 1: Kick*).
2. Right-click the small arrow at the bottom of the **Target Track** (e.g. *Track 2: Bass with SJ Channel Strip*).
3. Select **"Sidechain to this track"**.
4. Open the **SJ Channel Strip** plugin window on Track 2.
5. In the top-left corner of the plugin window, click the **Gear Icon** (Detailed Settings) > click the **Processing Tab** (plug with gear icon).
6. Under **Auxiliary Inputs**, set **Sidechain** to `1` (or right-click and pick the Kick track).
7. Return to the plugin UI, and in the **COMPRESSOR** or **GATE** section, set **`SC SRC`** to **`EXT`**.
8. Turn on **`LISTEN`** to audition the sidechain filter, or dial in the **Threshold** and **Ratio** to hear clean, clickless ducking.

---

## 6. Standalone Application Testing

For rapid auditioning and latency testing without launching a DAW:
- Executable path:
  ```text
  build-release\SJChannelStrip_artefacts\Release\Standalone\SJ Channel Strip.exe
  ```
- Double-click to launch. You can configure your ASIO or Windows Audio device directly in Options > Audio Settings.

---

## 7. Troubleshooting & FAQ

### Issue: FL Studio does not find the plugin after scanning
- **Solution:** Open FL Studio Plugin Manager (`Options` > `Manage Plugins`). Check `C:\Users\<username>\AppData\Local\Programs\Common\VST3` in the search path list. If it is missing, click the folder icon with a plus (`+`) to add it manually, then re-run *Find installed plugins*.

### Issue: Plugin appears as a Synth instead of an Effect
- **Solution:** In FL Studio Plugin Manager, find *SJ Channel Strip*, click the *Plugin* tab on the right, and ensure **Type** is set to **Effect**. Re-click *Apply changes*.

### Issue: Missing DLL error
- **Solution:** SJ Channel Strip is compiled with MSVC 2026 targeting the Universal C Runtime (UCRT). Ensure the [Microsoft Visual C++ Redistributable (x64)](https://aka.ms/vs/17/release/vc_redist.x64.exe) is installed on your Windows machine.

### Issue: User Presets are not saving
- **Solution:** User presets are safely saved as standard XML files under:
  ```text
  %APPDATA%\SJ Audio\SJ Channel Strip\Presets\
  ```
  Ensure your user account has write permissions to `%APPDATA%`.
