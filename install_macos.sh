#!/usr/bin/env bash
# ==============================================================================
# SJ Channel Strip — macOS Automated Installation & DAW Deployment Script
# Manufacturer: SJ Audio
# Formats: VST3 (FL Studio, Ableton, Reaper) & AU (Logic Pro, GarageBand)
# ==============================================================================

set -e

CONFIG="Release"
PRESET=""

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --config|-c) CONFIG="$2"; shift ;;
        --preset|-p) PRESET="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

echo "=========================================================="
echo "  SJ Channel Strip — macOS Deployment Installer"
echo "  Target Configuration: $CONFIG"
echo "=========================================================="

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Search candidate artifact locations
CANDIDATES=(
    "$SCRIPT_DIR/out/build/$PRESET/SJChannelStrip_artefacts/$CONFIG"
    "$SCRIPT_DIR/out/build/macos-arm64-release/SJChannelStrip_artefacts/$CONFIG"
    "$SCRIPT_DIR/out/build/macos-universal-release/SJChannelStrip_artefacts/$CONFIG"
    "$SCRIPT_DIR/out/build/macos-x64-release/SJChannelStrip_artefacts/$CONFIG"
    "$SCRIPT_DIR/build-macos/SJChannelStrip_artefacts/$CONFIG"
    "$SCRIPT_DIR/build/SJChannelStrip_artefacts/$CONFIG"
)

ARTIFACTS_DIR=""
for c in "${CANDIDATES[@]}"; do
    if [[ -d "$c/VST3/SJ Channel Strip.vst3" || -d "$c/AU/SJ Channel Strip.component" ]]; then
        ARTIFACTS_DIR="$c"
        break
    fi
done

if [[ -z "$ARTIFACTS_DIR" ]]; then
    echo "[-] Error: Built macOS plugin artifacts could not be found."
    echo "    Checked candidate directories:"
    for c in "${CANDIDATES[@]}"; do
        echo "      - $c"
    done
    echo "    Please run the build first:"
    echo "      cmake --preset macos-arm64-release"
    echo "      cmake --build --preset macos-arm64-release"
    exit 1
fi

echo "[+] Located plugin artifacts in: $ARTIFACTS_DIR"

# Destination directories
USER_VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
USER_AU_DIR="$HOME/Library/Audio/Plug-Ins/Components"
USER_APP_DIR="$HOME/Applications"

mkdir -p "$USER_VST3_DIR"
mkdir -p "$USER_AU_DIR"
mkdir -p "$USER_APP_DIR"

# 1. Install VST3 (FL Studio for Mac, Ableton, Reaper, Cubase)
VST3_SRC="$ARTIFACTS_DIR/VST3/SJ Channel Strip.vst3"
if [[ -d "$VST3_SRC" ]]; then
    VST3_DEST="$USER_VST3_DIR/SJ Channel Strip.vst3"
    echo "[+] Installing VST3 to: $VST3_DEST"
    rm -rf "$VST3_DEST"
    cp -R "$VST3_SRC" "$VST3_DEST"
    xattr -cr "$VST3_DEST" 2>/dev/null || true
    codesign --force --deep -s - "$VST3_DEST" 2>/dev/null || true
    echo "    -> VST3 installed successfully."
fi

# 2. Install AU (Logic Pro, GarageBand, Reaper, Ableton)
AU_SRC="$ARTIFACTS_DIR/AU/SJ Channel Strip.component"
if [[ -d "$AU_SRC" ]]; then
    AU_DEST="$USER_AU_DIR/SJ Channel Strip.component"
    echo "[+] Installing AU (Component) to: $AU_DEST"
    rm -rf "$AU_DEST"
    cp -R "$AU_SRC" "$AU_DEST"
    xattr -cr "$AU_DEST" 2>/dev/null || true
    codesign --force --deep -s - "$AU_DEST" 2>/dev/null || true
    echo "    -> AU installed successfully."
fi

# 3. Install Standalone App
APP_SRC="$ARTIFACTS_DIR/Standalone/SJ Channel Strip.app"
if [[ -d "$APP_SRC" ]]; then
    APP_DEST="$USER_APP_DIR/SJ Channel Strip.app"
    echo "[+] Installing Standalone App to: $APP_DEST"
    rm -rf "$APP_DEST"
    cp -R "$APP_SRC" "$APP_DEST"
    xattr -cr "$APP_DEST" 2>/dev/null || true
    codesign --force --deep -s - "$APP_DEST" 2>/dev/null || true
    echo "    -> Standalone App installed successfully."
fi

# 4. Preset Directory Initialization
PRESET_DIR="$HOME/Library/Application Support/SJ Audio/SJ Channel Strip/Presets"
mkdir -p "$PRESET_DIR"
echo "[+] Initialized user preset folder: $PRESET_DIR"

# 5. Flush macOS AudioComponent Cache so hosts detect changes immediately
echo "[+] Flushing macOS AudioComponent cache..."
killall -9 AudioComponentRegistrar 2>/dev/null || true

# 6. Optional auval validation if auval tool is available
if command -v auval &> /dev/null && [[ -d "$USER_AU_DIR/SJ Channel Strip.component" ]]; then
    echo "[+] Validating AU with Apple auval utility..."
    auval -v aufx Sjcs Sjau || echo "[!] auval reported warnings (normal during development without notarization)"
fi

echo "=========================================================="
echo "  INSTALLATION COMPLETE!"
echo "=========================================================="
echo "FL Studio for Mac:"
echo "  1. Open FL Studio > Options > Manage Plugins."
echo "  2. Ensure '~/Library/Audio/Plug-Ins/VST3' is checked."
echo "  3. Click 'Find installed plugins'."
echo "Logic Pro / GarageBand:"
echo "  1. Logic Pro will scan Audio Units upon launch."
echo "  2. Access under: Audio FX > Audio Units > SJ Audio > SJ Channel Strip."
echo "=========================================================="
